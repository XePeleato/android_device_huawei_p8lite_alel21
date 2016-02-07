/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_mib2.c,v $ $Revision: 1.50 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_mib2.c,v $
 *     $Author: kenneth $ $Date: 2009-01-12 13:46:36 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
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

#include "ipnet_config.h"

#ifdef IPCOM_USE_MIB2_NEW


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_mib2_api.h>

#include "ipnet_h.h"

#ifdef IPTCP
#include <iptcp.h>
#endif


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

/* Type used in calls to ipcom_route_walk_tree() */
typedef struct Ipnet_mib2_routewalk_struct
{
    int     (*cb)(void *, void *);
    void    *user;
    int     family;
    int     count;
    Ip_bool stop;
}
Ipnet_mib2_routewalk;


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipnet_radvd_get_sysvar(IP_CONST char *ifname, IP_CONST char *key,
                                     IP_CONST char *prefix, int default_val);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_ip_mib_cb_ipNetToMediaTable
 *===========================================================================
 * Description: Callback for ipcom_route_walk(). Will be called once for
 *              each entry in the route table. Skips non link layer entries.
 * Parameters:  rt - pointer to the route table entry.
 *              rw - pointer to the custom route walk structure.
 * Returns:     IP_FALSE (Do not delete entry)
 */
IP_STATIC Ip_bool
ipnet_mib2_ipNetToPhysicalTable(Ipnet_route_entry *rt, Ipnet_mib2_routewalk *rw)
{
    Ipcom_mib2_IpNetToPhysicalEntry entry;
    struct Ip_sockaddr_dl *dl;

    if (rt->netif == IP_NULL || rw->stop == IP_TRUE)
        goto exit;
    ipcom_memset(&entry, 0, sizeof(entry));

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_HIDDEN)    /* Do not use hidden entries */
        && IP_BIT_ISTRUE(rt->hdr.flags, IPNET_RTF_LLINFO))   /* Must be link layer */
    {
        ip_assert(rt->gateway != IP_NULL);
        ip_assert(rt->hdr.key != IP_NULL);
        dl = (struct Ip_sockaddr_dl *)rt->gateway;
        ip_assert(dl->sdl_alen <= 6);

        /* Populate table entry */
        entry.ipNetToPhysicalIfIndex        = rt->netif->ipcom.ifindex;
        if (rw->family == IP_AF_INET)
        {
            entry.ipNetToPhysicalNetAddressType           = 1;     /* ipv4(1) */
            *(Ip_u32 *)entry.ipNetToPhysicalNetAddress    = *(Ip_u32 *)rt->hdr.key;
        }
        else
        {
            entry.ipNetToPhysicalNetAddressType            = 2;    /* ipv6(2) */
            ((Ip_u32 *)entry.ipNetToPhysicalNetAddress)[0] = ((Ip_u32 *)rt->hdr.key)[0];
            ((Ip_u32 *)entry.ipNetToPhysicalNetAddress)[1] = ((Ip_u32 *)rt->hdr.key)[1];
            ((Ip_u32 *)entry.ipNetToPhysicalNetAddress)[2] = ((Ip_u32 *)rt->hdr.key)[2];
            ((Ip_u32 *)entry.ipNetToPhysicalNetAddress)[3] = ((Ip_u32 *)rt->hdr.key)[3];
        }
        ((Ip_u32 *)entry.ipNetToPhysicalPhysAddress)[0]    = 6;
        ipcom_memcpy(&entry.ipNetToPhysicalPhysAddress[4], IP_SOCKADDR_DL_LLADDR(rt->gateway), dl->sdl_alen);
        entry.ipNetToPhysicalLastUpdated    = rt->lastchange;
        entry.ipNetToPhysicalType           = IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_STATIC)
                                              ? 4 : 3; /* dynamic(3), static(4) */
        if (rw->family == IP_AF_INET)
        {
            entry.ipNetToPhysicalState      = 6; /* always unknown(6) for ipv4 */
        }
#ifdef IPCOM_USE_INET6
        else
        {
            Ipnet_ip6_nc_entry *nc;

            nc = IPNET_IP6_NC(rt);
            ip_assert(nc != IP_NULL);
            switch (nc->ne.state)
            {
            case IPNET_ND_REACHABLE :
                entry.ipNetToPhysicalState = 1;  /* reachable(1) */
                break;
            case IPNET_ND_STALE :
                entry.ipNetToPhysicalState = 2;  /* stale(2) */
                break;
            case IPNET_ND_DELAY :
                entry.ipNetToPhysicalState = 3;  /* delay(3) */
                break;
            case IPNET_ND_PROBE :
                entry.ipNetToPhysicalState = 4;  /* probe(4) */
                break;
            case IPNET_ND_INCOMPLETE :
                entry.ipNetToPhysicalState = 7;  /* incomplete(7) */
                break;
            default:
                entry.ipNetToPhysicalState = 6;  /* unknown(6) */
                break;
            }
        }
#endif
        entry.ipNetToPhysicalRowStatus     = 1;  /* active(1) */

        /* Callback function */
        rw->count++;
        if (rw->cb(&entry, rw->user) == 1)
            rw->stop = IP_TRUE;
    }

exit:
    /* Do not delete the entry */
    return IP_FALSE;
}


/*
 *===========================================================================
 *                      ipnet_mib2_ipDefaultRouterTable
 *===========================================================================
 * Description: Callback for ipcom_route_walk(). Will be called once for
 *              each entry in the route table. Skips non default gateway entries
 * Parameters:  rt - pointer to the route table entry.
 *              rw - pointer to the custom route walk structure.
 * Returns:     IP_FALSE (Do not delete entry)
 */
IP_STATIC Ip_bool
ipnet_mib2_ipDefaultRouterTable(Ipnet_route_entry *rt, Ipnet_mib2_routewalk *rw)
{
    Ipcom_mib2_IpDefaultRouterEntry entry;

    if (rt->netif == IP_NULL || rw->stop == IP_TRUE)
        goto exit;
    ipcom_memset(&entry, 0, sizeof(entry));

    /* Do not use hidden entries */
    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_HIDDEN)            /* Do not use hidden entries */
        && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO)           /* Must not be link layer */
        && IP_BIT_ISTRUE(rt->hdr.flags, IPNET_RTF_UP)                /* Route must be up */
        && IP_BIT_ISFALSE(rt->netif->ipcom.flags, IP_IFF_LOOPBACK)   /* Skip loopback entries */
        && IP_BIT_ISTRUE(rt->hdr.flags, IPNET_RTF_GATEWAY))          /* Must be a gateway */
    {
        ip_assert(rt->hdr.key != IP_NULL);
        ip_assert(rt->gateway != IP_NULL);

#ifdef IPCOM_USE_INET
        if (rw->family == IP_AF_INET)
        {
            struct Ip_sockaddr_in *sa = (struct Ip_sockaddr_in *)rt->gateway;

            if (*(Ip_u32 *)rt->hdr.key != 0)
                goto exit;

            entry.ipDefaultRouterAddressType        = 1;    /* ipv4(1) */
            *(Ip_u32 *)entry.ipDefaultRouterAddress = sa->sin_addr.s_addr;
        }
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        if (rw->family == IP_AF_INET6)
        {
            struct Ip_sockaddr_in6 *sa = (struct Ip_sockaddr_in6 *)rt->gateway;

            if (!IP_IN6_IS_ADDR_UNSPECIFIED(rt->hdr.key))
                goto exit;

            entry.ipDefaultRouterAddressType            = 2;    /* ipv6(2) */
            ((Ip_u32 *)entry.ipDefaultRouterAddress)[0] = sa->sin6_addr.in6.addr32[0];
            ((Ip_u32 *)entry.ipDefaultRouterAddress)[1] = sa->sin6_addr.in6.addr32[1];
            ((Ip_u32 *)entry.ipDefaultRouterAddress)[2] = sa->sin6_addr.in6.addr32[2];
            ((Ip_u32 *)entry.ipDefaultRouterAddress)[3] = sa->sin6_addr.in6.addr32[3];
        }
#endif /* #ifdef IPCOM_USE_INET6 */

        entry.ipDefaultRouterIfIndex    = rt->netif->ipcom.ifindex;
        entry.ipDefaultRouterLifetime   = rt->tmo == IP_NULL ? 65535 : (rt->tmo->msec - ipnet->msec_now) / 1000;
        entry.ipDefaultRouterPreference = 0;     /* medium(0) */

        /* Callback function */
        rw->count++;
        if (rw->cb(&entry, rw->user) == 1)
            rw->stop = IP_TRUE;
    }

exit:
    /* Do not delete the entry */
    return IP_FALSE;
}


/*
 *===========================================================================
 *                      ipnet_mib2_inetCidrRouteTable
 *===========================================================================
 * Description: Callback for ipcom_route_walk(). Will be called once for
 *              each entry in the route table. Skips link layer entries.
 * Parameters:  rt - pointer to the route table entry.
 *              rw - pointer to the custom route walk structure.
 * Returns:     IP_FALSE (Do not delete entry)
 */
IP_STATIC Ip_bool
ipnet_mib2_inetCidrRouteTable(Ipnet_route_entry *rt, Ipnet_mib2_routewalk *rw)
{
    Ipcom_mib2_InetCidrRouteEntry entry;

    if (rt->netif == IP_NULL || rw->stop == IP_TRUE)
        goto exit;
    ipcom_memset(&entry, 0, sizeof(entry));

    /* Do not use hidden entries */
    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_HIDDEN)            /* Do not use hidden entries */
        && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO)           /* Must not be link layer */
        && IP_BIT_ISTRUE(rt->hdr.flags, IPNET_RTF_UP)                /* Route must be up */
        && IP_BIT_ISFALSE(rt->netif->ipcom.flags, IP_IFF_LOOPBACK))  /* Skip loopback entries */
    {
        ip_assert(rt->hdr.key != IP_NULL);

#ifdef IPCOM_USE_INET
        if (rw->family == IP_AF_INET)
        {
            struct Ip_sockaddr_in *sa;

            /* Do not use multicast entries */
            if(IP_IN_CLASSD(*(Ip_u32 *)rt->hdr.key))
                goto exit;

            /* Do not use broadcast entries */
            if(ip_ntohl(*(Ip_u32 *)rt->hdr.key) == IP_INADDR_BROADCAST)
                goto exit;

            entry.inetCidrRouteDestType        = 1;    /* ipv4(1) */
            *(Ip_u32 *)entry.inetCidrRouteDest = *(Ip_u32 *)rt->hdr.key;
            entry.inetCidrRoutePfxLen          = rt->hdr.mask == IP_NULL
                                                 ? 32 : ipcom_mask_to_prefixlen(rt->hdr.mask, 32);
            if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
            {
                ip_assert(rt->gateway != IP_NULL);
                sa = (struct Ip_sockaddr_in *)rt->gateway;
                entry.inetCidrRouteNextHopType        = 1;    /* ipv4(1) */
                *(Ip_u32 *)entry.inetCidrRouteNextHop = sa->sin_addr.s_addr;
            }
            else
            {
                entry.inetCidrRouteNextHopType        = 0;    /* unknown(0) */
                *(Ip_u32 *)entry.inetCidrRouteNextHop = 0;
            }
        }
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        if (rw->family == IP_AF_INET6)
        {
            struct Ip_sockaddr_in6 *sa;

            /* Skip multicast addresses */
            if (IP_IN6_IS_ADDR_MULTICAST((struct Ip_in6_addr *)rt->hdr.key))
                goto exit;

            entry.inetCidrRouteDestType            = 2;    /* ipv6(2) */
            ((Ip_u32 *)entry.inetCidrRouteDest)[0] = ((Ip_u32 *)rt->hdr.key)[0];
            ((Ip_u32 *)entry.inetCidrRouteDest)[1] = ((Ip_u32 *)rt->hdr.key)[1];
            ((Ip_u32 *)entry.inetCidrRouteDest)[2] = ((Ip_u32 *)rt->hdr.key)[2];
            ((Ip_u32 *)entry.inetCidrRouteDest)[3] = ((Ip_u32 *)rt->hdr.key)[3];
            entry.inetCidrRoutePfxLen              = rt->hdr.mask == IP_NULL
                                                     ? 128 : ipcom_mask_to_prefixlen(rt->hdr.mask, 128);
            if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
            {
                ip_assert(rt->gateway != IP_NULL);
                sa = (struct Ip_sockaddr_in6 *)rt->gateway;
                entry.inetCidrRouteNextHopType            = 2;    /* ipv6(2) */
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[0] = sa->sin6_addr.in6.addr32[0];
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[1] = sa->sin6_addr.in6.addr32[1];
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[2] = sa->sin6_addr.in6.addr32[2];
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[3] = sa->sin6_addr.in6.addr32[3];
            }
            else
            {
                entry.inetCidrRouteNextHopType            = 0;    /* unknown(0) */
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[0] = 0;
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[1] = 0;
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[2] = 0;
                ((Ip_u32 *)entry.inetCidrRouteNextHop)[3] = 0;
            }
        }
#endif /* #ifdef IPCOM_USE_INET6 */

        if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_STATIC))
        {
            if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
                entry.inetCidrRouteProto = 3;    /* netmgt(3) */
            else
                entry.inetCidrRouteProto = 2;    /* local(2) */
        }
        else if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_DYNAMIC))
            entry.inetCidrRouteProto     = 4;    /* icmp(4) */
        else
            entry.inetCidrRouteProto     = 1;    /* other(1) */

        if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_BLACKHOLE))
            entry.inetCidrRouteType      = 5;    /* blackhole(5) */
        else if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_REJECT))
            entry.inetCidrRouteType      = 2;    /* reject(2) */
        else if(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
            entry.inetCidrRouteType      = 4;    /* remote(4) */
        else
            entry.inetCidrRouteType      = 3;    /* local(3) */

        entry.inetCidrRoutePolicy[0]     = 2;
        entry.inetCidrRoutePolicy[1]     = 0;
        entry.inetCidrRoutePolicy[2]     = 0;
        entry.inetCidrRouteIfIndex       = rt->netif->ipcom.ifindex;
        entry.inetCidrRouteAge           = (ipnet->msec_now - rt->msec_changed) / 1000;
        entry.inetCidrRouteNextHopAS     = 0;
        entry.inetCidrRouteMetric1       = (Ipcom_mib2_Integer32)rt->metrics.rmx_mtu;
        entry.inetCidrRouteMetric2       = (Ipcom_mib2_Integer32)rt->metrics.rmx_hopcount;
        entry.inetCidrRouteMetric3       = (Ipcom_mib2_Integer32)rt->metrics.rmx_expire;
        entry.inetCidrRouteMetric4       = (Ipcom_mib2_Integer32)rt->metrics.rmx_rtt;
        entry.inetCidrRouteMetric5       = (Ipcom_mib2_Integer32)rt->metrics.rmx_rttvar;
        entry.inetCidrRouteStatus        = 1;    /* active(1) */

        /* Callback function */
        rw->count++;
        if (rw->cb(&entry, rw->user) == 1)
            rw->stop = IP_TRUE;
    }

exit:
    /* Do not delete the entry */
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_mib2_inetCidrRouteNumberCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_inetCidrRouteTable().
 *              Only used to be able to calculate the number of entries.
 * Parameters:  entry - pointer to the route table entry.
 *              user  - pointer to the user derfined structure.
 * Returns:     0
 */
IP_STATIC int
ipnet_mib2_inetCidrRouteNumberCb(Ipcom_mib2_InetCidrRouteEntry *entry, void *user)
{
    (void)entry;
    (void)user;

    return 0;
}


/*
 ****************************************************************************
 * 10                  GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                  PUBLIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipcom_mib2_get_ip_group
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_ip_group(Ipcom_mib2_ip_group *group)
{
    void *suspend_handle;

    if (group == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    group->ipInReceives      = ipnet->mib2.ipInReceives;
    group->ipInHdrErrors     = ipnet->mib2.ipInHdrErrors;
    group->ipInAddrErrors    = ipnet->mib2.ipInAddrErrors;
    group->ipForwDatagrams   = ipnet->mib2.ipForwDatagrams;
    group->ipInUnknownProtos = ipnet->mib2.ipInUnknownProtos;
    group->ipInDiscards      = ipnet->mib2.ipInDiscards;
    group->ipInDelivers      = ipnet->mib2.ipInDelivers;
    group->ipOutRequests     = ipnet->mib2.ipOutRequests;
    group->ipOutDiscards     = ipnet->mib2.ipOutDiscards;
    group->ipOutNoRoutes     = ipnet->mib2.ipOutNoRoutes;
    group->ipReasmTimeout    = ipnet->mib2.ipReasmTimeout;
    group->ipReasmReqds      = ipnet->mib2.ipReasmReqds;
    group->ipReasmOKs        = ipnet->mib2.ipReasmOKs;
    group->ipReasmFails      = ipnet->mib2.ipReasmFails;
    group->ipFragOKs         = ipnet->mib2.ipFragOKs;
    group->ipFragFails       = ipnet->mib2.ipFragFails;
    group->ipFragCreates     = ipnet->mib2.ipFragCreates;
    group->ipRoutingDiscards = ipnet->mib2.ipRoutingDiscards;

    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_get_icmp_group
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_icmp_group(Ipcom_mib2_icmp_group *group)
{
    void *suspend_handle;

    if (group == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    group->icmpInMsgs           = ipnet->mib2.icmpInMsgs;
    group->icmpInErrors         = ipnet->mib2.icmpInErrors;
    group->icmpInDestUnreachs   = ipnet->mib2.icmpInDestUnreachs;
    group->icmpInTimeExcds      = ipnet->mib2.icmpInTimeExcds;
    group->icmpInParmProbs      = ipnet->mib2.icmpInParmProbs;
    group->icmpInSrcQuenchs     = ipnet->mib2.icmpInSrcQuenchs;
    group->icmpInRedirects      = ipnet->mib2.icmpInRedirects;
    group->icmpInEchos          = ipnet->mib2.icmpInEchos;
    group->icmpInEchoReps       = ipnet->mib2.icmpInEchoReps;
    group->icmpInTimestamps     = ipnet->mib2.icmpInTimestamps;
    group->icmpInTimestampReps  = ipnet->mib2.icmpInTimestampReps;
    group->icmpInAddrMasks      = ipnet->mib2.icmpInAddrMasks;
    group->icmpInAddrMaskReps   = ipnet->mib2.icmpInAddrMaskReps;
    group->icmpOutMsgs          = ipnet->mib2.icmpOutMsgs;
    group->icmpOutErrors        = ipnet->mib2.icmpOutErrors;
    group->icmpOutDestUnreachs  = ipnet->mib2.icmpOutDestUnreachs;
    group->icmpOutTimeExcds     = ipnet->mib2.icmpOutTimeExcds;
    group->icmpOutParmProbs     = ipnet->mib2.icmpOutParmProbs;
    group->icmpOutSrcQuenchs    = ipnet->mib2.icmpOutSrcQuenchs;
    group->icmpOutRedirects     = ipnet->mib2.icmpOutRedirects;
    group->icmpOutEchos         = ipnet->mib2.icmpOutEchos;
    group->icmpOutEchoReps      = ipnet->mib2.icmpOutEchoReps;
    group->icmpOutTimestamps    = ipnet->mib2.icmpOutTimestamps;
    group->icmpOutTimestampReps = ipnet->mib2.icmpOutTimestampReps;
    group->icmpOutAddrMasks     = ipnet->mib2.icmpOutAddrMasks;
    group->icmpOutAddrMaskReps  = ipnet->mib2.icmpOutAddrMaskReps;

    ipnet_resume_stack(suspend_handle);
    return 0;
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_mib2_get_inetCidrRouteNumber
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_inetCidrRouteNumber(Ipcom_mib2_Gauge32 *inetCidrRouteNumber)
{
    int count;

    if (inetCidrRouteNumber == IP_NULL)
        return -1;

    count = ipcom_mib2_iterate_inetCidrRouteTable(ipnet_mib2_inetCidrRouteNumberCb, IP_NULL);
    if (count < 0)
        return -1;
    *inetCidrRouteNumber = (Ipcom_mib2_Gauge32)count;
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_get_inetCidrRouteDiscards
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_inetCidrRouteDiscards(Ipcom_mib2_Counter32 *inetCidrRouteDiscards)
{
    void *suspend_handle;
    Ip_u32 num = 0;

    if (inetCidrRouteDiscards == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET
    num += ipnet->mib2.ipRoutingDiscards;
#endif
#ifdef IPCOM_USE_INET6
    num += ipnet->mib2.ipv6DiscardedRoutes;
#endif
    ipnet_resume_stack(suspend_handle);

    *inetCidrRouteDiscards = num;
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_get_tcp_group
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_tcp_group(Ipcom_mib2_tcp_group *group)
{
    void *suspend_handle;

    if (group == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    group->tcpRtoAlgorithm = ipnet->mib2.tcpRtoAlgorithm;
    group->tcpRtoMin       = ipnet->mib2.tcpRtoMin;
    group->tcpRtoMax       = ipnet->mib2.tcpRtoMax;
    group->tcpMaxConn      = ipnet->mib2.tcpMaxConn;
    group->tcpActiveOpens  = ipnet->mib2.tcpActiveOpens;
    group->tcpPassiveOpens = ipnet->mib2.tcpPassiveOpens;
    group->tcpAttemptFails = ipnet->mib2.tcpAttemptFails;
    group->tcpEstabResets  = ipnet->mib2.tcpEstabResets;
    group->tcpCurrEstab    = ipnet_stats_tcp_established();
    group->tcpInSegs       = ipnet->mib2.tcpInSegs;
    group->tcpOutSegs      = ipnet->mib2.tcpOutSegs;
    group->tcpRetransSegs  = ipnet->mib2.tcpRetransSegs;
    group->tcpInErrs       = ipnet->mib2.tcpInErrs;
    group->tcpOutRsts      = ipnet->mib2.tcpOutRsts;
    IP_U64_COPY(group->tcpHCInSegs, ipnet->mib2.tcpHCInSegs);
    IP_U64_COPY(group->tcpHCOutSegs, ipnet->mib2.tcpHCOutSegs);

    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_get_udp_group
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_udp_group(Ipcom_mib2_udp_group *group)
{
    void *suspend_handle;

    if (group == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    group->udpInDatagrams  = ipnet->mib2.udpInDatagrams;
    group->udpNoPorts      = ipnet->mib2.udpNoPorts;
    group->udpInErrors     = ipnet->mib2.udpInErrors;
    group->udpOutDatagrams = ipnet->mib2.udpOutDatagrams;
    IP_U64_COPY(group->udpHCInDatagrams, ipnet->mib2.udpHCInDatagrams);
    IP_U64_COPY(group->udpHCOutDatagrams, ipnet->mib2.udpHCOutDatagrams);

    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_get_ip_general_group
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET
IP_PUBLIC int
ipcom_mib2_get_ip_general_group(Ipcom_mib2_ip_general_group *group)
{
    void *suspend_handle;

    if (group == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    group->ipForwarding   = IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT) ? 1 : 2;
    group->ipDefaultTTL   = (Ipcom_mib2_Integer32)ipcom_sysvar_get_as_int0("ipnet.inet.BaseHopLimit", 64);
    group->ipReasmTimeout = (Ipcom_mib2_Integer32)ipnet->mib2.ipReasmTimeout;
    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipForwarding
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipForwarding(Ipcom_mib2_Integer ipForwarding)
{
    void *suspend_handle;
    int ret = 0;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    if (ipForwarding == 1)
        IP_BIT_SET(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT);
    else if (ipForwarding == 2)
        IP_BIT_CLR(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT);
    else
        ret = -1;
    ipnet_resume_stack(suspend_handle);
    return ret;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipDefaultTTL
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipDefaultTTL(Ipcom_mib2_Integer ipDefaultTTL)
{
    void *suspend_handle;
    int  ret = 0;
    char buf[16];

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    if (ipDefaultTTL < 1 || ipDefaultTTL > 255 )
        ret = -1;
    else
    {
        ipcom_sprintf(buf, "%d", ipDefaultTTL);
        if (ipcom_sysvar_set("ipnet.inet.BaseHopLimit",
                             buf,
                             IPCOM_SYSVAR_FLAG_OVERWRITE) != IPCOM_SUCCESS)
        {
            ret = -1;
        }
    }
    ipnet_resume_stack(suspend_handle);
    return ret;
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_mib2_get_ipv6_general_group
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET6
IP_PUBLIC int
ipcom_mib2_get_ipv6_general_group(Ipcom_mib2_ipv6_general_group *group)
{
    void *suspend_handle;

    if (group == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    group->ipv6IpForwarding      = IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT) ? 1 : 2;
    group->ipv6IpDefaultHopLimit = (Ipcom_mib2_Integer32)ipcom_sysvar_get_as_int0("ipnet.inet6.BaseHopLimit", 64);
    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipv6IpForwarding
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipv6IpForwarding(Ipcom_mib2_Integer ipv6IpForwarding)
{
    void *suspend_handle;
    int ret = 0;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    if (ipv6IpForwarding == 1)
        IP_BIT_SET(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT);
    else if (ipv6IpForwarding == 2)
        IP_BIT_CLR(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT);
    else
        ret = -1;
    ipnet_resume_stack(suspend_handle);
    return ret;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipv6IpDefaultHopLimit
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipv6IpDefaultHopLimit(Ipcom_mib2_Integer ipv6IpDefaultHopLimit)
{
    void *suspend_handle;
    int  ret = 0;
    char buf[16];

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    if (ipv6IpDefaultHopLimit < 0 || ipv6IpDefaultHopLimit > 255 )
        ret = -1;
    else
    {
        ipcom_sprintf(buf, "%d", ipv6IpDefaultHopLimit);
        if (ipcom_sysvar_set("ipnet.inet6.BaseHopLimit",
                             buf,
                             IPCOM_SYSVAR_FLAG_OVERWRITE) != IPCOM_SUCCESS)
        {
            ret = -1;
        }
    }
    ipnet_resume_stack(suspend_handle);
    return ret;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_mib2_get_ipv4InterfaceTableLastChange
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET
IP_PUBLIC int
ipcom_mib2_get_ipv4InterfaceTableLastChange(Ipcom_mib2_TimeStamp *timestamp)
{
    void *suspend_handle;

    if (timestamp == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    *timestamp = (Ipcom_mib2_TimeStamp)ipnet->mib2.ipv4InterfaceTableLastChange;
    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipv4InterfaceEnableStatus
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipv4InterfaceEnableStatus(Ipcom_mib2_InterfaceIndex ifindex,
                                         Ipcom_mib2_Integer ipv4InterfaceEnableStatus)
{
    void *suspend_handle;
    int ret = 0;
    Ipnet_netif *netif;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, (Ip_u32)ifindex);
    if (netif != IP_NULL)
    {
        if (ipv4InterfaceEnableStatus == 1)
        {
        }
        else if (ipv4InterfaceEnableStatus == 2)
        {
        }
        else
            ret = -1;
    }
    else
        ret = -1;
    ipnet_resume_stack(suspend_handle);
    return ret;
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_mib2_get_ipv6InterfaceTableLastChange
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET6
IP_PUBLIC int
ipcom_mib2_get_ipv6InterfaceTableLastChange(Ipcom_mib2_TimeStamp *timestamp)
{
    void *suspend_handle;

    if (timestamp == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    *timestamp = (Ipcom_mib2_TimeStamp)ipnet->mib2.ipv6InterfaceTableLastChange;
    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipv6InterfaceEnableStatus
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipv6InterfaceEnableStatus(Ipcom_mib2_InterfaceIndex ifindex,
                                         Ipcom_mib2_Integer ipv6InterfaceEnableStatus)
{
    void *suspend_handle;
    int ret = 0;
    Ipnet_netif *netif;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, (Ip_u32)ifindex);
    if (netif != IP_NULL)
    {
        if (ipv6InterfaceEnableStatus == 1)
        {
        }
        else if (ipv6InterfaceEnableStatus == 2)
        {
        }
        else
            ret = -1;
    }
    else
        ret = -1;
    ipnet_resume_stack(suspend_handle);
    return ret;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipv6InterfaceForwarding
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipv6InterfaceForwarding(Ipcom_mib2_InterfaceIndex ifindex,
                                       Ipcom_mib2_Integer ipv6InterfaceForwarding)
{
    void *suspend_handle;
    int         ret = 0;
    Ipnet_netif *netif;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, (Ip_u32)ifindex);
    if (netif != IP_NULL)
    {
        if (ipv6InterfaceForwarding == 1)
        {
            IP_BIT_CLR(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD);
        }
        else if (ipv6InterfaceForwarding == 2)
        {
            IP_BIT_SET(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD);
        }
        else
            ret = -1;
    }
    else
        ret = -1;
    ipnet_resume_stack(suspend_handle);
    return ret;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_mib2_get_ipIfStatsTableLastChange
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_ipIfStatsTableLastChange(Ipcom_mib2_TimeStamp *timestamp)
{
    void *suspend_handle;

    if (timestamp == IP_NULL)
        return -1;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    *timestamp = (Ipcom_mib2_TimeStamp)ipnet->mib2.ipIfStatsTableLastChange;
    ipnet_resume_stack(suspend_handle);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_get_ipAddressSpinLock
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_get_ipAddressSpinLock(Ipcom_mib2_TestAndIncr *spinlock)
{
    void *suspend_handle;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    *spinlock = (Ipcom_mib2_TestAndIncr)ipnet->mib2.ipAddressSpinLock;
    ipnet_resume_stack(suspend_handle);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipAddressSpinLock
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipAddressSpinLock(Ipcom_mib2_TestAndIncr spinlock)
{
    void *suspend_handle;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    ipnet->mib2.ipAddressSpinLock = (Ip_s32)spinlock;
    ipnet_resume_stack(suspend_handle);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_get_ipv6RouterAdvertSpinLock
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET6
IP_PUBLIC int
ipcom_mib2_get_ipv6RouterAdvertSpinLock(Ipcom_mib2_TestAndIncr *spinlock)
{
    void *suspend_handle;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    *spinlock = (Ipcom_mib2_TestAndIncr)ipnet->mib2.ipv6RouterAdvertSpinLock;
    ipnet_resume_stack(suspend_handle);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_ipv6RouterAdvertSpinLock
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_ipv6RouterAdvertSpinLock(Ipcom_mib2_TestAndIncr spinlock)
{
    void *suspend_handle;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    ipnet->mib2.ipv6RouterAdvertSpinLock = (Ip_s32)spinlock;
    ipnet_resume_stack(suspend_handle);

    return 0;
}
#endif


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipv4InterfaceTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET
IP_PUBLIC int
ipcom_mib2_iterate_ipv4InterfaceTable(Ipcom_mib2_ipv4InterfaceTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int                  i;
    int                           count = 0;
    Ipnet_netif                  *netif;
    Ipcom_mib2_Ipv4InterfaceEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    IPNET_NETIF_FOR_EACH(netif, i)
    {
        ip_assert(netif != IP_NULL);

        /* Populate table entry */
        entry.ipv4InterfaceIfIndex        = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
        entry.ipv4InterfaceReasmMaxSize   = (Ipcom_mib2_Integer32)0xffff;
        entry.ipv4InterfaceEnableStatus   = IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP) ? 1 : 2;
        entry.ipv4InterfaceRetransmitTime = (Ipcom_mib2_Unsigned32)ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "BaseRetransmitTime", 1);

        /* Callback function */
        count++;
        if (cb(&entry, user) == 1)
            break;
    }

    ipnet_resume_stack(suspend_handle);

    return count;
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipv6InterfaceTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET6
IP_PUBLIC int
ipcom_mib2_iterate_ipv6InterfaceTable(Ipcom_mib2_ipv6InterfaceTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int                  i;
    int                           count = 0;
    Ipnet_netif                  *netif;
    struct Ip_in6_addr            tmp;
    Ipcom_mib2_Ipv6InterfaceEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    IPNET_NETIF_FOR_EACH(netif, i)
    {
        ip_assert(netif != IP_NULL);

        /* Populate table entry */
        entry.ipv6InterfaceIfIndex        = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
        entry.ipv6InterfaceReasmMaxSize   = (Ipcom_mib2_Unsigned32)0xffff;
        entry.ipv6InterfaceEnableStatus   = IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP) ? 1 : 2;
        entry.ipv6InterfaceReachableTime  = (Ipcom_mib2_Unsigned32)ipnet_sysvar_netif_get_as_int(IP_AF_INET6, netif, "BaseReachableTime", 1);
        entry.ipv6InterfaceRetransmitTime = (Ipcom_mib2_Unsigned32)ipnet_sysvar_netif_get_as_int(IP_AF_INET6, netif, "BaseRetransmitTime", 1);
        entry.ipv6InterfaceForwarding     = IP_BIT_ISSET(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD) ? 2 : 1;
        ((Ip_u32 *)entry.ipv6InterfaceIdentifier)[0] = 8;
        if (netif->link_ip6_create_addr != IP_NULL)
            netif->link_ip6_create_addr(netif, &ip_in6addr_any, &tmp);
        else
            ipcom_memset(&tmp, 0, sizeof(tmp));
        ipcom_memcpy(&entry.ipv6InterfaceIdentifier[4], &tmp.in6.addr8[8], 8);

        /* Callback function */
        count++;
        if (cb(&entry, user) == 1)
            break;
    }

    ipnet_resume_stack(suspend_handle);

    return count;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipSystemStatsTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_ipSystemStatsTable(Ipcom_mib2_ipSystemStatsTableCb cb, void *user)
{
    void *suspend_handle;
    int                            count = 0;
    Ipcom_mib2_IpSystemStatsEntry *entry;

    if (cb == IP_NULL)
        return -1;

    entry = ipcom_malloc(sizeof(*entry));
    if (entry == IP_NULL)
        return -1;
    ipcom_memset(entry, 0, sizeof(*entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
    {
        ipcom_free(entry);
        return -1;
    }

#ifdef IPCOM_USE_INET

    /* Populate table entry */
    entry->ipSystemStatsIPVersion = 1; /* ipv4 */
    IP_U64_COPY(entry->ipSystemStatsHCInReceives,       ipnet->mib2.syswide.v4.ipSystemStatsHCInReceives);
    IP_U64_COPY(entry->ipSystemStatsHCInOctets,         ipnet->mib2.syswide.v4.ipSystemStatsHCInOctets);
    IP_U64_COPY(entry->ipSystemStatsHCInForwDatagrams,  ipnet->mib2.syswide.v4.ipSystemStatsHCInForwDatagrams);
    IP_U64_COPY(entry->ipSystemStatsHCInDelivers,       ipnet->mib2.syswide.v4.ipSystemStatsHCInDelivers);
    IP_U64_COPY(entry->ipSystemStatsHCOutRequests,      ipnet->mib2.syswide.v4.ipSystemStatsHCOutRequests);
    IP_U64_COPY(entry->ipSystemStatsHCOutForwDatagrams, ipnet->mib2.syswide.v4.ipSystemStatsHCOutForwDatagrams);
    IP_U64_COPY(entry->ipSystemStatsHCOutTransmits,     ipnet->mib2.syswide.v4.ipSystemStatsHCOutTransmits);
    IP_U64_COPY(entry->ipSystemStatsHCOutOctets,        ipnet->mib2.syswide.v4.ipSystemStatsHCOutOctets);
    IP_U64_COPY(entry->ipSystemStatsHCInMcastPkts,      ipnet->mib2.syswide.v4.ipSystemStatsHCInMcastPkts);
    IP_U64_COPY(entry->ipSystemStatsHCInMcastOctets,    ipnet->mib2.syswide.v4.ipSystemStatsHCInMcastOctets);
    IP_U64_COPY(entry->ipSystemStatsHCOutMcastPkts,     ipnet->mib2.syswide.v4.ipSystemStatsHCOutMcastPkts);
    IP_U64_COPY(entry->ipSystemStatsHCOutMcastOctets,   ipnet->mib2.syswide.v4.ipSystemStatsHCOutMcastOctets);
    IP_U64_COPY(entry->ipSystemStatsHCInBcastPkts,      ipnet->mib2.syswide.v4.ipSystemStatsHCInBcastPkts);
    IP_U64_COPY(entry->ipSystemStatsHCOutBcastPkts,     ipnet->mib2.syswide.v4.ipSystemStatsHCOutBcastPkts);
    entry->ipSystemStatsInReceives        = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCInReceives);
    entry->ipSystemStatsInOctets          = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCInOctets);
    entry->ipSystemStatsInForwDatagrams   = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCInForwDatagrams);
    entry->ipSystemStatsInDelivers        = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCInDelivers);
    entry->ipSystemStatsOutRequests       = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCOutRequests);
    entry->ipSystemStatsOutForwDatagrams  = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCOutForwDatagrams);
    entry->ipSystemStatsOutTransmits      = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCOutTransmits);
    entry->ipSystemStatsOutOctets         = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCOutOctets);
    entry->ipSystemStatsInMcastPkts       = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCInMcastPkts);
    entry->ipSystemStatsInMcastOctets     = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCInMcastOctets);
    entry->ipSystemStatsOutMcastPkts      = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCOutMcastPkts);
    entry->ipSystemStatsOutMcastOctets    = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCOutMcastOctets);
    entry->ipSystemStatsInBcastPkts       = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCInBcastPkts);
    entry->ipSystemStatsOutBcastPkts      = IP_U64_GETLO(ipnet->mib2.syswide.v4.ipSystemStatsHCOutBcastPkts);
    entry->ipSystemStatsOutNoRoutes       = ipnet->mib2.syswide.v4.ipSystemStatsOutNoRoutes;
    entry->ipSystemStatsInHdrErrors       = ipnet->mib2.syswide.v4.ipSystemStatsInHdrErrors;
    entry->ipSystemStatsInNoRoutes        = ipnet->mib2.syswide.v4.ipSystemStatsInNoRoutes;
    entry->ipSystemStatsInAddrErrors      = ipnet->mib2.syswide.v4.ipSystemStatsInAddrErrors;
    entry->ipSystemStatsInUnknownProtos   = ipnet->mib2.syswide.v4.ipSystemStatsInUnknownProtos;
    entry->ipSystemStatsInTruncatedPkts   = ipnet->mib2.syswide.v4.ipSystemStatsInTruncatedPkts;
    entry->ipSystemStatsReasmReqds        = ipnet->mib2.syswide.v4.ipSystemStatsReasmReqds;
    entry->ipSystemStatsReasmOKs          = ipnet->mib2.syswide.v4.ipSystemStatsReasmOKs;
    entry->ipSystemStatsReasmFails        = ipnet->mib2.syswide.v4.ipSystemStatsReasmFails;
    entry->ipSystemStatsInDiscards        = ipnet->mib2.syswide.v4.ipSystemStatsInDiscards;
    entry->ipSystemStatsOutDiscards       = ipnet->mib2.syswide.v4.ipSystemStatsOutDiscards;
    entry->ipSystemStatsOutFragReqds      = ipnet->mib2.syswide.v4.ipSystemStatsOutFragReqds;
    entry->ipSystemStatsOutFragOKs        = ipnet->mib2.syswide.v4.ipSystemStatsOutFragOKs;
    entry->ipSystemStatsOutFragFails      = ipnet->mib2.syswide.v4.ipSystemStatsOutFragFails;
    entry->ipSystemStatsOutFragCreates    = ipnet->mib2.syswide.v4.ipSystemStatsOutFragCreates;
    entry->ipSystemStatsDiscontinuityTime = 0;
    entry->ipSystemStatsRefreshRate       = 0;

    /* Callback function */
    count++;
    if (cb(entry, user) == 1)
        goto exit;
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6

    /* Populate table entry */
    entry->ipSystemStatsIPVersion = 2; /* ipv6 */
    IP_U64_COPY(entry->ipSystemStatsHCInReceives,       ipnet->mib2.syswide.v6.ipSystemStatsHCInReceives);
    IP_U64_COPY(entry->ipSystemStatsHCInOctets,         ipnet->mib2.syswide.v6.ipSystemStatsHCInOctets);
    IP_U64_COPY(entry->ipSystemStatsHCInForwDatagrams,  ipnet->mib2.syswide.v6.ipSystemStatsHCInForwDatagrams);
    IP_U64_COPY(entry->ipSystemStatsHCInDelivers,       ipnet->mib2.syswide.v6.ipSystemStatsHCInDelivers);
    IP_U64_COPY(entry->ipSystemStatsHCOutRequests,      ipnet->mib2.syswide.v6.ipSystemStatsHCOutRequests);
    IP_U64_COPY(entry->ipSystemStatsHCOutForwDatagrams, ipnet->mib2.syswide.v6.ipSystemStatsHCOutForwDatagrams);
    IP_U64_COPY(entry->ipSystemStatsHCOutTransmits,     ipnet->mib2.syswide.v6.ipSystemStatsHCOutTransmits);
    IP_U64_COPY(entry->ipSystemStatsHCOutOctets,        ipnet->mib2.syswide.v6.ipSystemStatsHCOutOctets);
    IP_U64_COPY(entry->ipSystemStatsHCInMcastPkts,      ipnet->mib2.syswide.v6.ipSystemStatsHCInMcastPkts);
    IP_U64_COPY(entry->ipSystemStatsHCInMcastOctets,    ipnet->mib2.syswide.v6.ipSystemStatsHCInMcastOctets);
    IP_U64_COPY(entry->ipSystemStatsHCOutMcastPkts,     ipnet->mib2.syswide.v6.ipSystemStatsHCOutMcastPkts);
    IP_U64_COPY(entry->ipSystemStatsHCOutMcastOctets,   ipnet->mib2.syswide.v6.ipSystemStatsHCOutMcastOctets);
    IP_U64_COPY(entry->ipSystemStatsHCInBcastPkts,      ipnet->mib2.syswide.v6.ipSystemStatsHCInBcastPkts);
    IP_U64_COPY(entry->ipSystemStatsHCOutBcastPkts,     ipnet->mib2.syswide.v6.ipSystemStatsHCOutBcastPkts);
    entry->ipSystemStatsInReceives        = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCInReceives);
    entry->ipSystemStatsInOctets          = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCInOctets);
    entry->ipSystemStatsInForwDatagrams   = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCInForwDatagrams);
    entry->ipSystemStatsInDelivers        = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCInDelivers);
    entry->ipSystemStatsOutRequests       = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCOutRequests);
    entry->ipSystemStatsOutForwDatagrams  = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCOutForwDatagrams);
    entry->ipSystemStatsOutTransmits      = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCOutTransmits);
    entry->ipSystemStatsOutOctets         = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCOutOctets);
    entry->ipSystemStatsInMcastPkts       = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCInMcastPkts);
    entry->ipSystemStatsInMcastOctets     = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCInMcastOctets);
    entry->ipSystemStatsOutMcastPkts      = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCOutMcastPkts);
    entry->ipSystemStatsOutMcastOctets    = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCOutMcastOctets);
    entry->ipSystemStatsInBcastPkts       = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCInBcastPkts);
    entry->ipSystemStatsOutBcastPkts      = IP_U64_GETLO(ipnet->mib2.syswide.v6.ipSystemStatsHCOutBcastPkts);
    entry->ipSystemStatsOutNoRoutes       = ipnet->mib2.syswide.v6.ipSystemStatsOutNoRoutes;
    entry->ipSystemStatsInHdrErrors       = ipnet->mib2.syswide.v6.ipSystemStatsInHdrErrors;
    entry->ipSystemStatsInNoRoutes        = ipnet->mib2.syswide.v6.ipSystemStatsInNoRoutes;
    entry->ipSystemStatsInAddrErrors      = ipnet->mib2.syswide.v6.ipSystemStatsInAddrErrors;
    entry->ipSystemStatsInUnknownProtos   = ipnet->mib2.syswide.v6.ipSystemStatsInUnknownProtos;
    entry->ipSystemStatsInTruncatedPkts   = ipnet->mib2.syswide.v6.ipSystemStatsInTruncatedPkts;
    entry->ipSystemStatsReasmReqds        = ipnet->mib2.syswide.v6.ipSystemStatsReasmReqds;
    entry->ipSystemStatsReasmOKs          = ipnet->mib2.syswide.v6.ipSystemStatsReasmOKs;
    entry->ipSystemStatsReasmFails        = ipnet->mib2.syswide.v6.ipSystemStatsReasmFails;
    entry->ipSystemStatsInDiscards        = ipnet->mib2.syswide.v6.ipSystemStatsInDiscards;
    entry->ipSystemStatsOutDiscards       = ipnet->mib2.syswide.v6.ipSystemStatsOutDiscards;
    entry->ipSystemStatsOutFragReqds      = ipnet->mib2.syswide.v6.ipSystemStatsOutFragReqds;
    entry->ipSystemStatsOutFragOKs        = ipnet->mib2.syswide.v6.ipSystemStatsOutFragOKs;
    entry->ipSystemStatsOutFragFails      = ipnet->mib2.syswide.v6.ipSystemStatsOutFragFails;
    entry->ipSystemStatsOutFragCreates    = ipnet->mib2.syswide.v6.ipSystemStatsOutFragCreates;
    entry->ipSystemStatsDiscontinuityTime = 0;
    entry->ipSystemStatsRefreshRate       = 0;


    /* Callback function */
    count++;
    if (cb(entry, user) == 1)
        goto exit;
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    ipcom_free(entry);
    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipIfStatsTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_ipIfStatsTable(Ipcom_mib2_ipIfStatsTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int               i;
    int                        count = 0;
    Ipnet_netif               *netif;
    Ipcom_mib2_IpIfStatsEntry *entry;

    if (cb == IP_NULL)
        return -1;

    entry = ipcom_malloc(sizeof(*entry));
    if (entry == IP_NULL)
        return -1;
    ipcom_memset(entry, 0, sizeof(*entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
    {
        ipcom_free(entry);
        return -1;
    }

#ifdef IPCOM_USE_INET
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        ip_assert(netif != IP_NULL);

        /* Populate table entry */
        entry->ipIfStatsIPVersion = 1; /* ipv4 */
        entry->ipIfStatsIfIndex   = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
        IP_U64_COPY(entry->ipIfStatsHCInReceives,       netif->ipcom.mib2.perif.v4.ipIfStatsHCInReceives);
        IP_U64_COPY(entry->ipIfStatsHCInOctets,         netif->ipcom.mib2.perif.v4.ipIfStatsHCInOctets);
        IP_U64_COPY(entry->ipIfStatsHCInForwDatagrams,  netif->ipcom.mib2.perif.v4.ipIfStatsHCInForwDatagrams);
        IP_U64_COPY(entry->ipIfStatsHCInDelivers,       netif->ipcom.mib2.perif.v4.ipIfStatsHCInDelivers);
        IP_U64_COPY(entry->ipIfStatsHCOutRequests,      netif->ipcom.mib2.perif.v4.ipIfStatsHCOutRequests);
        IP_U64_COPY(entry->ipIfStatsHCOutForwDatagrams, netif->ipcom.mib2.perif.v4.ipIfStatsHCOutForwDatagrams);
        IP_U64_COPY(entry->ipIfStatsHCOutTransmits,     netif->ipcom.mib2.perif.v4.ipIfStatsHCOutTransmits);
        IP_U64_COPY(entry->ipIfStatsHCOutOctets,        netif->ipcom.mib2.perif.v4.ipIfStatsHCOutOctets);
        IP_U64_COPY(entry->ipIfStatsHCInMcastPkts,      netif->ipcom.mib2.perif.v4.ipIfStatsHCInMcastPkts);
        IP_U64_COPY(entry->ipIfStatsHCInMcastOctets,    netif->ipcom.mib2.perif.v4.ipIfStatsHCInMcastOctets);
        IP_U64_COPY(entry->ipIfStatsHCOutMcastPkts,     netif->ipcom.mib2.perif.v4.ipIfStatsHCOutMcastPkts);
        IP_U64_COPY(entry->ipIfStatsHCOutMcastOctets,   netif->ipcom.mib2.perif.v4.ipIfStatsHCOutMcastOctets);
        IP_U64_COPY(entry->ipIfStatsHCInBcastPkts,      netif->ipcom.mib2.perif.v4.ipIfStatsHCInBcastPkts);
        IP_U64_COPY(entry->ipIfStatsHCOutBcastPkts,     netif->ipcom.mib2.perif.v4.ipIfStatsHCOutBcastPkts);
        entry->ipIfStatsInReceives        = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCInReceives);
        entry->ipIfStatsInOctets          = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCInOctets);
        entry->ipIfStatsInForwDatagrams   = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCInForwDatagrams);
        entry->ipIfStatsInDelivers        = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCInDelivers);
        entry->ipIfStatsOutRequests       = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCOutRequests);
        entry->ipIfStatsOutForwDatagrams  = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCOutForwDatagrams);
        entry->ipIfStatsOutTransmits      = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCOutTransmits);
        entry->ipIfStatsOutOctets         = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCOutOctets);
        entry->ipIfStatsInMcastPkts       = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCInMcastPkts);
        entry->ipIfStatsInMcastOctets     = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCInMcastOctets);
        entry->ipIfStatsOutMcastPkts      = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCOutMcastPkts);
        entry->ipIfStatsOutMcastOctets    = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCOutMcastOctets);
        entry->ipIfStatsInBcastPkts       = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCInBcastPkts);
        entry->ipIfStatsOutBcastPkts      = IP_U64_GETLO(netif->ipcom.mib2.perif.v4.ipIfStatsHCOutBcastPkts);
        entry->ipIfStatsInHdrErrors       = netif->ipcom.mib2.perif.v4.ipIfStatsInHdrErrors;
        entry->ipIfStatsInNoRoutes        = netif->ipcom.mib2.perif.v4.ipIfStatsInNoRoutes;
        entry->ipIfStatsInAddrErrors      = netif->ipcom.mib2.perif.v4.ipIfStatsInAddrErrors;
        entry->ipIfStatsInUnknownProtos   = netif->ipcom.mib2.perif.v4.ipIfStatsInUnknownProtos;
        entry->ipIfStatsInTruncatedPkts   = netif->ipcom.mib2.perif.v4.ipIfStatsInTruncatedPkts;
        entry->ipIfStatsReasmReqds        = netif->ipcom.mib2.perif.v4.ipIfStatsReasmReqds;
        entry->ipIfStatsReasmOKs          = netif->ipcom.mib2.perif.v4.ipIfStatsReasmOKs;
        entry->ipIfStatsReasmFails        = netif->ipcom.mib2.perif.v4.ipIfStatsReasmFails;
        entry->ipIfStatsInDiscards        = netif->ipcom.mib2.perif.v4.ipIfStatsInDiscards;
        entry->ipIfStatsOutDiscards       = netif->ipcom.mib2.perif.v4.ipIfStatsOutDiscards;
        entry->ipIfStatsOutFragReqds      = netif->ipcom.mib2.perif.v4.ipIfStatsOutFragReqds;
        entry->ipIfStatsOutFragOKs        = netif->ipcom.mib2.perif.v4.ipIfStatsOutFragOKs;
        entry->ipIfStatsOutFragFails      = netif->ipcom.mib2.perif.v4.ipIfStatsOutFragFails;
        entry->ipIfStatsOutFragCreates    = netif->ipcom.mib2.perif.v4.ipIfStatsOutFragCreates;
        entry->ipIfStatsDiscontinuityTime = 0;
        entry->ipIfStatsRefreshRate       = 0;

        /* Callback function */
        count++;
        if (cb(entry, user) == 1)
            goto exit;
    }
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        ip_assert(netif != IP_NULL);

        /* Populate table entry */
        entry->ipIfStatsIPVersion = 2; /* ipv6 */
        entry->ipIfStatsIfIndex   = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
        IP_U64_COPY(entry->ipIfStatsHCInReceives,       netif->ipcom.mib2.perif.v6.ipIfStatsHCInReceives);
        IP_U64_COPY(entry->ipIfStatsHCInOctets,         netif->ipcom.mib2.perif.v6.ipIfStatsHCInOctets);
        IP_U64_COPY(entry->ipIfStatsHCInForwDatagrams,  netif->ipcom.mib2.perif.v6.ipIfStatsHCInForwDatagrams);
        IP_U64_COPY(entry->ipIfStatsHCInDelivers,       netif->ipcom.mib2.perif.v6.ipIfStatsHCInDelivers);
        IP_U64_COPY(entry->ipIfStatsHCOutRequests,      netif->ipcom.mib2.perif.v6.ipIfStatsHCOutRequests);
        IP_U64_COPY(entry->ipIfStatsHCOutForwDatagrams, netif->ipcom.mib2.perif.v6.ipIfStatsHCOutForwDatagrams);
        IP_U64_COPY(entry->ipIfStatsHCOutTransmits,     netif->ipcom.mib2.perif.v6.ipIfStatsHCOutTransmits);
        IP_U64_COPY(entry->ipIfStatsHCOutOctets,        netif->ipcom.mib2.perif.v6.ipIfStatsHCOutOctets);
        IP_U64_COPY(entry->ipIfStatsHCInMcastPkts,      netif->ipcom.mib2.perif.v6.ipIfStatsHCInMcastPkts);
        IP_U64_COPY(entry->ipIfStatsHCInMcastOctets,    netif->ipcom.mib2.perif.v6.ipIfStatsHCInMcastOctets);
        IP_U64_COPY(entry->ipIfStatsHCOutMcastPkts,     netif->ipcom.mib2.perif.v6.ipIfStatsHCOutMcastPkts);
        IP_U64_COPY(entry->ipIfStatsHCOutMcastOctets,   netif->ipcom.mib2.perif.v6.ipIfStatsHCOutMcastOctets);
        IP_U64_COPY(entry->ipIfStatsHCInBcastPkts,      netif->ipcom.mib2.perif.v6.ipIfStatsHCInBcastPkts);
        IP_U64_COPY(entry->ipIfStatsHCOutBcastPkts,     netif->ipcom.mib2.perif.v6.ipIfStatsHCOutBcastPkts);
        entry->ipIfStatsInReceives        = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCInReceives);
        entry->ipIfStatsInOctets          = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCInOctets);
        entry->ipIfStatsInForwDatagrams   = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCInForwDatagrams);
        entry->ipIfStatsInDelivers        = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCInDelivers);
        entry->ipIfStatsOutRequests       = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCOutRequests);
        entry->ipIfStatsOutForwDatagrams  = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCOutForwDatagrams);
        entry->ipIfStatsOutTransmits      = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCOutTransmits);
        entry->ipIfStatsOutOctets         = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCOutOctets);
        entry->ipIfStatsInMcastPkts       = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCInMcastPkts);
        entry->ipIfStatsInMcastOctets     = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCInMcastOctets);
        entry->ipIfStatsOutMcastPkts      = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCOutMcastPkts);
        entry->ipIfStatsOutMcastOctets    = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCOutMcastOctets);
        entry->ipIfStatsInBcastPkts       = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCInBcastPkts);
        entry->ipIfStatsOutBcastPkts      = IP_U64_GETLO(netif->ipcom.mib2.perif.v6.ipIfStatsHCOutBcastPkts);
        entry->ipIfStatsInHdrErrors       = netif->ipcom.mib2.perif.v6.ipIfStatsInHdrErrors;
        entry->ipIfStatsInNoRoutes        = netif->ipcom.mib2.perif.v6.ipIfStatsInNoRoutes;
        entry->ipIfStatsInAddrErrors      = netif->ipcom.mib2.perif.v6.ipIfStatsInAddrErrors;
        entry->ipIfStatsInUnknownProtos   = netif->ipcom.mib2.perif.v6.ipIfStatsInUnknownProtos;
        entry->ipIfStatsInTruncatedPkts   = netif->ipcom.mib2.perif.v6.ipIfStatsInTruncatedPkts;
        entry->ipIfStatsReasmReqds        = netif->ipcom.mib2.perif.v6.ipIfStatsReasmReqds;
        entry->ipIfStatsReasmOKs          = netif->ipcom.mib2.perif.v6.ipIfStatsReasmOKs;
        entry->ipIfStatsReasmFails        = netif->ipcom.mib2.perif.v6.ipIfStatsReasmFails;
        entry->ipIfStatsInDiscards        = netif->ipcom.mib2.perif.v6.ipIfStatsInDiscards;
        entry->ipIfStatsOutDiscards       = netif->ipcom.mib2.perif.v6.ipIfStatsOutDiscards;
        entry->ipIfStatsOutFragReqds      = netif->ipcom.mib2.perif.v6.ipIfStatsOutFragReqds;
        entry->ipIfStatsOutFragOKs        = netif->ipcom.mib2.perif.v6.ipIfStatsOutFragOKs;
        entry->ipIfStatsOutFragFails      = netif->ipcom.mib2.perif.v6.ipIfStatsOutFragFails;
        entry->ipIfStatsOutFragCreates    = netif->ipcom.mib2.perif.v6.ipIfStatsOutFragCreates;
        entry->ipIfStatsDiscontinuityTime = 0;
        entry->ipIfStatsRefreshRate       = 0;

        /* Callback function */
        count++;
        if (cb(entry, user) == 1)
            goto exit;
    }
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    ipcom_free(entry);
    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipAddressPrefixTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_ipAddressPrefixTable(Ipcom_mib2_ipAddressPrefixTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int                    i;
    int                             count = 0;
    Ipnet_netif                     *netif;
    Ipcom_mib2_IpAddressPrefixEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        struct Ipnet_ip4_addr_entry_struct *addr;

        ip_assert(netif != IP_NULL);

        addr = netif->inet4_addr_list;
        while(addr != IP_NULL && !IP_IN_CLASSD(addr->ipaddr_n))
        {
            /* Populate table entry */
            entry.ipAddressPrefixIfIndex              = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
            entry.ipAddressPrefixType                 = 1; /* ipv4(1) */
            *(Ip_u32 *)entry.ipAddressPrefixPrefix    = addr->ipaddr_n & addr->netmask_n;
            entry.ipAddressPrefixLength               = ipcom_mask_to_prefixlen(&addr->netmask_n, 32);
            if (IP_BIT_ISSET(addr->flags, IPNET_IP4_ADDR_FLAG_AUTOMATIC) ||
                IP_BIT_ISSET(addr->flags, IPNET_IP4_ADDR_FLAG_LINK_LOCAL))
                entry.ipAddressPrefixOrigin           = 3; /* wellknown(3) */
            else
                entry.ipAddressPrefixOrigin           = 2; /* manual(2) */
            entry.ipAddressPrefixOnLinkFlag           = 1; /* true(1)  - default for ipv4 */
            entry.ipAddressPrefixAutonomousFlag       = 2; /* false(2) - default for ipv4 */
            entry.ipAddressPrefixAdvPreferredLifetime = 4294967295UL; /* default for ipv4 */
            entry.ipAddressPrefixAdvValidLifetime     = 4294967295UL; /* default for ipv4 */

            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;

            addr = addr->next;
        }
    }
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        struct Ipnet_ip6_addr_entry_struct *addr;
        Ip_u8 mask[16];
        int j;

        ip_assert(netif != IP_NULL);

        addr = netif->inet6_addr_list;
        while(addr != IP_NULL && !IP_IN6_IS_ADDR_MULTICAST(&addr->addr))
        {
            /* Populate table entry */
            entry.ipAddressPrefixIfIndex              = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
            entry.ipAddressPrefixType                 = 2; /* ipv6(2) */
            ipcom_memset(mask, 0, sizeof(mask));
            ipnet_route_create_mask(mask, addr->prefixlen);
            for (j = 0; j < 16; j++)
                entry.ipAddressPrefixPrefix[j]        = addr->addr.in6.addr8[j] & mask[j];
            entry.ipAddressPrefixLength               = addr->prefixlen;
            if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_AUTOMATIC))
                entry.ipAddressPrefixOrigin           = 3; /* wellknown(3) */
            else if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS))
                entry.ipAddressPrefixOrigin           = 5; /* routeradv(5) */
            else
                entry.ipAddressPrefixOrigin           = 2; /* manual(2) */
            /* TODO : Check cloning flag too below */
            entry.ipAddressPrefixOnLinkFlag           = IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS)
                ? 1 /* true(1) */ : 2; /* false(2) */
            entry.ipAddressPrefixAutonomousFlag       = IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS)
                ? 1 /* true(1) */ : 2; /* false(2) */
            if (addr->preferred_sec == IPCOM_ADDR_INFINITE)
                entry.ipAddressPrefixAdvPreferredLifetime = IPCOM_ADDR_INFINITE;
            else
                entry.ipAddressPrefixAdvPreferredLifetime = (ipnet->msec_now / 1000) > addr->preferred_sec
                    ? 0 : addr->preferred_sec - (ipnet->msec_now / 1000);
            entry.ipAddressPrefixAdvValidLifetime         = addr->expires_sec == IPCOM_ADDR_INFINITE
                ? IPCOM_ADDR_INFINITE : addr->expires_sec - (ipnet->msec_now / 1000);
            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;

            addr = addr->next;
        }
    }
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipAddressTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_ipAddressTable(Ipcom_mib2_ipAddressTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int              i;
    int                       count = 0;
    Ipnet_netif               *netif;
    Ipcom_mib2_IpAddressEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        struct Ipnet_ip4_addr_entry_struct *addr;

        ip_assert(netif != IP_NULL);

        addr = netif->inet4_addr_list;
        while(addr != IP_NULL && !IP_IN_CLASSD(addr->ipaddr_n))
        {
            /* Populate table entry */
            entry.ipAddressAddrType        = 1; /* ipv4(1) */
            *(Ip_u32 *)entry.ipAddressAddr = addr->ipaddr_n;
            entry.ipAddressIfIndex         = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
            entry.ipAddressType            = addr->type == IPNET_ADDR_TYPE_UNICAST ? 1 : 3; /* unicast(1) broadcast(3) */
            entry.ipAddressPrefix[0]       = 2;
            entry.ipAddressPrefix[1]       = 0;
            entry.ipAddressPrefix[2]       = 0;
            entry.ipAddressOrigin          = (ip_ntohl(addr->ipaddr_n) & 0xffff0000) == 0xa9fe0000 &&  /* 169.254.x.x */
                !IP_BIT_ISSET(addr->flags, IPNET_IP4_ADDR_FLAG_LINK_LOCAL)
                ? 6 /* random(6) */ : 2; /* manual(2) */
            entry.ipAddressStatus          = 1; /* always preferred(1) for ipv4 */
            entry.ipAddressCreated         = 0;
            entry.ipAddressLastChanged     = 0;
            entry.ipAddressRowStatus       = 1; /* active(1) */
            entry.ipAddressStorageType     = 2; /* volatile(2) */

            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;

            addr = addr->next;
        }
    }
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        struct Ipnet_ip6_addr_entry_struct *addr;

        ip_assert(netif != IP_NULL);

        addr = netif->inet6_addr_list;
        while(addr != IP_NULL && !IP_IN6_IS_ADDR_MULTICAST(&addr->addr))
        {
            /* Populate table entry */
            entry.ipAddressAddrType            = IP_IN6_IS_ADDR_LINK_LOCAL(&addr->addr) ? 4 : 2; /* ipv6(2) ipv6z(4) */
            ((Ip_u32 *)entry.ipAddressAddr)[0] = addr->addr.in6.addr32[0];
            ((Ip_u32 *)entry.ipAddressAddr)[1] = addr->addr.in6.addr32[1];
            ((Ip_u32 *)entry.ipAddressAddr)[2] = addr->addr.in6.addr32[2];
            ((Ip_u32 *)entry.ipAddressAddr)[3] = addr->addr.in6.addr32[3];
            ((Ip_u32 *)entry.ipAddressAddr)[4] = IP_IN6_IS_ADDR_LINK_LOCAL(&addr->addr) ? (Ip_u32)netif->ipcom.ifindex : 0;
            entry.ipAddressIfIndex             = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
            entry.ipAddressType                = addr->type == IPNET_ADDR_TYPE_UNICAST ? 1 : 2; /* unicast(1) anycast(2) */
            entry.ipAddressPrefix[0]           = 2;
            entry.ipAddressPrefix[1]           = 0;
            entry.ipAddressPrefix[2]           = 0;
            if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY))
                entry.ipAddressOrigin          = 6; /* random(6) */
            else if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS))
                entry.ipAddressOrigin          = 5; /* linklayer(5) */
            else
                entry.ipAddressOrigin          = 2; /* manual(2) */
            if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
                entry.ipAddressStatus          = 6; /* tentative(6) */
            else if (addr->preferred_sec != IPCOM_ADDR_INFINITE && (ipnet->msec_now / 1000) > addr->preferred_sec)
                entry.ipAddressStatus          = 2; /* deprecated(2) */
            else
                entry.ipAddressStatus          = 1; /* preferred(1) */
            entry.ipAddressCreated             = 0;
            entry.ipAddressLastChanged         = 0;
            entry.ipAddressRowStatus           = 1; /* active(1) */
            entry.ipAddressStorageType         = 2; /* volatile(2) */

            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;

            addr = addr->next;
        }
    }
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipAddrTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_ipAddrTable(Ipcom_mib2_ipAddrTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int              i;
    int                       count = 0;
    Ipnet_netif               *netif;
    Ipcom_mib2_IpAddrEntry    entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        struct Ipnet_ip4_addr_entry_struct *addr;

        ip_assert(netif != IP_NULL);

        addr = netif->inet4_addr_list;
        while(addr != IP_NULL && !IP_IN_CLASSD(addr->ipaddr_n))
        {
            /* Populate table entry */
            *(Ip_u32 *)entry.ipAdEntAddr    = addr->ipaddr_n;
            entry.ipAdEntIfIndex            = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
            *(Ip_u32 *)entry.ipAdEntNetMask = addr->netmask_n;
            entry.ipAdEntBcastAddr          = ip_ntohl((addr->ipaddr_n & addr->netmask_n) | ~addr->netmask_n) & 1;
            entry.ipAdEntReasmMaxSize       = 65535;

            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;

            addr = addr->next;
        }
    }
#endif /* #ifdef IPCOM_USE_INET */

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipNetToPhysicalTableCb
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_ipNetToPhysicalTable(Ipcom_mib2_ipNetToPhysicalTableCb cb, void *user)
{
    void *suspend_handle;
    Ipnet_mib2_routewalk rw;
    Ipcom_route          *rtab;

    if (cb == IP_NULL)
        return -1;

    rw.cb    = (int (*)(void *, void *))cb;
    rw.user  = user;
    rw.count = 0;
    rw.stop  = IP_FALSE;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET
    /* For now handle only the default route table */
    if (ipnet_route_get_rtab(IP_AF_INET, 0, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) < 0)
    {
        rw.count = -1;
        goto exit;
    }

    rw.family = IP_AF_INET;
    ipcom_route_walk_tree(rtab,
                          (Ip_bool(*)(Ipcom_route_entry *, void *))
                          ipnet_mib2_ipNetToPhysicalTable, &rw);

#endif /* #ifdef IPCOM_USE_INET */

    if (rw.stop == IP_TRUE)
        goto exit;

#ifdef IPCOM_USE_INET6
    /* For now handle only the default route table */
    if (ipnet_route_get_rtab(IP_AF_INET6, 0, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) < 0)
    {
        rw.count = -1;
        goto exit;
    }

    rw.family = IP_AF_INET6;
    ipcom_route_walk_tree(rtab,
                          (Ip_bool(*)(Ipcom_route_entry *, void *))
                          ipnet_mib2_ipNetToPhysicalTable, &rw);
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    return rw.count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipv6ScopeZoneIndexTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET6
IP_PUBLIC int
ipcom_mib2_iterate_ipv6ScopeZoneIndexTable(Ipcom_mib2_ipv6ScopeZoneIndexTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int                       i;
    int                                count = 0;
    Ipnet_netif                        *netif;
    Ipcom_mib2_Ipv6ScopeZoneIndexEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    IPNET_NETIF_FOR_EACH(netif, i)
    {
        ip_assert(netif != IP_NULL);

        entry.ipv6ScopeZoneIndexIfIndex           = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
        entry.ipv6ScopeZoneIndexLinkLocal         = (Ipcom_mib2_InetZoneIndex)netif->ipcom.ifindex;
        entry.ipv6ScopeZoneIndex3                 = 0;
        entry.ipv6ScopeZoneIndexAdminLocal        = 0;
        entry.ipv6ScopeZoneIndexSiteLocal         = 0;
        entry.ipv6ScopeZoneIndex6                 = 0;
        entry.ipv6ScopeZoneIndex7                 = 0;
        entry.ipv6ScopeZoneIndexOrganizationLocal = 0;
        entry.ipv6ScopeZoneIndex9                 = 0;
        entry.ipv6ScopeZoneIndexA                 = 0;
        entry.ipv6ScopeZoneIndexB                 = 0;
        entry.ipv6ScopeZoneIndexC                 = 0;
        entry.ipv6ScopeZoneIndexD                 = 0;

        /* Callback function */
        count++;
        if (cb(&entry, user) == 1)
            goto exit;
    }

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_ipDefaultRouterTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_ipDefaultRouterTable(Ipcom_mib2_ipDefaultRouterTableCb cb, void *user)
{
    void *suspend_handle;
    Ipnet_mib2_routewalk rw;
    Ipcom_route          *rtab;

    if (cb == IP_NULL)
        return -1;

    rw.cb    = (int (*)(void *, void *))cb;
    rw.user  = user;
    rw.count = 0;
    rw.stop  = IP_FALSE;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET
    /* For now handle only the default route table */
    if (ipnet_route_get_rtab(IP_AF_INET, 0, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) < 0)
    {
        rw.count = -1;
        goto exit;
    }

    rw.family = IP_AF_INET;
    ipcom_route_walk_tree(rtab,
                          (Ip_bool(*)(Ipcom_route_entry *, void *))
                          ipnet_mib2_ipDefaultRouterTable, &rw);
#endif /* #ifdef IPCOM_USE_INET */

    if (rw.stop == IP_TRUE)
        goto exit;

#ifdef IPCOM_USE_INET6
    /* For now handle only the default route table */
    if (ipnet_route_get_rtab(IP_AF_INET6, 0, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) < 0)
    {
        rw.count = -1;
        goto exit;
    }

    rw.family = IP_AF_INET6;
    ipcom_route_walk_tree(rtab,
                          (Ip_bool(*)(Ipcom_route_entry *, void *))
                          ipnet_mib2_ipDefaultRouterTable, &rw);
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    return rw.count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_Ipcom_mib2_ipv6RouterAdvertTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET6
IP_PUBLIC int
ipcom_mib2_iterate_ipv6RouterAdvertTable(Ipcom_mib2_ipv6RouterAdvertTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int                     i;
    int                              count = 0;
    Ipnet_netif                      *netif;
    Ipcom_mib2_Ipv6RouterAdvertEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    IPNET_NETIF_FOR_EACH(netif, i)
    {
        ip_assert(netif != IP_NULL);

        entry.ipv6RouterAdvertIfIndex         = (Ipcom_mib2_InterfaceIndex)netif->ipcom.ifindex;
        entry.ipv6RouterAdvertSendAdverts     = IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_ADV_ACCEPT)
            ? 1 /* true(1) */ : 2; /* false(2) */
        entry.ipv6RouterAdvertMaxInterval     = (Ipcom_mib2_Unsigned32)ipnet_radvd_get_sysvar(netif->ipcom.name,
                                                                                              "MaxRtrAdvInterval",
                                                                                              IP_NULL,
                                                                                              600000) / 1000;
        entry.ipv6RouterAdvertMinInterval     = (Ipcom_mib2_Unsigned32)ipnet_radvd_get_sysvar(netif->ipcom.name,
                                                                                              "MinRtrAdvInterval",
                                                                                              IP_NULL,
                                                                                              entry.ipv6RouterAdvertMaxInterval * 1000 / 3) / 1000;
        entry.ipv6RouterAdvertManagedFlag     = ipnet_radvd_get_sysvar(netif->ipcom.name, "AdvManagedFlag", IP_NULL, 0) == 1
            ? 1 /* true(1) */ : 2; /* false(2) */
        entry.ipv6RouterAdvertOtherConfigFlag = ipnet_radvd_get_sysvar(netif->ipcom.name, "AdvOtherConfigFlag", IP_NULL, 0) == 1
            ? 1 /* true(1) */ : 2; /* false(2) */
        entry.ipv6RouterAdvertLinkMTU         = (Ipcom_mib2_Unsigned32)ipnet_radvd_get_sysvar(netif->ipcom.name, "AdvLinkMTU", IP_NULL, 0);
        entry.ipv6RouterAdvertReachableTime   = (Ipcom_mib2_Unsigned32)ipnet_radvd_get_sysvar(netif->ipcom.name, "AdvReachableTime", IP_NULL, 0);
        entry.ipv6RouterAdvertRetransmitTime  = (Ipcom_mib2_Unsigned32)ipnet_radvd_get_sysvar(netif->ipcom.name, "AdvRetransTimer", IP_NULL, 0);
        entry.ipv6RouterAdvertCurHopLimit     = (Ipcom_mib2_Unsigned32)ipnet_radvd_get_sysvar(netif->ipcom.name, "AdvCurHopLimit", IP_NULL, 64);
        entry.ipv6RouterAdvertDefaultLifetime = (Ipcom_mib2_Unsigned32)ipnet_radvd_get_sysvar(netif->ipcom.name, "AdvDefaultLifetime", IP_NULL,
                                                                                              3000 * entry.ipv6RouterAdvertMaxInterval) / 1000;
        entry.ipv6RouterAdvertRowStatus       = 1; /* active(1) */

        /* Callback function */
        count++;
        if (cb(&entry, user) == 1)
            goto exit;
    }

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_icmpStatsTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_icmpStatsTable(Ipcom_mib2_icmpStatsTableCb cb, void *user)
{
    void *suspend_handle;
    int                       count = 0;
    Ipcom_mib2_IcmpStatsEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET

    /* Populate table entry */
    entry.icmpStatsIPVersion = 1; /* ipv4 */
    entry.icmpStatsInMsgs    = ipnet->mib2.syswide.v4.icmpStatsInMsgs;
    entry.icmpStatsInErrors  = ipnet->mib2.syswide.v4.icmpStatsInErrors;
    entry.icmpStatsOutMsgs   = ipnet->mib2.syswide.v4.icmpStatsOutMsgs;
    entry.icmpStatsOutErrors = ipnet->mib2.syswide.v4.icmpStatsOutErrors;


    /* Callback function */
    count++;
    if (cb(&entry, user) == 1)
        goto exit;
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6

    /* Populate table entry */
    entry.icmpStatsIPVersion = 2; /* ipv6 */
    entry.icmpStatsInMsgs    = ipnet->mib2.syswide.v6.icmpStatsInMsgs;
    entry.icmpStatsInErrors  = ipnet->mib2.syswide.v6.icmpStatsInErrors;
    entry.icmpStatsOutMsgs   = ipnet->mib2.syswide.v6.icmpStatsOutMsgs;
    entry.icmpStatsOutErrors = ipnet->mib2.syswide.v6.icmpStatsOutErrors;

    /* Callback function */
    count++;
    if (cb(&entry, user) == 1)
        goto exit;
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_icmpMsgStatsTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_icmpMsgStatsTable(Ipcom_mib2_icmpMsgStatsTableCb cb, void *user)
{
    void *suspend_handle;
    unsigned int                 i;
    int                          count = 0;
    Ipcom_mib2_IcmpMsgStatsEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET

    /* Populate table entry */
    entry.icmpMsgStatsIPVersion = 1; /* ipv4 */
    for (i = 0; i < 256; i++)
    {
        entry.icmpMsgStatsType    = i;
        entry.icmpMsgStatsInPkts  = ipnet->mib2.syswide.v4.icmpMsgStatsInPkts[i];
        entry.icmpMsgStatsOutPkts = ipnet->mib2.syswide.v4.icmpMsgStatsOutPkts[i];


        /* Callback function */
        count++;
        if (cb(&entry, user) == 1)
            goto exit;
    }
#endif /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6

    /* Populate table entry */
    entry.icmpMsgStatsIPVersion = 2; /* ipv6 */
    for (i = 0; i < 256; i++)
    {
        entry.icmpMsgStatsType    = i;
        entry.icmpMsgStatsInPkts  = ipnet->mib2.syswide.v6.icmpMsgStatsInPkts[i];
        entry.icmpMsgStatsOutPkts = ipnet->mib2.syswide.v6.icmpMsgStatsOutPkts[i];

        /* Callback function */
        count++;
        if (cb(&entry, user) == 1)
            goto exit;
    }
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_tcpConnectionTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_tcpConnectionTable(Ipcom_mib2_tcpConnectionTableCb cb, void *user)
{
    void *suspend_handle;
    int                           i, count = 0;
    Ipcom_mib2_TcpConnectionEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    for(i = 0; i < ipnet_conf_max_sockets; i++)
    {
        if (ipnet->sockets[i] != IP_NULL
            && ipnet->sockets[i]->ipcom.type == IP_SOCK_STREAM
            && ipnet->sockets[i]->proto == IP_IPPROTO_TCP
            && ipnet->sockets[i]->tcb != IP_NULL
            && ipnet->sockets[i]->tcb->state > IPTCP_STATE_LISTEN
            && (ipnet->sockets[i]->ipcom.domain == IP_AF_INET
                || ipnet->sockets[i]->ipcom.domain == IP_AF_INET6))
        {
#ifdef IPCOM_USE_INET
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET)
            {
                entry.tcpConnectionLocalAddressType        = 1; /* ipv4(1) */
                entry.tcpConnectionRemAddressType          = 1; /* ipv4(1) */
                *(Ip_u32 *)entry.tcpConnectionLocalAddress = ipnet->sockets[i]->ip4->saddr_n;
                *(Ip_u32 *)entry.tcpConnectionRemAddress   = ipnet->sockets[i]->ip4->daddr_n;
            }
#endif
#ifdef IPCOM_USE_INET6
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET6)
            {
                entry.tcpConnectionLocalAddressType = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->saddr)
                    ? 4 : 2; /* ipv6(2) ipv6z(4) */
                entry.tcpConnectionRemAddressType   = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->daddr)
                    ? 4 : 2; /* ipv6(2) ipv6z(4) */
                ((Ip_u32 *)entry.tcpConnectionLocalAddress)[0] = ipnet->sockets[i]->ip6->saddr.in6.addr32[0];
                ((Ip_u32 *)entry.tcpConnectionLocalAddress)[1] = ipnet->sockets[i]->ip6->saddr.in6.addr32[1];
                ((Ip_u32 *)entry.tcpConnectionLocalAddress)[2] = ipnet->sockets[i]->ip6->saddr.in6.addr32[2];
                ((Ip_u32 *)entry.tcpConnectionLocalAddress)[3] = ipnet->sockets[i]->ip6->saddr.in6.addr32[3];
                ((Ip_u32 *)entry.tcpConnectionLocalAddress)[4] = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->saddr)
                    ? ipnet->sockets[i]->ip6->iscope_id : 0;
                ((Ip_u32 *)entry.tcpConnectionRemAddress)[0]   = ipnet->sockets[i]->ip6->daddr.in6.addr32[0];
                ((Ip_u32 *)entry.tcpConnectionRemAddress)[1]   = ipnet->sockets[i]->ip6->daddr.in6.addr32[1];
                ((Ip_u32 *)entry.tcpConnectionRemAddress)[2]   = ipnet->sockets[i]->ip6->daddr.in6.addr32[2];
                ((Ip_u32 *)entry.tcpConnectionRemAddress)[3]   = ipnet->sockets[i]->ip6->daddr.in6.addr32[3];
                ((Ip_u32 *)entry.tcpConnectionRemAddress)[4]   = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->daddr)
                    ? ipnet->sockets[i]->ip6->iscope_id : 0;
            }
#endif
            entry.tcpConnectionLocalPort = (Ipcom_mib2_InetPortNumber)ipnet->sockets[i]->sport;
            entry.tcpConnectionRemPort   = (Ipcom_mib2_InetPortNumber)ipnet->sockets[i]->dport;
            entry.tcpConnectionState     = ipnet->sockets[i]->tcb->state;
            entry.tcpConnectionProcess   = (Ip_u32)ipnet->sockets[i]->ipcom.pid;

            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;

        }
    }

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_set_tcpConnectionState
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_set_tcpConnectionState(Ipcom_mib2_TcpConnectionEntry *entry,
                                  Ipcom_mib2_Integer tcpConnectionState)
{
    void *suspend_handle;
    int i;
    Ipnet_socket *sock = IP_NULL;
    Ip_fd fd = IP_INVALID_SOCKET;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    for(i = 0; i < ipnet_conf_max_sockets; i++)
    {
        if (ipnet->sockets[i] != IP_NULL
            && ipnet->sockets[i]->ipcom.type == IP_SOCK_STREAM
            && ipnet->sockets[i]->proto == IP_IPPROTO_TCP
            && ipnet->sockets[i]->tcb != IP_NULL
            && ipnet->sockets[i]->tcb->state > IPTCP_STATE_LISTEN)
        {
#ifdef IPCOM_USE_INET
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET)
            {

                if (entry->tcpConnectionLocalAddressType == 1
                    && *(Ip_u32 *)entry->tcpConnectionLocalAddress == ipnet->sockets[i]->ip4->saddr_n
                    && entry->tcpConnectionLocalPort == ipnet->sockets[i]->sport
                    && entry->tcpConnectionRemAddressType == 1
                    && *(Ip_u32 *)entry->tcpConnectionRemAddress == ipnet->sockets[i]->ip4->daddr_n
                    && entry->tcpConnectionRemPort == ipnet->sockets[i]->dport)
                {
                    /* Matched socket */
                    sock = ipnet->sockets[i];
                    break;
                }
            }
#endif
#ifdef IPCOM_USE_INET6
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET6)
            {

                if ((entry->tcpConnectionLocalAddressType == 2 || entry->tcpConnectionLocalAddressType == 4)
                    && ipcom_memcmp(entry->tcpConnectionLocalAddress,
                                    &ipnet->sockets[i]->ip6->saddr, sizeof(struct Ip_in6_addr)) == 0
                    && entry->tcpConnectionLocalPort == ipnet->sockets[i]->sport
                    && (entry->tcpConnectionRemAddressType == 2 || entry->tcpConnectionRemAddressType == 4)
                    && ipcom_memcmp(entry->tcpConnectionRemAddress,
                                    &ipnet->sockets[i]->ip6->daddr, sizeof(struct Ip_in6_addr)) == 0
                    && entry->tcpConnectionRemPort == ipnet->sockets[i]->dport)
                {
                    if (entry->tcpConnectionLocalAddressType == 4)
                    {
                        if (!IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->saddr)
                            || ((Ip_u32 *)entry->tcpConnectionLocalAddress)[4] != ipnet->sockets[i]->ip6->iscope_id)
                        {
                            continue;
                        }
                    }

                    if (entry->tcpConnectionRemAddressType == 4)
                    {
                        if (!IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->daddr)
                            || ((Ip_u32 *)entry->tcpConnectionRemAddress)[4] != ipnet->sockets[i]->ip6->iscope_id)
                        {
                            continue;
                        }
                    }

                    /* Matched socket */
                    sock = ipnet->sockets[i];
                    break;
                }
            }
#endif
        }
    }

    if (sock != IP_NULL && tcpConnectionState == IPTCP_STATE_DELETE_TCB)
        fd = sock->ipcom.fd;
    ipnet_resume_stack(suspend_handle);
    if (fd == IP_INVALID_SOCKET)
        return -1;

    /* Close the socket */
    (void)ipcom_socketclose(fd);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_tcpListenerTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_tcpListenerTable(Ipcom_mib2_tcpListenerTableCb cb, void *user)
{
    void *suspend_handle;
    int                         i, count = 0;
    Ipcom_mib2_TcpListenerEntry entry;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    for(i = 0; i < ipnet_conf_max_sockets; i++)
    {
        if (ipnet->sockets[i] != IP_NULL
            && ipnet->sockets[i]->ipcom.type == IP_SOCK_STREAM
            && ipnet->sockets[i]->proto == IP_IPPROTO_TCP
            && ipnet->sockets[i]->tcb != IP_NULL
            && ipnet->sockets[i]->tcb->state == IPTCP_STATE_LISTEN
            && (ipnet->sockets[i]->ipcom.domain == IP_AF_INET
                || ipnet->sockets[i]->ipcom.domain == IP_AF_INET6))
        {
#ifdef IPCOM_USE_INET
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET)
            {
                entry.tcpListenerLocalAddressType        = 1; /* ipv4(1) */
                *(Ip_u32 *)entry.tcpListenerLocalAddress = ipnet->sockets[i]->ip4->saddr_n;
            }
#endif
#ifdef IPCOM_USE_INET6
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET6)
            {
                entry.tcpListenerLocalAddressType = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->saddr)
                    ? 4 : 2; /* ipv6(2) ipv6z(4) */
                ((Ip_u32 *)entry.tcpListenerLocalAddress)[0] = ipnet->sockets[i]->ip6->saddr.in6.addr32[0];
                ((Ip_u32 *)entry.tcpListenerLocalAddress)[1] = ipnet->sockets[i]->ip6->saddr.in6.addr32[1];
                ((Ip_u32 *)entry.tcpListenerLocalAddress)[2] = ipnet->sockets[i]->ip6->saddr.in6.addr32[2];
                ((Ip_u32 *)entry.tcpListenerLocalAddress)[3] = ipnet->sockets[i]->ip6->saddr.in6.addr32[3];
                ((Ip_u32 *)entry.tcpListenerLocalAddress)[4] = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->saddr)
                    ? ipnet->sockets[i]->ip6->iscope_id : 0;
            }
#endif
            entry.tcpListenerLocalPort = (Ipcom_mib2_InetPortNumber)ipnet->sockets[i]->sport;
            entry.tcpListenerProcess   = (Ip_u32)ipnet->sockets[i]->ipcom.pid;

            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;

        }
    }

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_udpEndpointTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_udpEndpointTable(Ipcom_mib2_udpEndpointTableCb cb, void *user)
{
    void *suspend_handle;
    int                         i, count = 0;
    Ipcom_mib2_UdpEndpointEntry entry;
    Ipnet_socket                *ipnetsock;

    if (cb == IP_NULL)
        return -1;
    ipcom_memset(&entry, 0, sizeof(entry));

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    for(i = 0; i < ipnet_conf_max_sockets; i++)
    {
        if (ipnet->sockets[i] != IP_NULL
            && ipnet->sockets[i]->ipcom.type == IP_SOCK_DGRAM
            && ipnet->sockets[i]->proto == IP_IPPROTO_UDP
            && (IP_BIT_ISSET(ipnet->sockets[i]->flags, IPNET_SOCKET_FLAG_BOUND)
                || IP_BIT_ISSET(ipnet->sockets[i]->flags, IPNET_SOCKET_FLAG_CONNECTED))
            && (ipnet->sockets[i]->ipcom.domain == IP_AF_INET
                || ipnet->sockets[i]->ipcom.domain == IP_AF_INET6))
        {
#ifdef IPCOM_USE_INET
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET)
            {
                entry.udpEndpointLocalAddressType         = 1; /* ipv4(1) */
                entry.udpEndpointRemoteAddressType        = 1; /* ipv4(1) */
                *(Ip_u32 *)entry.udpEndpointLocalAddress  = ipnet->sockets[i]->ip4->saddr_n;
                *(Ip_u32 *)entry.udpEndpointRemoteAddress = ipnet->sockets[i]->ip4->daddr_n;
            }
#endif
#ifdef IPCOM_USE_INET6
            if (ipnet->sockets[i]->ipcom.domain == IP_AF_INET6)
            {
                entry.udpEndpointLocalAddressType  = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->saddr)
                    ? 4 : 2; /* ipv6(2) ipv6z(4) */
                entry.udpEndpointRemoteAddressType = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->daddr)
                    ? 4 : 2; /* ipv6(2) ipv6z(4) */
                ((Ip_u32 *)entry.udpEndpointLocalAddress)[0]  = ipnet->sockets[i]->ip6->saddr.in6.addr32[0];
                ((Ip_u32 *)entry.udpEndpointLocalAddress)[1]  = ipnet->sockets[i]->ip6->saddr.in6.addr32[1];
                ((Ip_u32 *)entry.udpEndpointLocalAddress)[2]  = ipnet->sockets[i]->ip6->saddr.in6.addr32[2];
                ((Ip_u32 *)entry.udpEndpointLocalAddress)[3]  = ipnet->sockets[i]->ip6->saddr.in6.addr32[3];
                ((Ip_u32 *)entry.udpEndpointLocalAddress)[4]  = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->saddr)
                    ? ipnet->sockets[i]->ip6->iscope_id : 0;
                ((Ip_u32 *)entry.udpEndpointRemoteAddress)[0] = ipnet->sockets[i]->ip6->daddr.in6.addr32[0];
                ((Ip_u32 *)entry.udpEndpointRemoteAddress)[1] = ipnet->sockets[i]->ip6->daddr.in6.addr32[1];
                ((Ip_u32 *)entry.udpEndpointRemoteAddress)[2] = ipnet->sockets[i]->ip6->daddr.in6.addr32[2];
                ((Ip_u32 *)entry.udpEndpointRemoteAddress)[3] = ipnet->sockets[i]->ip6->daddr.in6.addr32[3];
                ((Ip_u32 *)entry.udpEndpointRemoteAddress)[4] = IP_IN6_IS_ADDR_LINK_LOCAL(&ipnet->sockets[i]->ip6->daddr)
                    ? ipnet->sockets[i]->ip6->iscope_id : 0;
            }
#endif
            entry.udpEndpointLocalPort  = (Ipcom_mib2_InetPortNumber)ipnet->sockets[i]->sport;
            entry.udpEndpointRemotePort = (Ipcom_mib2_InetPortNumber)ipnet->sockets[i]->dport;
            entry.udpEndpointInstance   = 1;
            ipnetsock = ipnet->sockets[i];
            while (ipnetsock->prev != IP_NULL)
            {
                entry.udpEndpointInstance++;
                ipnetsock = ipnetsock->prev;
            }
            entry.udpEndpointProcess    = (Ip_u32)ipnet->sockets[i]->ipcom.pid;

            /* Callback function */
            count++;
            if (cb(&entry, user) == 1)
                goto exit;
        }
    }

 exit:
    ipnet_resume_stack(suspend_handle);

    return count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_iterate_inetCidrRouteTable
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_iterate_inetCidrRouteTable(Ipcom_mib2_inetCidrRouteTableCb cb, void *user)
{
    void *suspend_handle;
    Ipnet_mib2_routewalk rw;
    Ipcom_route          *rtab;

    if (cb == IP_NULL)
        return -1;

    rw.cb    = (int (*)(void *, void *))cb;
    rw.user  = user;
    rw.count = 0;
    rw.stop  = IP_FALSE;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

#ifdef IPCOM_USE_INET
    /* For now handle only the default route table */
    if (ipnet_route_get_rtab(IP_AF_INET, 0, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) < 0)
    {
        rw.count = -1;
        goto exit;
    }

    rw.family = IP_AF_INET;
    ipcom_route_walk_tree(rtab,
                          (Ip_bool(*)(Ipcom_route_entry *, void *))
                          ipnet_mib2_inetCidrRouteTable, &rw);
#endif /* #ifdef IPCOM_USE_INET */

    if (rw.stop == IP_TRUE)
        goto exit;

#ifdef IPCOM_USE_INET6
    /* For now handle only the default route table */
    if (ipnet_route_get_rtab(IP_AF_INET6, 0, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) < 0)
    {
        rw.count = -1;
        goto exit;
    }

    rw.family = IP_AF_INET6;
    ipcom_route_walk_tree(rtab,
                          (Ip_bool(*)(Ipcom_route_entry *, void *))
                          ipnet_mib2_inetCidrRouteTable, &rw);
#endif /* #ifdef IPCOM_USE_INET6 */

 exit:
    ipnet_resume_stack(suspend_handle);

    return rw.count;
}


/*
 *===========================================================================
 *                    ipcom_mib2_register_sysUpTimeFunc
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_mib2_register_sysUpTimeFunc(Ipcom_mib2_sysUpTimeFunc func)
{
    void *suspend_handle;

    if (func == IP_NULL)
        return -1;
    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return -1;

    ipnet->mib2.sysUpTimeFp = func;
    ipnet_resume_stack(suspend_handle);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mib2_clear
 *===========================================================================
 * Description: See ipcom_mib2_api.h
 * Parameters:
 * Returns:
 */
IP_PUBLIC void
ipcom_mib2_clear(void)
{
    void *suspend_handle;
    unsigned int i;
    Ipnet_netif  *netif;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        return;

    ipcom_memset(&ipnet->mib2, 0, sizeof(ipnet->mib2));
    IPNET_NETIF_FOR_EACH(netif, i)
    {
        ip_assert(netif != IP_NULL);
        ipcom_memset(&netif->ipcom.mib2, 0, sizeof(netif->ipcom.mib2));
    }

    ipnet_resume_stack(suspend_handle);
}

#else
int ipnet_mib2_empty_file;
#endif /* #ifdef IPCOM_USE_MIB2_NEW */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
