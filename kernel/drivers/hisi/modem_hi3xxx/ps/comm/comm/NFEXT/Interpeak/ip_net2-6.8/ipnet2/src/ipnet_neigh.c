/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_neigh.c,v $ $Revision: 1.27.18.1 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_neigh.c,v $
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
 * Helper functions for neighbor discovery (ARP, NDP and other).
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
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_syslog.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_neigh.h"

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_NEIGH_RESOLVE_NEXT_PKT(pkt) ((pkt)->next_fragment)

/* From RFC 4861, chapter 10 */
#define IPNET_MIN_RANDOM_FACTOR 50  /* Converted into percent */
#define IPNET_MAX_RANDOM_FACTOR 150 /* Converted into percent */
#define IPNET_REACHABLE_TIME    30  /* Converted into seconds */

/* Route metric when no IP-to-link-layer mapping exists, INCOMPLETE
   state. */
#define IPNET_UNRESOLVED_METRIC 10
/* Route metric when neighbor is in STALE state */
#define IPNET_STALE_METRIC      5
/* Route metric when neighbor is in REACHABLE, PROBE and DELAY
   states */
#define IPNET_REACHABLE_METRIC  2


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
 *                  ipnet_neigh_get_base_reachable_time
 *===========================================================================
 * Description: Calculates the duration (in seconds) that the nighbor should
 *              be in REACHABLE state.
 * Parameters:  domain - IP_AF_INET or IP_AF_INET6
 *              netif - interface for which the base time should be returned.
 * Returns:     Reachable time in seconds.
 *
 */
IP_STATIC Ip_u32
ipnet_neigh_get_base_reachable_time(int domain,
                                    Ipnet_netif *netif,
                                    Ip_u32 ra_specified_lifetime)
{
    Ip_u32 lifetime;
    Ip_u32 min_rand;
    Ip_u32 max_rand;
    Ip_u32 rand_val;

    /* RFC 4861, chapter 6.3.2
     ...
     ReachableTime  The time a neighbor is considered reachable after
                    receiving a reachability confirmation.

                    This value should be a uniformly distributed
                    random value between MIN_RANDOM_FACTOR and
                    MAX_RANDOM_FACTOR times BaseReachableTime
                    milliseconds.  A new random value should be
                    calculated when BaseReachableTime changes (due to
                    Router Advertisements) or at least every few
                    hours even if no Router Advertisements are
                    received.
    ... */

    min_rand = (Ip_u32) ipnet_sysvar_netif_get_as_int(domain,
                                                      netif,
                                                      "MinRandomFactor",
                                                      IPNET_MIN_RANDOM_FACTOR);
    max_rand = (Ip_u32) ipnet_sysvar_netif_get_as_int(domain,
                                                      netif,
                                                      "MaxRandomFactor",
                                                      IPNET_MAX_RANDOM_FACTOR);
    if (ra_specified_lifetime != 0)
        lifetime = ra_specified_lifetime;
    else
        lifetime = (Ip_u32) ipnet_sysvar_netif_get_as_int(domain,
                                                          netif,
                                                          "BaseReachableTime",
                                                          IPNET_REACHABLE_TIME);

    if (min_rand == max_rand)
        /* Avoid division by zero */
        rand_val = 0;
    else
        /* Using a different random number everytime will satisfy "...at
           least every few hours..." */
        rand_val = ipcom_rand() % (max_rand - min_rand);

    return lifetime * (min_rand + rand_val) / 100;
}


/*
 *===========================================================================
 *                    ipnet_neigh_rt_template_is_valid
 *===========================================================================
 * Description: Checks if the rt_template member of a route is valid.
 * Parameters:  domain - domain for the route entry.
 *              rt - a route entry
 * Returns:     IP_TRUE if the rt_template member is a valid template instance.
 *
 */
IP_STATIC Ip_bool
ipnet_neigh_rt_template_is_valid(int domain, Ipnet_route_entry *rt)
{
    if (rt->rt_template == IP_NULL)
        return IP_FALSE;
    switch (domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return (void*) rt->rt_template->hdr.parent != ipnet->ip4.dont_route_list;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return (void*) rt->rt_template->hdr.parent != ipnet->ip6.dont_route_list;
#endif
    default:
        break;
    }
    return IP_FALSE;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_neigh_queue_until_resolved
 *===========================================================================
 * Description: Queues a packet until an IP to link layer mapping exists.
 * Parameters:  wl - The wait parameters
 *              pkt - The frame that is waiting for IP to link address resolution.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_neigh_queue_until_resolved(Ipnet_neigh_res_wait_list *wl, Ipcom_pkt *pkt)
{
    Ipcom_pkt **pkt_it = &wl->pkt_list;

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 1, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                      ipnet_neigh_queue_until_resolved, IPCOM_WV_NETD_INFO_WAIT,
                      IPCOM_WV_IPNET_NEIGH_MODULE, IPCOM_WV_NETD_ND);
    IPNET_STATS(neigh_pending++);

    /* Add the new packet last in the list */
    ++wl->num;
    IPNET_NEIGH_RESOLVE_NEXT_PKT(pkt) = IP_NULL;
    while (*pkt_it != IP_NULL)
        pkt_it = &IPNET_NEIGH_RESOLVE_NEXT_PKT(*pkt_it);
    *pkt_it = pkt;

    if (wl->num > wl->max)
    {
        Ipcom_pkt *drop_pkt = wl->pkt_list;
        /* Too many packetes waiting for IP to link layer resoluion,
           drop the oldest (which is first packet in list) */

        --wl->num;
        wl->pkt_list = IPNET_NEIGH_RESOLVE_NEXT_PKT(drop_pkt);
        IPNET_NEIGH_RESOLVE_NEXT_PKT(drop_pkt) = IP_NULL;
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 2, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                          ipnet_neigh_queue_until_resolved, IPCOM_WV_NETD_INFO_DROPPED,
                          IPCOM_WV_IPNET_NEIGH_MODULE, IPCOM_WV_NETD_ND);
        IPNET_STATS(neigh_pending_drop++);

        if (wl->dropcb != IP_NULL)
            wl->dropcb(drop_pkt);

        ipcom_pkt_free(drop_pkt);

        IPCOM_LOG0(DEBUG, "Packet dropped while waiting for IP to link layer mapping");
    }
}


/*
 *===========================================================================
 *                    ipnet_neigh_free_all_pending
 *===========================================================================
 * Description: Frees all pending packets.
 * Parameters:  wl - The ND entry to free all pending packets for.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_neigh_free_all_pending(Ipnet_neigh_res_wait_list *wl)
{
    Ipcom_pkt          *next_pkt;
    Ipcom_pkt          *pkt = wl->pkt_list;
    Ipnet_neigh_dropcb  dropcb = wl->dropcb;

    if (wl->num)
        IPCOM_LOG1(DEBUG,
                   "%u Packet dropped since no IP to link mapping was found",
                   wl->num);

    wl->pkt_list = IP_NULL;
    wl->num      = 0;

    while (pkt != IP_NULL)
    {
        next_pkt = IPNET_NEIGH_RESOLVE_NEXT_PKT(pkt);
        IPNET_NEIGH_RESOLVE_NEXT_PKT(pkt) = IP_NULL;
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 3, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                          ipnet_neigh_free_all_pending, IPCOM_WV_NETD_INFO_DROPPED,
                          IPCOM_WV_IPNET_NEIGH_MODULE, IPCOM_WV_NETD_ND);
        IPNET_STATS(neigh_pending_drop++);

        if (dropcb != IP_NULL)
            dropcb(pkt);

        ipcom_pkt_free(pkt);
        pkt = next_pkt;
    }
}


/*
 *===========================================================================
 *                    ipnet_neigh_send_all_pending
 *===========================================================================
 * Description: Sends all packets that were waiting for IP to link address
 *              resolution.
 * Parameters:  wl - Wait list parameters.
 *              netif - The interface the pending packets should be sent from.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_neigh_send_all_pending(Ipnet_neigh_res_wait_list *wl,
                             struct Ipnet_netif_struct *netif)
{
    Ipcom_pkt          *next_pkt;
    Ipcom_pkt          *pkt = wl->pkt_list;
    Ipnet_neigh_resolvedcb resolvedcb = wl->resolvedcb;

    wl->pkt_list = IP_NULL;
    wl->num      = 0;

    IPNET_IF_LOCK(netif);
    while (pkt != IP_NULL)
    {
        /* Send all packets that are waiting for IP to MAC address resolution */
        next_pkt = IPNET_NEIGH_RESOLVE_NEXT_PKT(pkt);
        IPNET_NEIGH_RESOLVE_NEXT_PKT(pkt) = IP_NULL;

        if (pkt->ifindex != netif->ipcom.ifindex)
            /* arp_entry->pending_pkt->ifindex can be different from
             * netif->ifindex if two interfaces is connected to the same LAN
             */
            ipcom_pkt_free(pkt);
        else
        {
            resolvedcb((Ipnet_neigh_entry *) ((char *)wl - ip_offsetof(Ipnet_neigh_entry, pending)),
                       pkt);
        }
        pkt = next_pkt;
    }
    IPNET_IF_UNLOCK(netif);
}


/*
 *===========================================================================
 *                            ipnet_neigh_init
 *===========================================================================
 * Description: Initializes a route entry for usage as a neighbor entry.
 * Parameters:  rt - a route entry with the IPNET_RTF_LLINFO flag set.
 *              timedoutcb - handler for timeouts and cleanup.
 *              probecb - handler for sending neighbor solicitation.
 *              dropcb - handler to perform notification when pending packets
 *                       must be dropped.
 *              resolvedcb - handler that will send a packet when the link
 *                           address mapping has been received.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_neigh_init(Ipnet_route_entry *rt,
                 Ipnet_route_timedoutcb timedoutcb,
                 Ipnet_neigh_probecb probecb,
                 Ipnet_neigh_dropcb dropcb,
                 Ipnet_neigh_resolvedcb resolvedcb)
{
    Ipnet_neigh_entry *neigh;

    ip_assert(rt != IP_NULL);
    ip_assert(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO));

    neigh = IPNET_NEIGH_FROM_RT(rt);
    ip_assert(neigh != IP_NULL);

    neigh->rt = rt;
    rt->timedout_cb = timedoutcb;
    neigh->probecb = probecb;
    neigh->pending.dropcb     = dropcb;
    neigh->pending.resolvedcb = resolvedcb;
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_DONE))
        /* Static NDP entry added by user task */
        neigh->state = IPNET_ND_REACHABLE;
}


/*
 *===========================================================================
 *                         ipnet_neigh_set_state
 *===========================================================================
 * Description: Sets the state of the passed neighbor entry.
 * Parameters:  domain - IP_AF_INET or IP_AF_INET6
 *              ne - a neighbor entry
 *              new_state - new state for this neighbor entry.
 *              link_addr - new link address or IP_NULL leave the
 *                          link address unchanged.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_neigh_set_state(int domain,
                      Ipnet_neigh_entry *neigh,
                      Ipnet_nd_state_t new_state,
                      IP_CONST void *link_addr)
{
    Ipnet_route_entry *rt = neigh->rt;
    Ipnet_netif       *netif = rt->netif;
    Ip_u8              old_state;
    Ip_bool            is_making_forward_progress;
    Ip_bool            emit_notify_event = IP_FALSE;

    ip_assert(neigh != IP_NULL);
    ip_assert(rt != IP_NULL);

#ifdef IPNET_DEBUG
    if (neigh->state != new_state)
    {
        IPCOM_LOG4(DEBUG2, "neigh: state change on %s%%%s %s -> %s",
                   ipcom_inet_ntop(domain,
                                   neigh->rt->hdr.key,
                                   ipnet->log_buf,
                                   sizeof(ipnet->log_buf)),
                   netif->ipcom.name,
                   ipnet_nd_state_to_string(neigh->state),
                   ipnet_nd_state_to_string(new_state));
    }
#endif /* IPNET_DEBUG */

#ifdef IPCOM_USE_MIB2_NEW
    /* Update MIB2 last change */
    ipnet_eth_update_mib2_lastchange(rt);
#endif

    old_state = neigh->state;
    neigh->state = new_state;
    is_making_forward_progress = neigh->is_making_forward_progress;
    neigh->is_making_forward_progress = IP_FALSE;

    switch (new_state)
    {
    case IPNET_ND_PROXY:
    case IPNET_ND_REACHABLE:
        /* RFC 4861, chapter 7.3.2
           ...
           REACHABLE  Positive confirmation was received within the last
                      ReachableTime milliseconds that the forward path to
                      the neighbor was functioning properly.  While
                      REACHABLE, no special action takes place as packets
                      are sent.
           ...
        */
        if (ipnet_sysvar_netif_get_as_int_ex(domain,
                                             netif,
                                             "SmartbitNdMode",
                                             IP_FALSE,
                                             ipnet_bool_map))
        {
            /* The Spirent Smartbit measurement will fail if the
               neighbor entry times out during the test since the
               Smartbit does not answer to ND requests */
            ipnet_route_set_lifetime(rt, IPCOM_ADDR_INFINITE);
        }
        else if (rt->metrics.rmx_expire != IPCOM_ADDR_INFINITE)
        {
            Ip_u32 lifetime;
            Ip_u32 ra_specified_lifetime;

            switch (domain)
            {
#ifdef IPCOM_USE_INET6
            case IP_AF_INET6:
                ra_specified_lifetime = netif->inet6_nd_reachable_time / 1000;
                break;
#endif
            default:
                ra_specified_lifetime = 0;
                break;
            }

            lifetime = ipnet_neigh_get_base_reachable_time(domain,
                                                           netif,
                                                           ra_specified_lifetime);
            ipnet_route_set_lifetime(rt, lifetime);
        }
        break;

    case IPNET_ND_DELAY:
        /* RFC 4861, chapter 7.3.2
           ...
           DELAY  More than ReachableTime milliseconds have elapsed
                  since the last positive confirmation was received that
                  the forward path was functioning properly, and a
                  packet was sent within the last DELAY_FIRST_PROBE_TIME
                  seconds.  If no reachability confirmation is received
                  within DELAY_FIRST_PROBE_TIME seconds of entering the
                  DELAY state, send a Neighbor Solicitation and change
                  the state to PROBE.

                  The DELAY state is an optimization that gives upper-
                  layer protocols additional time to provide
                  reachability confirmation in those cases where
                  ReachableTime milliseconds have passed since the last
                  confirmation due to lack of recent traffic.  Without
                  this optimization, the opening of a TCP connection
                  after a traffic lull would initiate probes even though
                  the subsequent three-way handshake would provide a
                  reachability confirmation almost immediately.
           ...
        */
        ipnet_route_set_lifetime(rt, ipnet_sysvar_netif_get_as_int(domain,
                                                                   netif,
                                                                   "DelayFirstProbeTime", 5));
        break;

    case IPNET_ND_INCOMPLETE:
        /* RFC 4861, chapter 7.3.2
           ...
           INCOMPLETE  Address resolution is being performed on the entry.
                       Specifically, a Neighbor Solicitation has been sent to
                       the solicited-node multicast address of the target,
                       but the corresponding Neighbor Advertisement has not
                       yet been received.
           ...
        */
        if (old_state == IPNET_ND_INCOMPLETE
            && neigh->pending.linger == IP_FALSE)
            break;
        neigh->pending.max =
            (Ip_u8) ipnet_sysvar_netif_get_as_int(domain, netif, "MaxPending", 1);
        /* Fall through */
    case IPNET_ND_PROBE:
        /* RFC 4861, chapter 7.3.2
           ...
           PROBE  A reachability confirmation is actively sought by
                  retransmitting Neighbor Solicitations every
                  RetransTimer milliseconds until a reachability
                  confirmation is received.
           ...
        */
        neigh->remaining_ucast_probes =
            (Ip_s8) ipnet_sysvar_netif_get_as_int(domain, netif, "MaxUnicastSolicit", 0);
        neigh->remaining_mcast_probes =
            (Ip_s8) ipnet_sysvar_netif_get_as_int(domain, netif, "MaxMulticastSolicit", 0);
        neigh->remaining_app_probes =
            (Ip_s8) ipnet_sysvar_netif_get_as_int(domain, netif, "MaxApplicationSolicit", 0);
        /*
         * Schedule a timeout as fast as possible, the timeout handler
         * will send the probe. NOTE: must be a timeout or possibly a
         * softinterrupt since IPv6 need to have the pending packet
         * queued on this entry in order to figure out the right
         * source address to use in the NS probe.
         */
        ipnet_route_set_lifetime(rt, 0);
        break;

    case IPNET_ND_STALE:
        /* RFC 4861, chapter 7.3.2
           ...
           STALE  More than ReachableTime milliseconds have elapsed
                  since the last positive confirmation was received that
                  the forward path was functioning properly.  While
                  stale, no action takes place until a packet is sent.

                  The STALE state is entered upon receiving an
                  unsolicited Neighbor Discovery message that updates
                  the cached link-layer address.  Receipt of such a
                  message does not confirm reachability, and entering
                  the STALE state ensures reachability is verified
                  quickly if the entry is actually being used.  However,
                  reachability is not actually verified until the entry
                  is actually used.
            ...
        */
        if (is_making_forward_progress
            || ipnet_sysvar_netif_get_as_int_ex(domain,
                                                netif,
                                                "SmartbitNdMode",
                                                IP_FALSE,
                                                ipnet_bool_map))
        {
            /* Got forward progress indication from upper layer while
               beeing in reachable state, extend the reachable state
               RFC 2461 chapter 7.3.1 */
            ipnet_neigh_set_state(domain, neigh, IPNET_ND_REACHABLE, link_addr);
            return;
        }

        ipnet_route_set_lifetime(rt, ipnet_sysvar_netif_get_as_int(domain,
                                                                   netif,
                                                                   "NeighborCacheToLive", 60 * 5));
        break;

    default:
        /* Invalid state */
        IP_PANIC();
        return;
    }

    neigh->pending.linger = IP_FALSE;

    if (link_addr != IP_NULL)
    {
        struct Ip_sockaddr_dl *gw_dl = (struct Ip_sockaddr_dl*) rt->gateway;
        ipcom_memcpy(IP_SOCKADDR_DL_LLADDR(gw_dl),
                     link_addr,
                     gw_dl->sdl_alen);
    }

    if (ipnet_neigh_has_pending(&neigh->pending)
        && new_state != IPNET_ND_INCOMPLETE)
    {
        if (new_state == IPNET_ND_STALE)
            ipnet_neigh_set_state(domain,
                                  neigh,
                                  IPNET_ND_DELAY,
                                  IP_NULL);
        else
            ipnet_neigh_send_all_pending(&neigh->pending, netif);
    }

    if ((new_state == IPNET_ND_REACHABLE
         || new_state == IPNET_ND_STALE)
        && (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_DONE)
            || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_UP)))
    {
        IP_BIT_SET(rt->hdr.flags, IPNET_RTF_UP | IPNET_RTF_DONE);
#ifdef IPNET_USE_ROUTESOCK_LINKINFO
        emit_notify_event = IP_TRUE;
#endif
    }

    if (ipnet_neigh_rt_template_is_valid(domain, rt))
    {
        Ip_u32 old_hop_count = rt->metrics.rmx_hopcount;

        switch (new_state)
        {
        case IPNET_ND_PROXY:
            /* Do not touch metric in this state */
            break;
        case IPNET_ND_INCOMPLETE:
            rt->metrics.rmx_hopcount = IPNET_UNRESOLVED_METRIC;
            break;
        case IPNET_ND_STALE:
            rt->metrics.rmx_hopcount = IPNET_STALE_METRIC;
            break;
        default:
            rt->metrics.rmx_hopcount = IPNET_REACHABLE_METRIC;
            break;
        }

        if (old_hop_count != rt->metrics.rmx_hopcount)
        {
            ipnet_route_has_changed(rt);
            emit_notify_event = IP_TRUE;
        }

        if (emit_notify_event)
            IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));

        if (new_state != IPNET_ND_INCOMPLETE)
        {
            /*
             * Netlink wants to get events for each transition in the
             * neighbor state machine.
             */
            IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
        }
    }
}


/*
 *===========================================================================
 *                          ipnet_neigh_timeout
 *===========================================================================
 * Description: Timeout/cleanup handler for a neighbor entry.
 * Parameters:  domain - IP_AF_INET or IP_AF_INET6
 *              neigh - neighbor entry that has timed out.
 *              is_timeout - IP_TRUE called because the entry timeout
 *              IP_FALSE called because the entry is being deleted
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_neigh_timeout(int domain,
                    Ipnet_neigh_entry *neigh,
                    Ip_bool is_timeout)
{

    Ipnet_route_entry *rt = neigh->rt;
    Ipnet_netif       *netif = rt->netif;
    Ip_u32             lifetime;

    if (ipnet_neigh_rt_template_is_valid(domain, rt)
        && rt->metrics.rmx_mtu != rt->rt_template->metrics.rmx_mtu)
    {
        /* Restore the MTU to the prefix MTU, might have been lowered
           by a Message Too Big */
        rt->metrics.rmx_mtu = rt->rt_template->metrics.rmx_mtu;
        IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
    }

    if (is_timeout == IP_FALSE)
    {
        ipnet_neigh_free_all_pending(&neigh->pending);
        return;
    }

    switch (neigh->state)
    {
    case IPNET_ND_UNINITIALIZED:
        IPCOM_LOG0(DEBUG, "neighbor cache entry timed out in the uninitialized state.");
        break;

    case IPNET_ND_INCOMPLETE:
    case IPNET_ND_PROBE:
        if (neigh->remaining_ucast_probes <= 0
            && neigh->remaining_mcast_probes <= 0
            && neigh->remaining_app_probes <= 0)
        {
            /* No link layer mapping established after all probes has
               been sent */
            ipnet_neigh_free_all_pending(&neigh->pending);
            if (neigh->state == IPNET_ND_INCOMPLETE
                && neigh->pending.linger == IP_FALSE)
            {
                neigh->pending.linger = IP_TRUE;
                /* The entry will be removed after LingerTime seconds has passed */
                ipnet_route_set_lifetime(rt, ipnet_sysvar_netif_get_as_int(domain,
                                                                           netif,
                                                                           "LingerTime",
                                                                           0));
            }
            /* else: Let the entry be deleted (no new timout is set) */
            return;
        }

        /* Retry after BaseRetransmitTime */
#ifdef IPCOM_USE_INET6
        if (domain == IP_AF_INET6 && netif->inet6_nd_retransmit_time)
            /* The router for this link has specified the retransmit
               time all nodes must use. Round up the value to nearest
               whole second. */
            lifetime = (netif->inet6_nd_retransmit_time + 500) / 1000;
        else
#endif
            lifetime = ipnet_sysvar_netif_get_as_int(domain,
                                                     netif,
                                                     "BaseRetransmitTime",
                                                     1);
        ipnet_route_set_lifetime(rt, IP_MAX(lifetime, 1));

        if (neigh->remaining_ucast_probes > 0)
        {
            neigh->probecb(neigh, (neigh->state == IPNET_ND_PROBE));
            --neigh->remaining_ucast_probes;
        }
        else if (neigh->remaining_mcast_probes > 0)
        {
            neigh->probecb(neigh, IP_FALSE);
            --neigh->remaining_mcast_probes;
        }
        else if (neigh->remaining_app_probes > 0)
        {
            if (ipnet_neigh_rt_template_is_valid(domain, rt))
            {
                IPNET_ROUTESOCK(ipnet_routesock_rt_resolve(rt));
                IPNET_NETLINKSOCK(ipnet_rtnetlink_route_resolve(rt));
            }
            --neigh->remaining_app_probes;
        }
        break;

    case IPNET_ND_REACHABLE:
        ipnet_neigh_set_state(domain, neigh, IPNET_ND_STALE, IP_NULL);
        break;

    case IPNET_ND_DELAY:
        ipnet_neigh_set_state(domain, neigh, IPNET_ND_PROBE, IP_NULL);
        break;

    default:
        break;
    }
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

