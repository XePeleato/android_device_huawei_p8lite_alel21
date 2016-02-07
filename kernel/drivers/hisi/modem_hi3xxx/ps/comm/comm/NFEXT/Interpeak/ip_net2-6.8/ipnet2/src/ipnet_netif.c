/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_netif.c,v $ $Revision: 1.280.12.1.4.5 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_netif.c,v $
 *     $Author: dbush $
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
 * Functions for the netif - interfaces.
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
#include <ipcom_heap_sort.h>
#include <ipcom_syslog.h>

#include "ipnet_h.h"
#ifdef IPCOM_USE_ETHERNET
#include "ipnet_eth.h"
#endif
#ifdef IPNET_USE_ROUTESOCK
#include "ipnet_routesock.h"
#endif
#ifdef IPCOM_USE_INET
#include "ipnet_ip4.h"
#endif
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#endif
#ifdef IPMPLS
#include "ipnet_mpls.h"
#endif

#include "ipnet_loopback.h"
#include "ipnet_netlink_h.h"
#include "ipnet_pkt_queue.h"
#include "ipnet_rtnetlink_h.h"
#include "ipnet_usr_sock.h"

#ifdef IPTCP
#include <iptcp.h>
#endif /* IPTCP */

#ifdef IPFIREWALL
#include <ipfirewall.h>
#include <ipfirewall_h.h>
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif

#ifdef IPCOM_USE_FORWARDER
#include <ipcom_forwarder.h>
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

#ifdef IP_PORT_LKM
IP_EXTERN int ipnet_loopback_if_setup(Ipnet_netif *netif);
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_FASTTEXT IP_STATIC void
ipnet_pkt_output_done(Ipnet_netif *netif, Ipcom_pkt *pkt);

IP_FASTTEXT IP_STATIC void
ipnet_pkt_output_done_cb(Ipcom_pkt *pkt);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* Search key for interfaces */
typedef struct Ipnet_netif_lookup_struct
{
    Ip_u16      vr;
    const char *ifname;
}
Ipnet_netif_lookup;


/* Soft interrupt structure for delayed IOCTLs */
typedef struct Ipnet_netif_ioctl_softirq_struct
{
    Ipnet_softirq softirq;  /* SoftIRQ structure */
    Ip_u32        ifindex;  /* Interface on which the IOCTL should be run */
    Ip_u32        command;  /* IOCTL command to run */
    void         *data;     /* (Optional) IOCTL data */
}
Ipnet_netif_ioctl_softirq;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
/*
 *===========================================================================
 *                    ipnet_ifname_obj_hash
 *===========================================================================
 * Description: Obj hash utility function for the ifname hash table
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ifname_obj_hash(Ipnet_netif *netif)
{
    return ipcom_hash_update(netif->ipcom.name,
                             ipcom_strlen(netif->ipcom.name),
                             netif->vr_index);
}


/*
 *===========================================================================
 *                    ipnet_ifname_key_hash
 *===========================================================================
 * Description: Key hash utility function for the ifname hash table
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ifname_key_hash(Ipnet_netif_lookup *key)
{
    return ipcom_hash_update(key->ifname,
                             ipcom_strlen(key->ifname),
                             key->vr);
}


/*
 *===========================================================================
 *                    ipnet_ifname_hash_cmp
 *===========================================================================
 * Description: Compare hash utility function for the ifname hash table.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_ifname_hash_cmp(Ipnet_netif *netif, Ipnet_netif_lookup *key)
{
    if (netif->vr_index != key->vr)
        return IP_FALSE;
    return ipcom_strcmp(netif->ipcom.name, key->ifname) == 0;
}

#endif /* IPNET_USE_FAST_IFNAME_LOOKUP */



/*
 *===========================================================================
 *                    ipnet_netif_key_hash
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_netif_key_hash(Ip_u32 *ifindex)
{
    unsigned n = (unsigned) *ifindex;
    return ((n << 11) ^ ~(n >> 3)) + ((n >> 1) + ~(n << 7));
}


/*
 *===========================================================================
 *                    ipnet_netif_obj_hash
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_netif_obj_hash(Ipnet_netif *netif)
{
    return ipnet_netif_key_hash(&netif->ipcom.ifindex);
}


/*
 *===========================================================================
 *                    ipnet_netif_hash_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_netif_hash_cmp(Ipnet_netif *netif, Ip_u32 *ifindex)
{
    return netif->ipcom.ifindex == *ifindex;
}


/*
 *===========================================================================
 *                    ipnet_if_index_array_cmp_cb
 *===========================================================================
 * Description: Compares two ifindexes.
 * Parameters:  ifidx1 - An ifindex value.
 *              ifidx2 - Another ifindex value.
 * Returns:     1 - if ifidx1 should be be after ifidx2
 *             -1 - if ifidx1 should be be before ifidx2
 *              0 - if the order between ifidx1 and ifidx2 does not matter.
 *
 */
IP_STATIC int
ipnet_if_index_array_cmp_cb(const void *ifidx1, const void *ifidx2)
{
    Ip_u32 id1 = *(Ip_u32*) ifidx1;
    Ip_u32 id2 = *(Ip_u32*) ifidx2;
    if (id1 > id2)
        return 1;
    if (id1 < id2)
        return -1;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_pkt_output_done_cb
 *===========================================================================
 * Description: Soft interrupt handler for output done processing.
 * Parameters:  pkt - A packet that a driver is finished sending.
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC void
ipnet_pkt_output_done_cb(Ipcom_pkt *pkt)
{
    ipnet_pkt_output_done(ipnet_if_indextonetif(IPCOM_VR_ANY, pkt->ifindex),
                          pkt);
}


/*
 *===========================================================================
 *                    ipnet_pkt_output_done
 *===========================================================================
 * Description: Wakes up processes waiting to write on this interface
 *              and free the packet.
 * Parameters:  pkt - A packet that a driver is finished with. pkt->ifindex
 *              is the index of the interface this packet was last used by.
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC void
ipnet_pkt_output_done(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    IP_DEBUGLINE(ip_assert(pkt == IP_NULL || IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_FREEQ));

    if (IP_LIKELY(netif != IP_NULL))
    {
        IPNET_IF_LOCK(netif);
#ifndef IPCOM_USE_JOB_QUEUE
        ipcom_atomic_inc(&netif->snd_id);
#endif
        if (netif->snd_head != IP_NULL)
        {
            while (netif->snd_head != IP_NULL)
            {
                Ipcom_pkt *head_pkt = netif->snd_head;

                netif->snd_head = IPNET_NETIF_SND_Q_NEXT(head_pkt);
                IPNET_NETIF_SND_Q_NEXT(head_pkt) = IP_NULL;

                if (ipnet_if_drv_output(netif, head_pkt) == -IP_ERRNO_EWOULDBLOCK)
                {
                    IPNET_NETIF_SND_Q_NEXT(head_pkt) = netif->snd_head;
                    netif->snd_head = head_pkt;
                    break;
                }
            }
        }
        else
        {
            Ipnet_pkt_queue *snd_queue = netif->snd_queue;

            if (snd_queue != IP_NULL)
            {
                Ipcom_pkt *q_pkt;
                int        ret;

                while (IP_NULL != (q_pkt = snd_queue->dequeue(snd_queue)))
                {
                    IPCOM_PKT_TRACE(q_pkt, IPCOM_PKT_ID_NETIF_DEQUEUE);
                    ret = ipnet_if_drv_output(netif, q_pkt);
                    if (ret == -IP_ERRNO_EWOULDBLOCK)
                    {
                        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                          1, 1, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                          ipnet_pkt_output_done, IPCOM_WV_NETD_INFO_RETRANSMIT,
                                          IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
                        IPNET_STATS(pkt_output_requeue++);
                        snd_queue->requeue(snd_queue, q_pkt);
                        break;
                    }
                }
            }
        }

        IPNET_IF_UNLOCK(netif);
    }
    /* else: The interface has been detached */

    /****** pkt checks - check IPNET send callback. ******/
    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 3, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                      ipnet_pkt_output_done, IPCOM_WV_NETD_INFO_PKT_SEND,
                      IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
    IPNET_STATS(pkt_netif_output_done_pkt++);
    if (IP_LIKELY(pkt != IP_NULL))
    {
        Ipcom_socket_eventcb event_cb;

        if (pkt->fd != -1 &&                            /* Packet was sent in context of a socket */
            ipnet->sockets[pkt->fd] != IP_NULL &&       /* Active socket */
            (event_cb = ipnet->sockets[pkt->fd]->ipcom.event_cb) != IP_NULL) /* Send callback for this socket */
        {
            event_cb(&ipnet->sockets[pkt->fd]->ipcom,
                     pkt,
                     IP_SOEVENT_CB_OUTPKT);
        }

        /* Give back written packet to IPNET. */
        ipcom_pkt_free(pkt);
    }
}


/*
 *===========================================================================
 *                    ipnet_if_update_lastchange
 *===========================================================================
 * Description: Update MIB interface tablse last change entries
 * Parameters:  none.
 * Returns:     Nothing.
 */
#ifdef IPCOM_USE_MIB2_NEW
IP_STATIC void
ipnet_if_update_mib2_lastchange(void)
{
    Ipcom_mib2_TimeStamp timestamp;

    if (ipnet->mib2.sysUpTimeFp != IP_NULL)
    {
        timestamp = ipnet->mib2.sysUpTimeFp();
    }
    else
    {
        timestamp = (Ipcom_mib2_TimeStamp)ipnet_msec_now();
        timestamp /= 10; /* Convert to hundreds of seconds */
    }
#ifdef IPCOM_USE_INET
    ipnet->mib2.ipv4InterfaceTableLastChange = (Ip_u32)timestamp;
#endif
#ifdef IPCOM_USE_INET6
    ipnet->mib2.ipv6InterfaceTableLastChange = (Ip_u32)timestamp;
#endif
    ipnet->mib2.ipIfStatsTableLastChange     = (Ip_u32)timestamp;
}
#endif


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_if_get_index_array
 *===========================================================================
 * Description: Returns a array with the ifindexes of all attached interfaces
 *              sorted on ifindex (lowest first).
 * Parameters:  vr - The virtual router the interface must be assigned to
 *                   or IPCOM_VR_ANY for all interfaces.
 *              ifindex - The ifindex to return or 0 for all.
 *              array_len - The length of the returned array.
 * Returns:     An array of ifindexes, must be freed with ipcom_free().
 *
 */
IP_GLOBAL Ip_u32 *
ipnet_if_get_index_array(Ip_u16 vr, Ip_u32 ifindex, unsigned *array_len)
{
    int          num_elem;
    Ip_u32       i;
    Ipnet_netif *netif;
    Ip_u32      *ifindexes;

    ip_assert(array_len != IP_NULL);

    num_elem = (ifindex ? 1 : IPNET_NETIF_NUM_ATTACHED);
    ifindexes = ipcom_malloc((num_elem + 1) * sizeof(Ip_u32));
    if (ifindexes == IP_NULL)
        return IP_NULL;

    num_elem = 0;
    IPNET_NETIF_FOR_EACH_OR_SPECIFIC_ON_VR(netif, vr, ifindex, i)
    {
        ifindexes[num_elem++] = netif->ipcom.ifindex;
    }

    ipcom_heap_sort(ifindexes,
                    num_elem,
                    sizeof(Ip_u32),
                    ipnet_if_index_array_cmp_cb,
                    &ifindexes[num_elem]);
    *array_len = num_elem;
    return ifindexes;
}


/*
 *===========================================================================
 *                    ipnet_if_indextonetif
 *===========================================================================
 * Description: Returns the network interface matching the index..
 * Parameters:  vr - The virtual router the interface must be assigned to.
 *              ifindex - Network interface id returned from ipnet_if_attach()
 * Returns:     The network interface or IP_NULL if the call fails.
 *              ipcom_errno will return IP_ERRNO_ENXIO if this
 *              call fails.
 *
 */
IP_GLOBAL Ipnet_netif *
ipnet_if_indextonetif(Ip_u16 vr, Ip_u32 ifindex)
{
    Ipnet_netif *netif;

    netif = ipcom_hash_get(ipnet->netifs, &ifindex);
    if (netif != IP_NULL
        && (netif->vr_index == vr || vr == IPCOM_VR_ANY))
        return netif;

    /* Not found */
    (void)ipcom_errno_set(IP_ERRNO_ENXIO);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_if_nametonetif
 *===========================================================================
 * Description: Returns the network interface matching the ifname.
 * Parameters:  ifname - The name of the index.
 * Returns:     The network interface or IP_NULL if the call fails.
 *              ipcom_errno will return IP_ERRNO_ENXIO if this
 *              call fails.
 *
 */
IP_GLOBAL Ipnet_netif *
ipnet_if_nametonetif(Ip_u16 vr, IP_CONST char *ifname)
{
    char  name_buf[IP_IFNAMSIZ];
    char *name;
    char *vr_str;
    char *iter;

    ipcom_strncpy(name_buf, ifname, sizeof(name_buf));
    name = ipcom_strtok_r(name_buf, "#", &iter);
    if (name == IP_NULL)
        return IP_NULL;

    vr_str = ipcom_strtok_r(IP_NULL, "#", &iter);
    if (vr_str != IP_NULL)
    {
        if (ipnet_has_priv(IPNET_PRIV_CHVR))
            vr = (Ip_u16) ipcom_atoi(vr_str);
        else
            return IP_NULL;
    }

#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    {
        Ipnet_netif_lookup key;

        ip_assert(ipnet->ifname_tab != IP_NULL);

        key.vr     = vr;
        key.ifname = name;
        return (Ipnet_netif *)ipcom_hash_get(ipnet->ifname_tab, &key);
    }
#else
    {
        Ipnet_netif *netif;
        Ip_u32 i;

        IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
        {
            if (ipcom_strcmp(netif->ipcom.name, name) == 0)
                return netif;
        }
    }
#endif  /* IPNET_USE_FAST_IFNAME_LOOKUP */

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_if_to_sockaddr_dl
 *===========================================================================
 * Description: Create a Ip_sockaddr_dl structure for the hardware
 *              address of the specified interface.
 * Parameters:  netif - A network interface.
 *              dl - Will hold the interface address.
 * Returns:     The size of the address.
 *
 */
IP_GLOBAL Ip_size_t
ipnet_if_to_sockaddr_dl(Ipnet_netif *netif, struct Ip_sockaddr_dl *dl)
{
    Ip_size_t len;
    Ip_size_t name_len;

    ip_assert(netif != IP_NULL);

    for (name_len = 0; name_len < IP_IFNAMSIZ; name_len++)
        if (netif->ipcom.name[name_len] == '\0')
            break;

    /* Align the name_len to 2 bytes to make sure that the
       link address is located on an even address */
    len = sizeof(struct Ip_sockaddr_dl) - sizeof(dl->sdl_data) +
        IP_MAX(sizeof(dl->sdl_data),
               netif->ipcom.link_addr_size + IPNET_ALIGNED_SIZE(name_len, 2));
    /* Make sure the socket address length is 4 bytes aligned */
    len = IPNET_ALIGNED_SIZE(len, 4);

    if (dl == IP_NULL)
        return len;

    IPCOM_SA_LEN_SET(dl, len);
    dl->sdl_family = IP_AF_LINK;
    dl->sdl_index  = (Ip_u16)netif->ipcom.ifindex;
    dl->sdl_type   = (Ip_u8)netif->ipcom.type;
    dl->sdl_alen   = (Ip_u8)netif->ipcom.link_addr_size;
    dl->sdl_slen   = 0;

    IP_SOCKADDR_DL_STORE_NAME(dl, netif->ipcom.name, name_len);
    if (netif->ipcom.link_addr_size > 0)
        ipcom_memcpy(IP_SOCKADDR_DL_LLADDR(dl), netif->ipcom.link_addr, netif->ipcom.link_addr_size);

    return len;
}


/*
 *===========================================================================
 *                    ipnet_if_can_detach
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_bool
ipnet_if_can_detach(Ipcom_netif *netif)
{
#ifdef IP_PORT_LKM
    /* Since the device is already detached from the device layer,
     * IPNET can never refuse to detach when used with LKM
     */
    return 1;
#endif

#ifdef IPNET_USE_VLAN
    if (netif->type == IP_IFT_ETHER)
    {
        Ip_u32       i;
        Ipnet_netif *vlanif;

        IPNET_NETIF_FOR_EACH(vlanif, i)
        {
            if (vlanif->ipcom.type != IP_IFT_L2VLAN)
                continue;
            if (vlanif->eth->vlan.parent->ipcom.ifindex == netif->ifindex)
                return IP_FALSE;
        }
    }
#endif /* IPNET_USE_VLAN */

    return IP_BIT_ISFALSE(netif->flags,
                          IP_IFF_UP | (netif->type == IP_IFT_ETHER ? 0 : IP_IFF_RUNNING));
}


/*
 *===========================================================================
 *                    ipnet_if_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_if_free(Ipnet_netif *netif)
{
    if (netif->ipcom.plink)
        ipcom_free(netif->ipcom.plink);
#ifdef IPCOM_USE_ETHERNET
    if (netif->eth != IP_NULL)
        ipcom_free(netif->eth);
#endif
#ifdef IPNET_USE_WLAN
    if (netif->wlan != IP_NULL)
        ipcom_free(netif->wlan);
#endif
    ipnet_if_clean_snd_queue(netif);
    ipcom_free(netif);
}


/*
 *===========================================================================
 *                    ipnet_if_detach
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_if_detach(Ipnet_netif *netif)
{
    int   ret;

    /* All network interface specific timers must be canceled at this point */
#ifdef IPCOM_USE_INET
    ip_assert(netif->igmpv1_querier_present_tmo == IP_NULL);
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    ip_assert(netif->igmpv2_querier_present_tmo == IP_NULL);
#endif
#ifdef IPNET_USE_RFC1256
    ip_assert(netif->inet4_rfc1256_tmo == IP_NULL);
#endif
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    ip_assert(netif->mldv1_querier_present_tmo == IP_NULL);
#endif
    ip_assert(netif->inet6_rs_tmo == IP_NULL);
#endif /* IPCOM_USE_INET6 */

    /* Remove the interface. */
    if (ipcom_hash_remove(ipnet->netifs, &netif->ipcom) != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "Interface %s does not appear to be attached", netif->ipcom.name);
        return -IP_ERRNO_ENXIO;
    }
    ret = netif->ipcom.ifindex;

    /* Remove all IPv4 and IPv6 interface addresses and routes. */
    ipnet_if_clean(netif, IP_TRUE, IP_TRUE);

    ipnet_pkt_queue_delete(netif->snd_queue);
    netif->snd_queue = IP_NULL;

    /* Announce departure. */
    IPNET_NETLINKSOCK(ipnet_rtnetlink_link_delete(netif));
    IPNET_ROUTESOCK(ipnet_routesock_if_announce(netif, IPNET_IFAN_DEPARTURE));
    IPCOM_LOG2(NOTICE, "Detached network interface %s [%d]", netif->ipcom.name, netif->ipcom.ifindex);

#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    (void)ipcom_hash_remove(ipnet->ifname_tab,netif);
#endif  /* IPNET_USE_FAST_IFNAME_LOOKUP */

#ifdef IPCOM_USE_INET
#ifdef IPNET_USE_RFC3927
    ip_assert(netif->inet4_lladdr_tmo == IP_NULL);
#endif
#endif

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_if_clean_snd_queue
 *===========================================================================
 * Description: Frees all packets from the send queue and wakes up all
 *              processing sleeping on the output queue of this interface.ipnet_if_clean_snd_queue
 * Parameters:  netif - An IPNET interface pointer.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_if_clean_snd_queue(Ipnet_netif *netif)
{
    /* Reset the QoS output queue, if present. One of the
       responsibilities of the reset is to free all packets currently
       queued. */
    if (netif->snd_queue != IP_NULL)
        netif->snd_queue->reset(netif->snd_queue);

    /* Free all packets queued on the fast-FIFO output queue */
    while (IP_NULL != netif->snd_head)
    {
        Ipcom_pkt *pkt_next = netif->snd_head->next;

        ipcom_pkt_free(netif->snd_head);
        netif->snd_head = pkt_next;

    }
    netif->snd_tail = IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_if_drv_ioctl
 *===========================================================================
 * Description: Issues an IO control down to the driver.
 * Parameters:  netif - The network interface to do IO control for.
 *              command - The IO control command.
 *              data - The data associated with the command.
 * Returns:     Standard error code.
 *
 */
IP_GLOBAL int
ipnet_if_drv_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    int ret = -IP_ERRNO_EOPNOTSUPP;

    ip_assert(netif);

    if (netif->ipcom.drv_ioctl != IP_NULL)
    {
        ipnet_if_clean_snd_queue(netif);
        IPNET_IF_LOCK(netif);
        ret = netif->ipcom.drv_ioctl(&netif->ipcom, command, data);
        IPNET_IF_UNLOCK(netif);
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_if_link_ioctl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   0 or -IP_ERRNO_ENETDOWN if interface is released.
 *
 */
IP_GLOBAL int
ipnet_if_link_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    int ret;

    ip_assert(netif);

    IPNET_IF_LOCK(netif);
    ret = netif->link_ioctl(netif, command, data);
    IPNET_IF_UNLOCK(netif);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_if_clean
 *===========================================================================
 * Description:   Remove all interface routes and addresses.
 * Parameters:    netif - Interface to clean.
 *                clean_ip4 - IP_TRUE if clean IPv4 addresses and routes.
 *                clean_ip6 - IP_TRUE if clean IPv6 addresses and routes.
 * Returns:       -
 *
 */
IP_GLOBAL void
ipnet_if_clean(Ipnet_netif *netif, Ip_bool clean_ip4, Ip_bool clean_ip6)
{
    int           i;
    Ipnet_socket *sock;

    /* Wake all processes that are sleeping on a socket using this interface */
    for (i = IPNET_SOCKET_STARTFD; i < ipnet_conf_max_sockets; i++)
    {
        sock = ipnet_get_sock(i);
        if (IP_NULL != sock)
        {
            if (ipnet_sock_drop_all_multicast_memberships(sock, netif, clean_ip4, clean_ip6))
            {
#ifdef IPTCP
                if (sock->tcb != IP_NULL)
                    (void)iptcp_drop_connection(sock, -IP_ERRNO_ENETDOWN);
                else
#endif /* IPTCP */
                    ipnet_sock_data_avail(sock, IP_ERRNO_ENETDOWN, IP_SHUT_RDWR);
            }
            ipnet_release_sock(sock);
        }
    }

#ifdef IPCOM_USE_INET
    if (clean_ip4)
    {
        struct Ipnet_ip4_addr_entry_struct *del_addr;

        /* Remove all IPv4 addresses assigned to the interface */
        while (IP_NULL != (del_addr = netif->inet4_addr_list))
            (void) ipnet_ip4_remove_addr(netif, del_addr->ipaddr_n);

#ifdef IPNET_USE_RFC3927
        ipnet_timeout_cancel(netif->inet4_lladdr_tmo);
#endif
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (clean_ip6)
    {
        struct Ipnet_ip6_addr_entry_struct *del_addr;

        /* Remove all IPv6 addresses assigned to the interface */
        while (IP_NULL != (del_addr = netif->inet6_addr_list))
            (void) ipnet_ip6_remove_addr(netif, &del_addr->addr);
    }
#endif /* IPCOM_USE_INET6 */

    /* Remove all routes using this interface */
#ifdef IPCOM_USE_INET
    if (clean_ip4)
        ipnet_route_remove_all(IP_AF_INET, netif);
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (clean_ip6)
        ipnet_route_remove_all(IP_AF_INET6, netif);
#endif /* IPCOM_USE_INET6 */
}


/*
 *===========================================================================
 *                    ipnet_if_drv_output
 *===========================================================================
 * Description: Outputs a packet to the driver and updates output statistics.
 * Parameters:  netif - The interface the packet will be sent on.
 *              pkt - Packet to send. pkt->start is the offset to the
 *              link header.
 * Returns:     0 = success
 *             <0 = error code, most common error is -IP_ERRNO_WOULDBLOCK
 *              which is returen if the driver cannot accept any more packets
 *              at this time (a new attempt should be done after the next TX interrupt).
 *
 */
IP_FASTTEXT IP_GLOBAL int
ipnet_if_drv_output(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ip_u32 pkt_flags;
    int    pkt_octets;
    int    ret;

    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));
    ip_assert(pkt->end <= pkt->maxlen);
    ip_assert(pkt->start >= 0);
    ip_assert(pkt->start <= pkt->end);
    ip_assert(ipnet_is_stack_task());

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_RUNNING)))
    {
        /* Link layer not running, drop the packet silently */
        (void)ipcom_pkt_output_done(&netif->ipcom, pkt, IP_FLAG_FC_STACKCONTEXT);
        IPCOM_MIB2_NETIF(ifOutDiscards++);
        return 0;
    }

#ifdef IPFIREWALL_USE_MAC_FILTER
    if (netif->ipcom.type == IP_IFT_ETHER || netif->ipcom.type == IP_IFT_L2VLAN)
    {
        if (ipfirewall != IP_NULL
            && ipfirewall_mac_output_hook(&netif->ipcom, pkt) == 1)
        {
            /* MAC filter blocked packed */
            (void)ipcom_pkt_output_done(&netif->ipcom, pkt, IP_FLAG_FC_STACKCONTEXT);
            return 0;
        }
    }
#endif /* IPFIREWALL_USE_MAC_FILTER */
    pkt_flags      = pkt->flags;
    pkt_octets     = pkt->end - pkt->start;
    netif->obytes += pkt_octets;

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)
    ret = 0;
    if (ipcom_forwarder_transmit_pkt_cf(pkt, &netif->ipcom)) /* Returns zero if it transmitted packet*/

#endif
    {
      ret = netif->ipcom.drv_output(&netif->ipcom, pkt);
    }
    if (IP_LIKELY(ret >= 0))
    {
        netif->opackets++;
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 4, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                          ipnet_if_drv_output, IPCOM_WV_NETD_INFO_PKT_SEND,
                          IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
        IPNET_STATS(pkt_output_drv++);
        if (ret == 0)
        {
            if (IP_BIT_ISSET(pkt_flags, IPCOM_PKT_FLAG_BROADCAST))
                IPCOM_MIB2_NETIF(ifOutBroadcastPkts++);
            else if(IP_BIT_ISSET(pkt_flags, IPCOM_PKT_FLAG_MULTICAST))
                IPCOM_MIB2_NETIF(ifOutMulticastPkts++);
            else
                IPCOM_MIB2_NETIF(ifOutUcastPkts++);
            IPCOM_MIB2_NETIF(ifOutOctets += pkt_octets);
        }
    }
    else
    {
        if (ret == -IP_ERRNO_EWOULDBLOCK)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 5, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                              ipnet_if_drv_output, IPCOM_WV_NETD_INFO_QUEUE_FULL,
                              IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
            IPNET_STATS(pkt_output_ewouldblock++);
        }
        else
        {
            netif->oerrors++;
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 6, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                              ipnet_if_drv_output, IPCOM_WV_NETD_PKTLOST,
                              IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
            IPNET_STATS(pkt_output_err++);
            IPCOM_MIB2_NETIF(ifOutErrors++);
        }
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_if_output
 *===========================================================================
 * Description: Outputs a packet to the driver layer and blocks user
 *              processes if the driver and the interface queue is full.
 * Parameters:  netif - The interface that will be used to output this packet
 *              pkt - The packet to output. pkt->start is the offset to the
 *              link layer header.
 * Returns:     0 = success,
 *             <0 = error code (-IP_ERRNO_EWOULDBLOCK is possible when called
 *              in the context of a non-blocking user process).
 *
 */
IP_FASTTEXT IP_GLOBAL int
ipnet_if_output(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_queue *q;
    int              ret;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 7, IPCOM_WV_NETDEVENT_START,
                       ipnet_if_output, IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
    IPNET_STATS(pkt_output++);
    ip_assert(pkt->ifindex == netif->ipcom.ifindex
              || IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK));

#ifndef IPCOM_USE_JOB_QUEUE
    for (;;)
    {
        int snd_id = ipcom_atomic_get(&netif->snd_id);
#endif
        q = netif->snd_queue;

        if (q == IP_NULL)
        {
            if (IP_LIKELY(netif->snd_head == IP_NULL))
            {
                /* Try to send this packet directly to the interface */
                ret = ipnet_if_drv_output(netif, pkt);
                if (IP_LIKELY(ret != -IP_ERRNO_EWOULDBLOCK))
                    return ret;
            }
            /* else: interface congested, yield the CPU and then queue
               the packet */
        }
        else
        {
            /* Try add packet on output queue. */
            ret = q->enqueue(q, pkt);
            if (IP_UNLIKELY(ret < 0))
            {
                if (ret != -IP_ERRNO_ENOBUFS)
                {
                    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                      1, 10, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                                      ipnet_if_output, IPCOM_WV_NETD_PKTLOST,
                                      IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
                    IPNET_STATS(pkt_output_discard++);
                    IPCOM_MIB2_NETIF(ifOutDiscards++);

                    ipcom_pkt_free(pkt);
                    return 0;
                }
            }
            else
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 8, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                  ipnet_if_output, IPCOM_WV_NETD_INFO_PKT_SEND,
                                  IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
                IPNET_STATS(pkt_output_queue++);
                pkt = q->dequeue(q);
                if (IP_UNLIKELY(pkt == IP_NULL))
                    return 0;

                ret = ipnet_if_drv_output(netif, pkt);
                if (IP_LIKELY(ret != -IP_ERRNO_EWOULDBLOCK))
                    return ret;
            }
        }

#ifndef IPCOM_USE_JOB_QUEUE
        /* Yield the CPU to give other processes a chance to free up
           resources in the outgoing network interface */
        ipcom_proc_yield();
        /* #else no point in yielding when using the job queue, since
            it is the job queue that runs the ipcom_pkt_output_done()
            code as well */
#endif

        if (q == IP_NULL
            && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
        {
            /* don't queue forwarded packet on fast-FIFO queues since
               there is no rate limit on forwarded packets */
            IPNET_STATS(pkt_output_discard++);
            IPCOM_MIB2_NETIF(ifOutDiscards++);
            ipcom_pkt_free(pkt);
            return -IP_ERRNO_EWOULDBLOCK;
        }

#ifndef IPCOM_USE_JOB_QUEUE
        if (ipcom_atomic_get(&netif->snd_id) == snd_id)
        {
#endif
            if (q == IP_NULL)
            {
                /* put packet in in the fast-FIFO queue */
                netif->snd_tail = pkt;
                if (netif->snd_head == IP_NULL)
                {
                    netif->snd_head = pkt;
                    netif->snd_tail = pkt;
                }
                else
                {
                    IPNET_NETIF_SND_Q_NEXT(netif->snd_tail) = pkt;
                    /* Try to write the packet chain the the interface
                       again since virtual devices, like tunnels and
                       VLAN will not receive an event from the driver
                       when one should try again. */
                    ipnet_pkt_output_done(netif, IP_NULL);
                }
            }
            else
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 9, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                  ipnet_if_output, IPCOM_WV_NETD_INFO_RETRANSMIT,
                                  IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
                IPNET_STATS(pkt_output_requeue++);
                q->requeue(q, pkt);
            }
#ifndef IPCOM_USE_JOB_QUEUE
            break;
        }
        /* else: ipnet_pkt_output_done has been called at least once
           since we entered this function, lets try to send the packet
           again. */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 12, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                          ipnet_if_output, IPCOM_WV_NETD_INFO_SEND,
                          IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
        IPNET_STATS(pkt_output_fast_drv_ready++);
    }
#endif /* IPCOM_USE_JOB_QUEUE */

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_if_init_ppp_peer
 *===========================================================================
 * Description: Initializes the PPP structure.
 * Parameters:  netif - The network interface structure for this PPP interface.
 *              p - The PPP structure to initialize.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_if_init_ppp_peer(Ipnet_netif *netif, Ipnet_ppp_peer *p)
{
    ipcom_memset(p, 0, sizeof(*p));
    p->netif = netif;
}


/*
 *===========================================================================
 *                    ipnet_if_clean_ppp_peer
 *===========================================================================
 * Description: Frees the resouces allocated by setting peers.
 * Parameters:  p - A PPP structure.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_if_clean_ppp_peer(Ipnet_ppp_peer *p)
{
#ifdef IPCOM_USE_INET
    (void)ipnet_if_set_ipv4_ppp_peer(p, &ip_inaddr_any);
#endif
#ifdef IPCOM_USE_INET6
    (void)ipnet_if_set_ipv6_ppp_peer(p, &ip_in6addr_any);
#endif
}


/*
 *===========================================================================
 *                    ipnet_if_set_ipv4_ppp_peer
 *===========================================================================
 * Description: Initializes the PPP structure.
 * Parameters:  p - A PPP structure.
 *              peer4 - The IPv4 address of the peer.
 * Returns:     0 = success, <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET
IP_GLOBAL int
ipnet_if_set_ipv4_ppp_peer(Ipnet_ppp_peer *p, IP_CONST struct Ip_in_addr *peer4)
{
    int          ret = 0;
    Ipnet_netif *netif = p->netif;

    if (p->peer4.s_addr != ip_inaddr_any.s_addr)
    {
        /* Remove the route to the old peer */
        (void)ipnet_route_delete2(IP_AF_INET,
                                  netif->vr_index,
                                  IPCOM_ROUTE_TABLE_DEFAULT,
                                  &p->peer4,
                                  IP_NULL,
                                  IP_NULL,
                                  p->netif->ipcom.ifindex,
                                  0,
                                  0,
                                  IP_FALSE);
    }

    if (peer4->s_addr != ip_inaddr_any.s_addr)
    {
        struct Ipnet_route_add_param param;
        struct Ip_sockaddr_in        gw;


        /* Remove all routes to the peer address */
        while (ipnet_route_delete2(IP_AF_INET,
                                   netif->vr_index,
                                   0,
                                   peer4,
                                   IP_NULL,
                                   IP_NULL,
                                   0,
                                   0,
                                   0,
                                   IP_FALSE) == 0)
        {
            /* Do nothing */
        }

        p->peer4 = *peer4;
        /* Add a host route to the peer */
        ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
        param.flags  = (IPNET_RTF_UP
                        | IPNET_RTF_HOST
                        | IPNET_RTF_DONE
                        | IPNET_RTF_TUNNELEND);
        param.domain = IP_AF_INET;
        param.vr     = netif->vr_index;
        param.table  = IPCOM_ROUTE_TABLE_DEFAULT;
        param.netif  = netif;
        param.key    = peer4;
        /* The gateway must be the same as the key to work with GateD */
        ipcom_memset(&gw, 0, sizeof(struct Ip_sockaddr_in));
        gw.sin_family      = IP_AF_INET;
        IPCOM_SA_LEN_SET(&gw, sizeof(struct Ip_sockaddr_in));
        ipcom_memcpy(&gw.sin_addr, peer4, sizeof(gw.sin_addr));
        param.gateway = (struct Ip_sockaddr *)&gw;
        ret = ipnet_route_add(&param);
    }

    if (ret < 0)
        IP_PANIC2();
    return ret;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_if_set_ipv6_ppp_peer
 *===========================================================================
 * Description: Initializes the PPP structure.
 * Parameters:  p - A PPP structure.
 *              peer6 - The IPv6 address of the peer.
 * Returns:     0 = success, <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET6
IP_GLOBAL int
ipnet_if_set_ipv6_ppp_peer(Ipnet_ppp_peer *p, IP_CONST struct Ip_in6_addr *peer6)
{
    int          ret = 0;
    Ipnet_netif *netif = p->netif;

    if (!IP_IN6_IS_ADDR_UNSPECIFIED(&p->peer6))
    {
        (void) ipnet_route_delete2(IP_AF_INET6,
                                   netif->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &p->peer6,
                                   IP_NULL,
                                   IP_NULL,
                                   p->netif->ipcom.ifindex,
                                   0,
                                   0,
                                   IP_FALSE);
    }

    if (!IP_IN6_IS_ADDR_UNSPECIFIED(peer6))
    {
        struct Ipnet_route_add_param param;

        /* Remove all routes to the peer address */
        while (ipnet_route_delete2(IP_AF_INET6,
                                   netif->vr_index,
                                   0,
                                   peer6,
                                   IP_NULL,
                                   IP_NULL,
                                   0,
                                   0,
                                   0,
                                   IP_FALSE) == 0)
        {
            /* Do nothing */
        }

        p->peer6 = *peer6;
        /* Add a host route to the peer */
        ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
        param.flags  = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DONE;
        param.domain = IP_AF_INET6;
        param.vr     = netif->vr_index;
        param.table  = IPCOM_ROUTE_TABLE_DEFAULT;
        param.netif  = netif;
        param.key    = peer6;
        ret = ipnet_route_add(&param);
    }

    if (ret < 0)
        IP_PANIC2();
    return ret;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_sys_if_indexname
 *===========================================================================
 * Description: Returns the name of the interface that has the specified
 *              interface index.
 * Parameters:  sig - contains the interface index
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_if_indexname(Ipnet_sig *sig)
{
    struct Ipnet_sig_if_indexname *indexname = &sig->d.indexname;
    Ipnet_netif                   *netif;
    Ip_u16                         vr;

    vr = ipnet_get_effective_vr();

    if (sig->sig_type == IPNET_SIG_IF_INDEXTONAME)
        netif = ipnet_if_indextonetif(vr, indexname->ifindex);
    else
    {
        ip_assert(sig->sig_type == IPNET_SIG_IF_NAMETOINDEX);
        netif = ipnet_if_nametonetif(vr, indexname->ifname);
    }

    indexname->num_attached = IPNET_NETIF_NUM_ATTACHED;
    indexname->highest_assigned_ifindex = ipnet->next_ifindex;

    if (netif == IP_NULL)
        indexname->ifindex = 0;
    else
    {
        indexname->ifindex = netif->ipcom.ifindex;
        if (ipnet_get_effective_vr() == netif->vr_index)
            ipcom_strcpy(indexname->ifname, netif->ipcom.name);
        else
            ipcom_sprintf(indexname->ifname,
                          "%s#%u",
                          netif->ipcom.name,
                          netif->vr_index);
    }
}


/*
 *===========================================================================
 *                    ipnet_sys_if_nameindex
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_if_nametonetif(Ipnet_sig *sig)
{
    Ipnet_netif                     *netif;

    netif = ipnet_if_nametonetif(sig->d.netif.vr, sig->d.netif.ifname);

    if (netif != IP_NULL)
    {
        IPNET_IF_LOCK(netif);
        sig->d.netif.netif = &netif->ipcom;
    }
    else
        sig->d.netif.netif = IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_if_init
 *===========================================================================
 * Description: Initializes the usage of network interfaces
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_if_init(void)
{
    ipnet->netifs = ipcom_hash_new((Ipcom_hash_obj_func)ipnet_netif_obj_hash,
                                   (Ipcom_hash_key_func)ipnet_netif_key_hash,
                                   (Ipcom_hash_cmp_func)ipnet_netif_hash_cmp);
    if (ipnet->netifs == IP_NULL)
    {
        IPCOM_LOG0(ERR,
                   "ipnet_if_init() :: Failed to create netif table");
        return -IP_ERRNO_ENOMEM;
    }

#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    ipnet->ifname_tab = ipcom_hash_new((Ipcom_hash_obj_func)ipnet_ifname_obj_hash,
                                       (Ipcom_hash_key_func)ipnet_ifname_key_hash,
                                       (Ipcom_hash_cmp_func)ipnet_ifname_hash_cmp);
    if (ipnet->ifname_tab == IP_NULL)
    {
        IPCOM_LOG0(ERR,
                   "ipnet_if_init() :: Failed to create fast name lookup table");
        return -IP_ERRNO_ENOMEM;
    }
#endif /* IPNET_USE_FAST_IFNAME_LOOKUP */

#ifdef IPNET_USE_NETLINK
    ipnet_rtnetlink_link_init();
#endif
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ppp_if_init
 *===========================================================================
 * Description:   Initialize a PPP or PPPoE interface
 * Parameters:
 * Returns:
 *
 */
#ifdef IPPPP
IP_PUBLIC int
ipnet_ppp_if_init(Ipnet_netif *netif)
{
    if (ipnet->ppp.if_init == IP_NULL)
        return -IP_ERRNO_EINVAL;
    return ipnet->ppp.if_init(netif);
}
#endif /* IPPPP */


/*
 *===========================================================================
 *                    ipnet_netif_change_flags
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_netif_change_flags(Ipnet_netif **netif, Ip_u32 flags, Ip_u32 change)
{
    Ip_u32       ro_flags;
    Ip_u32       old_flags;
    int          ret = 0;

    if (change)
        flags = (flags & change) | ((*netif)->ipcom.flags & ~change);

    if (IP_BIT_ISSET(flags, IP_IFF_UP))
    {
        /* Flag interface up and interface is not UP. */
        if (IP_BIT_ISFALSE((*netif)->ipcom.flags, IP_IFF_UP))
            ret = ipnet_if_link_ioctl(*netif, IP_SIOCXOPEN, IP_NULL);
    }
    else
    {
        Ip_u32 ifindex = (*netif)->ipcom.ifindex;

        /* Always issue close message since the link layer might in startup */
        ret = ipnet_if_link_ioctl(*netif, IP_SIOCXCLOSE, IP_NULL);
        *netif = ipnet_if_indextonetif(IPCOM_VR_ANY, ifindex);
        if (*netif == IP_NULL)
            return 0;
    }

    /* Set new flags but leave all read-only flags unchanged. */
    ro_flags = IP_IFF_UP | IP_IFF_RUNNING | IP_IFF_BROADCAST
        | IP_IFF_POINTOPOINT | IP_IFF_SIMPLEX | IP_IFF_MULTICAST | IP_IFF_ALLMULTI;
    old_flags = (*netif)->ipcom.flags & ~ro_flags;
    (*netif)->ipcom.flags = (flags & ~ro_flags) | ((*netif)->ipcom.flags & ro_flags);

    if ((old_flags ^ (*netif)->ipcom.flags) & ~(IP_IFF_UP|IP_IFF_RUNNING))
    {
        ipnet_kioevent(*netif, IP_EIOXCHANGE, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
    }

    /**/
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sys_if_attach
 *===========================================================================
 * Description: Attaches a new network interface to IPNET.
 * Parameters:  netif - A initialized netif structure.
 * Returns:     The interface index for this interface or negative errno.
 *
 */
IP_GLOBAL int
ipnet_sys_if_attach(Ipcom_netif *ipcomif)
{
    Ipnet_netif *netif;
    Ip_u32       ifindex;
    int          ret = 0;

    netif = (Ipnet_netif *)ipcomif;

    ip_assert(*ipcomif->name);
    ip_assert(netif->link_ioctl != IP_NULL);
    ip_assert(netif->link_input != IP_NULL);
    ip_assert(netif->ipcom.mtu > 40);

#ifdef IPPPP
    if (ipnet->ppp.if_attach == IP_NULL
        && (ipcomif->type == IP_IFT_PPP || ipcomif->type == IP_IFT_PPPOE))
        /* IPNET built for IPPPP support but IPPPP is not installed */
        return -IP_ERRNO_EINVAL;
#endif

#ifndef IPNET_SKIP_IFNAME_CHECK_ON_ATTACH
    if (ipnet_if_nametonetif(netif->vr_index, netif->ipcom.name) != IP_NULL)
    {
        /* Name already exist */
        ret = -IP_ERRNO_EEXIST;
        goto cleanup;
    }
#endif

    if (netif->ipcom.ifindex != 0)
    {
        /* Caller has specified interface index */
        if (ipcom_hash_get(ipnet->netifs, &netif->ipcom.ifindex) != IP_NULL)
        {
            /* Index already occupied */
            ret = -IP_ERRNO_EEXIST;
            goto cleanup;
        }
    }
    else
    {
        /* Grab first available index */
        do {
            ifindex = ++ipnet->next_ifindex;
        } while (ifindex == 0 || ipcom_hash_get(ipnet->netifs, &ifindex) != IP_NULL);

        netif->ipcom.ifindex = (Ip_u32)ifindex;
    }

    /* Add the interface */
    if (ipcom_hash_add(ipnet->netifs, netif) != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "Out of memory when attaching %s", ipcomif->name);
        ret = -IP_ERRNO_ENOMEM;
        goto cleanup;
    }

#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    (void)ipcom_hash_add(ipnet->ifname_tab, netif);
#endif

#ifdef IPPPP
    if (ipcomif->type == IP_IFT_PPP || ipcomif->type == IP_IFT_PPPOE)
    {
        ip_assert(ipnet->ppp.if_attach != IP_NULL);
        (void)ipnet->ppp.if_attach(ipcomif);
    }
#endif

    if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6,
                                         netif,
                                         "AcceptRtAdv",
                                         IP_TRUE,
                                         ipnet_bool_map))
        IP_BIT_SET(netif->flags2, IPNET_IFF2_ADV_ACCEPT);
    else
        IP_BIT_CLR(netif->flags2, IPNET_IFF2_ADV_ACCEPT);

    IPNET_NETLINKSOCK(ipnet_rtnetlink_link_add(netif));
    IPNET_ROUTESOCK(ipnet_routesock_if_announce(netif, IPNET_IFAN_ARRIVAL));
    IPCOM_LOG2(NOTICE, "Attached network interface %s, ifindex is %u", ipcomif->name, netif->ipcom.ifindex);

#ifdef IP_PORT_LKM
    if (ipcomif->type == IP_IFT_LOOP)
        ret = ipnet_loopback_if_setup(netif);
#endif


#ifdef IPCOM_USE_MIB2_NEW
    ipnet_if_update_mib2_lastchange();
#endif

 cleanup:
    if (ret < 0)
        return ret;

    return netif->ipcom.ifindex;
}


/*
 *===========================================================================
 *                    ipnet_sys_if_detach
 *===========================================================================
 * Description: Detach a network interface with IPNET.
 * Parameters:  netif - The interface to detach.
 * Returns:     The interface index for this interface or negative errno.
 *
 */
IP_GLOBAL int
ipnet_sys_if_detach(Ipcom_netif *ipcomif)
{
    int          ret;
    Ipnet_netif *netif = (Ipnet_netif *)ipcomif;

    ip_assert(netif);
    ip_assert(netif->ipcom.ifindex > 0);

    IPNET_IF_LOCK(netif);

    /* Must bring an interface and its driver down before detaching it. */
    if (ipnet_if_can_detach(ipcomif))
        ret = ipnet_if_detach(netif);
    else
        ret = -IP_ERRNO_EBUSY;

#ifdef IPCOM_USE_MIB2_NEW
    if (ret >= 0)
        ipnet_if_update_mib2_lastchange();
#endif

    IPNET_IF_UNLOCK(netif);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sys_if_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_sys_if_free(Ipcom_netif *ipcomif)
{
    int ret = -IP_ERRNO_EBUSY;

    if (ipnet_if_can_detach(ipcomif))
    {
        IPNET_IF_UNLOCK((Ipnet_netif *)ipcomif);
        ret = 0;
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sys_if_malloc
 *===========================================================================
 * Description: Creates a new uninitialized network interface structure.
 * Parameters:  type - One of the IP_IFT_xxx types.
 * Returns:     The network interface structure of IP_NULL if out of memory.
 *
 */
IP_PUBLIC Ipcom_netif *
ipnet_sys_if_malloc(Ip_u16 type)
{
    Ipnet_netif *netif;
    int          i;
    int          err;

    err = -1;

    netif = ipcom_calloc(1, sizeof(Ipnet_netif));
    if (netif != IP_NULL)
    {
        netif->ipcom.metric = 1;
        netif->ipcom.type   = type;
        netif->vr_index     = ipnet_get_effective_vr();
        ipcom_atomic_set(&netif->ipcom.ref_count, 0);

#ifdef IPCOM_USE_FORWARDER
	netif->ipcom.hw_ix = -1;
#endif
        for (i = 0; ipnet_conf_link_layer[i].type != 0; i++)
        {
            if (ipnet_conf_link_layer[i].type == type)
            {
                err = ipnet_conf_link_layer[i].init_func(netif);
                break;
            }
        }
        if (err < 0)
            goto errout;

#ifdef IPNET_FASTPATH
        /* Make sure that the one-behind forwarding cache points to
           valid memory. It does not matter if the actual thing it
           points to is stale or not. */
#ifdef IPCOM_USE_INET
        netif->inet4_last_forward_cache = ipnet->ip4.forward_cache;
#endif
#ifdef IPCOM_USE_INET6
        netif->inet6_last_forward_cache = ipnet->ip6.forward_cache;
#endif
#endif /* IPNET_FASTPATH */
    }

    return (Ipcom_netif *)netif;

errout:
    if (netif != IP_NULL)
        ipcom_free(netif);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_sys_if_changename
 *===========================================================================
 * Description: Change the name of a network interface.
 * Parameters:  ipcomif - The interface for which the name has changed.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_if_changename(Ipcom_netif *ipcomif, const char *new_name)
{
#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    Ip_err err;

    err = ipcom_hash_remove(ipnet->ifname_tab, ipcomif);
#endif  /* IPNET_USE_FAST_IFNAME_LOOKUP */
    ipcom_strncpy(ipcomif->name, new_name, IP_IFNAMSIZ);
#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    if (err == IPCOM_SUCCESS)
        (void)ipcom_hash_add(ipnet->ifname_tab, ipcomif);
#endif  /* IPNET_USE_FAST_IFNAME_LOOKUP */
}


/*
 *===========================================================================
 *                    ipnet_sys_if_changemtu
 *===========================================================================
 * Description: Change the mtu of a network interface.
 * Parameters:  ipcomif - The interface for which the mtu has changed.
 *              new_mtu - new_mtu
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_if_changemtu(Ipcom_netif *ipcomif, Ip_u32 new_mtu)
{
    ipcomif->mtu = new_mtu;
}


/*
 *===========================================================================
 *                    ipnet_sys_if_changelinkaddr
 *===========================================================================
 * Description: Change the linkaddr of a network interface.
 * Parameters:  ipcomif - The interface for which the mtu has changed.
 *              lladdr  - New link address (byte array)
 *              size    - size of link address
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_if_changelinkaddr(Ipcom_netif *ipcomif, Ip_u8 *lladdr, Ip_u16 size)
{
    if (size == ipcomif->link_addr_size)
        ipcom_memcpy(ipcomif->link_addr, lladdr, size);
}


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_netif_has_valid_inet6_unicast_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
 IP_GLOBAL Ip_bool
 ipnet_netif_has_valid_inet6_unicast_addr(Ipnet_netif* netif)
 {
     Ipnet_ip6_addr_entry* addr_entry;
     for (addr_entry = netif->inet6_addr_list;
          addr_entry;
          addr_entry = addr_entry->next)
     {
         if (addr_entry->type == IPNET_ADDR_TYPE_UNICAST &&
             IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE | IPNET_IP6_ADDR_FLAG_DEPRECATED))
             return IP_TRUE;
     }

     return IP_FALSE;
 }
#endif /* IPCOM_USE_INET6 */

/*
 ****************************************************************************
 * 11                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_pkt_output_done
 *===========================================================================
 * Context:     Any
 * Description: Called by the driver when it has finished using a packet.
 * Parameters:  ipcomif - The interface structure matching the driver/network interface
 *              pkt - The packet that the driver no longer needs
 * Returns:     0 = success
 *
 */
IP_FASTTEXT IP_PUBLIC int
ipcom_pkt_output_done(Ipcom_netif *ipcomif, Ipcom_pkt *pkt, int fcflags)
{
    Ipnet_softirq *softirq;
    Ipnet_netif   *netif = (Ipnet_netif *) ipcomif;
    Ip_bool        fast_path = IP_FALSE;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 13, IPCOM_WV_NETDEVENT_START,
                       ipcom_pkt_output_done, IPCOM_WV_IPNET_NETIF_MODULE, IPCOM_WV_NETD_IF);
    IPNET_STATS(pkt_netif_output_done++);

    if (IP_UNLIKELY(pkt == IP_NULL))
    {
        /* pkt==IP_NULL happens if the driver wants to notify the stack that the driver has
           recovered from a transient error and packets can now be sent again */
        if (IP_BIT_ISSET(fcflags, IP_FLAG_FC_STACKCONTEXT))
            ipnet_pkt_output_done(netif, IP_NULL);
        else
            pkt = ipcom_pkt_malloc(1, fcflags);
        if (pkt == IP_NULL)
            return 0;
    }

    IP_DEBUGLINE(ip_assert(IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_FREEQ));
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
    {
        /* This function is called with the code lock held from loopback,
         * so ipnet_pkt_output_done() can be called directly
         */
        Ip_u32 zone;

        /* The zone the packet was sent from is stored in
           "pkt->ifindex". The stored zone can differ from the zone
           the loopback interface is assigned to if the packet was
           sent to one of the non-global IPv6 addresses assigned to
           this node. */
        zone = pkt->ifindex;
        ipnet_pkt_output_done(netif, pkt);
        pkt->ifindex = zone;
        return 0;
    }

    /* This line MUST be after processing of loopback handling,
       IPv6 link-local address communication will break otherwise */
    pkt->ifindex = ipcomif->ifindex;

    if (IP_BIT_ISSET(fcflags, IP_FLAG_FC_STACKCONTEXT))
    {
        ipnet_pkt_output_done(netif, pkt);
        return 0;
    }

#ifndef IP_PORT_LKM
	/* Never use fast path on the LKM port */
    fast_path = (netif->snd_head == IP_NULL
                 && netif->snd_queue == IP_NULL
                 && (pkt->fd == -1
                     || ipnet->sockets[pkt->fd] == IP_NULL
                     || ipnet->sockets[pkt->fd]->ipcom.event_cb == IP_NULL));
#endif
    if (!fast_path)
    {
        softirq = IPNET_PKT_TO_SOFTIRQ_STRUCT(pkt);
        ipnet_softirq_init(softirq);
        ipnet_softirq_schedule(softirq,
                               (Ipnet_softirq_handler) ipnet_pkt_output_done_cb,
                               pkt);
    }
#ifndef IPCOM_USE_JOB_QUEUE
    else
        ipcom_atomic_inc(&netif->snd_id);
#endif

    if (fast_path)
        ipcom_pkt_free(pkt);
    else
        IPCOM_STACKD_POST(IP_FALSE);

    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

