/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ip6_mcast.c,v $ $Revision: 1.50 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ip6_mcast.c,v $
 *   $Author: kenneth $
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

#if defined(IPCOM_USE_INET6) && defined(IPNET_USE_MCAST_ROUTING)

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_ip6.h"
#include "ipnet_icmp6.h"
#include "ipnet_loopback.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_MRT6_MAX_CACHE_QUEUE   3
#define IPNET_MRT6_CACHE_TO_LIVE     10

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipnet_inet6_mcast_struct
{
    Ipnet_socket             *mrouted_socket;   /* The socket used for controlling multicast routing */
    Ipcom_hash               *rt_tab;           /* The multicast route table */
    Ipcom_list                cached_mrts;
    struct Ipnet_mif6_struct *mifs[IP_MAXMIFS]; /* Array with multicast interfaces */
    Ip_bool                   pim;              /* Enable/disable of PIM module */
    int                       register_mif;     /* Multicast interface used for encapsulated packets
                                                   in PIM register messages */
}
Ipnet_inet6_mcast;

/* Multicast interface structure */
typedef struct Ipnet_mif6_struct
{
    struct Ipnet_mif6_struct *next;
    int                 ifindex;
    Ip_u64              bytes_in;
    Ip_u64              bytes_out;  /* Number of bytes sent on this interface */
    Ip_u64              pkts_in;    /* Number of packets recieved on this interface */
    Ip_u64              pkts_out;   /* Number of packets sent on this interface */
    Ip_u8               flags;      /* IP_MIFF_xxx flags */
    Ip_mifi_t           index;      /* The interface index */
}
Ipnet_mif6;

typedef struct Ipnet_mrt6_key_struct
{
    struct Ip_in6_addr   origin;    /* origin of multicast packet */
    struct Ip_in6_addr   mcastgrp;  /* group this entry belongs to */
}
Ipnet_mrt6_key;


/* Multicast routing table entry */
typedef struct Ipnet_mrt6_struct
{
    Ipcom_list          list_head;
    Ipnet_mrt6_key      key;
    Ip_mifi_t           parent;             /* interface packets for origin/mcastgrp is expected arrive on */
    int                 mifs_count;         /* Number of entries in the 'vifs' array */
    Ip_mifi_t           mifs[IP_MAXMIFS];   /* The vif index of interfaces in this route */
    Ip_u32              bytes;              /* Number of bytes sent along this route */
    Ip_u32              pkt;                /* Number of packets sent along this route */
    Ip_u32              wrong_if;           /* Number of packets matching this route but arrived on a VIF != parent */
    struct Ipnet_timeout_struct *tmo;       /* Timeout handle != IP_NULL if a timeout is currently running */
    int                 num_pending_pkt;    /* Number of pending packets */
    Ipcom_pkt          *pending_pkt_head;   /* Packets that is queue while waiting for route add */
    Ipcom_pkt          *pending_pkt_tail;
}
Ipnet_mrt6;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
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
 *                    ipnet_ip6_mcast_vr_obj_hash
 *===========================================================================
 * Description: Computes a hash value for a VR index.
 * Parameters:  inet6_mcast - multicast data for a VR.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip6_mcast_vr_obj_hash(Ipnet_inet6_mcast *inet6_mcast)
{
    return ipcom_hash_update(&inet6_mcast->mrouted_socket->vr_index,
                             sizeof(inet6_mcast->mrouted_socket->vr_index),
                             0);
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_vr_key_hash
 *===========================================================================
 * Description: Computes a hash value for a VR index.
 * Parameters:  vr - a virtual router index.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip6_mcast_vr_key_hash(Ip_u16 *vr)
{
    return ipcom_hash_update(vr, sizeof(*vr), 0);
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_vr_hash_cmp
 *===========================================================================
 * Description: Returns if a multicast route database matches a VR.
 * Parameters:  inet6_mcast - A multicast route database.
 *              vr - A VR.
 * Returns:     IP_TRUE if 'inet_mcast' is assigned to the specified VR.
 *
 */
IP_STATIC Ip_bool
ipnet_ip6_mcast_vr_hash_cmp(Ipnet_inet6_mcast *inet6_mcast, Ip_u16 *vr)
{
    return inet6_mcast->mrouted_socket->vr_index == *vr;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_rt_obj_hash
 *===========================================================================
 * Description: Computes a hash value for the specified multicast route entry.
 * Parameters:  mrt - A multicast route entry.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip6_mcast_rt_obj_hash(Ipnet_mrt6 *mrt)
{
    return ipcom_hash_update(&mrt->key, sizeof(mrt->key), 0);
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_rt_key_hash
 *===========================================================================
 * Description: Computes a hash value for the specified multicast route
 *              search key.
 * Parameters:  key - A multicast route search key.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_ip6_mcast_rt_key_hash(Ipnet_mrt6_key *key)
{
    return ipcom_hash_update(key, sizeof(*key), 0);
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_rt_hash_cmp
 *===========================================================================
 * Description: Compares a route entry with a route
 * Parameters:  mrt - A multicast route entry
 *              key - A multicast route entry search key.
 * Returns:     IP_TRUE if the multicast route entry matches the search key.
 *
 */
IP_STATIC Ip_bool
ipnet_ip6_mcast_rt_hash_cmp(Ipnet_mrt6 *mrt, Ipnet_mrt6_key *key)
{
    return ipcom_memcmp(&mrt->key, key, sizeof(mrt->key)) == 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_del_mrt
 *===========================================================================
 * Description: Deletes a cached route entry that never got valid.
 * Parameters:  mfc - The multicast route entry to free.
 *              is_in_cached_list - Set to IP_TRUE if the entry is in the
 *              list of cached entries.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_mcast_del_mrt(Ipnet_mrt6 *mrt, Ip_bool is_in_cached_list)
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
 *                    ipnet_ip6_mcast_hash_del_mrt
 *===========================================================================
 * Description: Deletes the multicast route table passed.
 * Parameters:  mfc - The multicast route entry to free.
 *              inet_mcast_rt_tab - The hash table the entry belongs to.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_mcast_hash_del_mrt(Ipnet_mrt6 *mrt, Ipcom_hash *inet_mcast_rt_tab)
{
    (void)ipcom_hash_remove(inet_mcast_rt_tab, mrt);
    ipnet_ip6_mcast_del_mrt(mrt, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_get_cached_mrt
 *===========================================================================
 * Description: Returns the cached route matching the specified key.
 *              The entry is removed from the cache list if found.
 * Parameters:  inet6_mcast - The IPv6 multicast FIB for a virtual router.
 *              key - The search key for the route.
 * Returns:
 *
 */
IP_STATIC Ipnet_mrt6 *
ipnet_ip6_mcast_get_cached_mrt(Ipnet_inet6_mcast *inet6_mcast, Ipnet_mrt6_key *key)
{
    Ipnet_mrt6 *mrt;

    for (mrt = (Ipnet_mrt6 *) ipcom_list_first(&inet6_mcast->cached_mrts);
         mrt != IP_NULL;
         mrt = (Ipnet_mrt6 *) ipcom_list_next(&mrt->list_head))
    {
        if (ipcom_memcmp(&mrt->key, key, sizeof(mrt->key)) == 0)
            return mrt;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_mrt_done
 *===========================================================================
 * Description: Frees all resources allocated by the multicast module.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_mcast_mrt_init(Ipnet_socket *sock)
{
    Ipnet_inet6_mcast *inet6_mcast;

    if (sock->ipcom.domain != IP_AF_INET6
        || sock->ipcom.type != IP_SOCK_RAW
        || sock->proto != IP_IPPROTO_ICMPV6)
        return IPNET_ERRNO(EOPNOTSUPP);

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EADDRINUSE);

    if (ipnet->ip6_mcast == IP_NULL)
    {
        ipnet->ip6_mcast =
            ipcom_hash_new((Ipcom_hash_obj_func) ipnet_ip6_mcast_vr_obj_hash,
                           (Ipcom_hash_key_func) ipnet_ip6_mcast_vr_key_hash,
                           (Ipcom_hash_cmp_func) ipnet_ip6_mcast_vr_hash_cmp);
        if (ipnet->ip6_mcast == IP_NULL)
            return IPNET_ERRNO(ENOMEM);
    }

    inet6_mcast = ipcom_calloc(1, sizeof(Ipnet_inet6_mcast));
    if (inet6_mcast == IP_NULL)
        goto errout;

    inet6_mcast->rt_tab =
        ipcom_hash_new(
                       (Ipcom_hash_obj_func) ipnet_ip6_mcast_rt_obj_hash,
                       (Ipcom_hash_key_func) ipnet_ip6_mcast_rt_key_hash,
                       (Ipcom_hash_cmp_func) ipnet_ip6_mcast_rt_hash_cmp);
    if (inet6_mcast->rt_tab == IP_NULL)
        goto errout;

    inet6_mcast->register_mif   = -1;
    inet6_mcast->mrouted_socket = sock;
    ipcom_list_init(&inet6_mcast->cached_mrts);
    if (ipcom_hash_add(ipnet->ip6_mcast, inet6_mcast) != IPCOM_SUCCESS)
        goto errout;

    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL);
    return 0;

 errout:
    if (inet6_mcast != IP_NULL && inet6_mcast->rt_tab != IP_NULL)
        ipcom_hash_delete(inet6_mcast->rt_tab);
    ipcom_free(inet6_mcast);
    return IPNET_ERRNO(ENOMEM);
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_netiftomif
 *===========================================================================
 * Description: Returns the multicast interface matching the specfied net if.
 * Parameters:  inet6_mcast - The IPv6 multicast FIB for a virtual router.
 *              netif - A network interface.
 * Returns:     The multicast interface or IP_NULL if no match was found.
 *
 */
IP_STATIC Ipnet_mif6 *
ipnet_ip6_mcast_netiftomif(Ipnet_inet6_mcast *inet6_mcast, Ipnet_netif *netif)
{
    int        i;
    Ipnet_mif6 *mif;

    if (netif == IP_NULL)
    {
        if (inet6_mcast->register_mif < 0)
            return IP_NULL;

        return inet6_mcast->mifs[inet6_mcast->register_mif];
    }

    for (i = 0; i < IP_MAXMIFS; i++)
    {
        mif = inet6_mcast->mifs[i];
        if (mif != IP_NULL && mif->ifindex == (Ip_mifi_t) netif->ipcom.ifindex)
            return mif;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_mrt_report
 *===========================================================================
 * Description: Sends notification to mrouted.
 * Parameters:  inet6_mcast - The IPv6 multicast FIB for a virtual router.
 *              mif  - The multicast interface affected.
 *              pkt  - The packet that caused the notification.
 *              type - The notification to send.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_mcast_mrt_report(Ipnet_inet6_mcast *inet6_mcast,
                           Ipnet_mif6 *mif,
                           Ipcom_pkt *original,
                           Ip_u8 type)
{
    struct Ip_mrt6msg *im6;
    Ipcom_pkt         *pkt;
    Ipnet_pkt_ip6     *ip6hdr;
    int                msg_len;

    msg_len = (type == IP_MRT6MSG_WHOLEPKT ? original->end - original->ipstart : 0);

    pkt = ipcom_pkt_malloc(msg_len + sizeof(struct Ip_mrt6msg),
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;

    pkt->ifindex = original->ifindex;
    pkt->end     = pkt->maxlen;
    pkt->ipstart = pkt->end - msg_len;
    pkt->start   = pkt->ipstart - sizeof(struct Ip_mrt6msg);
    ipcom_memcpy(&pkt->data[pkt->ipstart], &original->data[original->ipstart], msg_len);

    im6 = (struct Ip_mrt6msg *) &pkt->data[pkt->start];
    im6->im6_msgtype = type;
    im6->im6_mbz     = 0;
    im6->im6_mif     = mif->index;

    /* Copy the source and destination address from the original packet */
    ip6hdr = (Ipnet_pkt_ip6 *) &original->data[original->ipstart];
    IPNET_IP6_SET_ADDR(&im6->im6_src, &ip6hdr->src);
    IPNET_IP6_SET_ADDR(&im6->im6_dst, &ip6hdr->dst);

    (void) ipnet_queue_received_packet(pkt, inet6_mcast->mrouted_socket);
}



/*
 *===========================================================================
 *                    ipnet_ip6_mcast_mrt_timeout
 *===========================================================================
 * Description: Timeout handler for cached multicast routes.
 * Parameters:  mrt - The cached multicast route entry that timed out.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_mcast_mrt_timeout(Ipnet_mrt6 *mrt)
{
    ipnet_ip6_mcast_del_mrt(mrt, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_cache_unresolved
 *===========================================================================
 * Description: Caches a multicast route for a packet for which no entry
 *              existed.
 * Parameters:  inet6_mcast - The IPv6 multicast FIB for a virtual router.
 *              mif - The multicast interface affected.
 *              original - The packet that did not match any multicast route.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_mcast_cache_unresolved(Ipnet_inet6_mcast *inet6_mcast,
                                 Ipnet_mif6 *mif,
                                 Ipcom_pkt *original)
{
    Ipnet_pkt_ip6 *ip6hdr = (Ipnet_pkt_ip6 *) &original->data[original->ipstart];
    Ipnet_mrt6_key key;
    Ipnet_mrt6    *mrt;
    Ipcom_pkt     *pkt;


    IPNET_IP6_SET_ADDR(&key.origin, &ip6hdr->src);
    IPNET_IP6_SET_ADDR(&key.mcastgrp, &ip6hdr->dst);

    mrt = ipnet_ip6_mcast_get_cached_mrt(inet6_mcast, &key);
    if (mrt == IP_NULL)
    {
        /* No matching route entry and not cached */
        ipnet_ip6_mcast_mrt_report(inet6_mcast, mif, original, IP_MRT6MSG_NOCACHE);

        mrt = ipcom_calloc(1, sizeof(Ipnet_mrt6));
        if (mrt == IP_NULL
            || ipnet_timeout_schedule(IPNET_MRT6_CACHE_TO_LIVE * 1000,
                                      (Ipnet_timeout_handler) ipnet_ip6_mcast_mrt_timeout,
                                      mrt,
                                      &mrt->tmo) != 0)
        {
            ipcom_free(mrt);
            return;
        }
        mrt->key         = key;
        ipcom_list_insert_last(&inet6_mcast->cached_mrts, &mrt->list_head);
    }

    if (mrt->num_pending_pkt >= IPNET_MRT6_MAX_CACHE_QUEUE
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
 *                    ipnet_ip6_mcast_output
 *===========================================================================
 * Description: Outputs a packet to the specified multicast interface.
 * Parameters:  inet6_mcast - The IPv6 multicast FIB for a virtual router.
 *              mif - The multicast interface to send the packet through.
 *              mrt - The multicast route this packet matched.
 *              original - The packet to output.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_mcast_output(Ipnet_inet6_mcast *inet6_mcast,
                       Ipnet_mif6 *mif,
                       Ipnet_mrt6 *mrt,
                       Ipcom_pkt *original)
{
    Ipcom_pkt              *pkt;
    Ipnet_pkt_ip6          *ip6hdr;
    Ipnet_ip6_output_param  param;
    Ip_u64                  one;
    Ipnet_netif            *netif;

    IP_U64_SET(one, 0, 1);

    if (IP_BIT_ISSET(mif->flags, IP_MRT6_MIFF_REGISTER))
    {
        ipnet_ip6_mcast_mrt_report(inet6_mcast, mif, original, IP_MRT6MSG_WHOLEPKT);
        return;
    }

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, mif->ifindex);
    if (netif == IP_NULL)
    {
        IPCOM_LOG2(WARNING, "IPv6 mcast: physical interface %d mapping"
                   "against virtual interface %d has been detached",
                   mif->ifindex, mif->index);
        return;
    }

    ip6hdr = (Ipnet_pkt_ip6 *) &original->data[original->ipstart];

    /* Don't forward packets with hoplimit <= 1 */
    if (ip6hdr->hlim <= 1)
        return;

    /* Don't forward packets with destined to a node/link local address */
    if ((IPNET_IP6_MC_SCOPE(&ip6hdr->dst) == IPNET_IP6_SCOPE_NODE_LOCAL)
        || (IPNET_IP6_MC_SCOPE(&ip6hdr->dst) == IPNET_IP6_SCOPE_LINK_LOCAL))
        return;

    /* Don't forward packets with unspecified source address */
    if (IPNET_IP6_IS_ADDR_UNSPECIFIED(&ip6hdr->src))
        return;

    pkt = ipnet_pkt_clone(original, IP_TRUE);
    if (pkt == IP_NULL)
        return;

    IP_U64_ADD32(mif->bytes_out, original->end - original->ipstart);
    IP_U64_ADD(mif->pkts_out, one);

    pkt->start = pkt->ipstart;
    IP_BIT_SET(pkt->flags, (IPCOM_PKT_FLAG_HAS_IP_HDR
                            | IPCOM_PKT_FLAG_FORWARDED
                            | IPCOM_PKT_FLAG_NONBLOCKING
                            | IPCOM_PKT_FLAG_MULTICAST));

    ip6hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];

    ipcom_memset(&param, 0, sizeof(param));
    param.from                = param.real_from = &mrt->key.origin;
    param.to                  = &mrt->key.mcastgrp;
    param.final_to            = &mrt->key.mcastgrp;
    param.netif               = netif;
    param.flowinfo            = IPNET_IP6_GET_FLOW(ip6hdr);
    param.tclass              = IPNET_IP6_GET_CLASS(ip6hdr);
    param.nxt_hdr             = ip6hdr->nxt;
    param.hlim                = --ip6hdr->hlim;

    (void) ipnet_ip6_output(&param, pkt);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ip6_mcast_add_mif
 *===========================================================================
 * Description: Adds a new virtual interface.
 * Parameters:  sock - The mrouted socket.
 *              mifctl - Paramters for the new virtual interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_mcast_add_mif(Ipnet_socket *sock, struct Ip_mif6ctl *mifctl)
{
    Ipnet_mif6        *mif;
    Ipnet_netif       *netif = IP_NULL;
    int                ret;
    Ipnet_inet6_mcast *inet6_mcast;

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    if (inet6_mcast->mifs[mifctl->mif6c_mifi] != IP_NULL)
        /* The MIF index selected by mrouted is already in use */
        return IPNET_ERRNO(EADDRINUSE);

    if (IP_BIT_ISSET(mifctl->mif6c_flags, IP_MRT6_MIFF_REGISTER))
    {
        if (inet6_mcast->register_mif >= 0)
            return IPNET_ERRNO(EADDRINUSE);
    }
    else
    {
        Ip_bool  promisc = IP_TRUE;

        /* The supplied interface index is an Ip_u16.. */
        netif = ipnet_if_indextonetif(sock->vr_index, (Ip_u32)mifctl->mif6c_pifi);

        if (netif == IP_NULL)
            /* No interface found that matches the specified index */
            return IPNET_ERRNO(ENXIO);

        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_MULTICAST) == IP_FALSE)
            return IPNET_ERRNO(EOPNOTSUPP);

        /* Enable promiscuous reception on the interface */
        ret = ipnet_if_drv_ioctl(netif, IP_SIOCXPROMISC, &promisc);

        if (ret < 0)
            return ret;
    }

    mif = ipcom_calloc(1, sizeof(Ipnet_mif6));
    if (mif == IP_NULL)
        return IPNET_ERRNO(ENOMEM);

    mif->flags = mifctl->mif6c_flags;
    mif->index = mifctl->mif6c_mifi;

    if (IP_BIT_ISSET(mifctl->mif6c_flags, IP_MRT6_MIFF_REGISTER))
        inet6_mcast->register_mif = mifctl->mif6c_mifi;
    else
        mif->ifindex = netif->ipcom.ifindex;

    inet6_mcast->mifs[mifctl->mif6c_mifi] = mif;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_del_mif
 *===========================================================================
 * Description: Deletes the specifed multicast interface.
 * Parameters:  sock - The mrouted socket.
 *              vifi - The index of the multicast interface to delete.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_mcast_del_mif(Ipnet_socket *sock, Ip_mifi_t mifi)
{
    Ipnet_mif6        *mif;
    Ipnet_netif       *netif;
    Ip_bool            promisc = IP_FALSE;
    Ipnet_inet6_mcast *inet6_mcast;

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    mif = inet6_mcast->mifs[mifi];

    if (mif == IP_NULL)
        return IPNET_ERRNO(EADDRNOTAVAIL);

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, mif->ifindex);
    if (netif != IP_NULL)
    {
        if (IP_BIT_ISSET(mif->flags, IP_MRT6_MIFF_REGISTER))
            inet6_mcast->register_mif = -1;
        else
        {
            /* Using a physcal and multicast capable interface.
               Disable promiscuous reception of all IP multicasts from the interface */
            (void)ipnet_if_drv_ioctl(netif, IP_SIOCXPROMISC, &promisc);
        }
    }

    ipcom_free(mif);
    inet6_mcast->mifs[mifi] = IP_NULL;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_add_mfc
 *===========================================================================
 * Description: Adds a multicast route.
 * Parameters:  sock - The mrouted socket.
 *              mfcctl - route paramters.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_mcast_add_mfc(Ipnet_socket *sock, struct Ip_mf6cctl *mfcctl)
{
    Ipnet_mrt6        *mrt;
    Ipnet_mrt6_key     key;
    int                i;
    Ipnet_inet6_mcast *inet6_mcast;

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    if ((mfcctl->mf6cc_origin.sin6_family != IP_AF_INET6)
        || (mfcctl->mf6cc_mcastgrp.sin6_family != IP_AF_INET6))
    {
        return IPNET_ERRNO(EINVAL);
    }

    IPNET_IP6_SET_ADDR(&key.origin, &mfcctl->mf6cc_origin.sin6_addr);
    IPNET_IP6_SET_ADDR(&key.mcastgrp, &mfcctl->mf6cc_mcastgrp.sin6_addr);

    mrt = ipcom_hash_get(inet6_mcast->rt_tab, &key);
    if (mrt == IP_NULL)
    {
        /* Create a new entry */
        mrt = ipnet_ip6_mcast_get_cached_mrt(inet6_mcast, &key);
        if (mrt)
        {
            ipnet_timeout_cancel(mrt->tmo);
            ipcom_list_remove(&mrt->list_head);
        }
        else
        {
            mrt = ipcom_calloc(1, sizeof(Ipnet_mrt6));
            if (mrt == IP_NULL)
                return IPNET_ERRNO(ENOMEM);

            ipcom_memcpy(&mrt->key, &key, sizeof(mrt->key));
        }

        if (ipcom_hash_add(inet6_mcast->rt_tab, mrt) != IPCOM_SUCCESS)
        {
            ipnet_ip6_mcast_del_mrt(mrt, IP_FALSE);
            return IPNET_ERRNO(ENOMEM);
        }
    }
    /* else: update the current entry */

    mrt->parent     = mfcctl->mf6cc_parent;
    mrt->mifs_count = 0;
    for (i = 0; i < IP_MAXMIFS; i++)
    {
        if (IP_MRT6_IF_ISSET(i, &mfcctl->mf6cc_ifset))
            mrt->mifs[mrt->mifs_count++] = (Ip_mifi_t) i;
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
            ipnet_ip6_mcast_input(netif, pkt);
        ipcom_pkt_free(pkt);
    }
    mrt->pending_pkt_tail = IP_NULL;
    mrt->num_pending_pkt = 0;

    return 0; /*lint !e429 */
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_del_mfc
 *===========================================================================
 * Description: Deletes a multicast route.
 * Parameters:  sock - The mrouted socket.
 *              mfcctl - The route paramters.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_mcast_del_mfc(Ipnet_socket *sock, struct Ip_mf6cctl *mfcctl)
{
    Ipnet_mrt6        *mrt;
    Ipnet_mrt6_key     key;
    Ipnet_inet6_mcast *inet6_mcast;

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    if ((mfcctl->mf6cc_origin.sin6_family != IP_AF_INET6)
        || (mfcctl->mf6cc_mcastgrp.sin6_family != IP_AF_INET6))
    {
        return IPNET_ERRNO(EINVAL);
    }

    IPNET_IP6_SET_ADDR(&key.origin, &mfcctl->mf6cc_origin.sin6_addr);
    IPNET_IP6_SET_ADDR(&key.mcastgrp, &mfcctl->mf6cc_mcastgrp.sin6_addr);

    mrt = ipcom_hash_get(inet6_mcast->rt_tab, &key);
    if (mrt == IP_NULL)
        return IPNET_ERRNO(ESRCH);

    ipnet_ip6_mcast_hash_del_mrt(mrt, inet6_mcast->rt_tab);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_set_pim
 *===========================================================================
 * Description: Enable/disable PIM.
 * Parameters:  sock - The mrouted socket.
 *              val - IP_TRUE to enable PIM, IP_FALSE to disable.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_mcast_set_pim(Ipnet_socket *sock, Ip_bool val)
{
    Ipnet_inet6_mcast *inet6_mcast;

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    inet6_mcast->pim = val;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_mrt_done
 *===========================================================================
 * Description: Frees all resources allocated by the multicast module.
 * Parameters:  sock - The mrouted socket.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_mcast_mrt_done(Ipnet_socket *sock)
{
    Ip_mifi_t          mifi;
    Ipnet_inet6_mcast *inet6_mcast;

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    /* Free all cached entries */
    while (inet6_mcast->cached_mrts.size > 0)
    {
        Ipnet_mrt6 *mrt = (Ipnet_mrt6 *) ipcom_list_first(&inet6_mcast->cached_mrts);
        ipnet_ip6_mcast_del_mrt(mrt, IP_TRUE);
    }

    /* Free all multicast route entries */
    ipcom_hash_for_each(inet6_mcast->rt_tab,
                        (void(*)(void *, void *)) ipnet_ip6_mcast_hash_del_mrt,
                        inet6_mcast->rt_tab);
    ipcom_hash_delete(inet6_mcast->rt_tab);

    /* Detach and free all MIF:s */
    for (mifi = 0; mifi < IP_MAXMIFS; mifi++)
        (void) ipnet_ip6_mcast_del_mif(sock, mifi);

    (void) ipcom_hash_remove(ipnet->ip6_mcast, inet6_mcast);
    ipcom_free(inet6_mcast);

    IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_getsockopt
 *===========================================================================
 * Description: Socket option where level was set to IP_IPPROTO_IPV6.
 * Parameters:  [in] sock - A socket.
 *              [in] optname - The socket option to set.
 *              [out] optval - Buffer where the option data should be stored.
 *              [in, out] optlen - The length of 'optval', number of bytes
 *              copied to 'optval' when the function returns.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_mcast_getsockopt(Ipnet_socket *sock, int optname, void *optval,
                           Ip_socklen_t *optlen)
{
    int                val;
    Ipnet_inet6_mcast *inet6_mcast;

    if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EACCES);

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    switch (optname)
    {
    case IP_MRT6_PIM:
        val = inet6_mcast->pim;
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
 *                    ipnet_ip6_mcast_setsockopt
 *===========================================================================
 * Description: Socket option where level was set to IP_IPPROTO_IPV6.
 * Parameters:  [in] sock - A socket.
 *              [in] optname - The socket option to set.
 *              [in] optval - The option data to set.
 *              [in] optlen - The length of 'optval'
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_mcast_setsockopt(Ipnet_socket *sock, int optname, IP_CONST void *optval,
                           Ip_socklen_t optlen)
{
    struct Ip_mif6ctl *mifctl;

    if (optname == IP_MRT6_INIT)
        return ipnet_ip6_mcast_mrt_init(sock);

    if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EACCES);

    switch (optname)
    {
    case IP_MRT6_DONE:
        return ipnet_ip6_mcast_mrt_done(sock);

    case IP_MRT6_ADD_MIF:
    case IP_MRT6_DEL_MIF:
        if (optlen != sizeof(struct Ip_mif6ctl))
            return IPNET_ERRNO(EINVAL);

        mifctl = (struct Ip_mif6ctl *) optval;

        if (mifctl->mif6c_mifi >= IP_MAXMIFS)
            return IPNET_ERRNO(EINVAL);

        if (optname == IP_MRT6_ADD_MIF)
            return ipnet_ip6_mcast_add_mif(sock, mifctl);
        return ipnet_ip6_mcast_del_mif(sock, mifctl->mif6c_mifi);

    case IP_MRT6_ADD_MFC:
    case IP_MRT6_DEL_MFC:
        if (optlen != sizeof(struct Ip_mf6cctl))
            return IPNET_ERRNO(EINVAL);

        if (optname == IP_MRT6_ADD_MFC)
            return ipnet_ip6_mcast_add_mfc(sock, (struct Ip_mf6cctl *) optval);
        return ipnet_ip6_mcast_del_mfc(sock, (struct Ip_mf6cctl *) optval);

    case IP_MRT6_PIM:
        if (optlen != sizeof(int))
            return IPNET_ERRNO(EINVAL);
        ipnet_ip6_mcast_set_pim(sock, *(int *) optval != 0);
        break;

    default:
        return IPNET_ERRNO(EOPNOTSUPP);
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_ioctl
 *===========================================================================
 * Description: IO control for IPv6 multicast.
 * Parameters:  sock - The mrouted socket.
 *              [in] request - The reqeust id.
 *              [in, out] data - The reqeust data.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int ipnet_ip6_mcast_ioctl(Ipnet_socket *sock,
                                    unsigned long request,
                                    void *data)
{
    struct Ip_sioc_mif_req6 *mif_req;
    Ipnet_mif6              *mif;
    struct Ip_sioc_sg_req6  *sg_req;
    Ipnet_mrt6              *mrt;
    Ipnet_mrt6_key          key;
    Ipnet_inet6_mcast      *inet6_mcast;

    if (sock->ipcom.domain != IP_AF_INET6
        || IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        return IPNET_ERRNO(EOPNOTSUPP);

    inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &sock->vr_index);
    ip_assert(inet6_mcast != IP_NULL);

    switch (request)
    {
    case IP_SIOCGETMIFCNT_IN6:
        mif_req = (struct Ip_sioc_mif_req6 *) data;

        if (mif_req->mifi >= IP_MAXMIFS)
            return IPNET_ERRNO(EINVAL);

        mif = inet6_mcast->mifs[mif_req->mifi];
        if (mif == IP_NULL)
            return IPNET_ERRNO(EADDRNOTAVAIL);

        IP_U64_COPY(mif_req->ibytes, mif->bytes_in);
        IP_U64_COPY(mif_req->icount, mif->pkts_in);
        IP_U64_COPY(mif_req->obytes, mif->bytes_out);
        IP_U64_COPY(mif_req->ocount, mif->pkts_out);
        break;

    case IP_SIOCGETSGCNT_IN6:
        sg_req = (struct Ip_sioc_sg_req6 *) data;

        IPNET_IP6_SET_ADDR(&key.origin, &sg_req->src.sin6_addr);
        IPNET_IP6_SET_ADDR(&key.mcastgrp, &sg_req->grp.sin6_addr);

        mrt = (Ipnet_mrt6 *) ipcom_hash_get(inet6_mcast->rt_tab, &key);

        if (mrt == IP_NULL)
            return IPNET_ERRNO(ESRCH);

        IP_U64_ADD32(sg_req->bytecnt, mrt->bytes);
        IP_U64_ADD32(sg_req->pktcnt, mrt->pkt);
        IP_U64_ADD32(sg_req->wrong_if, mrt->wrong_if);
        break;

    default:
        return IPNET_ERRNO(EINVAL);
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_mcast_input
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
ipnet_ip6_mcast_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip6     *ip6hdr;
    Ipnet_mrt6_key     key;
    Ipnet_mrt6        *mrt;
    Ipnet_mif6        *mif;
    int                i;
    Ip_u64             one;
    Ipnet_inet6_mcast *inet6_mcast = IP_NULL;

    if (ipnet->ip6_mcast != IP_NULL)
        inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &pkt->vr_index);

    if (inet6_mcast == IP_NULL
        || (netif != IP_NULL && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)))
        /* No mrouted running or looped back packet, ignore packet */
        return;

    ip6hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];

    IPNET_IP6_SET_ADDR(&key.origin, &ip6hdr->src);
    IPNET_IP6_SET_ADDR(&key.mcastgrp,&ip6hdr->dst);

    mrt = ipcom_hash_get(inet6_mcast->rt_tab, &key);

    if(mrt == IP_NULL)
    {
        /* Try with source == ANY */
        ipcom_memset(&key.origin, 0, sizeof(struct Ip_in6_addr));
        mrt = ipcom_hash_get(inet6_mcast->rt_tab, &key);
    }

    if (mrt == IP_NULL)
    {
        /* No route entry for this packet */
        mif = ipnet_ip6_mcast_netiftomif(inet6_mcast, netif);
        if (mif != IP_NULL)
            ipnet_ip6_mcast_cache_unresolved(inet6_mcast, mif, pkt);
        return;
    }

    mrt->pkt++;
    mrt->bytes += pkt->end - pkt->ipstart;

    mif = inet6_mcast->mifs[mrt->parent];

    if (mif == IP_NULL
        || (netif != IP_NULL && mif->ifindex != (Ip_mifi_t) netif->ipcom.ifindex))
    {
        /* Wrong interface */
        mrt->wrong_if++;
        mif = ipnet_ip6_mcast_netiftomif(inet6_mcast, netif);
        if (mif != IP_NULL)
            ipnet_ip6_mcast_mrt_report(inet6_mcast, mif, pkt, IP_MRT6MSG_WRONGMIF);
        return;
    }

    IP_U64_ADD32(mif->bytes_in, pkt->end - pkt->ipstart);
    IP_U64_SET(one, 0, 1);
    IP_U64_ADD(mif->pkts_in, one);

    for (i = 0; i < mrt->mifs_count; i++)
    {
        mif = inet6_mcast->mifs[mrt->mifs[i]];
        if (mif == IP_NULL)
            /* The mif has been deleted */
            continue;
        ipnet_ip6_mcast_output(inet6_mcast, mif, mrt, pkt);
    }
}



/*
 *===========================================================================
 *                    ipnet_pim_ip6_input
 *===========================================================================
 * Description: Process PIM register messages
 * Parameters:  pkt - A packet sent to the IP_IPPROTO_PIM protocol.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_pim_ip6_input(Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip6           *ip6hdr;
    struct Ip_in6_addr      src_addr;
    struct Ip_in6_addr      dst_addr;
    int                     original_start;
    int                     proto;
    Ipnet_process_ext_param ext_param;
    Ipnet_inet6_mcast      *inet6_mcast;
    Ip_pkt_pim *pimhdr = (Ip_pkt_pim *) &pkt->data[pkt->start];

    if (pkt->end - pkt->start < (int) sizeof(Ip_pkt_pim)
        || IP_PIM_GET_VERSION(pimhdr) != IP_PIM_VERSION)
        /* Not a valid PIM message */
        goto drop_pkt;

    /* Deliver the packet to RAW sockets */
    original_start = pkt->start;
    pkt->start = pkt->ipstart;
        ip6hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];

    IPNET_IP6_SET_ADDR(&src_addr, &ip6hdr->src);
    IPNET_IP6_SET_ADDR(&dst_addr, &ip6hdr->dst);

    ext_param.ppkt          = &pkt;
    ext_param.route         = IP_FALSE;
    ext_param.error         = IP_FALSE;
    ext_param.ip6_dst_entry = IP_NULL;
    proto = ipnet_ip6_process_exthdr(&ext_param);

    (void) ipnet_raw_input(pkt,
                           IP_FALSE,
                           proto,
                           &src_addr, 0,
                           &dst_addr, 0,
                           (Ipnet_sock_lookup_f) ipnet_sock_ip6_lookup);

    if (ipnet->ip6_mcast == IP_NULL)
        inet6_mcast = IP_NULL;
    else
        inet6_mcast = ipcom_hash_get(ipnet->ip6_mcast, &pkt->vr_index);

    if (inet6_mcast == IP_NULL || inet6_mcast->pim == IP_FALSE)
        /* PIM not enabled. Drop packet */
        goto drop_pkt;

    if (IP_PIM_GET_TYPE(pimhdr) == IP_PIM_TYPE_REGISTER
        && IP_BIT_ISFALSE(IP_GET_NTOHL(&pimhdr->flags), IP_PIM_FLAG_NULL_REGISTER))
    {
        Ipnet_pkt_ip6 *encap_ip6hdr;

        pkt->start = original_start;
        if (inet6_mcast->register_mif < 0
            || pkt->end - pkt->start < (int) (sizeof(Ip_pkt_pim) + sizeof(Ipnet_pkt_ip6))
            || (ipnet_ip6_checksum(&ip6hdr->src,
                                   &ip6hdr->dst,
                                   (Ip_u16)proto,
                                   (Ip_u16 *)pimhdr,
                                   sizeof(Ip_pkt_pim),
                                   pkt) != 0
                && ipnet_ip6_checksum(&ip6hdr->src,
                                      &ip6hdr->dst,
                                      (Ip_u16)proto,
                                      (Ip_u16 *)pimhdr,
                                      (Ip_u16) (pkt->end - pkt->start),
                                      pkt) != 0))
            /* The first checksum calculation should be enough,
               but some implementation (incorrectly) includes the encapsulated data */
            goto drop_pkt;

        encap_ip6hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start + sizeof(Ip_pkt_pim)];

        if (!IP_IN6_IS_ADDR_MULTICAST(&encap_ip6hdr->dst))
            goto drop_pkt;

        /* Move to the encapsulated multicast packet */
        pkt->ipstart = pkt->start = pkt->start + sizeof(Ip_pkt_pim);
        pkt->ifindex = 0;
        /* IP_NULL as interface means that this packet originates from a PIM register message */
        (void)ipnet_ip6_mcast_input(IP_NULL, pkt);
    }

    ipcom_pkt_free(pkt);
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

