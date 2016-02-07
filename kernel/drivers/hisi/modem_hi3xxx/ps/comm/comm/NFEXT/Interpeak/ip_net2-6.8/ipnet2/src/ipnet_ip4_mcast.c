/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ip4_mcast.c,v $ $Revision: 1.74 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ip4_mcast.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/* IPv4 multicast routing functions.
   Based on draft-helmy-pim-sm-implem-00.txt
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

#if defined(IPCOM_USE_INET) && defined(IPNET_USE_MCAST_ROUTING)

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
#include <ipcom_sock.h>
#include <ipcom_syslog.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_ip4.h"
#include "ipnet_tunnel.h"
#include "ipnet_loopback.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_MIN_MROUTED_VERSION   0x0305
#define IPNET_MRT_ASSERT_THRESH     5000       /* 5 seconds */
#define IPNET_MRT_MAX_CACHE_QUEUE   3
#define IPNET_MRT_CACHE_TO_LIVE     10



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
 *                    ipnet_ip4_mcast_vr_obj_hash
 *===========================================================================
 * Description: Computes a hash value for a VR index.
 * Parameters:  inet_mcast - multicast data for a VR.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip4_mcast_vr_obj_hash(Ipnet_inet_mcast *inet_mcast)
{
    return ipcom_hash_update(&inet_mcast->mrouted_socket->vr_index,
                             sizeof(inet_mcast->mrouted_socket->vr_index),
                             0);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_vr_key_hash
 *===========================================================================
 * Description: Computes a hash value for a VR index.
 * Parameters:  vr - a virtual router index.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip4_mcast_vr_key_hash(Ip_u16 *vr)
{
    return ipcom_hash_update(vr, sizeof(*vr), 0);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_vr_hash_cmp
 *===========================================================================
 * Description: Returns if a multicast route database matches a VR.
 * Parameters:  inet_mcast - A multicast route database.
 *              vr - A VR.
 * Returns:     IP_TRUE if 'inet_mcast' is assigned to the specified VR.
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_mcast_vr_hash_cmp(Ipnet_inet_mcast *inet_mcast, Ip_u16 *vr)
{
    return inet_mcast->mrouted_socket->vr_index == *vr;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_rt_obj_hash
 *===========================================================================
 * Description: Computes a hash value for the specified multicast route entry.
 * Parameters:  mrt - A multicast route entry.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip4_mcast_rt_obj_hash(Ipnet_mrt *mrt)
{
    return ipcom_hash_update(&mrt->key, sizeof(mrt->key), 0);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_rt_key_hash
 *===========================================================================
 * Description: Computes a hash value for the specified multicast route
 *              search key.
 * Parameters:  key - A multicast route search key.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip4_mcast_rt_key_hash(Ipnet_mrt_key *key)
{
    return ipcom_hash_update(key, sizeof(*key), 0);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_rt_hash_cmp
 *===========================================================================
 * Description: Compares a route entry with a route
 * Parameters:  mrt - A multicast route entry
 *              key - A multicast route entry search key.
 * Returns:     IP_TRUE if the multicast route entry matches the search key.
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_mcast_rt_hash_cmp(Ipnet_mrt *mrt, Ipnet_mrt_key *key)
{
    return ipcom_memcmp(&mrt->key, key, sizeof(mrt->key)) == 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_del_mrt
 *===========================================================================
 * Description: Deletes a cached route entry that never got valid.
 * Parameters:  mrt - The multicast route entry to free.
 *              is_in_cached_list - Set to IP_TRUE if the entry is in the
 *              list of cached entries.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_mcast_del_mrt(Ipnet_mrt *mrt, Ip_bool is_in_cached_list)
{
    Ipcom_pkt *del_pkt;

    if (is_in_cached_list)
    {
        ipnet_timeout_cancel(mrt->tmo);
        ipcom_list_remove(&mrt->list_head);
    }

    while (IP_NULL != (del_pkt = mrt->pending_pkt_head))
    {
        mrt->pending_pkt_head = del_pkt->next;
        ipcom_pkt_free(del_pkt);
    }
    ipcom_free(mrt);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_hash_del_mrt
 *===========================================================================
 * Description: Deletes the multicast route table passed.
 * Parameters:  mfc - The multicast route entry to free.
 *              inet_mcast_rt_tab - The hash table the entry belongs to.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_mcast_hash_del_mrt(Ipnet_mrt *mrt, Ipcom_hash *inet_mcast_rt_tab)
{
    (void)ipcom_hash_remove(inet_mcast_rt_tab, mrt);
    ipnet_ip4_mcast_del_mrt(mrt, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_get_cached_mrt
 *===========================================================================
 * Description: Returns the cached route matching the specified key.
 *              The entry is removed from the cache list if found.
 * Parameters:  inet_mcast - The multicast database for this virtual router.
 *              key - The search key for the route.
 * Returns:
 *
 */
IP_STATIC Ipnet_mrt *
ipnet_ip4_mcast_get_cached_mrt(Ipnet_inet_mcast *inet_mcast, Ipnet_mrt_key *key)
{
    Ipnet_mrt *mrt;

    for (mrt = (Ipnet_mrt *) ipcom_list_first(&inet_mcast->cached_mrts);
         mrt != IP_NULL;
         mrt = (Ipnet_mrt *) ipcom_list_next(&mrt->list_head))
    {
        if (ipcom_memcmp(&mrt->key, key, sizeof(mrt->key)) == 0)
            return mrt;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_mrt_init
 *===========================================================================
 * Description: Initializes the multicast forwarding engine.
 * Parameters:  sock - The socket to be used as the multicast routing control
 *                     socket.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip4_mcast_mrt_init(Ipnet_socket *sock)
{
    Ipnet_inet_mcast *inet_mcast;

    if (sock->ipcom.domain != IP_AF_INET
        || sock->ipcom.type != IP_SOCK_RAW
        || sock->proto != IP_IPPROTO_IGMP)
        return IPNET_ERRNO(EOPNOTSUPP);

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EADDRINUSE);

    if (ipnet->ip4_mcast == IP_NULL)
    {
        ipnet->ip4_mcast =
            ipcom_hash_new((Ipcom_hash_obj_func) ipnet_ip4_mcast_vr_obj_hash,
                           (Ipcom_hash_key_func) ipnet_ip4_mcast_vr_key_hash,
                           (Ipcom_hash_cmp_func) ipnet_ip4_mcast_vr_hash_cmp);
        if (ipnet->ip4_mcast == IP_NULL)
            return IPNET_ERRNO(ENOMEM);
    }

    inet_mcast = ipcom_calloc(1, sizeof(Ipnet_inet_mcast));
    if (inet_mcast == IP_NULL)
        goto errout;

    inet_mcast->rt_tab =
        ipcom_hash_new((Ipcom_hash_obj_func) ipnet_ip4_mcast_rt_obj_hash,
                       (Ipcom_hash_key_func) ipnet_ip4_mcast_rt_key_hash,
                       (Ipcom_hash_cmp_func) ipnet_ip4_mcast_rt_hash_cmp);

    if (inet_mcast->rt_tab == IP_NULL)
        goto errout;

    inet_mcast->register_vif   = -1;
    inet_mcast->assert_report  = IP_FALSE;
    inet_mcast->mrouted_socket = sock;
    ipcom_list_init(&inet_mcast->cached_mrts);
    if (ipcom_hash_add(ipnet->ip4_mcast, inet_mcast) != IPCOM_SUCCESS)
        goto errout;

    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL);
    return 0;

 errout:
    if (inet_mcast != IP_NULL && inet_mcast->rt_tab != IP_NULL)
        ipcom_hash_delete(inet_mcast->rt_tab);
    ipcom_free(inet_mcast);
    return IPNET_ERRNO(ENOMEM);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_netiftovif
 *===========================================================================
 * Description: Returns the virtual interface matching the specfied net if.
 * Parameters:  inet_mcast - The multicast database for this virtual router.
 *              netif - A network interface.
 * Returns:     The virtual interface or IP_NULL if no match was found.
 *
 */
IP_STATIC Ipnet_vif *
ipnet_ip4_mcast_netiftovif(Ipnet_inet_mcast *inet_mcast, Ipnet_netif *netif)
{
    int        i;
    Ipnet_vif *vif;

    if (netif == IP_NULL)
    {
        if (inet_mcast->register_vif < 0)
            return IP_NULL;
        return inet_mcast->vifs[inet_mcast->register_vif];
    }

    for (i = 0; i < IP_MAXVIFS; i++)
    {
        vif = inet_mcast->vifs[i];
        if (vif != IP_NULL && vif->ifindex == (Ip_vifi_t) netif->ipcom.ifindex)
            return vif;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_mrt_report
 *===========================================================================
 * Description: Sends notification to mrouted.
 * Parameters:  inet_mcast - The multicast database for this virtual router.
 *              vif - The virtual interface affected.
 *              pkt - The packet that caused the notification.
 *              igmpmsg - The notification to send.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_mcast_mrt_report(Ipnet_inet_mcast *inet_mcast,
                           Ipnet_vif *vif,
                           Ipcom_pkt *original,
                           Ip_u8 type)
{
    struct Ip_igmpmsg *im;
    Ipcom_pkt         *pkt;
    Ipnet_pkt_ip      *ip4hdr;
    int                msg_len;

    msg_len = (type == IP_IGMPMSG_WHOLEPKT ? original->end - original->ipstart : 0);

    pkt = ipcom_pkt_malloc(msg_len + sizeof(struct Ip_igmpmsg),
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;
    pkt->ifindex = original->ifindex;
    pkt->end     = pkt->maxlen;
    pkt->ipstart = pkt->end - msg_len;
    pkt->start   = pkt->ipstart - sizeof(struct Ip_igmpmsg);
    ipcom_memcpy(&pkt->data[pkt->ipstart], &original->data[original->ipstart], msg_len);

    im = (struct Ip_igmpmsg *) &pkt->data[pkt->start];
    im->im_msgtype = type;
    im->im_mbz     = 0;
    im->im_vif     = vif->index;

    /* Copy the source and destination address from the original packet */
    ip4hdr = (Ipnet_pkt_ip *) &original->data[original->ipstart];
    ipcom_memcpy(&im->im_src, ip4hdr->src, sizeof(im->im_src));
    ipcom_memcpy(&im->im_dst, ip4hdr->dst, sizeof(im->im_dst));

    (void) ipnet_queue_received_packet(pkt, inet_mcast->mrouted_socket);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_mrt_timeout
 *===========================================================================
 * Description: Timeout handler for cached multicast routes.
 * Parameters:  mrt - The cached multicast route entry that timed out.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_mcast_mrt_timeout(Ipnet_mrt *mrt)
{
    ipnet_ip4_mcast_del_mrt(mrt, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_cache_unresolved
 *===========================================================================
 * Description: Caches a multicast route for a packet for which no entry
 *              existed.
 * Parameters:  inet_mcast - The multicast database for this virtual router.
 *              vif - The virtual interface affected.
 *              original - The packet that did not match any multicast route.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_mcast_cache_unresolved(Ipnet_inet_mcast *inet_mcast,
                                 Ipnet_vif *vif,
                                 Ipcom_pkt *original)
{
    Ipnet_pkt_ip *ip4hdr = (Ipnet_pkt_ip *) &original->data[original->ipstart];
    Ipnet_mrt_key key;
    Ipnet_mrt    *mrt;
    Ipcom_pkt    *pkt;

    key.origin.s_addr   = IP_GET_32ON16(ip4hdr->src);
    key.mcastgrp.s_addr = IP_GET_32ON16(ip4hdr->dst);

    mrt = ipnet_ip4_mcast_get_cached_mrt(inet_mcast, &key);
    if (mrt == IP_NULL)
    {
        /* No matching route entry and not cached */
        ipnet_ip4_mcast_mrt_report(inet_mcast, vif, original, IP_IGMPMSG_NOCACHE);

        mrt = ipcom_calloc(1, sizeof(Ipnet_mrt));
        if (mrt == IP_NULL
            || ipnet_timeout_schedule(IPNET_MRT_CACHE_TO_LIVE * 1000,
                                      (Ipnet_timeout_handler) ipnet_ip4_mcast_mrt_timeout,
                                      mrt,
                                      &mrt->tmo) != 0)
        {
            ipcom_free(mrt);
            return;
        }
        mrt->key         = key;
        ipcom_list_insert_last(&inet_mcast->cached_mrts, &mrt->list_head);
    }

    if (mrt->num_pending_pkt >= IPNET_MRT_MAX_CACHE_QUEUE
        || IP_NULL == (pkt = ipnet_pkt_clone(original, IP_TRUE)))
        return;

    mrt->num_pending_pkt++;
    if (mrt->pending_pkt_tail)
        mrt->pending_pkt_tail->next = pkt;
    else
        mrt->pending_pkt_head = pkt;
    mrt->pending_pkt_tail = pkt;
    pkt->next = IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_output
 *===========================================================================
 * Description: Outputs a packet to the specified virtual interface.
 * Parameters:  inet_mcast - The multicast database for this virtual router.
 *              vif - The virtual interface to send the packet through.
 *              mrt - The multicast route this packet matched.
 *              original - The packet to output.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_mcast_output(Ipnet_inet_mcast *inet_mcast,
                       Ipnet_vif *vif,
                       Ipnet_mrt *mrt,
                       Ipcom_pkt *original)
{
    Ipcom_pkt             *pkt;
    Ipnet_pkt_ip          *ip4hdr;
    Ipnet_ip4_output_param param;
    Ipnet_netif           *netif;

    if (IP_BIT_ISSET(vif->flags, IP_VIFF_REGISTER))
    {
        ipnet_ip4_mcast_mrt_report(inet_mcast, vif, original, IP_IGMPMSG_WHOLEPKT);
        return;
    }

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, vif->ifindex);
    if (netif == IP_NULL)
    {
        IPCOM_LOG2(WARNING, "IPv4 mcast: physical interface %d mapping"
                   "against virtual interface %d has been detached",
                   vif->ifindex, vif->index);
        return;
    }

    ip4hdr = (Ipnet_pkt_ip *) &original->data[original->ipstart];
    if (ip4hdr->ttl <= mrt->ttls[vif->index])
        /* Too low time to live */
        return;

    pkt = ipnet_pkt_clone(original, IP_TRUE);
    if (pkt == IP_NULL)
        return;

    vif->bytes_out += original->end - original->ipstart;
    vif->pkts_out++;

    pkt->start = pkt->ipstart;
    IP_BIT_SET(pkt->flags, (IPCOM_PKT_FLAG_HAS_IP_HDR
                            | IPCOM_PKT_FLAG_FORWARDED
                            | IPCOM_PKT_FLAG_NONBLOCKING
                            | IPCOM_PKT_FLAG_MULTICAST));

    if (IP_BIT_ISFALSE(vif->flags, IP_VIFF_TUNNEL))
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST);

    ip4hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    ip4hdr->ttl -= mrt->ttls[vif->index];

    ipcom_memset(&param, 0, sizeof(param));
    param.netif = netif;
    param.from  = &mrt->key.origin;
    param.to    = &mrt->key.mcastgrp;
    param.proto = ip4hdr->p;
    param.tos   = ip4hdr->tos;
    param.ttl   = ip4hdr->ttl;
    if (IP_BIT_ISSET(ip4hdr->off, IPNET_OFF_DF))
        IP_BIT_SET(param.flags, IPNET_IP4_OPF_DONT_FRAG);

    ip4hdr->sum = 0;
    ip4hdr->sum = ipcom_in_checksum((Ip_u16 *)ip4hdr, IPNET_IP4_GET_HDR_LEN(ip4hdr) * sizeof(Ip_u32));
    (void) ipnet_ip4_output(&param, pkt);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_add_vif
 *===========================================================================
 * Description: Adds a new virtual interface.
 * Parameters:  sock - The mrouted socket.
 *              vifctl - Paramters for the new virtual interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_mcast_add_vif(Ipnet_socket *sock, struct Ip_vifctl *vifctl)
{
    Ipnet_vif                *vif;
    Ipnet_netif              *netif = IP_NULL;
    struct Ip_ip_tunnel_param tunnel_param;
    int                       ret;
    Ipnet_inet_mcast         *inet_mcast;

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    if (inet_mcast->vifs[vifctl->vifc_vifi] != IP_NULL)
        /* The VIF index selected by mrouted is already in use */
        return IPNET_ERRNO(EADDRINUSE);

#ifdef IPNET_USE_TUNNEL
    if (IP_BIT_ISSET(vifctl->vifc_flags, IP_VIFF_TUNNEL))
    {
        int i = 0;

        ipcom_memset(&tunnel_param, 0, sizeof(tunnel_param));
        do
        {
            ipcom_sprintf(tunnel_param.ifr_name, "gifmcast%d", i);
        } while (ipnet_if_nametonetif(sock->vr_index, tunnel_param.ifr_name));
        tunnel_param.local_addr.sin.sin_family  = IP_AF_INET;
        tunnel_param.local_addr.sin.sin_addr    = vifctl->vifc_lcl_addr;
        tunnel_param.remote_addr.sin.sin_family = IP_AF_INET;
        tunnel_param.remote_addr.sin.sin_addr   = vifctl->vifc_rmt_addr;
        ret = ipnet_tunnel_add(sock, tunnel_param.ifr_name, IP_ARPHRD_TUNNEL, &netif);
        if (ret < 0)
            goto cleanup;
        /*
        netif = ipnet_if_indextonetif(sock->vr_index, ret);
        */
        ret = netif->link_ioctl(netif, IP_SIOCCHGTUNNEL, &tunnel_param);
        if (ret < 0)
            goto cleanup;
        ret = netif->link_ioctl(netif, IP_SIOCXOPEN, IP_NULL);
        if (ret < 0)
            goto cleanup;
        ip_assert(netif != IP_NULL);
    }
    else
#endif /* IPNET_USE_TUNNEL */
    if (IP_BIT_ISSET(vifctl->vifc_flags, IP_VIFF_REGISTER))
    {
        if (inet_mcast->register_vif >= 0)
            return IPNET_ERRNO(EADDRINUSE);
    }
    else
    {
        Ip_u32            i;
        struct Ip_in_addr inaddr;

        IPNET_NETIF_FOR_EACH_ON_VR(netif, sock->vr_index, i)
        {
            if (netif->ipcom.type == IP_IFT_TUNNEL)
                continue;

            if (ipnet_ip4_get_addr_type2(vifctl->vifc_lcl_addr.s_addr, netif)
                == IPNET_ADDR_TYPE_UNICAST)
                break;
        }

        if (netif == IP_NULL)
            /* No interface found that matches the specified address */
            return IPNET_ERRNO(EADDRNOTAVAIL);

        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_MULTICAST) == IP_FALSE)
            return IPNET_ERRNO(EOPNOTSUPP);

        /* Enable promiscuous reception of all IP multicasts from the interface */
        inaddr.s_addr = IP_INADDR_ANY;
        ret = netif->link_ioctl(netif, IP_SIOCXADDMULTI_IN, &inaddr);
        if (ret < 0)
            return ret;
    }

    vif = ipcom_calloc(1, sizeof(Ipnet_vif));
    if (vif == IP_NULL)
    {
        ret = -IP_ERRNO_ENOMEM;
        goto cleanup;
    }

    if (netif != IP_NULL)
        vif->ifindex    = netif->ipcom.ifindex;
    vif->threshhold = vifctl->vifc_threshold;
    vif->flags      = vifctl->vifc_flags;
    vif->index      = vifctl->vifc_vifi;
    vif->local      = vifctl->vifc_lcl_addr;
    if (IP_BIT_ISSET(vif->flags, IP_VIFF_TUNNEL))
        vif->peer = vifctl->vifc_rmt_addr;
    else if (IP_BIT_ISSET(vifctl->vifc_flags, IP_VIFF_REGISTER))
        inet_mcast->register_vif = vifctl->vifc_vifi;

    inet_mcast->vifs[vifctl->vifc_vifi] = vif;

    return 0;

cleanup:
    if (netif != IP_NULL && netif->ipcom.type == IP_IFT_TUNNEL)
        (void) netif->link_ioctl(netif, IP_SIOCIFDESTROY, &tunnel_param);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_del_vif
 *===========================================================================
 * Description: Deletes the specifed virtual interface.
 * Parameters:  sock - The mrouted socket.
 *              vifi - The index of the virtual interface to delete.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_mcast_del_vif(Ipnet_socket *sock, Ip_vifi_t vifi)
{
    struct Ip_in_addr         inaddr;
    Ipnet_vif                *vif;
    struct Ip_ip_tunnel_param tunnel_param;
    Ipnet_netif              *netif;
    Ipnet_inet_mcast         *inet_mcast;

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    vif = inet_mcast->vifs[vifi];

    if (vif == IP_NULL)
        return IPNET_ERRNO(EADDRNOTAVAIL);

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, vif->ifindex);
    if (netif != IP_NULL)
    {

        if (IP_BIT_ISSET(vif->flags, IP_VIFF_TUNNEL))
        {
            ipcom_memset(&tunnel_param, 0, sizeof(tunnel_param));
            ipcom_strncpy(tunnel_param.ifr_name, netif->ipcom.name, IP_IFNAMSIZ);
            (void) netif->link_ioctl(netif, IP_SIOCXCLOSE, IP_NULL);
            (void) netif->link_ioctl(netif, IP_SIOCIFDESTROY, &tunnel_param);
        }
        else if (IP_BIT_ISSET(vif->flags, IP_VIFF_REGISTER))
            inet_mcast->register_vif = -1;
        else
        {
            /* Using a physcal and multicast capable interface.
               Disable promiscuous reception of all IP multicasts from the interface */
            inaddr.s_addr = IP_INADDR_ANY;
            (void) netif->link_ioctl(netif, IP_SIOCXDELMULTI_IN, &inaddr);
        }
    }
    ipcom_free(vif);
    inet_mcast->vifs[vifi] = IP_NULL;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_add_mfc
 *===========================================================================
 * Description: Adds a multicast route.
 * Parameters:  sock - The mrouted socket.
 *              mfcctl - route paramters.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_mcast_add_mfc(Ipnet_socket *sock, struct Ip_mfcctl *mfcctl)
{
    Ipnet_mrt        *mrt;
    Ipnet_mrt_key     key;
    int               i;
    Ipnet_inet_mcast *inet_mcast;

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    key.origin   = mfcctl->mfcc_origin;
    key.mcastgrp = mfcctl->mfcc_mcastgrp;

    if (IP_IN_CLASSD(key.origin.s_addr))
        return IPNET_ERRNO(EINVAL);

    mrt = ipcom_hash_get(inet_mcast->rt_tab, &key);

    if (mrt == IP_NULL)
    {
        /* Create a new entry */
        mrt = ipnet_ip4_mcast_get_cached_mrt(inet_mcast, &key);
        if (mrt)
        {
            ipnet_timeout_cancel(mrt->tmo);
            ipcom_list_remove(&mrt->list_head);
        }
        else
        {
            mrt = ipcom_calloc(1, sizeof(Ipnet_mrt));
            if (mrt == IP_NULL)
                return IPNET_ERRNO(ENOMEM);
            ipcom_memcpy(&mrt->key, &key, sizeof(mrt->key));
        }

        if (ipcom_hash_add(inet_mcast->rt_tab, mrt) != IPCOM_SUCCESS)
        {
            ipnet_ip4_mcast_del_mrt(mrt, IP_FALSE);
            return IPNET_ERRNO(ENOMEM);
        }
    }
    /* else: update the current entry */

    mrt->parent     = mfcctl->mfcc_parent;
    mrt->vifs_count = 0;
    for (i = 0; i < IP_MAXVIFS; i++)
    {
        mrt->ttls[i] = mfcctl->mfcc_ttls[i];
        if (mrt->ttls[i] && mrt->ttls[i] < 255 && inet_mcast->vifs[i])
            mrt->vifs[mrt->vifs_count++] = (Ip_vifi_t) i;
    }

    /* Send all cached entries */
    while (mrt->pending_pkt_head)
    {
        Ipnet_netif *netif;
        Ipcom_pkt   *pkt;

        pkt = mrt->pending_pkt_head;
        mrt->pending_pkt_head = pkt->next;
        pkt->next = IP_NULL;
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING);
        netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);
        if (netif || pkt->ifindex == 0)
            ipnet_ip4_mcast_input(netif, pkt);
        ipcom_pkt_free(pkt);
    }
    mrt->pending_pkt_tail = IP_NULL;
    mrt->num_pending_pkt = 0;

    return 0; /*lint !e429 */
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_del_mfc
 *===========================================================================
 * Description: Deletes a multicast route.
 * Parameters:  sock - The mrouted socket.
 *              mfcctl - The route paramters.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_mcast_del_mfc(Ipnet_socket *sock, struct Ip_mfcctl *mfcctl)
{
    Ipnet_mrt        *mrt;
    Ipnet_mrt_key     key;
    Ipnet_inet_mcast *inet_mcast;

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    key.origin   = mfcctl->mfcc_origin;
    key.mcastgrp = mfcctl->mfcc_mcastgrp;

    mrt = ipcom_hash_get(inet_mcast->rt_tab, &key);
    if (mrt == IP_NULL)
        return IPNET_ERRNO(ESRCH);

    ipnet_ip4_mcast_hash_del_mrt(mrt, inet_mcast->rt_tab);
    return 0;
}


/*
 *===========================================================================
 *                  ipnet_ip4_mcast_set_assert
 *===========================================================================
 * Description: Enables/disabls the assert report function
 * Parameters:  sock - The mrouted socket.
 *              val - IP_TRUE to enable, IP_FALSE to disable.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip4_mcast_set_assert(Ipnet_socket *sock, Ip_bool val)
{
    Ipnet_inet_mcast *inet_mcast;

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    inet_mcast->assert_report = val;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_mrt_done
 *===========================================================================
 * Description: Frees all resources allocated by the multicast module.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_mcast_mrt_done(Ipnet_socket *sock)
{
    Ipnet_inet_mcast *inet_mcast;
    Ip_vifi_t         vifi;

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    /* Free all cached entries */
    while (inet_mcast->cached_mrts.size > 0)
    {
        Ipnet_mrt *mrt = (Ipnet_mrt *) ipcom_list_first(&inet_mcast->cached_mrts);
        ipnet_ip4_mcast_del_mrt(mrt, IP_TRUE);
    }

    /* Free all multicast route entries */
    ipcom_hash_for_each(inet_mcast->rt_tab,
                        (void(*)(void *, void *)) ipnet_ip4_mcast_hash_del_mrt,
                        inet_mcast->rt_tab);
    ipcom_hash_delete(inet_mcast->rt_tab);

    /* Detach and free all VIF:s */
    for (vifi = 0; vifi < IP_MAXVIFS; vifi++)
        (void) ipnet_ip4_mcast_del_vif(sock, vifi);

    (void)ipcom_hash_remove(ipnet->ip4_mcast, inet_mcast);
    ipcom_free(inet_mcast);

    IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_getsockopt
 *===========================================================================
 * Description: Socket option where level was set to IP_IPPROTO_IP.
 * Parameters:  [in] sock - A socket.
 *              [in] optname - The socket option to set.
 *              [out] optval - Buffer where the option data should be stored.
 *              [in, out] optlen - The length of 'optval', number of bytes
 *              copied to 'optval' when the function returns.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_mcast_getsockopt(Ipnet_socket *sock, int optname, void *optval,
                           Ip_socklen_t *optlen)
{
    int               val;
    Ipnet_inet_mcast *inet_mcast;

    if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EACCES);

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    switch (optname)
    {
    case IP_MRT_VERSION:
        val = IPNET_MIN_MROUTED_VERSION;
        break;

    case IP_MRT_ASSERT:
        val = inet_mcast->assert_report;
        break;

    default:
        return IPNET_ERRNO(EOPNOTSUPP);
    }

    if (*optlen < sizeof(val))
        return IPNET_ERRNO(EINVAL);
    *(int*) optval = val;
    *optlen = sizeof(val);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_setsockopt
 *===========================================================================
 * Description: Socket option where level was set to IP_IPPROTO_IP.
 * Parameters:  [in] sock - A socket.
 *              [in] optname - The socket option to set.
 *              [in] optval - The option data to set.
 *              [in] optlen - The length of 'optval'
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_mcast_setsockopt(Ipnet_socket *sock, int optname, IP_CONST void *optval,
                           Ip_socklen_t optlen)
{
    struct Ip_vifctl *vifctl;

    if (optname == IP_MRT_INIT)
        return ipnet_ip4_mcast_mrt_init(sock);

    if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EACCES);

    switch (optname)
    {
    case IP_MRT_DONE:
        return ipnet_ip4_mcast_mrt_done(sock);

    case IP_MRT_ADD_VIF:
    case IP_MRT_DEL_VIF:
        if (optlen != sizeof(struct Ip_vifctl))
            return IPNET_ERRNO(EINVAL);

        vifctl = (struct Ip_vifctl *) optval;

        if (vifctl->vifc_vifi >= IP_MAXVIFS)
            return IPNET_ERRNO(EINVAL);

        if (optname == IP_MRT_ADD_VIF)
            return ipnet_ip4_mcast_add_vif(sock, vifctl);
        return ipnet_ip4_mcast_del_vif(sock, vifctl->vifc_vifi);

    case IP_MRT_ADD_MFC:
    case IP_MRT_DEL_MFC:
        if (optlen != sizeof(struct Ip_mfcctl))
            return IPNET_ERRNO(EINVAL);

        if (optname == IP_MRT_ADD_MFC)
            return ipnet_ip4_mcast_add_mfc(sock, (struct Ip_mfcctl *) optval);
        return ipnet_ip4_mcast_del_mfc(sock, (struct Ip_mfcctl *) optval);

    case IP_MRT_ASSERT:
        if (optlen != sizeof(int))
            return IPNET_ERRNO(EINVAL);

        ipnet_ip4_mcast_set_assert(sock, *(int *) optval != 0);
        break;

    default:
        return IPNET_ERRNO(EOPNOTSUPP);
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_ioctl
 *===========================================================================
 * Description: IO control for IPv4 multicast.
 * Parameters:  sock - The mrouted socket.
 *              [in] request - The reqeust id.
 *              [in, out] data - The reqeust data.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int ipnet_ip4_mcast_ioctl(Ipnet_socket *sock,
                                    unsigned long request,
                                    void *data)
{
    struct Ip_sioc_vif_req *vif_req;
    Ipnet_vif              *vif;
    struct Ip_sioc_sg_req  *sg_req;
    Ipnet_mrt              *mrt;
    Ipnet_inet_mcast       *inet_mcast;

    if (sock->ipcom.domain != IP_AF_INET
        || IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EOPNOTSUPP);

    inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &sock->vr_index);
    ip_assert(inet_mcast != IP_NULL);

    switch (request)
    {
    case IP_SIOCGETVIFCNT:
        vif_req = (struct Ip_sioc_vif_req *) data;
        if (vif_req->vifi >= IP_MAXVIFS)
            return IPNET_ERRNO(EINVAL);

        vif = inet_mcast->vifs[vif_req->vifi];
        if (vif == IP_NULL)
            return IPNET_ERRNO(EADDRNOTAVAIL);

        vif_req->ibytes = vif->bytes_in;
        vif_req->icount = vif->pkts_in;
        vif_req->obytes = vif->bytes_out;
        vif_req->ocount = vif->pkts_out;
        break;

    case IP_SIOCGETSGCNT:
        sg_req = (struct Ip_sioc_sg_req *) data;
        mrt = (Ipnet_mrt *) ipcom_hash_get(inet_mcast->rt_tab, (Ipnet_mrt_key *) &sg_req->src);

        if (mrt == IP_NULL)
            return IPNET_ERRNO(ESRCH);

        sg_req->bytecnt  = mrt->bytes;
        sg_req->pktcnt   = mrt->pkt;
        sg_req->wrong_if = mrt->wrong_if;
        break;

    default:
        return IPNET_ERRNO(EINVAL);
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_mcast_input
 *===========================================================================
 * Description: Processes the multicast packet, the caller will retain
 *              the ownership of the packet. pkt->start is the offset to the
 *              transport layer.
 * Parameters:  netif - The interface the packet was received or IP_NULL
 *              to represent the PIM register interface.
 *              pkt - The packet to process.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip4_mcast_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip     *iphdr;
    Ipnet_mrt_key    *key;
    Ipnet_mrt        *mrt;
    Ipnet_vif        *vif;
    int               i;
    Ipnet_inet_mcast *inet_mcast = IP_NULL;

    if (ipnet->ip4_mcast != IP_NULL)
        inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &pkt->vr_index);

    if (inet_mcast == IP_NULL
        || (netif != IP_NULL && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)))
        /* No mrouted running or looped back packet, ignore packet */
        return;

    iphdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    if (iphdr->p == IP_IPPROTO_IGMP)
        /* Do not forward IGMP packets */
        return;

    key = (Ipnet_mrt_key *) iphdr->src;

    mrt = ipcom_hash_get(inet_mcast->rt_tab, key);
    if (mrt == IP_NULL)
    {
        /* No route entry for this packet */
        vif = ipnet_ip4_mcast_netiftovif(inet_mcast, netif);
        if (vif != IP_NULL)
            ipnet_ip4_mcast_cache_unresolved(inet_mcast, vif, pkt);
        return;
    }

    mrt->pkt++;
    mrt->bytes += pkt->end - pkt->ipstart;

    vif = inet_mcast->vifs[mrt->parent];
    if (vif == IP_NULL
        || (netif != IP_NULL && vif->ifindex != (Ip_vifi_t) netif->ipcom.ifindex))
    {
        /* Wrong interface */
        mrt->wrong_if++;
        vif = ipnet_ip4_mcast_netiftovif(inet_mcast, netif);
        if (vif != IP_NULL
            && inet_mcast->assert_report
            && IPCOM_IS_GT(ipnet_msec_now(), inet_mcast->last_assert_report + IPNET_MRT_ASSERT_THRESH))
        {
            inet_mcast->last_assert_report = ipnet_msec_now();
            ipnet_ip4_mcast_mrt_report(inet_mcast, vif, pkt, IP_IGMPMSG_WRONGVIF);
        }
        return;
    }

    vif->bytes_in += pkt->end - pkt->ipstart;
    vif->pkts_in++;

    for (i = 0; i < mrt->vifs_count; i++)
    {
        vif = inet_mcast->vifs[mrt->vifs[i]];
        if (vif == IP_NULL)
            /* The vif has been deleted */
            continue;
        ipnet_ip4_mcast_output(inet_mcast, vif, mrt, pkt);
    }
}


/*
 *===========================================================================
 *                    ipnet_pim_ip4_input
 *===========================================================================
 * Description: Process PIM register messages
 * Parameters:  pkt - A packet sent to the IP_IPPROTO_PIM protocol.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_pim_ip4_input(Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip     *iphdr;
    Ip_u32            src_addr_n;
    Ip_u32            dst_addr_n;
    int               original_start;
    Ipnet_inet_mcast *inet_mcast;
    Ip_pkt_pim       *pimhdr = (Ip_pkt_pim *) &pkt->data[pkt->start];

    if (pkt->end - pkt->start < (int) sizeof(Ip_pkt_pim)
        || IP_PIM_GET_VERSION(pimhdr) != IP_PIM_VERSION)
        /* Not a valid PIM message */
        goto drop_pkt;

    /* Deliver the packet to RAW sockets */
    original_start = pkt->start;
    pkt->start = pkt->ipstart;
    iphdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    src_addr_n = IPNET_IP4_GET_IPADDR(iphdr->src);
    dst_addr_n = IPNET_IP4_GET_IPADDR(iphdr->dst);
    (void) ipnet_raw_input(pkt,
                           IP_FALSE,
                           iphdr->p,
                           &src_addr_n, 0,
                           &dst_addr_n, 0,
                           (Ipnet_sock_lookup_f) ipnet_sock_ip4_lookup);

    if (IP_PIM_GET_TYPE(pimhdr) == IP_PIM_TYPE_REGISTER
        && IP_BIT_ISFALSE(IP_GET_NTOHL(&pimhdr->flags), IP_PIM_FLAG_NULL_REGISTER))
    {
        Ipnet_pkt_ip *encap_iphdr;

        if (ipnet->ip4_mcast == IP_NULL)
            inet_mcast = IP_NULL;
        else
            inet_mcast = ipcom_hash_get(ipnet->ip4_mcast, &pkt->vr_index);

        pkt->start = original_start;
        if (inet_mcast == IP_NULL
            || inet_mcast->register_vif < 0
            || (pkt->end - pkt->start) < (int) (sizeof(Ip_pkt_pim) + sizeof(Ipnet_pkt_ip))
            || (ipcom_in_checksum(pimhdr, sizeof(Ip_pkt_pim)) != 0
                && ipcom_in_checksum(pimhdr, pkt->end - pkt->start) != 0))
            /* The first checksum calculation should be enough,
               but some implementation (incorrectly) includes the encapsulated data */
            goto drop_pkt;

        encap_iphdr = (Ipnet_pkt_ip *) &pkt->data[pkt->start + sizeof(Ip_pkt_pim)];
        if (!IP_IN_CLASSD(IPNET_IP4_GET_IPADDR(encap_iphdr->dst)))
            goto drop_pkt;

        /* Move to the encapsulated multicast packet */
        pkt->ipstart = pkt->start = pkt->start + sizeof(Ip_pkt_pim);
        pkt->ifindex = 0;
        /* IP_NULL as interface means that this packet originates from a PIM register message */
        (void)ipnet_ip4_mcast_input(IP_NULL, pkt);
    }

    ipcom_pkt_free (pkt);
    return 0;

 drop_pkt:
    ipcom_pkt_free(pkt);
    return IPNET_ERRNO(EINVAL);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* defined(IPCOM_USE_INET) && defined(IPNET_USE_MCAST_ROUTING) */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

