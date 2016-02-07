/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ip4.c,v $ $Revision: 1.666.16.1 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ip4.c,v $
 *     $Author: ulf $
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
 *===========================================================================
 *                         misc
 *===========================================================================
#define IPCOM_USE_PROFILING_ENABLE
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_INET

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_inet.h>
#include <ipcom_prof.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_ip4.h"
#include "ipnet_loopback.h"

#include "ipnet_eth.h"

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPSCTP
#include <ipsctp.h>
#endif
#ifdef IPIPSEC2
#include <ipipsec.h>
#endif

#ifdef IPNET_USE_ROUTESOCK
#include "ipnet_routesock.h"
#endif

#ifdef IPNET_USE_NETLINK
#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"
#endif

#ifdef IPNET_USE_TUNNEL
#include "ipnet_tunnel.h"
#endif

#ifdef IPMPLS
#include "ipnet_mpls.h"
#endif

#ifdef IPNET_USE_VRRP
#include "ipnet_vrrp.h"
#endif

#ifdef IPFIREWALL
#include <ipfirewall.h>
#include <ipfirewall_h.h>
#endif

#ifdef IPROHC
#include <iprohc.h>
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Maximum number of bytes of original IP datagram to send in ICMP error reply. */
#define IPNET_ICMP_MAX_SIZE    256  /* enough to handle tunnels better. */

/*
 *===========================================================================
 *                         misc
 *===========================================================================
 */

#define IPNET_ICMP_HDR_SIZE    (8)


/*
 *===========================================================================
 *                         ICMP for IPv4
 *===========================================================================
 */
#define IPNET_ICMP4_TYPE_ECHO_REPLY            0
#define IPNET_ICMP4_TYPE_DST_UNREACHABLE       3
#define IPNET_ICMP4_TYPE_SOURCEQUENCH          4
#define IPNET_ICMP4_TYPE_REDIRECT              5
#define IPNET_ICMP4_TYPE_ECHO_REQUEST          8
#define IPNET_ICMP4_TYPE_ROUTER_ADVERT         9
#define IPNET_ICMP4_TYPE_ROUTER_SOLICIT       10
#define IPNET_ICMP4_TYPE_TIME_EXCEEDED        11
#define IPNET_ICMP4_TYPE_PARAMPROB            12
#define IPNET_ICMP4_TYPE_TSTAMP_REQUEST       13
#define IPNET_ICMP4_TYPE_TSTAMP_REPLY         14
#define IPNET_ICMP4_TYPE_MASK_REQUEST         17
#define IPNET_ICMP4_TYPE_MASK_REPLY           18

/* [3] Destination unreachable codes */
#define IPNET_ICMP4_CODE_DST_UNREACH_NET      0  /* Network Unreachable */
#define IPNET_ICMP4_CODE_DST_UNREACH_HOST     1  /* Host Unreachable */
#define IPNET_ICMP4_CODE_DST_UNREACH_PROTO    2  /* Protocol Unreachable */
#define IPNET_ICMP4_CODE_DST_UNREACH_PORT     3  /* Port Unreachable */
#define IPNET_ICMP4_CODE_DST_NEEDFRAG         4  /* Fragmentation needed but no frag. bit set */
#define IPNET_ICMP4_CODE_DST_SRCFAIL          5  /* Source routing failed */
#define IPNET_ICMP4_CODE_DST_PROHIBITED_NET   9  /* Destination network administratively prohibited */
#define IPNET_ICMP4_CODE_DST_PROHIBITED_HOST 10  /* Destination host administratively prohibited */
#define IPNET_ICMP4_CODE_DST_PROHIBITED_ADM  13  /* Communication Administratively Prohibited */

/* [5] Redirect codes */
#define IPNET_ICMP4_CODE_RED_NETWORK          0
#define IPNET_ICMP4_CODE_RED_HOST             1
#define IPNET_ICMP4_CODE_RED_TOS_AND_NETWORK  2
#define IPNET_ICMP4_CODE_DST_TOS_AND_HOST     3

/* [11] Time exceeded codes */
#define IPNET_ICMP4_CODE_TIM_TTL              0
#define IPNET_ICMP4_CODE_TIM_REASSEMBLY       1

/* TOS to use for Destination Unreachable,
   Redirect, Time Exceeded, and Parameter Problem */
#define IPNET_ICMP4_TOS_INTERNETWORK_CONTROL  6

/*
 *===========================================================================
 *                         IGMP for IPv4
 *===========================================================================
 */

/* IGMP version 1 is defined in RFC 1112 */
/* IGMP version 2 is defined in RFC 2236 */
/* IGMP version 3 is defined in RFC 3376 */
#define IPNET_IGMP4_TYPE_MEMBERSHIP_QUERY      0x11
#define IPNET_IGMP4_TYPE_V1_MEMBERSHIP_REPORT  0x12
#define IPNET_IGMP4_TYPE_V2_MEMBERSHIP_REPORT  0x16
#define IPNET_IGMP4_TYPE_V2_LEAVE_GROUP        0x17
#define IPNET_IGMP4_TYPE_V3_MEMBERSHIP_REPORT  0x22

/* The interface is running on a link with at least one IGMPv1 host/router */
#define IPNET_IGMPV1_MODE   1
/* The interface is running on a link with at least one IGMPv2 host/router */
#define IPNET_IGMPV2_MODE   2
/* The interface is running in IGMPv3 mode */
#define IPNET_IGMPV3_MODE   3

#ifdef IPNET_USE_RFC3927
/*
                                                <- defended -
                                               |            ^
                                               |            |
     DISABLED -> INIT -> PROBE -> ANNOUNCE -> ACTIVE -> DEFEND
                  ^        |         |           |        |
                  |        |         |           |        |
                  \        /         /           /        /
                   - duplicate <----- <---------- <-------
*/
#define IPNET_IP4_LLADDR_STATE_DISABLED 0  /* Link local IPv4 address not used */
#define IPNET_IP4_LLADDR_STATE_INIT     1  /* Generate link local IPv4 address */
#define IPNET_IP4_LLADDR_STATE_PROBE    2  /* Check if another host has the address */
#define IPNET_IP4_LLADDR_STATE_DEFEND   3  /* Detected another host using the same address
                                              try to make this host the sole owner of it */
#define IPNET_IP4_LLADDR_STATE_ANNOUNCE 4  /* Update the ARP cache of all hosts,
                                              address assigned to interface */
#define IPNET_IP4_LLADDR_STATE_ACTIVE   5  /* Address is in use */
#define IPNET_IP4_LLADDR_STATE_MAX      5
#endif /* IPNET_USE_RFC3927*/

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Search key for IPv4 addresses */
typedef struct Ipnet_ip4_addr_lookup_struct
{
    Ip_u32            ifindex;
    struct Ip_in_addr addr;
    Ip_u16            vr;
}
Ipnet_ip4_addr_lookup;


/* IGMPv3 report callback */
struct Ipnet_igmpv3_report_for_each_data
{
    Ipnet_ip4_addr_entry          *addr_entry;
    Ipnet_pkt_igmpv3_group_record *group_record;
    Ipcom_pkt                     *pkt;
    Ipcom_set                     *set;
};

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
IP_STATIC int
ipnet_ip4_deliver_to_raw_sock(Ipcom_pkt *pkt,
                              Ipnet_ip4_output_param *ip4_param,
                              Ip_bool take_ownership_of_pkt);

IP_STATIC Ip_u32
ipnet_ip4_pseudo_header_checksum_update(IP_CONST struct Ip_in_addr *src,
                                        IP_CONST struct Ip_in_addr *dst,
                                        Ip_u16 proto, Ip_u16 len);


#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_STATIC void
ipnet_igmpv3_create_membership_report(Ipcom_pkt *pkt, Ipnet_ip4_addr_entry *addr_entry);
#endif

IP_STATIC void
ipnet_igmp_report_general_query(Ipnet_ip4_addr_entry *addr_entry);

IP_STATIC void
ipnet_igmp_report_specific_query(Ipnet_ip4_addr_entry *addr_entry);

IP_STATIC void
ipnet_igmp_report_filter_change(Ipnet_ip4_addr_entry *addr_entry);

#ifdef IPNET_USE_RFC1256
IP_STATIC void ipnet_ip4_rfc1256_state_run(Ipnet_netif *netif);
IP_STATIC void ipnet_ip4_rfc1256_advertise_schedule(Ipnet_netif    *netif,
                                                    Ip_u32         tmo);
#endif


#ifdef IPNET_USE_RFC3927
IP_STATIC void
ipnet_ip4_lladdr_set_state(Ipnet_netif *netif, Ip_u8 state);
#endif

IP_GLOBAL struct Ip_sockaddr *
ipnet_ip4_addr_to_sockaddr(struct Ip_sockaddr_in *sin, Ip_u32 in_addr_n);

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
 *                      ipnet_ip4_addr_key_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip4_addr_key_func(Ipnet_ip4_addr_lookup *key)
{
    unsigned v;
    v = ipcom_hash_update(&key->ifindex, sizeof(key->ifindex), 0);
    v = ipcom_hash_update(&key->addr, sizeof(key->addr), v);
    return ipcom_hash_update(&key->vr, sizeof(key->vr), v);
}


/*
 *===========================================================================
 *                      ipnet_ip4_addr_obj_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip4_addr_obj_func(Ipnet_ip4_addr_entry *addr)
{
    unsigned v;
    v = ipcom_hash_update(&addr->netif->ipcom.ifindex, sizeof(addr->netif->ipcom.ifindex), 0);
    v = ipcom_hash_update(&addr->ipaddr_n, sizeof(addr->ipaddr_n), v);
    return ipcom_hash_update(&addr->netif->vr_index, sizeof(addr->netif->vr_index), v);
}


/*
 *===========================================================================
 *                      ipnet_ip4_addr_cmp_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_addr_cmp_func(Ipnet_ip4_addr_entry *addr, Ipnet_ip4_addr_lookup *key)
{
    return addr->netif->ipcom.ifindex == key->ifindex
        && addr->ipaddr_n == key->addr.s_addr
        && addr->netif->vr_index == key->vr;
}


/*
 *===========================================================================
 *                    ipnet_ip4_addr_ignore_if_key_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip4_addr_ignore_if_key_func(Ipnet_ip4_addr_lookup *key)
{
    unsigned v;
    v = ipcom_hash_update(&key->addr, sizeof(key->addr), 0);
    return ipcom_hash_update(&key->vr, sizeof(key->vr), v);
}


/*
 *===========================================================================
 *                    ipnet_ip4_addr_ignore_if_obj_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip4_addr_ignore_if_obj_func(Ipnet_ip4_addr_entry *addr)
{
    unsigned v;
    v = ipcom_hash_update(&addr->ipaddr_n, sizeof(addr->ipaddr_n), 0);
    return ipcom_hash_update(&addr->netif->vr_index, sizeof(addr->netif->vr_index), v);
}


/*
 *===========================================================================
 *                    ipnet_ip4_addr_ignore_if_cmp_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_addr_ignore_if_cmp_func(Ipnet_ip4_addr_entry *addr, Ipnet_ip4_addr_lookup *key)
{
    return addr->ipaddr_n == key->addr.s_addr
        && addr->netif->vr_index == key->vr;
}


/*
 *===========================================================================
 *                    ipnet_ip4_addr_to_sockaddr
 *===========================================================================
 * Description: Creates a socket address structure for the specified
 *                    IPv4 address
 * Parameters:  sin - buffer to store the socket address.
 *              in_addr_n - an IPv4 address
 * Returns:     The socket address for the specified IPv4 address.
 *
 */
IP_GLOBAL struct Ip_sockaddr *
ipnet_ip4_addr_to_sockaddr(struct Ip_sockaddr_in *sin, Ip_u32 in_addr_n)
{
    ipcom_memset(sin, 0, sizeof(struct Ip_sockaddr_in));
    sin->sin_family      = IP_AF_INET;
    IPCOM_SA_LEN_SET(sin, sizeof(struct Ip_sockaddr_in));
    sin->sin_addr.s_addr = in_addr_n;
    return (struct Ip_sockaddr *) sin;
}


/*
 *===========================================================================
 *                  ipnet_ip4_addr_exit_tentative_state
 *===========================================================================
 * Description: Moves an IPv4 address out of tentative state.
 * Parameters:  addr - address entry
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_addr_exit_tentative_state(Ipnet_ip4_addr_entry *addr)
{
    IP_BIT_CLR(addr->flags, IPNET_IP4_ADDR_FLAG_TENTATIVE);
}


/*
 *===========================================================================
 *                    ipnet_ip4_assign_addr
 *===========================================================================
 * Description: Assigns the address to the inteface (which must be UP).
 * Parameters:  addr - The address entry to assign.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip4_assign_addr(Ipnet_ip4_addr_entry *addr)
{
    Ipnet_netif *netif = addr->netif;
    int          ret = 0;

    /* Invalidate the route cache tag since assigning this address
       might have affects on the outcome of lookups */
    ipnet_route_cache_invalidate();

    ip_assert(IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP));

    if (addr->type == IPNET_ADDR_TYPE_MULTICAST)
    {
        if (addr->ipaddr_n != ip_htonl(IP_INADDR_ALLHOSTS_GROUP)
            && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_RUNNING))
        {
            addr->mcast.filter_change_resend_count = (Ip_u8) netif->igmp_robustness_variable;
            ipnet_igmp_report_filter_change(addr);
        }
    }
    else if (addr->type == IPNET_ADDR_TYPE_UNICAST
             && addr->ipaddr_n != IP_INADDR_ANY)
    {
        Ip_bool auto_proxy_arp_enabled;

        auto_proxy_arp_enabled = ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                                                  netif,
                                                                  "AutoProxyArp",
                                                                  IP_FALSE,
                                                                  ipnet_bool_map);

        if (IP_BIT_ISFALSE(addr->flags, IPNET_IP4_ADDR_FLAG_LOOPBACK_RT))
        {
            /* Add route to loopback all packets sent to this address from this host. */
            struct Ipnet_route_add_param param;
            struct Ip_sockaddr_in        gw;

            ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
            if (netif->ipcom.type == IP_IFT_LOOP)
                param.netif = netif;
            else
                param.netif = ipnet_loopback_get_netif(netif->vr_index);
            param.domain  = IP_AF_INET;
            param.vr      = netif->vr_index;
            param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
            param.key     = &addr->ipaddr_n;
            param.flags   = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DONE;
            param.gateway = ipnet_ip4_addr_to_sockaddr(&gw, addr->ipaddr_n);
            if (auto_proxy_arp_enabled)
                 IP_BIT_SET(param.flags, IPNET_RTF_PROTO2);
            ret = ipnet_route_add(&param);
            if (ret < 0)
                goto cleanup;

            IP_BIT_SET(addr->flags, IPNET_IP4_ADDR_FLAG_LOOPBACK_RT);
        }

        if (IP_BIT_ISFALSE(addr->flags, IPNET_IP4_ADDR_FLAG_NETWORK_RT)
            && addr->netmask_n != 0xffffffff)
        {
            /* Add route for the (sub)network reachable by this new address */
            struct Ipnet_route_add_param param;
            Ip_u32                       netaddr_n;
            struct Ip_sockaddr_in        local_addr;

            ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
            param.domain  = IP_AF_INET;
            param.vr      = netif->vr_index;
            param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
            param.netif   = netif;
            netaddr_n     = addr->ipaddr_n & addr->netmask_n;
            param.key     = &netaddr_n;
            param.netmask = &addr->netmask_n;
            param.flags   = IPNET_RTF_UP | IPNET_RTF_DONE | IPNET_RTF_MASK | IPNET_RTF_CLONING;
            if (addr->type == IPNET_ADDR_TYPE_UNICAST)
                /* The gateway field will is the default source address
                   for packets sent to the network for this address */
                param.gateway = ipnet_ip4_addr_to_sockaddr(&local_addr, addr->ipaddr_n);
            if (auto_proxy_arp_enabled)
                 IP_BIT_SET(param.flags, IPNET_RTF_PROTO2);
            ret = ipnet_route_add(&param);
            if (ret >= 0 || ret == -IP_ERRNO_EEXIST)
            {
                IP_BIT_SET(addr->flags, IPNET_IP4_ADDR_FLAG_NETWORK_RT);
                ret = 0;
            }
        }
#ifdef IPCOM_USE_ETHERNET
        if (ret >= 0)
        {
            if ((netif->ipcom.type == IP_IFT_ETHER
                 || netif->ipcom.type == IP_IFT_L2VLAN))
            {
                IP_BIT_CLR(addr->flags, IPNET_IP4_ADDR_FLAG_TENTATIVE);
                if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                                    netif,
                                                    "SendGratuitousArp",
                                                    IP_TRUE,
                                                    ipnet_bool_map))
                {
                    Ip_u32 tentative_duration;

                    tentative_duration = ipnet_sysvar_netif_get_as_int(IP_AF_INET,
                                                                       netif,
                                                                       "BaseRetransmitTime",
                                                                       1);
                    tentative_duration *= 1000; /* to msec */
                    ipnet_timeout_schedule(tentative_duration,
                                           (Ipnet_timeout_handler)ipnet_ip4_addr_exit_tentative_state,
                                           addr,
                                           &addr->tmo);

                    IP_BIT_SET(addr->flags, IPNET_IP4_ADDR_FLAG_TENTATIVE);
                    /* Send gratuitous ARP request. */
                    ipnet_arp_request(netif,
                                      addr->ipaddr_n,
                                      IP_TRUE,
                                      IP_NULL,
                                      IP_NULL);
                }
            }
        }
#endif /* IPCOM_USE_ETHERNET */
    }

    if (IP_BIT_ISFALSE(addr->flags, IPNET_IP4_ADDR_FLAG_NEWADDR_DONE)
        && (addr->type == IPNET_ADDR_TYPE_UNICAST || addr->type == IPNET_ADDR_TYPE_MULTICAST))
    {
        IP_BIT_SET(addr->flags, IPNET_IP4_ADDR_FLAG_NEWADDR_DONE);
        ipnet_kioevent(netif, IP_EIOXNEWADDR, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        IPNET_ROUTESOCK(ipnet_routesock_addr_add(netif, IP_AF_INET, addr));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_ip4_addr_add(netif, addr));
    }

 cleanup:
    if (ret < 0)
    {
        IPCOM_LOG3(NOTICE,
                   "IPv4: Failed to assign address %s to interface %s, code=%d",
                   ipcom_inet_ntop(IP_AF_INET, &addr->ipaddr_n, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   netif->ipcom.name,
                   -ret);
        (void) ipnet_ip4_remove_addr(netif, addr->ipaddr_n);
        return ret;
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_ip4_get_ip_opt
 *===========================================================================
 * Description: Returns a pointer to the specified option in the packet.
 * Parameters:  pkt - packet that might contain a timestamp option.
 *                    pkt->ipstart must be the offset to the IP header.
 * Returns:     A pointer to the option or IP_NULL if no such option is
 *              present.
 *
 */
IP_STATIC void *
ipnet_ip4_get_ip_opt(Ipcom_pkt *pkt, Ip_u8 optid)
{
    Ipnet_pkt_ip_opt *opt;
    int               offset;
    Ipnet_pkt_ip     *ip_hdr;

    if (pkt->data[pkt->ipstart] == 0x45)
        /* No options present in this packet */
        return IP_NULL;

    ip_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    offset = IPNET_IP_HDR_SIZE;
    while (offset < IPNET_IP4_GET_HDR_OCTET_LEN(ip_hdr))
    {
        opt = (Ipnet_pkt_ip_opt *) ((Ip_u8*) ip_hdr + offset);
        if (opt->flag_class_num == optid)
            return opt;

        switch (opt->flag_class_num)
        {
        case IP_IPOPT_END:
            return IP_NULL;
        case IP_IPOPT_NOOP:
            offset++;
            break;
        default:
            /* No check for zero length options since all packets that
               is passed this function has passed the sanity tests in
               ipnet_ip4_process_options() */
            offset += opt->len;
            break;
        }
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip4_process_ts_option
 *===========================================================================
 * Description: Processes the timestamp option
 * Parameters:  ts - The timestamp option buffer.
 *              local_addr_n - The final destination or source address of the packet.
 *              pkt - The packet that holds the option.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip4_process_ts_option(Ipnet_pkt_ip_opt_timestamp *ts,
                            Ip_u32 *local_addr_n,
                            Ipcom_pkt *pkt)
{
    int          ts_len;
    Ip_u32       timestamp;
    Ipnet_netif *netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);

    switch (IPNET_OPT_TS_GET_FLAGS(ts))
    {
    case IP_IPOPT_TS_TSONLY:
        ts_len = 4;
        break;
    case IP_IPOPT_TS_TSANDADDR:
    case IP_IPOPT_TS_PRESPEC:
        ts_len = 8;
        break;
    default:
        /* Unknown format, ignore this option */
        return 0;
    }

    if (ts->pointer < 5 || ((ts->pointer - 5) & (ts_len - 1)) != 0)
        /* RFC791; the Pointer is the number of octets from the beginning of this
           option to the end of timestamps plus one (i.e., it points to the
           octet beginning the space for next timestamp).  The smallest
           legal value is 5.  */
        return -IP_ERRNO_EINVAL;

    if (ts->pointer + ts_len > ts->len + 1)
    {
        if (ts->pointer <= ts->len)
            /* RFC791; The timestamp area is full when the pointer
               is greater than the length. */
            return -IP_ERRNO_EINVAL;
        if (IPNET_OPT_TS_GET_OVERFLOW_COUNT(ts) == 0xf)
            return -IP_ERRNO_ENOSPC;
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_OUTPUT))
            /* The overflow count must only be increased 1 time per IP module */
            IPNET_OPT_TS_INC_OVERFLOW_COUNT(ts);
        return 0;
    }

    /* RFC791, page 22; If the time is not available in milliseconds or
       cannot be provided with respect to midnight UT then any time may
       be inserted as a timestamp provided the high order bit of the
       timestamp field is set to one to indicate the use of a non-standard value.
    */
    timestamp = (ipnet_msec_now() % (1000 * 60 * 60 * 24)) | 0x80000000;

    switch (IPNET_OPT_TS_GET_FLAGS(ts))
    {
    case IP_IPOPT_TS_TSANDADDR:
        if (ipnet_ip4_get_addr_type(IP_GET_32ON16(local_addr_n),
                                    netif->vr_index,
                                    IP_NULL) != IPNET_ADDR_TYPE_NOT_LOCAL)
            ipcom_memcpy(&ts->timestamp[ts->pointer - 5], local_addr_n, 4);
        else
        {
            if (netif->inet4_addr_list == IP_NULL)
                return 0;
            ipcom_memcpy(&ts->timestamp[ts->pointer - 5],
                         &netif->inet4_addr_list->ipaddr_n,
                         4);
        }
        break;
    case IP_IPOPT_TS_PRESPEC:
        if (ipnet_ip4_get_addr_type(IP_GET_32ON8(&ts->timestamp[ts->pointer - 5]),
                                    netif->vr_index,
                                    IP_NULL) == IPNET_ADDR_TYPE_NOT_LOCAL)
            return 0;
        break;
    default:
        break;
    }

    ts->pointer += ts_len;
    IP_SET_HTONL(&ts->timestamp[ts->pointer - 9], timestamp);
    return 0;
}



/*
 *===========================================================================
 *                    ipnet_ip4_process_options
 *===========================================================================
 * Description: Processes IPv4 header options.
 * Parameters:  pkt - Received IPv4 packet. (pkt->start is offset to IPv4 header)
 *              opt_len - Total number of option octets.
 *              ip4_param - The cached IPv4 parameters from the IP header.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip4_process_options(Ipcom_pkt *pkt,
                          Ip_size_t opt_len,
                          Ipnet_ip4_output_param *ip4_param)
{
    Ipnet_pkt_ip_opt *opt;
    Ip_size_t         offset = 0;
    Ipnet_icmp_param  icmp_param;
    int               ret;
    Ip_u32            seen_options = 0;
    Ip_u8             options[40];
    Ipnet_pkt_ip     *ip_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

    /* Copy all options so they can be restored to their original value in case of error */
    ip_assert(IPNET_IP4_GET_OPTS_OCTET_LEN(ip_hdr) <= (int) sizeof(options));
    ipcom_memcpy(options,
                 &pkt->data[pkt->ipstart + IPNET_IP_HDR_SIZE],
                 IPNET_IP4_GET_OPTS_OCTET_LEN(ip_hdr));

    while (offset < opt_len)
    {
        opt = (Ipnet_pkt_ip_opt *) ((Ip_u8*) ip_hdr + IPNET_IP_HDR_SIZE + offset);

        switch (opt->flag_class_num)
        {
        case IP_IPOPT_END:
            return 0;
        case IP_IPOPT_NOOP:
            offset++;
            continue;
        default:
            if (offset + 1 >= opt_len || opt->len == 0 || (offset + opt->len) > opt_len)
            {
                /* byte offset into the packet. */
                icmp_param.data.param_pointer = (Ip_u8)(&opt->len - &pkt->data[pkt->ipstart]);
                goto send_parameter_prob;
            }
            /* Length ok. */
            break;
        }

        icmp_param.data.param_pointer = (Ip_u8)((Ip_u8 *) opt - &pkt->data[pkt->ipstart]);

        /* Verify that options that can only be included once has not been seen before in this datagram */
        switch (opt->flag_class_num)
        {
        case IP_IPOPT_RA:
        case IP_IPOPT_TIMESTAMP:
            if (IP_BIT_ISSET(seen_options, 1 << IP_IPOPT_NUMBER(opt->flag_class_num)))
                goto send_parameter_prob;
            IP_BIT_SET(seen_options, 1 << IP_IPOPT_NUMBER(opt->flag_class_num));
            break;
        default:
            break;
        }

        /* Specific option handling. */
        switch (opt->flag_class_num)
        {
        case IP_IPOPT_RA:
            /* packet contains the router alert option */
            IP_BIT_SET(ip4_param->flags, IPNET_IP4_OPF_ROUTER_ALERT);
            break;

        case IP_IPOPT_TIMESTAMP:
            /* Move parameter pointer to it the length field */
            icmp_param.data.param_pointer++;
            if (opt->len < 4)
                goto send_parameter_prob;
            /* Move parameter pointer to the pointer field */
            icmp_param.data.param_pointer++;
            ret = ipnet_ip4_process_ts_option((Ipnet_pkt_ip_opt_timestamp *) opt,
                                              (Ip_u32 *) ip4_param->to,
                                              pkt);
            if (ret)
            {
                if (ret == -IP_ERRNO_ENOSPC)
                    /* Move parameter pointer to the overflow field */
                    icmp_param.data.param_pointer++;
                goto send_parameter_prob;
            }
            break;

            /* Source routing disabled for security reasons. */
        case IP_IPOPT_LSRR:
        case IP_IPOPT_SSRR:
            icmp_param.type = IPNET_ICMP4_TYPE_DST_UNREACHABLE;
            icmp_param.code = IPNET_ICMP4_CODE_DST_SRCFAIL;
            goto send_icmp;

            /* case IPNET_IP4_OPT_NUM_RECORD_ROUTE:  ignored for security reasons. */

            /* ignore all unknown options. */
        default:
            break;
        }
        offset += opt->len;
    }
    return 0;

 send_parameter_prob:
    icmp_param.type = IPNET_ICMP4_TYPE_PARAMPROB;
    icmp_param.code = 0; /* IP Header is invalid, offset used. */

 send_icmp:
    /* Copy all options so they can be restored to their original value in case of error */
    ipcom_memcpy(&pkt->data[pkt->ipstart + IPNET_IP_HDR_SIZE],
                 options,
                 IPNET_IP4_GET_OPTS_OCTET_LEN(ip_hdr));
    icmp_param.vr_index = pkt->vr_index;
    icmp_param.netif    = IP_NULL;
    icmp_param.recv_pkt = pkt;
    icmp_param.from     = IP_NULL;
    icmp_param.to       = ip4_param->from;

    (void) ipnet_icmp4_send(&icmp_param, IP_FALSE);
    return -IP_ERRNO_ENOSYS;  /* discard packet. */
}


#ifdef IPNET_USE_RFC1256

/* Just stringify the state number/mode */
#define IPNET_RFC1256_MODE(m)       #m
#define IPNET_RFC1256_STATE(m,s)    #s



/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_sysvar_address
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_rfc1256_sysvar_address(Ipnet_netif        *netif,
                                 const char         *key,
                                 struct Ip_in_addr  *addr,
                                 Ip_u32             defaddr)
{
    char        addr_str[40];
    Ip_size_t   addr_str_len = sizeof(addr_str);

    ipcom_strcpy(ipnet->log_buf, netif->ipcom.name);
    ipcom_strcat(ipnet->log_buf, ".ipnet.rtdisc.");
    ipcom_strcat(ipnet->log_buf, key);

    /* */
    if(ipcom_sysvar_get(ipnet->log_buf, addr_str, &addr_str_len) != IP_NULL)
    {
        if(ipcom_inet_pton(IP_AF_INET, addr_str, addr) == 1)
        {
            /* Sanity; must be default or 255.255.255.255 */
            if (addr->s_addr == defaddr || addr->s_addr == IP_INADDR_BROADCAST)
                return;
        }
    }

    /* Store default */
    addr->s_addr = defaddr;
}


/*
 *===========================================================================
 *                  ipnet_ip4_rfc1256_route_remove_auto
 *===========================================================================
 * Description: Callback used when all routes are to be removed from a specified
 *              interface.
 * Parameters:  [in] rt - The current route in the walk.
 *              [in] netif - The interface the route should be using to be removed.
 * Returns:     IP_TRUE - remove this route
 *              IP_FALSE - do not remove this route.
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_rfc1256_route_remove_auto(Ipnet_route_entry *rt, Ipnet_netif *netif)
{
    Ipnet_route_entry *widen;
    Ipnet_route_entry *prev;

    /* Have to check the route with rt->narrow == IP_NULL and rt->prev == IP_NULL
       last since that route MUST be deleted by the caller ("rt" will be equal to
       the passed "rt" value in that case) */
    while (rt->narrow != IP_NULL)
        rt = rt->narrow;
    for (; rt != IP_NULL; rt = widen)
    {
        widen = rt->widen;

        while (rt->next != IP_NULL)
            rt = rt->next;
        for (; rt != IP_NULL; rt = prev)
        {
            prev = rt->prev;
            if (rt->netif == netif
                && IP_BIT_ISTRUE(rt->hdr.flags, IPNET_RTF_X_AUTO))
            {
                if (rt->narrow == IP_NULL && rt->widen == IP_NULL
                    && rt->next == IP_NULL && rt->prev == IP_NULL)
                    /* No entries using the same key, let the IPCOM code remove this route */
                    return IP_TRUE;

                (void)ipnet_route_delete2(IPNET_ROUTE_GET_FAMILY(rt->head),
                                          IPNET_ROUTE_GET_VR(rt->head),
                                          IPNET_ROUTE_GET_TABLE(rt->head),
                                          rt->hdr.key,
                                          rt->hdr.mask,
                                          rt->gateway,
                                          netif->ipcom.ifindex,
                                          0,
                                          0,
                                          IP_FALSE);
            }
        }
    }
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_state_change
 *===========================================================================
 * Description: Change to a new state.
 * Parameters:  netif       - The network interface.
 *              state       - The state.
 * Returns:     IP_TRUE if the state is changed.
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_rfc1256_state_change(Ipnet_netif *netif,
                               Ip_u8       state)
{
    /* */
    if (netif->inet4_rfc1256_state != state)
    {
        IPCOM_LOG4(DEBUG2,
                   "IPv4: %s %s RFC1256 changing state from %s to %s",
                   netif->ipcom.name,
                   IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                   IPNET_RFC1256_STATE(netif->inet4_rfc1256_mode, netif->inet4_rfc1256_state),
                   IPNET_RFC1256_STATE(netif->inet4_rfc1256_mode, state));

        ipnet_timeout_cancel(netif->inet4_rfc1256_tmo);
        netif->inet4_rfc1256_num    = 0;
        netif->inet4_rfc1256_state  = state;
        ipnet_ip4_rfc1256_state_run(netif);

        /* State has changed */
        return IP_TRUE;
    }

    /* Same state */
    return IP_FALSE;
}

/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_mode
 *===========================================================================
 * Description: Determine if a network interface is in HOST or ROUTER
 *              or NONE mode.
 * Parameters:  netif   - The network interface
 * Returns:     HOST, ROUTER or NONE
 *
 */
IP_STATIC Ip_u8
ipnet_ip4_rfc1256_mode(Ipnet_netif  *netif)
{
    Ip_bool ipnet_router;
    Ip_bool netif_router;

    /* Check the forwarding capabilities */
    ipnet_router = IP_BIT_ISTRUE(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT);
    netif_router = IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_NO_IPV4_FORWARD);

    if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
    {
        /* Don't do anything if the interface is not UP */
        return IPNET_RFC1256_MODE_NONE;
    }

    /* Determine the RFC1256 state for this particular interface */
    if (ipnet_router && netif_router)
    {
        /* We're configured as a router; see if router advertisements are
         * enabled */
#ifndef IPNET_RFC1256_ENABLE_ADVERTISEMENT
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET, netif, "rtdisc.PerformRouterAdvertisement", 0, ipnet_bool_map) != 0)
#else
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET, netif, "rtdisc.PerformRouterAdvertisement", 1, ipnet_bool_map) != 0)
#endif
        {
            /* They're enabled, we need to flag us as a ROUTER interface */
            return IPNET_RFC1256_MODE_ROUTER;
        }
    }
    else
    {
        /* We're configured as a host; see if router solicitations are
         * enabled */
#ifndef IPNET_RFC1256_ENABLE_SOLICITATION
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET, netif, "rtdisc.PerformRouterDiscovery", 0, ipnet_bool_map) != 0)
#else
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET, netif, "rtdisc.PerformRouterDiscovery", 1, ipnet_bool_map) != 0)
#endif
        {
            /* They're enabled, we need to flag us as a HOST interface */
            return IPNET_RFC1256_MODE_HOST;
        }
    }

    /* We're in 'no mode' */
    return IPNET_RFC1256_MODE_NONE;
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_advertise_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_rfc1256_advertise(Ipnet_netif *netif)
{
    Ipnet_ip4_addr_entry    *addr;
    Ipnet_pkt_icmp          *icmp;
    Ip_u32                  count   = 0;
    Ip_u32                  max;
    Ip_u32                  min;
    Ip_u32                  life;
    Ip_u32                  next_tmo;
    Ipnet_icmp_param        param;
    Ip_bool                 is_multicast;
    Ipcom_pkt               *pkt;
    struct Ip_in_addr       dst;


    /* Get the basic values for timeouts */
    max = ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "rtdisc.MaxAdvertisementInterval", 600);
    max = IP_MIN(1800, IP_MAX(4, max));
    min = ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "rtdisc.MinAdvertisementInterval", (max * 3) / 4);
    min = IP_MIN(max, IP_MAX(min, 3));

    next_tmo =  ipcom_random();
    next_tmo %= ((max - min) * 1000);
    next_tmo += min * 1000;

    /* Next advertisement interval */
    if (netif->inet4_rfc1256_state == IPNET_RFC1256_ROUTER_STATE_BROADCASTING)
    {
        /* Send the advertisement */
        if (IP_BIT_ISFALSE(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_ROUTER_PENDING))
            ++netif->inet4_rfc1256_num;

        if (netif->inet4_rfc1256_num < IPNET_RFC1256_ROUTER_MAX_INITIAL_ADVERTISEMENTS)
            next_tmo = IP_MIN(next_tmo,IPNET_RFC1256_ROUTER_MAX_INITIAL_ADVERT_INTERVAL);
        else
        {
            /* Go advertising */
            ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_ROUTER_STATE_ADVERTISING);
        }
    }

    /* No longer pending */
    IP_BIT_CLR(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_ROUTER_PENDING);

    /* Create a reply packet. */
    pkt = ipcom_pkt_malloc(ipnet_conf_ip4_min_mtu,
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                          1, 1, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                          ipnet_ip4_rfc1256_advertise, IPCOM_WV_NETD_NOBUFS,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(icmp4_send_nomem++);
        IPCOM_MIB2(icmpOutErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsOutErrors, 1);
        goto schedule_next;
    }

    /* Set pkt->start to the beginning of ICMPv4 data */
    pkt->start = ipcom_conf_max_link_hdr_size + IPNET_IP_HDR_SIZE;

    pkt->ipstart = pkt->start - IPNET_IP_HDR_SIZE;
    ipcom_memset(&pkt->data[pkt->ipstart], 0, IPNET_IP_HDR_SIZE + IPNET_ICMP_HDR_SIZE);

    /* Make room for the ICMPv4 header */
    pkt->end = pkt->start + IPNET_ICMP_HDR_SIZE;

    /* Grab the ICMP header */
    icmp = (Ipnet_pkt_icmp *) &pkt->data[pkt->start];

    /* Verify whether we should be advertising this address */
    for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
        if (addr->type == IPNET_ADDR_TYPE_UNICAST)
        {
            char        addr_str[50];
            char        tmp_str[20];

            /**/
            ipcom_strcpy(addr_str, "rtdisc.");
            ipcom_inet_ntop(IP_AF_INET, &addr->ipaddr_n, tmp_str, sizeof(tmp_str));
            ipcom_strcat(addr_str, tmp_str);
            ipcom_strcat(addr_str, ".Advertise");

            /* Is it enabled? */
            if (ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, addr_str, 1) == 0)
                continue;

            /* Address */
            icmp->data.advertise.advert[count++] = addr->ipaddr_n;

            /* Preference */
            ipcom_strcpy(addr_str, "rtdisc.");
            ipcom_inet_ntop(IP_AF_INET, &addr->ipaddr_n, tmp_str, sizeof(tmp_str));
            ipcom_strcat(addr_str, tmp_str);
            ipcom_strcat(addr_str, ".PreferenceLevel");

            icmp->data.advertise.advert[count++] =
                ip_htonl(ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, addr_str, 0));

            /* Increase the size of the ICMP frame */
            pkt->end += sizeof(Ip_u32) * 2;
        }

    /* Nothing to advertise */
    if (count == 0)
    {
        /* SEND THE SOLICITATION */
        IPCOM_LOG2(DEBUG2,
                   "IPv4: %s %s RFC1256 advertise send ignored, no usable addresses",
                   netif->ipcom.name,
                   IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));
        ipcom_pkt_free(pkt);
        goto schedule_next;
    }

    life    = ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "rtdisc.AdvertisementLifetime", max * 3);
    life    = IP_MAX(max, IP_MIN(9000, life));

    /* */
    icmp->data.advertise.num_addrs          = (Ip_u8) (count / 2);
    icmp->data.advertise.addr_entry_size    = 2;
    icmp->data.advertise.lifetime           = ip_htons((Ip_u16) life);


    /* Clear the parameters */
    ipcom_memset(&param, 0, sizeof(param));

    /* Setup the basics */
    param.to        = &dst;
    param.netif     = netif;
    param.vr_index  = netif->vr_index;
    param.type      = IPNET_ICMP4_TYPE_ROUTER_ADVERT;
    param.recv_pkt  = pkt;

    is_multicast    = IP_BIT_ISTRUE(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_ROUTER_MULTICAST);

    /* Use broadcast or multicast? */
    if (is_multicast)
    {
        dst.s_addr = ip_htonl(IP_INADDR_ALLHOSTS_GROUP);
        param.ttl  = 1;
    }
    else
    {
        dst.s_addr = ip_htonl(IP_INADDR_BROADCAST);
    }

    ipnet_icmp4_send(&param, IP_FALSE);

schedule_next:
    ipnet_ip4_rfc1256_advertise_schedule(netif, next_tmo);
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_advertise_schedule
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_rfc1256_advertise_schedule(Ipnet_netif    *netif,
                                     Ip_u32         tmo)
{
    /* Verify that we're in a state where its allowed */
    if (netif->inet4_rfc1256_mode == IPNET_RFC1256_MODE_ROUTER)
    {
        switch (netif->inet4_rfc1256_state)
        {
        case IPNET_RFC1256_ROUTER_STATE_BROADCASTING:
        case IPNET_RFC1256_ROUTER_STATE_ADVERTISING:
            ipnet_timeout_cancel(netif->inet4_rfc1256_tmo);
            (void)ipnet_timeout_schedule(tmo,
                                         (Ipnet_timeout_handler) ipnet_ip4_rfc1256_advertise,
                                         netif,
                                         &netif->inet4_rfc1256_tmo);
            break;
        default:
            /* No; clear the advert bit */
            IP_BIT_CLR(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_ROUTER_PENDING);
            break;
        }
    }
}


#ifdef IPNET_DEBUG
/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_advertise_tmo_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ip4_rfc1256_advertise_tmo_to_string(Ipnet_netif *netif, char *buf, int buf_len)
{
    return ipcom_snprintf(buf,
                          buf_len,
                          "ICMP Router Discovery Message on %s",
                          netif->ipcom.name);
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_solicit_tmo_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ip4_rfc1256_solicit_tmo_to_string(Ipnet_netif *netif, char *buf, int buf_len)
{
    return ipcom_snprintf(buf,
                          buf_len,
                          "ICMP Router Solicitation Message on %s",
                          netif->ipcom.name);
}
#endif /* IPNET_DEBUG */


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_solicit
 *===========================================================================
 * Description: Start the solicitation.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_rfc1256_solicit(Ipnet_netif *netif)
{
    Ipnet_icmp_param    param;
    Ip_bool             is_multicast;
    struct Ip_in_addr   dst;

    /* Clear the parameters */
    ipcom_memset(&param, 0, sizeof(param));

    /* Setup the basics */
    param.to        = &dst;
    param.netif     = netif;
    param.vr_index  = netif->vr_index;
    param.type      = IPNET_ICMP4_TYPE_ROUTER_SOLICIT;

    is_multicast    = IP_BIT_ISTRUE(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_HOST_MULTICAST);

    /* Use broadcast or multicast? */
    if (is_multicast)
    {
        dst.s_addr  = ip_htonl(IP_INADDR_ALLRTRS_GROUP);
        param.ttl   = 1;
    }
    else
        dst.s_addr  = ip_htonl(IP_INADDR_BROADCAST);

    /* Send the message */
    ipnet_icmp4_send(&param, IP_FALSE);

    /* SEND THE SOLICITATION */
    IPCOM_LOG2(DEBUG2,
               "IPv4: %s %s RFC1256 sending solicitation",
               netif->ipcom.name,
               IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));

    /* Should I schedule a new solicitation */
    if (++netif->inet4_rfc1256_num < IPNET_RFC1256_HOST_MAX_SOLICITATIONS)
    {
            if (ipnet_timeout_schedule(IPNET_RFC1256_HOST_SOLICITATION_INTERVAL,
                                       (Ipnet_timeout_handler) ipnet_ip4_rfc1256_solicit,
                                       netif,
                                       &netif->inet4_rfc1256_tmo) == 0)
                return;
    }

    /* Go IDLE */
    ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_HOST_STATE_IDLE);
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_state_run
 *===========================================================================
 * Description: Run the currently set state.
 * Parameters:  netif - The network interface.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_rfc1256_state_run(Ipnet_netif *netif)
{
    Ip_u32 tmp;

    /* If we're running and up, do start the rfc1256 processing */
    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_RUNNING|IP_IFF_UP) == (IP_IFF_RUNNING|IP_IFF_UP))
    {
        /* Did we retrieve a state? */
        switch (netif->inet4_rfc1256_mode)
        {
        case IPNET_RFC1256_MODE_NONE:
            break;

        case IPNET_RFC1256_MODE_HOST:
            switch (netif->inet4_rfc1256_state)
            {
            case IPNET_RFC1256_GENERIC_STATE_NONE:
                break;
            case IPNET_RFC1256_GENERIC_STATE_SHUTDOWN:
                {
                    Ipcom_route *rtab;

                    /* Try lookup the default route table for this netif */
                    if (ipnet_route_get_rtab(IP_AF_INET, netif->vr_index, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) == 0)
                    {
                        ipcom_route_walk_tree(rtab,
                                              (Ipcom_route_walk_cb) ipnet_ip4_rfc1256_route_remove_auto,
                                              netif);
                    }

                }
                /* Go to the 'no' state */
                ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_GENERIC_STATE_NONE);
                break;
            case IPNET_RFC1256_GENERIC_STATE_INITIALIZING:
                /* Go solicit */
                ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_HOST_STATE_SOLICIT);

                /* Calculate an initial delay */
                tmp = ipcom_random();
                tmp %= IPNET_RFC1256_HOST_MAX_SOLICITATION_DELAY;

                /* Move to SETUP */
                if (tmp == 0
                    || ipnet_timeout_schedule(tmp,
                                              (Ipnet_timeout_handler) ipnet_ip4_rfc1256_solicit,
                                              netif,
                                              &netif->inet4_rfc1256_tmo) != 0)
                {
                    /* No delay or failed to schedule timeout */
                    ipnet_ip4_rfc1256_solicit(netif);
                }
                break;
            case IPNET_RFC1256_HOST_STATE_SOLICIT:
                /* We're soliciting */
                break;
            case IPNET_RFC1256_HOST_STATE_IDLE:
                break;
            }
            break;
        case IPNET_RFC1256_MODE_ROUTER:
            switch (netif->inet4_rfc1256_state)
            {
            case IPNET_RFC1256_GENERIC_STATE_NONE:
                break;
            case IPNET_RFC1256_GENERIC_STATE_SHUTDOWN:
                /* Go to the 'no' state */
                ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_GENERIC_STATE_NONE);
                break;
            case IPNET_RFC1256_GENERIC_STATE_INITIALIZING:
                /* Go to the 'no' state */
                ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_ROUTER_STATE_BROADCASTING);

                /* Calculate an initial delay */
                tmp =  ipcom_random();
                tmp %= IPNET_RFC1256_ROUTER_MAX_RESPONSE_DELAY;

                /* Schedule an advertisement */
                ipnet_ip4_rfc1256_advertise_schedule(netif, tmp);
                break;
            case IPNET_RFC1256_ROUTER_STATE_BROADCASTING:
                break;
            case IPNET_RFC1256_ROUTER_STATE_ADVERTISING:
                break;
            }
            break;
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_solicit_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_rfc1256_solicit_input(Ipnet_netif             *netif,
                                Ipnet_pkt_icmp          *icmp_hdr,
                                int                     icmp_len,
                                Ipnet_ip4_output_param *ip4_param)
{
    /* Is this interface a router? */
    if (netif->inet4_rfc1256_mode == IPNET_RFC1256_MODE_ROUTER)
    {
        if (icmp_len < 8)
        {
            IPCOM_LOG2(DEBUG2,
                       "IPv4: %s %s RFC1256 received solicitation too short --> discarding",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));
            return;
        }

        if (icmp_hdr->code != 0)
        {
            IPCOM_LOG3(DEBUG2,
                       "IPv4: %s %s RFC1256 received solicitation has code %u, should be 0 --> discarding",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                       icmp_hdr->code);
            return;
        }

        /* Verify that its sent from a valid source address */
        if (ip4_param->from != IP_NULL && ip4_param->from->s_addr != IP_INADDR_ANY)
        {
            /* We've got an address; check the interface in order to verify that
             * we've got a correct submask/net location */
            /* Verify whether we should be advertising this address */
            Ipnet_ip4_addr_entry    *addr;
            for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
            {
                if (addr->type == IPNET_ADDR_TYPE_UNICAST)
                {
                    /* These two seems to be on the same subnet */
                    if ((addr->ipaddr_n & addr->netmask_n) == (ip4_param->from->s_addr & addr->netmask_n))
                        break;
                }
            }

            /* No go */
            if (addr == IP_NULL)
            {
                IPCOM_LOG3(DEBUG2,
                           "IPv4: %s %s RFC1256 received solicitation from invalid source address %s --> discarding",
                           netif->ipcom.name,
                           IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                           ipcom_inet_ntop(IP_AF_INET, &ip4_param->from->s_addr, ipnet->log_buf, sizeof(ipnet->log_buf)));
                return;
            }
        }

        /* Do we have any advertisements already pending? */
        if (IP_BIT_ISFALSE(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_ROUTER_PENDING))
        {
            Ip_u32  next_tmo;

            /* No; set the bit and schedule an advertisement */
            IP_BIT_SET(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_ROUTER_PENDING);

            /* Randomize the timeout */
            next_tmo = ipcom_random();
            next_tmo %= IPNET_RFC1256_ROUTER_MAX_RESPONSE_DELAY;

            /* Make certain that its atleast 1 */
            next_tmo = IP_MAX(1, next_tmo);

            IPCOM_LOG2(DEBUG2,
                       "IPv4: %s %s RFC1256 receiving solicitation, scheduling advertisement",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));

            /* Do schedule the advertisement */
            ipnet_ip4_rfc1256_advertise_schedule(netif, next_tmo);
        }
        else
        {
            IPCOM_LOG2(DEBUG2,
                       "IPv4: %s %s RFC1256 receiving redundant solicitation, advertisement already scheduled",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_solicit_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_rfc1256_advertise_input(Ipnet_netif             *netif,
                                  Ipnet_pkt_icmp          *icmp_hdr,
                                  int                     icmp_len,
                                  Ipnet_ip4_output_param *ip4_param)
{
    IPCOM_UNUSED_ARG(ip4_param);

    /* Is this interface a host (that accepts advertisements)? */
    if (netif->inet4_rfc1256_mode == IPNET_RFC1256_MODE_HOST)
    {
        Ip_u32 rtaddr;
        Ip_u16 lifetime;
        Ip_u32 rtpref;
        Ip_u32 tmp;
        Ip_u8  *data;

        if (icmp_hdr->code != 0)
        {
            IPCOM_LOG3(DEBUG2,
                       "IPv4: %s %s RFC1256 received advertisement has code %u, should be 0 --> discarding",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                       icmp_hdr->code);
            return;
        }

        if (icmp_len < 8)
        {
            IPCOM_LOG2(DEBUG2,
                       "IPv4: %s %s RFC1256 received advertisement too short --> discarding",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));
            return;
        }

        if (icmp_hdr->data.advertise.num_addrs == 0)
        {
            IPCOM_LOG2(DEBUG2,
                       "IPv4: %s %s RFC1256 received advertisement contains no addresses --> discarding",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));
            return;
        }

        if (icmp_hdr->data.advertise.addr_entry_size < 2)
        {
            IPCOM_LOG2(DEBUG2,
                       "IPv4: %s %s RFC1256 received advertisement has too small entry size --> discarding",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode));
            return;
        }

        /* Verify that it indeed holds all the advertised information */
        tmp = icmp_hdr->data.advertise.num_addrs * icmp_hdr->data.advertise.addr_entry_size * 4;
        if (icmp_len < (int) tmp)
        {
            IPCOM_LOG4(DEBUG2,
                       "IPv4: %s %s RFC1256 received advertisement too short (%u, should be %u)--> discarding",
                       netif->ipcom.name,
                       IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                       icmp_len,
                       tmp);
            return;
        }

        /* Get the lifetime */
        lifetime = IP_GET_NTOHS(&icmp_hdr->data.advertise.lifetime);

        /* Go through the addresses; */
        data = (Ip_u8 *)&icmp_hdr->data.advertise.advert[0];
        while (icmp_hdr->data.advertise.num_addrs--)
        {
            Ipnet_ip4_addr_entry    *addr;

            /* Grab the address and preference levels */
            rtaddr = IP_GET_32ON8(data);
            rtpref = IPNET_RFC1256_SIGNED_TO_HOPCOUNT(IP_GET_NTOHL((data + 4)));

            /* Advance past this entry */
            data += icmp_hdr->data.advertise.addr_entry_size * 4;

            /* Determine what to do */
            if (rtpref == 0xffffffff)
            {
                IPCOM_LOG3(DEBUG2,
                           "IPv4: %s %s RFC1256 advertised address %s tagged as non-default --> skipping",
                           netif->ipcom.name,
                           IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                           ipcom_inet_ntop(IP_AF_INET, &rtaddr, ipnet->log_buf, sizeof(ipnet->log_buf)));
                continue;
            }

            /* We've got an address; check the interface in order to verify that
             * we've got a correct submask/net location */
            for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
            {
                if (addr->type == IPNET_ADDR_TYPE_UNICAST)
                {
                    /* These two seems to be on the same subnet */
                    if ((addr->ipaddr_n & addr->netmask_n) == (rtaddr & addr->netmask_n))
                        break;
                }
            }

            /* Found an address? */
            if (addr == IP_NULL)
            {
                IPCOM_LOG3(DEBUG2,
                           "IPv4: %s %s RFC1256 advertised address %s not valid on interface --> trying next",
                           netif->ipcom.name,
                           IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                           ipcom_inet_ntop(IP_AF_INET, &rtaddr, ipnet->log_buf, sizeof(ipnet->log_buf)));
                continue;
            }
            else
            {
                /* We've got one; do add this default gateway with the appropriate timeout (first verify if it exists) */
                int                     ret;
                Ipnet_route_entry       *rt;
                struct Ip_sockaddr_in   gw;

                ipcom_memset(&gw, 0, sizeof(gw));
                gw.sin_family       = IP_AF_INET;
                IPCOM_SA_LEN_SET(&gw, sizeof(gw));
                gw.sin_addr.s_addr  = rtaddr;
                ret = ipnet_route_raw_lookup2(IP_AF_INET,
                                              netif->vr_index,
                                              IPCOM_ROUTE_TABLE_DEFAULT,
                                              0,
                                              &ip_inaddr_any,
                                              0,
                                              IP_NULL,
                                              (struct Ip_sockaddr *) &gw,
                                              netif->ipcom.ifindex,
                                              &rt);

                if (ret == IPNET_ROUTE_PERFECT_MATCH)
                {
                    /* Already existed; update parameters */
                    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_AUTO))
                    {
                        /* Added by something else; statically or by some
                         * other means of configuration; do not modify */
                        continue;
                    }

                    if (lifetime == 0)
                    {
                        /* Delete this particular route */
                        (void)ipnet_route_delete2(IP_AF_INET,
                                                  netif->vr_index,
                                                  IPNET_ROUTE_GET_TABLE(rt->head),
                                                  rt->hdr.key,
                                                  rt->hdr.mask,
                                                  rt->gateway,
                                                  rt->netif->ipcom.ifindex,
                                                  0,
                                                  0,
                                                  IP_FALSE);
                    }
                    else
                    {
                        if (rt->metrics.rmx_hopcount != rtpref)
                        {
                            /* Update the metrics */
                            rt->metrics.rmx_hopcount = rtpref;
                            ipnet_route_has_changed(rt);
                        }

                        /* Update the lifetime of the route */
                        ipnet_route_set_lifetime(rt, lifetime);
                    }
                }
                else if (lifetime != 0)
                {
                    struct Ipnet_route_add_param param;
                    struct Ipnet_rt_metrics      metrics;

                    /* Only add it if we've gotten a lifetime; otherwise
                     * its a delete request */
                    ipcom_memset(&metrics, 0, sizeof(metrics));
                    metrics.rmx_expire   = lifetime;
                    metrics.rmx_hopcount = rtpref;

                    /* Add the route */
                    ipcom_memset(&param, 0, sizeof(param));
                    param.domain     = IP_AF_INET;
                    param.vr         = netif->vr_index;
                    param.table      = IPCOM_ROUTE_TABLE_DEFAULT;
                    param.netif      = netif;
                    param.flags      = IPNET_RTF_UP | IPNET_RTF_DONE | IPNET_RTF_GATEWAY | IPNET_RTF_X_AUTO;
                    param.key        = &ip_inaddr_any;
                    param.netmask    = &ip_inaddr_any;
                    param.gateway    = (struct Ip_sockaddr *) &gw;
                    param.metrics    = &metrics;

                    (void) ipnet_route_add(&param);
                }
            }

        }
    }
}


/*
 *===========================================================================
 *                    ipnet_ip4_rfc1256_mode_update
 *===========================================================================
 * Description: Initialize the RFC1256 portion of the interface
 * Parameters:  vr      - The virtual router
 *              netif   - The network interface
 * Returns:     IP_TRUE if mode's been changed.
 *
 */
IP_GLOBAL Ip_bool
ipnet_ip4_rfc1256_mode_update(Ip_u16        vr,
                              Ipnet_netif   *netif)
{
    int mode;

    if (netif == IP_NULL)
    {
        unsigned i;

        /* Need to do a global update; some major flag has changed */
        IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
        {
            /* Go through them all and update the mode */
            ipnet_ip4_rfc1256_mode_update(vr, netif);
        }

        return IP_TRUE;
    }


    mode = ipnet_ip4_rfc1256_mode(netif);

    /* New mode? */
    if (mode != netif->inet4_rfc1256_mode)
    {
        struct Ip_in_addr addr;

        IPCOM_LOG3(DEBUG2,
                   "IPv4: %s RFC1256 changing mode from %s to %s",
                   netif->ipcom.name,
                   IPNET_RFC1256_MODE(netif->inet4_rfc1256_mode),
                   IPNET_RFC1256_MODE(mode));

        /* Cleanup the current mode */
        ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_GENERIC_STATE_SHUTDOWN);

        /* We're changing mode; have we registered ourselves to the ROUTER multicast? */
        if (mode == IPNET_RFC1256_MODE_ROUTER)
        {
            /* Remove it */
            ipnet_ip4_remove_addr(netif, ip_htonl(IP_INADDR_ALLRTRS_GROUP));
        }

        /* Switch to the selected mode */
        netif->inet4_rfc1256_mode   = mode;
        netif->inet4_rfc1256_flags  = 0;

        /**/
        if (mode == IPNET_RFC1256_MODE_ROUTER)
        {
            /* Retrieve the configured address */
            ipnet_ip4_rfc1256_sysvar_address(netif,
                                             "AdvertisementAddress",
                                             &addr,
                                             ip_htonl(IP_INADDR_ALLHOSTS_GROUP));

            /* Check the configuration; do we use multicast or broadcast? */
            if (addr.s_addr == ip_htonl(IP_INADDR_ALLHOSTS_GROUP))
            {
                /* Remember that we're interested in using multicast advertisements */
                IP_BIT_SET(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_ROUTER_MULTICAST);
            }

            /* Add the multicast address */
            ipnet_ip4_add_addr2(netif,
                                ip_htonl(IP_INADDR_ALLRTRS_GROUP),
                                IP_IN_CLASSD_NET,
                                IPNET_IP4_ADDR_FLAG_AUTOMATIC,
                                IPNET_ADDR_TYPE_MULTICAST);

        }
        else if (mode == IPNET_RFC1256_MODE_HOST)
        {
            /* Retrieve the configured address */
            ipnet_ip4_rfc1256_sysvar_address(netif,
                                             "SolicitationAddress",
                                             &addr,
                                             ip_htonl(IP_INADDR_ALLRTRS_GROUP));
            /* Check the configuration; do we use multicast or broadcast? */
            if (addr.s_addr == ip_htonl(IP_INADDR_ALLRTRS_GROUP))
            {
                /* Remember that we're interested in multicast solicitations */
                IP_BIT_SET(netif->inet4_rfc1256_flags, IPNET_RFC1256_FLAG_HOST_MULTICAST);
            }
        }


        netif->inet4_rfc1256_state  = IPNET_RFC1256_GENERIC_STATE_INITIALIZING;
        netif->inet4_rfc1256_num    = 0;

        /* Reset the state */
        ipnet_ip4_rfc1256_state_run(netif);

        /* We've changed state */
        return IP_TRUE;
    }

    /* No mode change */
    return IP_FALSE;
}

#endif /* IPNET_USE_RFC1256 */


#ifdef IPNET_USE_RFC3927
/*
 *===========================================================================
 *                    ipnet_ip4_lladdr_generate
 *===========================================================================
 * Description: Generate a link local IPv4 address
 * Parameters:  addr - Pointer where the address should be stored
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_lladdr_generate(Ipnet_netif *netif)
{
    if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET, netif, "linklocal.TEST", 0, ipnet_bool_map) == 0)
    {
        Ip_u32 range;
        Ip_u32 base;

        /* Genererate an address in the range 169.254.1.0 - 169.254.254.255 */
        (void) ipcom_inet_pton(IP_AF_INET, "169.254.1.0", &base);
        base = ip_ntohl(base);
        (void) ipcom_inet_pton(IP_AF_INET, "169.254.254.255", &range);
        range = ip_ntohl(range) - base + 1;
        netif->inet4_lladdr.s_addr = ip_htonl(ipcom_rand() % range  + base);
    }
    else
    {
        static char *addrs[] = {
            "169.254.1.0",
            "169.254.1.1",
        };

        /* Testing, use a specific address */
        (void) ipcom_inet_pton(IP_AF_INET,
                               addrs[netif->inet4_lladdr_conflics % 2],
                               &netif->inet4_lladdr);
    }
}


/*
 *===========================================================================
 *                     ipnet_ip4_lladdr_defend
 *===========================================================================
 * Description: Timeout for the DEFEND state.
 * Parameters:  netif - The interface where the link address is assigned to.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_lladdr_defend(Ipnet_netif *netif)
{
    /* The address was successfully defended */
    ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_ACTIVE);
}


/*
 *===========================================================================
 *                    ipnet_ip4_lladdr_announce
 *===========================================================================
 * Description: Timeout handler for the ANNOUNCE state.
 * Parameters:  netif - The interface the link local address will be
 *                      assigned to.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_lladdr_announce(Ipnet_netif *netif)
{
    Ip_bool do_arp = IP_TRUE;

    if (netif->inet4_lladdr_num == 255)
    {
        netif->inet4_lladdr_num = (Ip_u8)
            ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "linklocal.ANNOUNCE_NUM", 2);
        /* ARP already sent when the address was added */
        do_arp = IP_FALSE;
    }

    if (netif->inet4_lladdr_num-- == 0)
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_ACTIVE);
    else
    {
        ipnet_timeout_cancel(netif->inet4_lladdr_tmo);
        if (ipnet_timeout_schedule(1000 * ipnet_sysvar_netif_get_as_int(IP_AF_INET,
                                                                        netif,
                                                                        "linklocal.ANNOUNCE_INTERVAL",
                                                                        2),
                                   (Ipnet_timeout_handler) ipnet_ip4_lladdr_announce,
                                   netif,
                                   &netif->inet4_lladdr_tmo))
            ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DISABLED);
        else if (do_arp)
            /* Send ARP gratuitous probes */
            ipnet_arp_request(netif,
                              netif->inet4_lladdr.s_addr,
                              IP_TRUE,
                              IP_NULL,
                              IP_NULL);
    }
}


/*
 *===========================================================================
 *                    ipnet_ip4_lladdr_probe
 *===========================================================================
 * Description: Timeout handler for the PROBE state
 * Parameters:  netif - The interface the link local address will be
 *                      assigned to.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_lladdr_probe(Ipnet_netif *netif)
{
    if (netif->inet4_lladdr_num-- == 0)
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_ANNOUNCE);
    else
    {
        Ip_u32 base;
        Ip_u32 range;

        base = ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "linklocal.PROBE_MIN", 1) * 1000;
        range = ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "linklocal.PROBE_MAX", 3) * 1000;
        range = (range <= base ? 1 : range - base);

        ipnet_timeout_cancel(netif->inet4_lladdr_tmo);
        if (ipnet_timeout_schedule(ipcom_rand() % range + base,
                                   (Ipnet_timeout_handler) ipnet_ip4_lladdr_probe,
                                   netif,
                                   &netif->inet4_lladdr_tmo))
            ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DISABLED);
        else
            /* Send ARP probes */
            ipnet_arp_request(netif,
                              netif->inet4_lladdr.s_addr,
                              -1,
                              IP_NULL,
                              IP_NULL);
    }
}


/*
 *===========================================================================
 *                     ipnet_ip4_lladdr_probe_wait
 *===========================================================================
 * Description: Timeout handler for the probe wait timer.
 * Parameters:  netif - The interface to configure for IPv4 link local address.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_lladdr_probe_wait(Ipnet_netif *netif)
{
    ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_PROBE);
}


/*
 *===========================================================================
 *                     ipnet_ip4_lladdr_init
 *===========================================================================
 * Description: Initializes usage of link local address on the interface.
 * Parameters:  netif - The interface to configure for IPv4 link local address.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_lladdr_init(Ipnet_netif *netif)
{
    Ip_bool enabled;

    /* RFC3927 currently only implemented for Ethernet */
    if (netif->eth == IP_NULL)
    {
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DISABLED);
        return;
    }

    /* Using ipnet.inet.AutoConf is the preferred way, but VxWorks
       still uses the ipnet.inet.linklocal.interfaces variable since
       it fit much better into the CDF machinery */
    enabled = ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                               netif,
                                               "AutoConf",
                                               0,
                                               ipnet_bool_map);
    if (!enabled)
    {
        char *iflist;
        char *ifname;
        char *saveptr;

        iflist = ipcom_sysvar_get("ipnet.inet.linklocal.interfaces", IP_NULL, IP_NULL);
        if (iflist != IP_NULL)
        {
            for (ifname = ipcom_strtok_r(iflist, " ,", &saveptr);
                 ifname != IP_NULL;
                 ifname = ipcom_strtok_r(IP_NULL, " ,", &saveptr))
                if (ipcom_strcmp(ifname, netif->ipcom.name) == 0)
                {
                    enabled = IP_TRUE;
                    break;
                }
            ipcom_free(iflist);
        }
    }

    if (enabled)
    {
        Ip_u32 msec;

        netif->inet4_lladdr.s_addr = IP_INADDR_ANY;

        if (IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_RANDOM_SEED))
        {
            /* init the seed with a random number so that other interfaces
               seed also has an effect */
            unsigned seed = ipcom_random();
            int      i;

            for (i = 0; i < netif->ipcom.link_addr_size; i++)
                seed += netif->ipcom.link_addr[i] << (8 * (i & 3));
            IP_BIT_SET(netif->flags2, IPNET_IFF2_RANDOM_SEED);
            ipcom_srandom(seed);
        }

        ipnet_ip4_lladdr_generate(netif);

        msec = ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "linklocal.PROBE_WAIT", 1) * 1000;
        msec = ipcom_rand() % msec;

        ipnet_timeout_cancel(netif->inet4_lladdr_tmo);
        (void) ipnet_timeout_schedule(msec,
                                      (Ipnet_timeout_handler) ipnet_ip4_lladdr_probe_wait,
                                      netif,
                                      &netif->inet4_lladdr_tmo);
    }
}


/*
 *===========================================================================
 *                     ipnet_ip4_lladdr_set_state
 *===========================================================================
 * Description: Change state of the link local address on an interface.
 * Parameters:  netif - A network interface.
 *              state - One of the IPNET_IP4_LLADDR_STATE_xxx constants
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_lladdr_set_state(Ipnet_netif *netif, Ip_u8 state)
{
    static const char *states[] = {
        "DISABLED",
        "INIT",
        "PROBE",
        "DEFEND",
        "ANNOUNCE",
        "ACTIVE",
    };
    Ip_u32 lladdr_n = netif->inet4_lladdr.s_addr;
    Ip_u32 sec;

    (void)states;

    if (state > IPNET_IP4_LLADDR_STATE_MAX)
    {
        IP_PANIC();
        return;
    }

    if (netif->inet4_lladdr_state == state)
        return;

    if (netif->inet4_addr_list != IP_NULL
        && netif->inet4_addr_list->type == IPNET_ADDR_TYPE_UNICAST
        && (state == IPNET_IP4_LLADDR_STATE_INIT
            || state == IPNET_IP4_LLADDR_STATE_PROBE
            || state == IPNET_IP4_LLADDR_STATE_ANNOUNCE))
    {
        /* RFC 3927, chapter 1.9
           A host with an address on a link can
           communicate with all other devices on that link, whether those
           devices use Link-Local addresses, or routable addresses.  For these
           reasons, a host SHOULD NOT have both an operable routable address and
           an IPv4 Link-Local address configured on the same interface.
        */
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DISABLED);
        return;
    }

    switch (state)
    {
    case IPNET_IP4_LLADDR_STATE_DISABLED:
        ipnet_timeout_cancel(netif->inet4_lladdr_tmo);
        netif->inet4_lladdr.s_addr = IP_INADDR_ANY;
        break;

    case IPNET_IP4_LLADDR_STATE_INIT:
        if (netif->inet4_lladdr_state == IPNET_IP4_LLADDR_STATE_ACTIVE)
            /* Continue with with address as active */
            return;

        if (ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "linklocal.MAX_CONFLICTS", 10)
            >= (int) netif->inet4_lladdr_conflics)
            ipnet_ip4_lladdr_init(netif);
        else
        {
            sec = ipnet_sysvar_netif_get_as_int(IP_AF_INET,
                                                netif,
                                                "linklocal.RATE_LIMIT_INTERVAL",
                                                60);

            ipnet_timeout_cancel(netif->inet4_lladdr_tmo);
            (void) ipnet_timeout_schedule(sec * 1000,
                                          (Ipnet_timeout_handler) ipnet_ip4_lladdr_init,
                                          netif,
                                          &netif->inet4_lladdr_tmo);
        }
        lladdr_n = netif->inet4_lladdr.s_addr;
        break;

    case IPNET_IP4_LLADDR_STATE_PROBE:
        netif->inet4_lladdr_num = (Ip_u8)
            ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "linklocal.PROBE_NUM", 3);
        ipnet_ip4_lladdr_probe(netif);
        break;

    case IPNET_IP4_LLADDR_STATE_ANNOUNCE:
        if (ipnet_ip4_add_addr2(netif,
                                netif->inet4_lladdr.s_addr,
                                ip_htonl(0xffff0000),
                                IPNET_IP4_ADDR_FLAG_LINK_LOCAL,
                                IPNET_ADDR_TYPE_UNICAST))
        {
            /* Failed to add the address for some reason */
            ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DISABLED);
            return;
        }
        netif->inet4_lladdr_num = 255;
        ipnet_ip4_lladdr_announce(netif);
        break;

    case IPNET_IP4_LLADDR_STATE_DEFEND:
        sec = ipnet_sysvar_netif_get_as_int(IP_AF_INET,
                                            netif,
                                            "linklocal.DEFEND_INTERVAL",
                                            10);
        ipnet_timeout_cancel(netif->inet4_lladdr_tmo);
        if (ipnet_timeout_schedule(sec * 1000,
                                   (Ipnet_timeout_handler) ipnet_ip4_lladdr_defend,
                                   netif,
                                   &netif->inet4_lladdr_tmo) == 0)
        {
            ipnet_arp_request(netif,
                              netif->inet4_lladdr.s_addr,
                              IP_TRUE,
                              IP_NULL,
                              IP_NULL);
        }
        break;

    default:
        break;
    }

    IPCOM_LOG3(INFO,
               "IPv4: Changing state to %s for address %s on interface %s",
               states[state],
               ipcom_inet_ntop(IP_AF_INET, &lladdr_n, ipnet->log_buf, sizeof(ipnet->log_buf)),
               netif->ipcom.name);

    netif->inet4_lladdr_state = state;
}


/*
 *===========================================================================
 *                    ipnet_ip4_lladdr_conflict
 *===========================================================================
 * Description: Conflict detected for the link local IPv4 address
 * Parameters:  netif - The interface the conflict was detected at.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip4_lladdr_conflict(Ipnet_netif *netif)
{
    Ip_bool defend = IP_FALSE;

    ipnet_timeout_cancel(netif->inet4_lladdr_tmo);

    if (netif->inet4_lladdr_conflics < 255)
        /* Address conflic since the interface was brought up */
        netif->inet4_lladdr_conflics++;

    if (netif->inet4_lladdr_state == IPNET_IP4_LLADDR_STATE_ACTIVE)
    {
        /* Check if there is a reason to defend this address */
        Ipnet_ip4_addr_entry *addr_entry;
        Ipnet_socket         *sock;

        addr_entry = ipnet_ip4_get_addr_entry(netif->inet4_lladdr.s_addr,
                                              netif->vr_index,
                                              netif);
        if (addr_entry != IP_NULL)
            for (sock = addr_entry->socket_list;
                 sock != IP_NULL && defend == IP_FALSE;
                 sock = sock->addr_next)
                defend = (sock->ipcom.type == IP_SOCK_STREAM);
    }

    if (defend)
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DEFEND);
    else
    {
        IPCOM_LOG2(NOTICE,
                   "IPv4: Conflict for address %s on interface %s",
                   ipcom_inet_ntop(IP_AF_INET, &netif->inet4_lladdr, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   netif->ipcom.name);

        if (netif->inet4_lladdr_state >= IPNET_IP4_LLADDR_STATE_DEFEND)
            (void) ipnet_ip4_remove_addr(netif, netif->inet4_lladdr.s_addr);
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_INIT);
    }
}
#endif /* IPNET_USE_RFC3927 */


/*
 *===========================================================================
 *                    ipnet_ip4_if_configure
 *===========================================================================
 * Description: Adds automatic IPv4 addresses for interfaces that just
 *              entered up state.
 * Parameters:  netif - The interface that is going to be configured.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_if_configure(Ipnet_netif *netif)
{
    struct Ip_in_addr addr;
    struct Ip_in_addr mask;

    if (IP_BIT_ISSET(netif->flags2, IPNET_IFF2_NO_IPV4_SUPPORT))
        return;

    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_MULTICAST))
    {
        /* Add the all multicast hosts address */
        (void) ipcom_inet_pton(IP_AF_INET, "224.0.0.1", &addr);
        (void) ipcom_inet_pton(IP_AF_INET, "240.0.0.0", &mask);
        (void)ipnet_ip4_add_addr2(netif,
                                  addr.s_addr,
                                  mask.s_addr,
                                  IPNET_IP4_ADDR_FLAG_AUTOMATIC,
                                  IPNET_ADDR_TYPE_MULTICAST);
    }

#ifdef IP_PORT_LKM
    if (netif->ipcom.type == IP_IFT_PPP)
    {
        Ipnet_ppp_peer  *p = netif->private_data;

        /* Setup the peer route */
        (void)ipnet_if_set_ipv4_ppp_peer(p,&p->peer4);
    }
#endif

#ifdef IPNET_USE_RFC3927
    netif->inet4_lladdr_conflics = 0;
    ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_INIT);
#endif /* IPNET_USE_RFC3927 */

#ifdef IPNET_USE_RFC1256
    /* Initialize the RFC1256 specific portion of the netif */
    ipnet_ip4_rfc1256_mode_update(netif->vr_index, netif);
#endif
}


/*
 *===========================================================================
 *                    ipnet_ip4_if_unconfigure
 *===========================================================================
 * Description: Removes all automatic IPv4 addresses.
 * Parameters:  netif - The interface that is going to be unconfigured.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_if_unconfigure(Ipnet_netif *netif)
{
    Ipnet_ip4_addr_entry *addr_entry;

    for (addr_entry = netif->inet4_addr_list;
         addr_entry != IP_NULL;
         addr_entry = addr_entry->next)
        IP_BIT_CLR(addr_entry->flags, IPNET_IP4_ADDR_FLAG_UPDATE_DONE);

    addr_entry = netif->inet4_addr_list;
    while (addr_entry != IP_NULL)
    {
        if (IP_BIT_ISSET(addr_entry->flags, IPNET_IP4_ADDR_FLAG_UPDATE_DONE)
            || IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP4_ADDR_FLAG_AUTOMATIC))
        {
            IP_BIT_SET(addr_entry->flags, IPNET_IP4_ADDR_FLAG_UPDATE_DONE);
            addr_entry = addr_entry->next;
        }
        else
        {
            IP_BIT_SET(addr_entry->flags, IPNET_IP4_ADDR_FLAG_UPDATE_DONE);
            (void )ipnet_ip4_remove_addr(netif, addr_entry->ipaddr_n);
            addr_entry = netif->inet4_addr_list;
        }
    }

    ipnet_timeout_cancel(netif->igmpv1_querier_present_tmo);
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    ipnet_timeout_cancel(netif->igmpv2_querier_present_tmo);
#endif

#ifdef IPNET_USE_RFC3927
    if (netif->inet4_lladdr_state != IPNET_IP4_LLADDR_STATE_ACTIVE)
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DISABLED);
#endif

#ifdef IPNET_USE_RFC1256
    /* Initialize the RFC1256 specific portion of the netif */
    ipnet_ip4_rfc1256_mode_update(netif->vr_index, netif);
#endif
}


/*
 *===========================================================================
 *                    ipnet_ip4_is_part_of_same_pkt
 *===========================================================================
 * Description: Returns if the two fragments are parts in the same packet.
 * Parameters:  frag1 - A IPv4 fragment.
 *              frag2 - Another IPv4 fragment.
 * Returns:     IP_TRUE or IP_FALSE.
 *
 */
IP_GLOBAL Ip_bool
ipnet_ip4_is_part_of_same_pkt(Ipcom_pkt *frag1, Ipcom_pkt *frag2)
{
    Ipnet_pkt_ip *ip4_hdr1;
    Ipnet_pkt_ip *ip4_hdr2;

    ip4_hdr1 = (Ipnet_pkt_ip *) &frag1->data[frag1->ipstart];
    ip4_hdr2 = (Ipnet_pkt_ip *) &frag2->data[frag2->ipstart];

    /* RFC 791 p24
       The internet identification field (ID) is used together with the
       source and destination address, and the protocol fields, to identify
       datagram fragments for reassembly. */
    if (ip4_hdr1->id != ip4_hdr2->id
        || ip4_hdr1->p != ip4_hdr2->p
        || IPNET_IP4_GET_IPADDR(ip4_hdr1->src) != IPNET_IP4_GET_IPADDR(ip4_hdr2->src)
        || IPNET_IP4_GET_IPADDR(ip4_hdr1->dst) != IPNET_IP4_GET_IPADDR(ip4_hdr2->dst))
        return IP_FALSE;
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_ip4_get_offset
 *===========================================================================
 * Description: Returns the offset for this segment into the IP packet.
 * Parameters:  frag - A IPv4 fragment.
 * Returns:     The offset.
 *
 */
IP_STATIC int
ipnet_ip4_get_offset(Ipcom_pkt *frag)
{
    Ipnet_pkt_ip *ip4_hdr;

    ip4_hdr = (Ipnet_pkt_ip *) &frag->data[frag->ipstart];
    return ip_ntohs(ip4_hdr->off & IPNET_OFF_MASK) << 3;
}


/*
 *===========================================================================
 *                    ipnet_ip4_more_fragments
 *===========================================================================
 * Description: Returns if this is the last fragment or not.
 * Parameters:  frag - A IPv4 fragment.
 * Returns:     IP_TRUE or IP_FALSE.
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_more_fragments(Ipcom_pkt *frag)
{
    Ipnet_pkt_ip *ip4_hdr;

    ip4_hdr = (Ipnet_pkt_ip *) &frag->data[frag->ipstart];
    return IP_BIT_ISSET(ip4_hdr->off, IPNET_OFF_MF);
}


/*
 *===========================================================================
 *                    ipnet_ip4_update_ip_header
 *===========================================================================
 * Description: Sets the IPv4 length, offset and chk for the reassembled
 *              packet.
 * Parameters:  pkt - The reassembled IPv4 packet.
 *              frag_list - The individual fragments that was received.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_update_ip_header(Ipcom_pkt *pkt, Ipcom_pkt *frag_list)
{
    Ipnet_pkt_ip *ip4_hdr;

    IPCOM_UNUSED_ARG(frag_list);
    ip4_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    /* This packet does not contain a fragmentation header */
    ip4_hdr->len = (Ip_u16) ip_htons(pkt->end - pkt->start);
    ip4_hdr->off = 0;
    /* Do not need to calculate the checksum since it was done
       on the individual fragments */
    ip4_hdr->sum = 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_fragment_timeout
 *===========================================================================
 * Description: Called if reassembly of the packet times out.
 * Parameters:  frag_head - The first packet in the list of fragments.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_fragment_timeout(Ipcom_pkt *frag_head)
{
    Ipcom_pkt        *f;
    Ipnet_pkt_ip     *ip_hdr;
    struct Ip_in_addr to;
    Ipnet_icmp_param  p;
#ifdef IPCOM_USE_MIB2
    Ipnet_netif *netif = ipnet_if_indextonetif(frag_head->vr_index, frag_head->ifindex);

    if (netif != IP_NULL)
    {
        IPCOM_MIB2(ipReasmFails++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsReasmFails, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsReasmFails, 1, netif);
    }
#endif /* IPCOM_USE_MIB2 */

    /* Unhook the fragment list from the reassembly list */
    ip_assert(ipnet->ip4.reassembly_list == frag_head);
    ipnet->ip4.reassembly_list = frag_head->next;

    /* Possible send a time exceeded message,
       will only be sent if the first fragment has been received */
    ip_hdr = (Ipnet_pkt_ip *) &frag_head->data[frag_head->ipstart];
    to.s_addr = IPNET_IP4_GET_IPADDR(ip_hdr->src);
    ipcom_memset(&p, 0, sizeof(p));
    p.type       = IPNET_ICMP4_TYPE_TIME_EXCEEDED;
    p.code       = IPNET_ICMP4_CODE_TIM_REASSEMBLY;
    p.recv_pkt   = frag_head;
    p.vr_index   = frag_head->vr_index;
    p.to         = &to;
    p.tos        = ip_hdr->tos;
    p.rate_limit = IP_TRUE;
    (void) ipnet_icmp4_send(&p, IP_FALSE);
    ipcom_pkt_free(frag_head);

    /* Add a timeout for the next entry (if any) */
    if (IP_NULL != (f = ipnet->ip4.reassembly_list))
        (void) ipnet_timeout_schedule(IPNET_PKT_GET_TIMEOUT(f) * 1000,
                                      (Ipnet_timeout_handler) ipnet_ip4_fragment_timeout,
                                      f,
                                      IPNET_PKT_GET_TMO_PTR(f));
}


/*
 *===========================================================================
 *                    ipnet_ip4_fragment_tmo_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_ip4_fragment_tmo_to_string(Ipcom_pkt *frag_head, char *buf, int buf_len)
{
    Ipnet_pkt_ip *ip_hdr;
    char          dst[IP_INET_ADDRSTRLEN];
    char          src[IP_INET_ADDRSTRLEN];

    ip_hdr = (Ipnet_pkt_ip *) &frag_head->data[frag_head->ipstart];
    return ipcom_snprintf(buf,
                          buf_len,
                          "IPv4 fragment id %d  src %s  dst %s",
                          ip_ntohs(ip_hdr->id),
                          ipcom_inet_ntop(IP_AF_INET, ip_hdr->src, src, sizeof(src)),
                          ipcom_inet_ntop(IP_AF_INET, ip_hdr->dst, dst, sizeof(dst)));
}
#endif /* IPNET_DEBUG */


/*
 *===========================================================================
 *                     ipnet_ip4_get_next_binding_rt
 *===========================================================================
 * Description: Gets the next binding route.
 * Parameters:  rt - A route entry.
 * Returns:     The route for the next binding or IP_NULL if no such route
 *              exists.
 *
 */
IP_STATIC Ipnet_route_entry *
ipnet_ip4_get_next_binding_rt(Ipnet_route_entry *rt)
{
    Ipnet_route_entry *next_binding_rt = rt->next;

    while (next_binding_rt != IP_NULL
           && IP_BIT_ISFALSE(next_binding_rt->hdr.flags, IPNET_RTF_MBINDING))
        next_binding_rt = next_binding_rt->next;

    return next_binding_rt;
}


/*
 *===========================================================================
 *                     ipnet_icmp4_param_init
 *===========================================================================
 * Description: Initializes the ICMPv4 parameter structure to create
 *              ICMP messages on failing packets.
 * Parameters:  [out] icmp_param - Initialized ICMP param struct
 *              [in] pkt - The packet that caused the error to be sent.
 *              [in] ip4_param - The IPv4 parameters for error packet.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_icmp4_param_init(Ipnet_icmp_param *icmp_param,
                       Ipcom_pkt *pkt,
                       Ipnet_ip4_output_param *ip4_param)
{
    ipcom_memset(icmp_param, 0, sizeof(*icmp_param));
    if (ipnet_ip4_get_addr_type(ip4_param->to->s_addr, pkt->vr_index, IP_NULL)
        == IPNET_ADDR_TYPE_UNICAST)
        icmp_param->from = ip4_param->to;
#ifdef IPNET_USE_VRRP
    else if (pkt != IP_NULL)
    {
        Ipnet_netif *netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);
        Ip_u8        vrrp_id = IPCOM_PKT_GET_VRID(pkt);

        if (ipnet_vrrp_owns_addr(netif,
                                 vrrp_id,
                                 *ip4_param->to))
            icmp_param->from = ip4_param->to;
        else if (vrrp_id != 0)
        {
            /* Sent to this host via an VRRP link address, use a VRRP address
               in the response */
            Ipnet_vrrp_addr_t *vrrp_addr;
            vrrp_addr = ipnet_vrrp_get_addr_entry(netif, vrrp_id);
            if (vrrp_addr != IP_NULL)
                icmp_param->from = &vrrp_addr->addrs[0];
        }
    }
#endif

    if (icmp_param->from == IP_NULL)
        /* Select the local address that is most appropriate for the
           address the failing packet was sent to */
        icmp_param->from = ipnet_ip4_get_src_addr(pkt->vr_index,
                                                  ip4_param->to,
                                                  IP_NULL,
                                                  IP_NULL);

    icmp_param->to         = ip4_param->from;
    icmp_param->tos        = ip4_param->tos;
    icmp_param->vr_index   = pkt->vr_index;
    icmp_param->recv_pkt   = pkt;
    icmp_param->rate_limit = IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_icmp4_rate_reseed
 *===========================================================================
 * Description: Reseeds the rate limit counter for ICMP packets.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp4_rate_reseed(void *unused)
{
    IPCOM_UNUSED_ARG(unused);
    ipnet->ip4.icmp_send_limit = (Ip_u32)
        ipcom_sysvar_get_as_int0("ipnet.inet.IcmpRatelimitBucketsize", 10);
}


/*
 *===========================================================================
 *                    ipnet_ip4_send_host_unreachable
 *===========================================================================
 * Description: Sends a ICMP host unreachable
 * Parameters:  pkt - Packet that could not be sent.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip4_send_host_unreachable(Ipcom_pkt *pkt)
{
    Ipnet_icmp_param  icmp_param;
    Ipnet_pkt_ip     *ip = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    struct Ip_in_addr error_from;
    struct Ip_in_addr error_to;
    Ipnet_ip4_output_param p;

    if (pkt->ipstart < pkt->start
        || pkt->ipstart + IPNET_IP_HDR_SIZE > pkt->end
        || (ip->v_hl & 0xF0) != 0x40)
        /* Cannot be a valid IPv4 packet */
        return;

    error_from.s_addr = IPNET_IP4_GET_IPADDR(ip->src);
    error_to.s_addr   = IPNET_IP4_GET_IPADDR(ip->dst);

    ipcom_memset(&p, 0, sizeof(p));
    p.from = &error_from;
    p.to   = &error_to;
    p.tos  = ip->tos;

    ipnet_icmp4_param_init(&icmp_param, pkt, &p);
    icmp_param.type = IPNET_ICMP4_TYPE_DST_UNREACHABLE;
    icmp_param.code = IPNET_ICMP4_CODE_DST_UNREACH_HOST;

    (void) ipnet_icmp4_send(&icmp_param, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_icmp4_send
 *===========================================================================
 * Description: Creates and send a ICMPv4 message.
 * Parameters:  The parameters for to create the ICMP message.
 * Returns:     0 = success, <0 error code.
 *
 */
IP_GLOBAL int
ipnet_icmp4_send(Ipnet_icmp_param *icmp_param, Ip_bool is_igmp)
{
    Ipnet_ip4_output_param param;
    Ipnet_pkt_icmp        *icmp;
    Ipcom_pkt             *pkt;
    Ip_bool                request_reply_msg    = IP_FALSE;
    Ipnet_netif           *recv_netif           = IP_NULL;

    ip_assert(icmp_param->to != IP_NULL);

    if (icmp_param->recv_pkt != IP_NULL)
        recv_netif = ipnet_if_indextonetif(icmp_param->recv_pkt->vr_index, icmp_param->recv_pkt->ifindex);

    switch (icmp_param->type)
    {
    case IPNET_ICMP4_TYPE_TIME_EXCEEDED:
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                             recv_netif,
                                             "IcmpSendTimeExceeded",
                                             1,
                                             ipnet_bool_map) == 0)
            /* Silently ignore */
            return 0;
        break;

    case IPNET_ICMP4_TYPE_DST_UNREACHABLE:
        if (icmp_param->code == IPNET_ICMP4_CODE_DST_UNREACH_NET ||
            icmp_param->code == IPNET_ICMP4_CODE_DST_UNREACH_HOST)
        {
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInNoRoutes, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInNoRoutes, 1, recv_netif);
        }

        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                             recv_netif,
                                             "IcmpSendDestinationUnreachable",
                                             1,
                                             ipnet_bool_map) == 0)
            /* Silently ignore */
            return 0;
        break;

    case IPNET_ICMP4_TYPE_ROUTER_ADVERT:
    case IPNET_ICMP4_TYPE_ECHO_REPLY:
    case IPNET_ICMP4_TYPE_TSTAMP_REPLY:
        request_reply_msg = IP_TRUE;
        break;

    default:
        break;
    }

    if (ipnet_route_is_virtual_router_valid(icmp_param->vr_index) == IP_FALSE)
    {
        /* The virtual router has been deleted */
        IPCOM_LOG4(NOTICE,
                   "IPv4: send ICMP type:%d code:%d to %s dropped since VR#%u has been removed",
                   icmp_param->type,
                   icmp_param->code,
                   ipcom_inet_ntop(IP_AF_INET, icmp_param->to, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   icmp_param->vr_index);
        goto abort_send;
    }

    if (icmp_param->rate_limit
        && (icmp_param->recv_pkt == IP_NULL
            || ipnet_is_loopback(icmp_param->vr_index, icmp_param->recv_pkt->ifindex)))
    {
        if (ipnet->ip4.icmp_send_limit_tmo == IP_NULL)
        {
            int ratelimit_interval;
            if (ipnet->ip4.icmp_send_limit == 0)
                /* First time called since boot, initialize the ICMP message count */
                ipnet_icmp4_rate_reseed(IP_NULL);

            ratelimit_interval = ipcom_sysvar_get_as_int0("ipnet.inet.IcmpRatelimitInterval", 100);
            if (ratelimit_interval)
                (void) ipnet_timeout_schedule(ratelimit_interval,
                                              (Ipnet_timeout_handler) ipnet_icmp4_rate_reseed,
                                              IP_NULL,
                                              &ipnet->ip4.icmp_send_limit_tmo);
            else
                /* Disable ratelimit */
                ipnet->ip4.icmp_send_limit = ~0u;
        }

        if (ipnet->ip4.icmp_send_limit == 0)
        {
            IPCOM_LOG3(DEBUG2,
                       "IPv4: send ICMPv4 type:%d code:%d to %s dropped due to rate limit policy",
                       icmp_param->type,
                       icmp_param->code,
                       ipcom_inet_ntop(IP_AF_INET, icmp_param->to, ipnet->log_buf, sizeof(ipnet->log_buf)));
            goto abort_send;
        }
        --ipnet->ip4.icmp_send_limit;
    }

    if (icmp_param->to->s_addr == IP_INADDR_ANY)
        /* We have read back our own broadcast when running as DHCP client */
        return -IP_ERRNO_EINVAL;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 2, IPCOM_WV_NETDEVENT_START,
                       ipnet_icmp4_send, IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
    IPNET_STATS(icmp4_send++);

    ipcom_memset(&param, 0, sizeof(param));
    param.to    = icmp_param->to;
    param.from  = icmp_param->from;
    param.netif = icmp_param->netif;

    if (is_igmp)
    {
        ip_assert(icmp_param->type == IPNET_IGMP4_TYPE_V1_MEMBERSHIP_REPORT
                  || icmp_param->type == IPNET_IGMP4_TYPE_V2_MEMBERSHIP_REPORT
                  || icmp_param->type == IPNET_IGMP4_TYPE_V3_MEMBERSHIP_REPORT
                  || icmp_param->type == IPNET_IGMP4_TYPE_V2_LEAVE_GROUP);
        param.proto = IP_IPPROTO_IGMP;
    }
    else
    {
        ip_assert(icmp_param->type == IPNET_ICMP4_TYPE_TIME_EXCEEDED
                  || icmp_param->type == IPNET_ICMP4_TYPE_REDIRECT
                  || icmp_param->type == IPNET_ICMP4_TYPE_DST_UNREACHABLE
                  || icmp_param->type == IPNET_ICMP4_TYPE_PARAMPROB
                  || icmp_param->type == IPNET_ICMP4_TYPE_ECHO_REPLY
                  || icmp_param->type == IPNET_ICMP4_TYPE_ROUTER_SOLICIT
                  || icmp_param->type == IPNET_ICMP4_TYPE_ROUTER_ADVERT
                  || icmp_param->type == IPNET_ICMP4_TYPE_TSTAMP_REPLY);
        param.proto = IP_IPPROTO_ICMP;
    }

    /* If TTL has been specified, set */
    if (icmp_param->ttl != 0)
        param.ttl = icmp_param->ttl;
    else
    {
        if (is_igmp)
            /* IGMP/MULTICAST messages should only be link-local */
            param.ttl = 1;
        else
            param.ttl = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.inet.BaseHopLimit", 64);
    }


    if (request_reply_msg)
    {
        Ipnet_pkt_ip *ip_hdr;
#ifdef IP_PORT_LKM
        /* Clone the received packet and just change the type */
        pkt = ipnet_pkt_clone(icmp_param->recv_pkt, IP_TRUE);
        ipcom_pkt_free(icmp_param->recv_pkt);

        if (pkt == IP_NULL)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                              1, 3, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                              ipnet_icmp4_send, IPCOM_WV_NETD_NOBUFS,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(icmp4_send_nomem++);
            IPCOM_MIB2(icmpOutErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsOutErrors, 1);
            return -IP_ERRNO_ENOMEM;
        }

#else
        /* Use the received packet and just change the type */
        pkt = icmp_param->recv_pkt;
#endif
        pkt->ifindex = 0;
        icmp_param->recv_pkt = IP_NULL;

        /* Copy the don't fragment bit */
        ip_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
        if (IP_BIT_ISSET(ip_hdr->off, IPNET_OFF_DF))
            IP_BIT_SET(param.flags, IPNET_IP4_OPF_DONT_FRAG);
    }
    else
    {
        /* Create a reply packet. */
        pkt = ipcom_pkt_malloc(param.netif != IP_NULL
                               ? param.netif->ipcom.mtu
                               : IPNET_IP_HDR_SIZE + IPNET_ICMP_HDR_SIZE + IPNET_ICMP_MAX_SIZE,
                               IP_FLAG_FC_STACKCONTEXT);
        if (pkt == IP_NULL)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                              1, 4, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                              ipnet_icmp4_send, IPCOM_WV_NETD_NOBUFS,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(icmp4_send_nomem++);
            IPCOM_MIB2(icmpOutErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsOutErrors, 1);
            return -IP_ERRNO_ENOMEM;
        }
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING);
        /* Set pkt->start to the beginning of ICMPv4 data */
        pkt->start = ipcom_conf_max_link_hdr_size + IPNET_IP_HDR_SIZE;
        /* Make room for the ICMPv4 header */
        pkt->end = pkt->start + IPNET_ICMP_HDR_SIZE;
    }
    icmp = (Ipnet_pkt_icmp*) &pkt->data[pkt->start];
    if (request_reply_msg == IP_FALSE)
        ipcom_memset(&icmp->data, 0, sizeof(icmp->data));
    pkt->vr_index = icmp_param->vr_index;

    /* Add the IP header + Min 64 bits of the failing packet (RFC 792) */
    if (icmp_param->type == IPNET_ICMP4_TYPE_DST_UNREACHABLE
        || icmp_param->type == IPNET_ICMP4_TYPE_REDIRECT
        || icmp_param->type == IPNET_ICMP4_TYPE_PARAMPROB
        || icmp_param->type == IPNET_ICMP4_TYPE_TIME_EXCEEDED)
    {
        Ipcom_pkt    *failing_pkt = icmp_param->recv_pkt;
        Ipnet_pkt_ip *ip_hdr;
        int           len;

        if (failing_pkt->end < failing_pkt->ipstart)
        {
            ipcom_pkt_free(pkt);
            return -IP_ERRNO_EINVAL;
        }

        ip_hdr = (Ipnet_pkt_ip *) &failing_pkt->data[failing_pkt->ipstart];
        if (ip_hdr->off & IPNET_OFF_MASK
            || (ip_hdr->p == IP_IPPROTO_ICMP
                && *((Ip_u8*)ip_hdr + IPNET_IP4_GET_HDR_OCTET_LEN(ip_hdr))
                == IPNET_ICMP4_TYPE_PARAMPROB))
        {
            /* RFC 792 p1
               To avoid the infinite regress of messages about messages
               etc., no ICMP messages are sent about ICMP messages. Also ICMP
               messages are only sent about errors in handling fragment zero of
               fragemented datagrams.*/
            ipcom_pkt_free(pkt);
            return 0;
        }

        len = IP_MIN(pkt->maxlen - pkt->end, failing_pkt->end - failing_pkt->ipstart);
        len = IP_MIN(len, IPNET_IP_HDR_SIZE + IPNET_ICMP_HDR_SIZE + IPNET_ICMP_MAX_SIZE);
        ipcom_memcpy(&pkt->data[pkt->end],
                     &failing_pkt->data[failing_pkt->ipstart],
                     len);
        pkt->end += len;
    }

    /* ICMP/IGMP type specific data. */
    switch (icmp_param->type)
    {
    case IPNET_ICMP4_TYPE_DST_UNREACHABLE:
        if (icmp_param->code == IPNET_ICMP4_CODE_DST_NEEDFRAG)
            IP_SET_HTONL(icmp->data.failing_pkt.next_hop_mtu, icmp_param->data.max_path_mtu);
        break;

    case IPNET_ICMP4_TYPE_REDIRECT:
        ip_assert(icmp_param->data.gateway_addr != IP_NULL);
        ipcom_memcpy(icmp->data.redirect.gateway_addr,
                     icmp_param->data.gateway_addr,
                     sizeof(struct Ip_in_addr));
        break;

    case IPNET_ICMP4_TYPE_PARAMPROB:
        icmp->data.param.pointer = icmp_param->data.param_pointer;
        break;

    case IPNET_IGMP4_TYPE_V1_MEMBERSHIP_REPORT:
    case IPNET_IGMP4_TYPE_V2_MEMBERSHIP_REPORT:
    case IPNET_IGMP4_TYPE_V2_LEAVE_GROUP:
        ip_assert(icmp_param->data.igmp_addr_entry != IP_NULL);
        ipcom_memcpy(icmp->data.igmp.multicast_addr,
                     &icmp_param->data.igmp_addr_entry->ipaddr_n,
                     sizeof(struct Ip_in_addr));
        if (icmp_param->type != IPNET_IGMP4_TYPE_V1_MEMBERSHIP_REPORT)
            IP_BIT_SET(param.flags, IPNET_IP4_OPF_ROUTER_ALERT);
        break;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    case IPNET_IGMP4_TYPE_V3_MEMBERSHIP_REPORT:
        ipnet_igmpv3_create_membership_report(pkt, icmp_param->data.igmp_addr_entry);
        /*
         * RFC 3376 state that TOS of all IGMPv3 messages must be 0xc0
         * and that they must include the router alert option.
         */
        icmp_param->tos = 0xc0;
        IP_BIT_SET(param.flags, IPNET_IP4_OPF_ROUTER_ALERT);
        break;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    case IPNET_ICMP4_TYPE_TSTAMP_REPLY:
        {
            Ip_u32 millisec = ipnet_msec_now();
            IP_SET_HTONL(icmp->data.timestamp.receive, millisec);
            IP_SET_HTONL(icmp->data.timestamp.transmit, millisec);
        }
        break;
    case IPNET_ICMP4_TYPE_ECHO_REPLY:
        {
            Ipnet_pkt_ip_opt_timestamp *ts;

            if (IP_NULL != (ts = ipnet_ip4_get_ip_opt(pkt, IP_IPOPT_TIMESTAMP)))
            {
                /* RFC 1812, pages 58 and 59:
                   If a Record Route and/or Timestamp option is received in an ICMP
                   Echo Request, this option (these options) SHOULD be updated to
                   include the current router and included in the IP header of the
                   Echo Reply message, without truncation. */
                struct Ipnet_ip4_sock_opts *opts = IP_NULL;

                if (pkt->maxlen - pkt->end >= (int) (ts->len + sizeof(int)))
                    opts = (struct Ipnet_ip4_sock_opts *) &pkt->data[pkt->end];
                else if (pkt->start > (int) (ts->len + sizeof(int)))
                    opts = (struct Ipnet_ip4_sock_opts *) pkt->data;

                if (opts != IP_NULL)
                {
                    /* Move the option of the packet buffer so it
                       isn't overwritten during output. */
                    IP_SET_32ON8(&opts->len, ts->len);
                    ipcom_memcpy(opts->opts, ts, ts->len);
                    param.opts = opts;
                }
                /* else: just ignore to include the timestamp option
                   since there is no safe place to put it in the
                   current packet */
            }
        }
    default:
        break;
    }

    icmp->type  = (Ip_u8)icmp_param->type;
    icmp->code  = (Ip_u8)icmp_param->code;
    icmp->cksum = 0;
    icmp->cksum = ipcom_in_checksum(icmp, pkt->end - pkt->start);

    /*
     * Set the TOS field according to RFC 1812 chapter 4.3.2.5
     */
    switch (icmp->type)
    {
    case IPNET_ICMP4_TYPE_DST_UNREACHABLE:
    case IPNET_ICMP4_TYPE_REDIRECT:
    case IPNET_ICMP4_TYPE_TIME_EXCEEDED:
    case IPNET_ICMP4_TYPE_PARAMPROB:
        param.tos = IPNET_ICMP4_TOS_INTERNETWORK_CONTROL;
        break;
    default:
        param.tos = icmp_param->tos;
        break;
    }

    /* Update MIB-2 stats */
    IPCOM_MIB2(icmpOutMsgs++);
    IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsOutMsgs, 1);
    IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpMsgStatsOutPkts[icmp->type], 1);
    switch (icmp->type)
    {
    case IPNET_ICMP4_TYPE_ECHO_REPLY :
        IPCOM_MIB2(icmpOutEchoReps++);
        break;
    case IPNET_ICMP4_TYPE_DST_UNREACHABLE :
        IPCOM_MIB2(icmpOutDestUnreachs++);
        break;
    case IPNET_ICMP4_TYPE_SOURCEQUENCH :
        IPCOM_MIB2(icmpOutSrcQuenchs++);
        break;
    case IPNET_ICMP4_TYPE_REDIRECT :
        IPCOM_MIB2(icmpOutRedirects++);
        break;
    case IPNET_ICMP4_TYPE_ECHO_REQUEST :
        /* Handled separately */
        break;
    case IPNET_ICMP4_TYPE_TIME_EXCEEDED :
        IPCOM_MIB2(icmpOutTimeExcds++);
        break;
    case IPNET_ICMP4_TYPE_PARAMPROB :
        IPCOM_MIB2(icmpOutParmProbs++);
        break;
    case IPNET_ICMP4_TYPE_TSTAMP_REQUEST :
        IPCOM_MIB2(icmpOutTimestamps++);
        break;
    case IPNET_ICMP4_TYPE_TSTAMP_REPLY :
        IPCOM_MIB2(icmpOutTimestampReps++);
        break;
    case IPNET_ICMP4_TYPE_MASK_REQUEST :
        IPCOM_MIB2(icmpOutAddrMasks++);
        break;
    case IPNET_ICMP4_TYPE_MASK_REPLY :
        IPCOM_MIB2(icmpOutAddrMaskReps++);
        break;
    default:
        break;
    }

    return ipnet_ip4_output(&param, pkt);

 abort_send:
    if (request_reply_msg)
    {
        ipcom_pkt_free(icmp_param->recv_pkt);
        icmp_param->recv_pkt = IP_NULL;
    }
    return 0;
}


#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
/*
 *===========================================================================
 *                  ipnet_igmpv3_add_source_to_group_record
 *===========================================================================
 * Description: Possibly adds one source address to a group record.
 * Parameters:  source_addr - The source address that might be added.
 *              d - Callback data.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmpv3_add_source_to_group_record(Ip_u32 *source_addr,
                                        struct Ipnet_igmpv3_report_for_each_data *d)
{
    if (d->set != IP_NULL)
    {
        if (ipcom_set_contains(d->set, source_addr) == IP_FALSE)
            /* Do not add this address, since it is not part of the interface filter */
            return;
    }

    /* This source should be added to the message */
    if (d->pkt->maxlen - d->pkt->end < (int) sizeof(*source_addr))
    {
        /* TODO: create more messages */
        return;
    }
    d->pkt->end += sizeof(*source_addr);
    IPNET_IP4_SET_IPADDR(&d->group_record->source_addr[d->group_record->number_of_sources],
                         *source_addr);
    d->group_record->number_of_sources++;
}


/*
 *===========================================================================
 *                  ipnet_igmpv3_create_membership_report
 *===========================================================================
 * Description: Creates a IGMPv3 membership report.
 * Parameters:  pkt - The packet where the report should be stored.
 *              addr_entry - The multicast address entry a report should
 *                           be created for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmpv3_create_membership_report(Ipcom_pkt *pkt, Ipnet_ip4_addr_entry *addr_entry)
{
    struct Ipnet_igmpv3_report_for_each_data d;
    Ipnet_pkt_igmpv3_report *report;
    Ipcom_set               *set;

    report = (Ipnet_pkt_igmpv3_report *) &pkt->data[pkt->start];
    report->number_of_group_records = ip_htons(1);

    d.pkt          = pkt;
    d.addr_entry   = addr_entry;
    d.set          = IP_NULL;
    d.group_record = &report->group_records[0];
    d.group_record->aux_data_len      = 0;
    d.group_record->number_of_sources = 0;
    IPNET_IP4_SET_IPADDR(&d.group_record->multicast_addr, addr_entry->ipaddr_n);
    pkt->end = pkt->start + IPNET_IGMPV3_REPORT_MIN_SIZE;

    switch (addr_entry->mcast.report_type)
    {
    case IPNET_MCAST_REPORT_SPECIFIC_QUERY:
        set = addr_entry->mcast.specific_query_sources;
        if (set == IP_NULL || set->user == IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE)
            goto current_state_record;

        /* "Current-State Record" as response to group and source specific query */
        ip_assert(set->user == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE);
        d.set = addr_entry->mcast.filter;
        d.group_record->record_type = (Ip_u8) d.set->user;
        break;
    case IPNET_MCAST_REPORT_FILTER_CHANGE:
        /* "Filter-Mode-Change Record" or "Source-List-Change Record" */
        set = addr_entry->mcast.filter_change_sources;
        if (set == IP_NULL)
            goto current_state_record;

        d.group_record->record_type = (Ip_u8) set->user;
        break;
    case IPNET_MCAST_REPORT_GENERAL_QUERY:
        /* "Current-State Record" */
    current_state_record:
        set = addr_entry->mcast.filter;
        if (set == IP_NULL)
        {
            /* Treat as mode IS_EXCLUDE with empty set */
            d.group_record->record_type = IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE;
            return;
        }
        d.group_record->record_type = (Ip_u8) set->user;
        break;
    default:
        return;
    }

    ipcom_set_for_each(set,
                       (Ipcom_set_foreach_cb_func) ipnet_igmpv3_add_source_to_group_record,
                       &d);
    d.group_record->number_of_sources = ip_htons(d.group_record->number_of_sources);
}


/*
 *===========================================================================
 *                  ipnet_igmpv3_build_if_filter
 *===========================================================================
 * Description: Builds filter for multicast address maddr.
 * Parameters:  sockfd - pointer to the socket descriptor which included
 *              and/or excluded sources on this multicast address now will
 *              be included in the filter.
 *              maddr - multicast address to build filter for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmpv3_build_if_filter(int *sockfd, Ipnet_ip4_addr_entry *maddr)
{
    Ipnet_ip4_sock_mcast_data *mcast_data;
    Ipnet_socket              *sock = ipnet->sockets[*sockfd];

    if (sock == IP_NULL)
    {
        /* The socket should always be removed from the set if closed */
        IP_PANIC2();
        return;
    }

    mcast_data = *ipnet_sock_ip4_get_mcast_data(sock,
                                                maddr->netif,
                                                (struct Ip_in_addr *) &maddr->ipaddr_n);
    if (mcast_data == IP_NULL)
    {
        /* The socket must not be in the socket set if is has not joined a group */
        IP_PANIC2();
        return;
    }

    ipnet_mcast_build_if_filter(&maddr->mcast, mcast_data->sources);
}


/*
 *===========================================================================
 *                  ipnet_igmpv3_report_change
 *===========================================================================
 * Description: Creates a report about filter change on a multicast address.
 * Parameters:  netif - The inteface the multicast address is/was assigned to
 *              group - An multicast address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_igmpv3_report_change(Ipnet_netif *netif, struct Ip_in_addr *group)
{
    Ipnet_ip4_addr_entry *addr_entry;

    addr_entry = ipnet_ip4_get_addr_entry(group->s_addr, netif->vr_index, netif);
    if (addr_entry == IP_NULL)
        /* No longer member of this multicast address on this interface */
        return;

    if (ipnet_mcast_build_source_change_report(&addr_entry->mcast,
                                               (Ipcom_set_foreach_cb_func) ipnet_igmpv3_build_if_filter,
                                               addr_entry))
    {
        addr_entry->mcast.filter_change_resend_count = (Ip_u8) netif->igmp_robustness_variable;
        ipnet_igmp_report_filter_change(addr_entry);
    }
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */


/*
 *===========================================================================
 *                    ipnet_igmp_host_compatibility_mode
 *===========================================================================
 * Description: Returns the IGMP mode the host is running at on the specified
 *              interface.
 * Parameters:  netif - The interface that joined the multicast group.
 * Returns:     IPNET_IGMP_V[1|2|3}_MODE
 *
 */
IP_STATIC int
ipnet_igmp_host_compatibility_mode(Ipnet_netif *netif)
{
    int compat_version;
    int maximum_igmp_version =
        ipnet_sysvar_netif_get_as_int(IP_AF_INET,
                                      netif,
                                      "IgmpVersion",
                                      IPNET_IGMPV3_MODE);

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    compat_version = IPNET_IGMPV3_MODE;
#else
    compat_version = IPNET_IGMPV2_MODE;
#endif

    if (netif->igmpv1_querier_present_tmo != IP_NULL)
        compat_version = IPNET_IGMPV1_MODE;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    else if (netif->igmpv2_querier_present_tmo != IP_NULL)
        compat_version = IPNET_IGMPV2_MODE;
#endif

    return IP_MIN(compat_version, maximum_igmp_version);
}


/*
 *===========================================================================
 *                    ipnet_igmp_should_send_message
 *===========================================================================
 * Description: Determines if an IGMP message should be sent for the
 *              specified multicast address.
 * Parameters:
 * Returns:     >=0 = success , <0 = error code.
 *
 */
IP_STATIC Ip_bool
ipnet_igmp_should_send_message(IP_CONST Ipnet_ip4_addr_entry *addr_entry)
{
    ip_assert(IP_IN_CLASSD(addr_entry->ipaddr_n));

    if (IP_BIT_ISSET(addr_entry->netif->ipcom.flags, IP_IFF_LOOPBACK))
        return IP_FALSE;

    /* RFC 3376, chapter 5
       ...
       The all-systems multicast address, 224.0.0.1, is handled as a
       special case.  On all systems -- that is all hosts and routers,
       including multicast routers -- reception of packets destined to
       the all-systems multicast address, from all sources, is
       permanently enabled on all interfaces on which multicast
       reception is supported.  No IGMP messages are ever sent
       regarding the all-systems multicast address.
       ...
    */

    return addr_entry->ipaddr_n != ip_htonl(IP_INADDR_ALLHOSTS_GROUP);
}


/*
 *===========================================================================
 *                    ipnet_igmp_report
 *===========================================================================
 * Description: Sends a host membership report for the multicast address.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 *              report_type - One of the IPNET_MCAST_REPORT_xxx constants.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmp_report(Ipnet_ip4_addr_entry *addr_entry, Ip_u8 report_type)
{
    Ipnet_icmp_param  icmp_param;
    int               mode;
    Ipnet_netif      *netif = addr_entry->netif;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    struct Ip_in_addr igmpv3_routers;
#endif

    if (!ipnet_igmp_should_send_message(addr_entry))
        return;

    mode = ipnet_igmp_host_compatibility_mode(netif);

    ipcom_memset(&icmp_param, 0, sizeof(icmp_param));
    icmp_param.to = (struct Ip_in_addr *) &addr_entry->ipaddr_n;

    switch (mode)
    {
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    case IPNET_IGMPV3_MODE:
        icmp_param.type = IPNET_IGMP4_TYPE_V3_MEMBERSHIP_REPORT;
        icmp_param.to   = &igmpv3_routers;
        (void)ipcom_inet_pton(IP_AF_INET, "224.0.0.22", &igmpv3_routers);
        break;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */
    case IPNET_IGMPV2_MODE:
        icmp_param.type = IPNET_IGMP4_TYPE_V2_MEMBERSHIP_REPORT;
        break;
    default:
        icmp_param.type = IPNET_IGMP4_TYPE_V1_MEMBERSHIP_REPORT;
        break;
    }

    addr_entry->mcast.report_type = report_type;
    icmp_param.netif    = netif;
    icmp_param.vr_index = netif->vr_index;
    icmp_param.data.igmp_addr_entry = addr_entry;
    (void) ipnet_icmp4_send(&icmp_param, IP_TRUE);

    ipnet_mcast_report_finish(&addr_entry->mcast,
                              (Ipnet_timeout_handler) ipnet_igmp_report_filter_change,
                              addr_entry);
}


/*
 *===========================================================================
 *                    ipnet_igmp_send_host_leave_group
 *===========================================================================
 * Description: Sends a host membership report for the multicast address.
 * Parameters:  addr_entry - The multicast address the host removing.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmp_send_host_leave_group(Ipnet_ip4_addr_entry *addr_entry)
{
    Ipnet_icmp_param  icmp_param;
    struct Ip_in_addr allrtrs;
    int               mode;
    Ipnet_netif      *netif = addr_entry->netif;

    if (!ipnet_igmp_should_send_message(addr_entry))
        return;

    mode = ipnet_igmp_host_compatibility_mode(netif);

    if (mode == IPNET_IGMPV1_MODE)
        return;

    if (mode == IPNET_IGMPV2_MODE)
    {
        ipcom_memset(&icmp_param, 0, sizeof(icmp_param));
        allrtrs.s_addr = ip_htonl(IP_INADDR_ALLRTRS_GROUP);
        icmp_param.type     = IPNET_IGMP4_TYPE_V2_LEAVE_GROUP;
        icmp_param.to       = &allrtrs;
        icmp_param.netif    = netif;
        icmp_param.vr_index = netif->vr_index;
        icmp_param.data.igmp_addr_entry = addr_entry;
        (void) ipnet_icmp4_send(&icmp_param, IP_TRUE);
        return;
    }

    /* IGMPV3 mode */
    ipnet_mcast_clear(&addr_entry->mcast);
    ipnet_igmp_report(addr_entry, IPNET_MCAST_REPORT_FILTER_CHANGE);
}


/*
 *===========================================================================
 *                    ipnet_igmp_report_general_query
 *===========================================================================
 * Description: Sends a host membership report for the multicast address
 *              as a response to a general query.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmp_report_general_query(Ipnet_ip4_addr_entry *addr_entry)
{
    ipnet_igmp_report(addr_entry, IPNET_MCAST_REPORT_GENERAL_QUERY);
}


/*
 *===========================================================================
 *                    ipnet_igmp_report_specific_query
 *===========================================================================
 * Description: Sends a host membership report for the multicast address
 *              as a response to a specific query.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmp_report_specific_query(Ipnet_ip4_addr_entry *addr_entry)
{
    ipnet_igmp_report(addr_entry, IPNET_MCAST_REPORT_SPECIFIC_QUERY);
}


/*
 *===========================================================================
 *                    ipnet_igmp_report_filter_change
 *===========================================================================
 * Description: Sends a host membership report for the multicast address
 *              as a response to a filter change at the socket level.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmp_report_filter_change(Ipnet_ip4_addr_entry *addr_entry)
{
    if (addr_entry->mcast.filter_change_resend_count
        == addr_entry->netif->igmp_robustness_variable)
    {
        ipnet_timeout_cancel(addr_entry->mcast.filter_change_tmo);
        --addr_entry->mcast.filter_change_resend_count;
    }
    ipnet_igmp_report(addr_entry, IPNET_MCAST_REPORT_FILTER_CHANGE);
}


/*
 *===========================================================================
 *                    ipnet_igmp_report_all
 *===========================================================================
 * Description: Sends a host membership report for each multicast address
 *              joined on the interface.
 * Parameters:  netif - The interface to report multicast addresses for.
 *              max_delay_msec - A response must be sent within
 *                               [0..max_delay_msec] milliseconds.
 * Returns:
 *
 */
IP_STATIC void
ipnet_igmp_report_all(Ipnet_netif *netif, Ip_u32 max_delay_msec)
{
    Ipnet_ip4_addr_entry *addr;

    for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
        if (addr->type == IPNET_ADDR_TYPE_MULTICAST)
            ipnet_mcast_schedule_membership_report(&addr->mcast,
                                                   IP_FALSE,
                                                   max_delay_msec,
                                                   (Ipnet_timeout_handler)
                                                   ipnet_igmp_report_general_query,
                                                   addr);
}


/*
 *===========================================================================
 *                    ipnet_igmp_input
 *===========================================================================
 * Description: Handles received IGMP messages. The IGMP checksum is NOT
 *              checked by this function.
 * Parameters:  [in] pkt - The IGMP packet (pkt->start is the offset to the
 *              IGMP header), this function takes ownership of the packet.
 *              [in] ip4_param - The IPv4 parameters for this packet.
 * Returns:     0 = success, <0 error code.
 *
 */
IP_STATIC int
ipnet_igmp_input(Ipnet_netif *netif,
                 Ipcom_pkt *pkt,
                 Ipnet_ip4_output_param *ip4_param)
{
    Ip_u32                max_delay_msec;
    Ipnet_timeout       **ptmo;
    Ip_u32                group_n;
    Ip_bool               is_specific_query = IP_FALSE;
    Ipnet_ip4_addr_entry *addr_entry = IP_NULL;
    unsigned              msg_len = (unsigned) (pkt->end - pkt->start);
    Ipnet_pkt_icmp       *igmp_hdr = (Ipnet_pkt_icmp *) &pkt->data[pkt->start];

    switch (igmp_hdr->type)
    {
    case IPNET_IGMP4_TYPE_MEMBERSHIP_QUERY:
        if (msg_len != IPNET_IGMPV2_QUERY_SIZE
            && (msg_len < IPNET_IGMPV3_MIN_QUERY_SIZE
                || (msg_len != IPNET_IGMPV3_MIN_QUERY_SIZE
                    + ip_ntohs(igmp_hdr->data.igmp.number_of_sources) * sizeof(Ip_u32))))
            /* Not a valid IGMPv1, v2 or v3 query */
            break;

        group_n = IPNET_IP4_GET_IPADDR(igmp_hdr->data.igmp.multicast_addr);
        if (group_n != IP_INADDR_ANY)
        {
            if (IP_IN_CLASSD(group_n) == IP_FALSE)
                /* Invalid group */
                break;
            addr_entry = ipnet_ip4_get_addr_entry(group_n, pkt->vr_index, netif);
        }

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        if (msg_len == IPNET_IGMPV2_QUERY_SIZE)
#endif
        {
            netif->igmp_query_interval = IPNET_MCAST_DEFAULT_QUERY_INTERVAL;

            if (igmp_hdr->code == 0)
            {
                /* send by a IGMPv1 multicast router */
                max_delay_msec = 10 * 1000;
                ptmo = &netif->igmpv1_querier_present_tmo;
            }
            else
            {
                /* send by a IGMPv2 multicast router */
                max_delay_msec = igmp_hdr->code * 100;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
                ptmo = &netif->igmpv2_querier_present_tmo;
#else
                ptmo = IP_NULL;
#endif
            }

#ifndef IPNET_USE_SOURCE_SPECIFIC_MCAST
            if (ptmo != IP_NULL)
#endif
            {
                if (*ptmo == IP_NULL)
                    IPCOM_LOG2(INFO, "IPv4: enter IGMPv%d compatibility mode on interface %s",
                               igmp_hdr->code ? 2 : 1,
                               netif->ipcom.name);
                else
                    ipnet_timeout_cancel(*ptmo);

                (void) ipnet_timeout_schedule(1000 * netif->igmp_robustness_variable * netif->igmp_query_interval
                                              + max_delay_msec,
                                              IP_NULL,
                                              IP_NULL,
                                              ptmo);
            }
        }
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        else
        {
            Ipcom_set *sources = IP_NULL;

            /* IGMPv3 */
            if (addr_entry != IP_NULL)
            {
                if (addr_entry->mcast.specific_query_sources != IP_NULL)
                    /*  Source specific query already pending, append sources to this one */
                    sources = addr_entry->mcast.specific_query_sources;
                else
                {
                    sources = ipcom_set_new(sizeof(struct Ip_in_addr));
                    if (sources != IP_NULL)
                    {
                        sources->user = IPNET_MCAST_RECORD_TYPE_NOT_SET;
                        addr_entry->mcast.specific_query_sources = sources;
                    }
                }
            }

            if (sources != IP_NULL)
            {
                unsigned i;

                is_specific_query = IP_TRUE;

                if (igmp_hdr->data.igmp.number_of_sources == 0
                    || sources->user == IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE)
                {
                out_of_mem:
                    sources->user = IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE;
                    ipcom_set_remove_all(sources);
                }
                else
                {
                    sources->user = IPNET_MCAST_RECORD_TYPE_IS_INCLUDE;
                    for (i = 0; i < ip_ntohs(igmp_hdr->data.igmp.number_of_sources); i++)
                        if (ipcom_set_add(sources, &igmp_hdr->data.igmp.source_addr[i * 2]) == IPCOM_ERR_NO_MEMORY)
                            /* Send report for everything on this group */
                            goto out_of_mem;
                }
            }

            /* The Max Resp Code field specifies the maximum time allowed before
               sending a responding report.  The actual time allowed, called the Max
               Resp Time, is represented in units of 1/10 second */
            max_delay_msec = ipnet_mcast_time_to_msec(igmp_hdr->code) / 10;
            netif->igmp_query_interval = ipnet_mcast_time_to_msec(igmp_hdr->data.igmp.qqic);
            if (netif->igmp_query_interval == 0)
                netif->igmp_query_interval = IPNET_MCAST_DEFAULT_QUERY_INTERVAL;
        }
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

        if (max_delay_msec == 0)
            /* RFC3376 does not directly say that this is invalid,
               but it would result in "ack-implosion" that is specifically mention
               as something not allowed */
            break;

        if (group_n == IP_INADDR_ANY)
            /* Create a Host Membership Report message for each
               multicast group joined on interface 'netif' */
            ipnet_igmp_report_all(netif, max_delay_msec);
        else if (addr_entry != IP_NULL)
            ipnet_mcast_schedule_membership_report(&addr_entry->mcast,
                                                   is_specific_query,
                                                   max_delay_msec,
                                                   (Ipnet_timeout_handler)
                                                   (is_specific_query
                                                    ? ipnet_igmp_report_specific_query
                                                    : ipnet_igmp_report_general_query),
                                                   addr_entry);
        break;
    default:
        break;
    }

    (void)ipnet_ip4_deliver_to_raw_sock(pkt, ip4_param, IP_TRUE);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_deliver_to_raw_sock
 *===========================================================================
 * Description: Delivers a copy of the packet to all matching sockets.
 * Parameters:  pkt - The packet.
 *              ip4_param - cached IP information.
 *              take_ownership_of_pkt - Set to IP_TRUE if the control of the
 *              packet lifetime should be taken care of by this function.
 * Returns:     =0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip4_deliver_to_raw_sock(Ipcom_pkt *pkt,
                           Ipnet_ip4_output_param *ip4_param,
                           Ip_bool take_ownership_of_pkt)
{
    int org_start;
    int matching_sockets;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 5, IPCOM_WV_NETDEVENT_START,
                       ipnet_ip4_deliver_to_raw_sock, IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
    IPNET_STATS(raw4_input++);
    /* Raw packets must contain the IP header. */
    org_start = pkt->start;
    pkt->start = pkt->ipstart;
    matching_sockets = ipnet_raw_input(pkt, take_ownership_of_pkt, ip4_param->proto,
                                       ip4_param->from, 0, ip4_param->to, 0,
                                       (Ipnet_sock_lookup_f) ipnet_sock_ip4_lookup);
    if (take_ownership_of_pkt == IP_FALSE)
        pkt->start = org_start;
    return matching_sockets > 0 ? 0 : -IP_ERRNO_EPROTONOSUPPORT;
}


/*
 *===========================================================================
 *                    ipnet_ip4_try_fast_forward_path
 *===========================================================================
 * Description: Fast packet forward path.
 * Parameters:  innetif - Incoming network interface.
 *              pkt - The incoming packet.
 *              ip_hdr - The IPv4 header for the packet.
 * Returns:     One of the IPNET_FORWARD_CACHE_xxx constants.
 *
 */
#ifdef IPNET_FASTPATH
IP_STATIC int
ipnet_ip4_try_fast_forward_path(Ipnet_netif *innetif, Ipcom_pkt *pkt, Ipnet_pkt_ip *ip_hdr)
{
    Ipnet_ip4_rt_cache *cache = innetif->inet4_last_forward_cache;
    int                 pass;

    /* Try the last used entry in the first pass and look in the
       forward cache in the 2:nd pass */
    for (pass = 0; pass < 2; pass++)
    {
        if (ipnet_route_cache_is_valid(cache->rt_id)
            && cache->dst[0]  == ip_hdr->dst[0]
            && cache->dst[1]  == ip_hdr->dst[1]
            && cache->src[0]  == ip_hdr->src[0]
            && cache->src[1]  == ip_hdr->src[1]
            && cache->ifindex == innetif->ipcom.ifindex
            && cache->tos     == ip_hdr->tos
            && cache->p       == ip_hdr->p
            && ip_hdr->ttl > 1
            && ip_hdr->v_hl == 0x45
            && IP_LIKELY(IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER)))
        {
            Ipnet_netif *outnetif;
            Ip_u32       path_mtu;

            outnetif = cache->rt_next_hop->netif;
            if (cache->rt_next_hop->metrics.rmx_mtu)
                path_mtu = cache->rt_next_hop->metrics.rmx_mtu;
            else
                path_mtu = outnetif->ipcom.mtu;

            if (IP_LIKELY(path_mtu >= (Ip_u32) (pkt->end - pkt->start)
                          && pkt->recurs_level < IPCOM_PKT_MAX_RECURSE_LEVEL
                          && pkt->start >= outnetif->ipcom.link_hdr_size))
            {
#ifdef IPFIREWALL_USE_IP_FILTER
                if (ipfirewall != IP_NULL
                    && ipfirewall_input_hook(&innetif->ipcom, pkt, (void *)cache))
                    return IPNET_FORWARD_CACHE_BLOCKED_BY_FW;
#endif
                IP_INCREMENTAL_CHECKSUM(ip_hdr);

                pkt->ifindex = outnetif->ipcom.ifindex;
#ifdef IPFIREWALL_USE_IP_FILTER
                if (ipfirewall != IP_NULL
                    && ipfirewall_output_hook(&outnetif->ipcom, pkt, (void *)cache))
                    return IPNET_FORWARD_CACHE_BLOCKED_BY_FW;
#endif
#ifdef IPNET_USE_NAT
                if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_NAT)
                    && outnetif->ipcom.ifindex != innetif->ipcom.ifindex)
                {
                    Ip_s32 natret;

                    natret = ipnet_nat_output_hook(&pkt, IP_FALSE);
                    if (natret == IPNET_NAT_TRANSLATED)
                        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NAT);
                    else if (natret == IPNET_NAT_DROP)
                        return IPNET_FORWARD_CACHE_BLOCKED_BY_NAT;
                }
#endif

                ++cache->rt_next_hop->use;
                IPCOM_MIB2(ipForwDatagrams++);
                IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInForwDatagrams, 1);
                IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInForwDatagrams, 1, innetif);
                IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutForwDatagrams, 1);
                IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutForwDatagrams, 1, outnetif);
                IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutTransmits, 1);
                IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutTransmits, 1, outnetif);
                IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
                IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutOctets, pkt->end - pkt->start, outnetif);

                ++pkt->recurs_level;
                IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);

                IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED);

#ifdef IPROHC
                if (iprohc.opened
                    && (outnetif->ipcom.type != IP_IFT_MPLS_TUNNEL)
                    && (outnetif->ipcom.type != IP_IFT_TUNNEL))
                {                 /*
                                   * Do ROHC on a not (MPLS and TUNNEL) interface and
                                   * packet has not been processed by MPLS.
                                   */
                    if (iprohc_output_hook(&outnetif->ipcom, pkt) != IPCOM_SUCCESS)
                    {
                        /* ROHC failed */
                        IPCOM_LOG1(WARNING, "Discarding IPv4 datagram on %s, ROHC failed.", outnetif->ipcom.name);
                        return IPNET_FORWARD_CACHE_BLOCKED_BY_ROHC;
                    }
                }
#endif /* IPROHC */

                if (outnetif->link_ip4_output(outnetif, cache->rt_next_hop, pkt))
                    IPCOM_MIB2(ipRoutingDiscards++);

                innetif->inet4_last_forward_cache = cache;
                return IPNET_FORWARD_CACHE_TAKEN;
            }
        }
        /* The per interface one-behind cache did not match, lets try
           the shared forwarding cache. */
        IPNET_IP4_GET_FORWARD_CACHE_ENTRY(cache, ip_hdr);
    }

    return IPNET_FORWARD_CACHE_MISS;
}
#endif /* IPNET_FASTPATH */


/*
 *===========================================================================
 *                    ipnet_ip4_apply_ancillary_data
 *===========================================================================
 * Description: Applies ancillary data and sticky options to parameters
 *              of the outgoing packet.
 * Parameters:  [in] sock - The socket used when sending the packet.
 *              [in] msg - Message data (can be IP_NULL).
 *              [in, out] param - The IPv4 send parameters.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip4_apply_ancillary_data(Ipnet_socket *sock,
                               IP_CONST struct Ip_msghdr *msg,
                               Ipnet_ip4_output_param *param,
                               Ipcom_pkt *pkt)
{
    struct Ip_cmsghdr     *cmsg;
    struct Ip_in_pktinfo  *pktinfo = IP_NULL;
    struct Ip_sockaddr_in *nexthop = IP_NULL;

    if (msg != IP_NULL && msg->msg_controllen > 0)
    {
        /* Set extensions header specified to sendmsg() (this overrides any sticky options) */
        ip_assert(msg->msg_control != IP_NULL);
        for (cmsg = IP_CMSG_FIRSTHDR(msg);
             cmsg != IP_NULL;
             cmsg = IP_CMSG_NXTHDR(msg, cmsg))
        {
            if (cmsg->cmsg_level != IP_IPPROTO_IP)
                continue;

            switch (cmsg->cmsg_type)
            {
            case IP_IP_TOS:
                param->tos = (Ip_u8) (*(int *) IP_CMSG_DATA(cmsg));
                break;
            case IP_IP_TTL:
                param->ttl = (Ip_u8) (*(int *) IP_CMSG_DATA(cmsg));
                break;
            case IP_IP_ROUTER_ALERT:
                if (*(Ip_u8 *) IP_CMSG_DATA(cmsg))
                    IP_BIT_SET(param->flags, IPNET_IP4_OPF_ROUTER_ALERT);
                break;
            case IP_IP_DONTFRAG:
                if (*(Ip_u8 *) IP_CMSG_DATA(cmsg))
                    IP_BIT_SET(param->flags, IPNET_IP4_OPF_DONT_FRAG);
                break;
            case IP_IP_PKTINFO:
                pktinfo = IP_CMSG_DATA(cmsg);
                break;
            case IP_IP_NEXTHOP:
                nexthop = IP_CMSG_DATA(cmsg);
                break;
            case IP_IP_X_VRID:
                IPCOM_PKT_SET_VRID(pkt, *(Ip_u8*) IP_CMSG_DATA(cmsg));
                break;
            default:
                IPCOM_LOG1(DEBUG, "IPv4: unsupported ancillary data type (%d)",
                           cmsg->cmsg_type);
                return -IP_ERRNO_EOPNOTSUPP;
            }
        }
    }

    if (pktinfo != IP_NULL)
    {
        struct Ip_in_addr *from;

        if (pktinfo->ipi_ifindex != 0)
        {
            param->netif = ipnet_if_indextonetif(sock->vr_index, pktinfo->ipi_ifindex);
            if (param->netif == IP_NULL)
                /* Invalid interface */
                return -IP_ERRNO_ENXIO;
        }

#ifdef IP_PORT_LKM
        from = &pktinfo->ipi_spec_dest;
#else
        from = &pktinfo->ipi_addr;
#endif

        if (from->s_addr != IP_INADDR_ANY)
        {
            /* Check the interface addresses */
            if (ipnet_ip4_get_addr_type(from->s_addr, sock->vr_index, IP_NULL)
                != IPNET_ADDR_TYPE_UNICAST)
            {
                /* Invalid source address */
                return -IP_ERRNO_EADDRNOTAVAIL;
            }
            param->from = from;
        }
    }

    if (nexthop != IP_NULL)
    {
        Ipnet_route_entry *rt_next_hop;
        int                ret;

        ret = ipnet_route_lookup(IP_AF_INET,
                                 sock->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_LINK_LOCAL,
                                 &nexthop->sin_addr,
                                 0,
                                 param->netif ? param->netif->ipcom.ifindex : 0,
                                 &rt_next_hop);
        if (ret < 0)
            return ret;

        /**/
        param->nexthop = rt_next_hop;
    }

    return 0;
}


/*
 *===========================================================================
 *                    IP_STATIC
 *===========================================================================
 * Description: Returns a global address that is most resonable to use by default
 *              The actual address used can be changed by ipcom_bind().
 * Parameters:  vr - The virtual route table to use.
 *              dst_addr - The destination addres
 * Returns:     The 'best' match address or IP_NULL if no address was found
 *              that matches the criteria.
 *
 */
IP_STATIC IP_CONST struct Ip_in_addr *
ipnet_ip4_get_global_src_addr(Ip_u16                       vr,
                              IP_CONST struct Ip_in_addr   *dst_addr)
{
    Ip_u32                dst_n;
    Ipnet_ip4_addr_entry *addr;

    IPCOM_UNUSED_ARG(vr);

    dst_n = IP_GET_32ON16(dst_addr);
    if (!IP_IN_CLASSD(dst_n) && dst_n != IP_INADDR_BROADCAST && dst_n != IP_INADDR_ANY)
    {
        /* Check the global list */
        for (addr = ipnet->ip4.globals; addr != IP_NULL; addr = addr->global_next)
        {
            /* Don't bother with anything but Unicast */
            if (addr->type != IPNET_ADDR_TYPE_UNICAST)
                continue;

            if (IP_BIT_ISFALSE(addr->flags, IPNET_IP4_ADDR_FLAG_PREFERRED))
                continue;

            /* Must be one the correct VR */
            if (vr != addr->netif->vr_index)
                continue;

            /* Must be up */
            if (IP_BIT_ISSET(addr->netif->ipcom.flags, IP_IFF_UP))
                return (struct Ip_in_addr *) &addr->ipaddr_n;
        }
    }

    return IP_NULL;
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ip4_insert_addr_cache
 *===========================================================================
 * Description: Adds an address to a set of hash tables to make it possible
 *              to find the entry in constant time for all types of addresses.
 * Parameters:  addr - The address entry to cache.
 * Returns:     0 = success
 *              -IP_ERRNO_ENOMEM = not enough memory
 *
 */
IP_GLOBAL int
ipnet_ip4_insert_addr_cache(Ipnet_ip4_addr_entry *addr)
{
    Ipnet_ip4_addr_lookup l;
    Ipnet_ip4_addr_entry *a;

    l.ifindex     = addr->netif->ipcom.ifindex;
    l.addr.s_addr = addr->ipaddr_n;
    l.vr          = addr->netif->vr_index;

    ip_assert(ipcom_hash_get(ipnet->ip4.addrs, &l) == IP_NULL);

    /* Direct match on interface, route table and address */
    if (ipcom_hash_add(ipnet->ip4.addrs, addr) != IPCOM_SUCCESS)
        return -IP_ERRNO_ENOMEM;

    if (addr->type != IPNET_ADDR_TYPE_UNICAST)
        return 0;

    /* Match on route table and address */
    a = ipcom_hash_get(ipnet->ip4.addrs_ignore_if, &l);

    if (a == IP_NULL)
    {
        if (ipcom_hash_add(ipnet->ip4.addrs_ignore_if, addr) != IPCOM_SUCCESS)
            return -IP_ERRNO_ENOMEM;
    }
    else
    {
        addr->next_dup_addr = a->next_dup_addr;
        a->next_dup_addr = addr;
    }

    /**/
    if (IP_BIT_ISTRUE(addr->flags, IPNET_IP4_ADDR_FLAG_HOMEADDRESS))
    {
        addr->global_next   = ipnet->ip4.globals;
        ipnet->ip4.globals  = addr;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_remove_addr_cache
 *===========================================================================
 * Description:
 * Parameters:  addr - The address entry to remove the cache for.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip4_remove_addr_cache(Ipnet_ip4_addr_entry *addr)
{
    Ipnet_ip4_addr_entry *a;
    Ipnet_ip4_addr_lookup l;

    (void)ipcom_hash_remove(ipnet->ip4.addrs, addr);

    if (addr->type != IPNET_ADDR_TYPE_UNICAST)
        return;

    /* Match on route table and address */
    l.ifindex     = addr->netif->ipcom.ifindex;
    l.vr          = addr->netif->vr_index;
    l.addr.s_addr = addr->ipaddr_n;
    a = ipcom_hash_get(ipnet->ip4.addrs_ignore_if, &l);
    if (a != IP_NULL)
    {
        if (a == addr)
        {
            (void)ipcom_hash_remove(ipnet->ip4.addrs_ignore_if, a);
            if (a->next_dup_addr != IP_NULL)
                (void)ipcom_hash_add(ipnet->ip4.addrs_ignore_if, a->next_dup_addr);
        }
        else
        {
            while (a->next_dup_addr != addr)
                a = a->next_dup_addr;
            a->next_dup_addr = addr->next_dup_addr;
        }
        addr->next_dup_addr = IP_NULL;
    }

    /**/
    if (IP_BIT_ISTRUE(addr->flags, IPNET_IP4_ADDR_FLAG_HOMEADDRESS))
    {
        Ipnet_ip4_addr_entry **ap;

        /* Go through the globals */
        for (ap = &ipnet->ip4.globals; *ap != IP_NULL; ap = &((*ap)->global_next))
        {
            /* Unlink it */
            if ((*ap) == addr)
            {
                *ap = addr->global_next;
                addr->global_next = IP_NULL;
                break;
            }
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_ip4_remove_addr
 *===========================================================================
 * Description: Removed a address from a interface.
 * Parameters:  netif - The interface that has the address to remove.
 *              ipaddr_n - The address to remove.
 * Returns:     >=0 = success (value returned is the reference count), <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_remove_addr(Ipnet_netif *netif, Ip_u32 ipaddr_n)
{
    Ipnet_ip4_addr_entry **addr_it;
    Ipnet_ip4_addr_entry  *del_addr = IP_NULL;
    int                    ret = 0;

    for (addr_it = &netif->inet4_addr_list; *addr_it != IP_NULL; addr_it = &(*addr_it)->next)
    {
        if ((*addr_it)->ipaddr_n == ipaddr_n)
        {
            del_addr = *addr_it;
            break;
        }
    }

    if (del_addr == IP_NULL)
        /* No such address */
        return -IP_ERRNO_EADDRNOTAVAIL;

    if (--del_addr->ref_count > 0)
        return 0;

    /* Cancel any running timer for this address */
    ipnet_timeout_cancel(del_addr->tmo);

    /* Remove the entry from the list */
    *addr_it = del_addr->next;

    if (del_addr->socket_list != IP_NULL)
    {
        Ipnet_ip4_addr_entry *addr_entry = IP_NULL;

        /* Clients can bind to multicast addresses not joined by IPNET
           since that some UNIX application expects that to be possible */
        ip_assert(del_addr->type != IPNET_ADDR_TYPE_MULTICAST);
        /* One or more sockets are bound to this address */
        if (del_addr->type == IPNET_ADDR_TYPE_UNICAST)
            addr_entry = del_addr->next_dup_addr;
        else
        {
            Ipnet_route_entry *rt;
            int                r;

            r = ipnet_route_lookup(IP_AF_INET,
                                   netif->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   0,
                                   &del_addr->ipaddr_n,
                                   0,
                                   0,
                                   &rt);
            while (r == IPNET_ROUTE_PERFECT_MATCH
                   && addr_entry == IP_NULL
                   && rt != IP_NULL)
            {
                addr_entry = ipnet_ip4_get_addr_entry(del_addr->ipaddr_n, netif->vr_index, rt->netif);
                rt = rt->next;
            }
        }

        if (addr_entry == IP_NULL)
            /* No other interface has this address assigned, notify all sockets */
            ipnet_sock_bind_addr_removed(del_addr->socket_list);
        else
            ipnet_sock_change_addr_entry(del_addr->socket_list, &addr_entry->socket_list, addr_entry);
    }

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP4_ADDR_FLAG_LOOPBACK_RT))
        (void) ipnet_route_delete2(IP_AF_INET,
                                   netif->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &del_addr->ipaddr_n,
                                   IP_NULL,
                                   IP_NULL,
                                   0,
                                   0,
                                   0,
                                   IP_TRUE);

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP4_ADDR_FLAG_HOST_RT))
        (void) ipnet_route_delete2(IP_AF_INET,
                                   netif->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &del_addr->ipaddr_n,
                                   IP_NULL,
                                   IP_NULL,
                                   0,
                                   0,
                                   0,
                                   IP_TRUE);

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP4_ADDR_FLAG_NETWORK_RT))
    {
        Ip_u32 netaddr_n;

        netaddr_n = del_addr->ipaddr_n & del_addr->netmask_n;
        (void) ipnet_route_delete2(IP_AF_INET,
                                   netif->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &netaddr_n,
                                   &del_addr->netmask_n,
                                   IP_NULL,
                                   netif->ipcom.ifindex,
                                   0,
                                   0,
                                   IP_TRUE);
    }

    /* Invalidate the route cache tag since assigning this address
       might have affects on the outcome of lookups */
    ipnet_route_cache_invalidate();
    ipnet_ip4_remove_addr_cache(del_addr);


    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP4_ADDR_FLAG_NETBRD))
    {
        Ip_u32 brdcast_n;

        /*
         * Remove new style broadcast {<network>,-1}
         */
        brdcast_n = (del_addr->netmask_n & del_addr->ipaddr_n) | ~del_addr->netmask_n;
        (void) ipnet_ip4_remove_addr(netif, brdcast_n);

        /*
         * Remove old style broadcast {<network>,-1}
         */
        brdcast_n = (del_addr->netmask_n & del_addr->ipaddr_n);
        (void) ipnet_ip4_remove_addr(netif, brdcast_n);
    }

    if (del_addr->type == IPNET_ADDR_TYPE_UNICAST || del_addr->type == IPNET_ADDR_TYPE_MULTICAST)
    {
        ipnet_kioevent(netif, IP_EIOXDELADDR, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        IPNET_ROUTESOCK(ipnet_routesock_addr_delete(netif, IP_AF_INET, del_addr));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_ip4_addr_del(netif, del_addr));
        IPCOM_LOG2(INFO, "IPv4: removed %s from %s",
                   ipcom_inet_ntop(IP_AF_INET, &ipaddr_n, ipnet->log_buf,
                                   sizeof(ipnet->log_buf)), netif->ipcom.name);
    }
    else
    {
        IPCOM_LOG2(DEBUG, "IPv4: removed %s from %s",
                   ipcom_inet_ntop(IP_AF_INET, &ipaddr_n, ipnet->log_buf,
                                   sizeof(ipnet->log_buf)), netif->ipcom.name);
    }

    if (del_addr->type == IPNET_ADDR_TYPE_MULTICAST)
    {
        if (del_addr->ipaddr_n != ip_htonl(IP_INADDR_ALLHOSTS_GROUP))
            ipnet_igmp_send_host_leave_group(del_addr);
        ipnet_mcast_free(&del_addr->mcast);
    }

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP4_ADDR_FLAG_MCAST))
        ret = ipnet_if_link_ioctl(netif,
                                  IP_SIOCXDELMULTI_IN,
                                  (void*) &ipaddr_n);

#ifdef IPNET_USE_RFC3927
    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP4_ADDR_FLAG_LINK_LOCAL))
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_DISABLED);
    if (del_addr->type == IPNET_ADDR_TYPE_UNICAST
        && (netif->inet4_addr_list == IP_NULL
            || netif->inet4_addr_list->type != IPNET_ADDR_TYPE_UNICAST))
        /* RFC 3927, chapter 1.9
          3. If a host finds that an interface no longer has an operable
          routable address available, the host MAY identify a usable IPv4
          Link-Local address (as described in section 2) and assign that
          address to the interface.*/
        ipnet_ip4_lladdr_set_state(netif, IPNET_IP4_LLADDR_STATE_INIT);
#endif /* IPNET_USE_RFC3927 */

    ipcom_slab_free(ipnet->ip4.addr_slab, del_addr);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip4_change_addr_mask
 *===========================================================================
 * Description: Changes the mask for an existing addres.
 * Parameters:  addr - the address entry to change the mask for.
 *              new_netmask_n - The new mask.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_change_addr_mask(Ipnet_ip4_addr_entry *addr, Ip_u32 new_netmask_n)
{
    Ipnet_netif *netif;
    int          ret;

    if (addr->type == IPNET_ADDR_TYPE_NETBROADCAST)
    {
        /* Mask for network broadcast must always be all ones */
        ip_assert(addr->netmask_n == ~0u);
        return 0;
    }

    if (addr->netmask_n == new_netmask_n)
        /* Mask is already correct */
        return 0;

    if (new_netmask_n != 0 && (new_netmask_n & ip_htonl(0xff000000)) == 0)
    {
        /* The mask did not pass basic sanity checks */
        return -IP_ERRNO_EINVAL;
    }

    ret = 0;
    netif = addr->netif;

    if (IP_BIT_ISSET(addr->flags, IPNET_IP4_ADDR_FLAG_NETWORK_RT))
    {
        struct Ipnet_route_add_param param;
        Ip_u32                       netaddr_n;

        if (new_netmask_n != 0xffffffff)
        {
            /*
             * Add the new network interface route
             */
            ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
            param.domain  = IP_AF_INET;
            param.vr      = netif->vr_index;
            param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
            param.netif   = netif;
            netaddr_n     = addr->ipaddr_n & new_netmask_n;
            param.key     = &netaddr_n;
            param.netmask = &new_netmask_n;
            param.flags   = IPNET_RTF_UP | IPNET_RTF_DONE
                | IPNET_RTF_MASK | IPNET_RTF_CLONING;
            ret = ipnet_route_add(&param);
            if (ret < 0)
                return ret;
        }

        /*
         * Remove the old network interface route
         */
        netaddr_n = addr->ipaddr_n & addr->netmask_n;
        (void) ipnet_route_delete2(IP_AF_INET,
                                   netif->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &netaddr_n,
                                   &addr->netmask_n,
                                   IP_NULL,
                                   addr->netif->ipcom.ifindex,
                                   0,
                                   0,
                                   IP_TRUE);
    }

    if (IP_BIT_ISSET(addr->netif->ipcom.flags, IP_IFF_BROADCAST))
    {
        /*
         * Change the subnet broadcast address
         */
        Ip_u32 netbrd_n;
        int ret2;

        if (addr->netmask_n != 0xffffffff && addr->netmask_n != 0)
        {
            /*
             * Remove the old subnet broadcast address
             * new style broadcast...
             */
            netbrd_n = (addr->ipaddr_n & addr->netmask_n) | ~addr->netmask_n;
            ret = ipnet_ip4_remove_addr(netif, netbrd_n);

            /*
             * ...and old style
             */
            netbrd_n &= addr->netmask_n;
            ret2 = ipnet_ip4_remove_addr(netif, netbrd_n);

            ret = (ret ? ret : ret2);
            IP_BIT_CLR(addr->flags, IPNET_IP4_ADDR_FLAG_NETBRD);
        }

        if (ret >= 0 && new_netmask_n != 0xffffffff)
        {
            /*
             * New style network broadcast {<network>,-1}
             */
            netbrd_n = (new_netmask_n & addr->ipaddr_n) | ~new_netmask_n;
            ret = ipnet_ip4_add_addr(netif,
                                     netbrd_n,
                                     0,
                                     IP_FALSE,
                                     IPNET_ADDR_TYPE_NETBROADCAST);

            /*
             * Old style network broadcast {<network>,0}
             */
            netbrd_n &= new_netmask_n;
            ret2 = ipnet_ip4_add_addr(netif,
                                      netbrd_n,
                                      0,
                                      IP_FALSE,
                                      IPNET_ADDR_TYPE_NETBROADCAST);

            ret = (ret ? ret : ret2);
            IP_BIT_SET(addr->flags, IPNET_IP4_ADDR_FLAG_NETBRD);
        }
    }

    if (ret >= 0)
        addr->netmask_n = new_netmask_n;

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip4_add_addr2
 *===========================================================================
 * Description: Adds an address to an interface.
 * Parameters:  netif - The interface to which the address should be assigned.
 *              ipaddr_n - The address to assign.
 *              netmask_n - The netmask for the address.
 *              primary - Set to IP_TRUE if this address is the new primary address.
 *              addr_type - One of the IPNET_ADDR_TYPE_xxx constants.
 * Returns:     0 = success (value is address ref. count), <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_add_addr(Ipnet_netif *netif, Ip_u32 ipaddr_n, Ip_u32 netmask_n,
                   Ip_bool primary, enum Ipnet_addr_type addr_type)
{
    return ipnet_ip4_add_addr2(netif,
                               ipaddr_n,
                               netmask_n,
                               primary ? IPNET_IP4_ADDR_FLAG_PRIMARY : 0,
                               addr_type);
}


/*
 *===========================================================================
 *                       ipnet_ip4_netif_get_subbrd
 *===========================================================================
 * Description: Gets broadcast address of netif
 * Parameters:  netif - The interface to which the address should be
 *                    assigned.
 * Returns:     subnet broadcast address or 0
 *
 */
IP_GLOBAL int
ipnet_ip4_netif_get_subbrd(Ipnet_netif *netif)
{

    Ipnet_ip4_addr_entry **plast_addr = &netif->inet4_addr_list;
    Ipnet_ip4_addr_entry  *addr;
    Ip_u32                 subbrd_n=0;

    /* Add it after all unicast address(es), any link local IPv4 will
       always be the last unicast address */

    addr = netif->inet4_addr_list;
    if (addr != IP_NULL && addr->type != IPNET_ADDR_TYPE_UNICAST)
        addr = IP_NULL;
    if(addr != IP_NULL)
        subbrd_n  = (addr->ipaddr_n & addr->netmask_n) | ~addr->netmask_n;  /* Old code 1 line */

    while (*plast_addr != IP_NULL
           && ((*plast_addr)->type != IPNET_ADDR_TYPE_NETBROADCAST || ((*plast_addr)->ipaddr_n == subbrd_n)))
            plast_addr = &(*plast_addr)->next;

    /* There was a subnet broadcast address found */
    if(*plast_addr != IP_NULL && (*plast_addr)->type == IPNET_ADDR_TYPE_NETBROADCAST && (*plast_addr)->ipaddr_n != subbrd_n )
        return (*plast_addr)->ipaddr_n;

    return 0;

}


/*
 *===========================================================================
 *                    ipnet_ip4_add_addr2
 *===========================================================================
 * Description: Adds anaddress to an interface.
 * Parameters:  netif - The interface to which the address should be assigned.
 *              ipaddr_n - The address to assign.
 *              netmask_n - The netmask for the address.
 *              flags - IPNET_IP4_ADDR_FLAG_xxx flags that is read/writable.
 *              addr_type - One of the IPNET_ADDR_TYPE_xxx constants.
 * Returns:     0 = success (value is address ref. count), <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_add_addr2(Ipnet_netif *netif, Ip_u32 ipaddr_n, Ip_u32 netmask_n,
                    Ip_u16 flags, enum Ipnet_addr_type addr_type)
{
    Ipnet_ip4_addr_entry *addr;
    int                   ret;

    if (IP_BIT_ISSET(netif->flags2, IPNET_IFF2_NO_IPV4_SUPPORT))
        return -IP_ERRNO_EPFNOSUPPORT;

    if (netmask_n == 0)
        /* No mask specified, used the old class based netmask:s as
           default */
        netmask_n = IP_NETMASK(ipaddr_n);

    addr = ipnet_ip4_get_addr_entry(ipaddr_n, netif->vr_index, netif);
    if (addr != IP_NULL)
    {
        /* Address exist already */
        if (addr->type == IPNET_ADDR_TYPE_MULTICAST
            || addr->type == IPNET_ADDR_TYPE_NETBROADCAST)
                /* Reference count multicast addresses */
            addr->ref_count++;

#ifdef IPTCP
        if (IP_BIT_ISSET(addr->flags, IPNET_IP4_ADDR_FLAG_HOMEADDRESS))
        {
            Ipnet_socket *sock = addr->socket_list;
            Ipnet_socket *next_sock;

            while (sock != IP_NULL)
            {
                /* Locate next socket with the same address */
                next_sock = sock->addr_next;

                /* Do we have a notify CB */
                if (sock->notify_cb != IP_NULL)
                {
                    /* Kickstart it */
                    (*sock->notify_cb) (sock, IPTCP_SOCKET_EVENT_KICKSTART);
                }

                /* Do next socket */
                sock = next_sock;
            }
        }
#endif

        return ipnet_ip4_change_addr_mask(addr, netmask_n);
    }

    addr = ipcom_slab_alloc(ipnet->ip4.addr_slab);
    if (addr == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    /* Initialize the address entry */
#ifdef IPCOM_USE_INET6
    addr->ipv6_mapped_space[2] = ip_htonl(0xffff);
#endif /* IPCOM_USE_INET6 */
    addr->ipaddr_n  = ipaddr_n;
    addr->type      = addr_type;
    addr->ref_count = 1;
    addr->netif     = netif;
    addr->flags     = IPNET_IP4_ADDR_FLAG_RW_MASK & flags;

    if (IP_BIT_ISSET(flags, IPNET_IP4_ADDR_FLAG_PRIMARY))
    {
        addr->next = netif->inet4_addr_list;
        netif->inet4_addr_list = addr;
    }
    else
    {
        Ipnet_ip4_addr_entry **plast_addr = &netif->inet4_addr_list;

        /* Add it after all unicast address(es), any link local IPv4 will
           always be the last unicast address */
        while (*plast_addr != IP_NULL
               && (*plast_addr)->type == IPNET_ADDR_TYPE_UNICAST
               && (addr_type != IPNET_ADDR_TYPE_UNICAST
                   || IP_BIT_ISFALSE((*plast_addr)->flags, IPNET_IP4_ADDR_FLAG_LINK_LOCAL)))
            plast_addr = &(*plast_addr)->next;
        addr->next = *plast_addr;
        *plast_addr = addr;
    }

    ret = ipnet_ip4_insert_addr_cache(addr);

    switch (addr_type)
    {
    case IPNET_ADDR_TYPE_MULTICAST:
        /* Join the multicast link address for this address */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        if ((ret = ipnet_mcast_init(&addr->mcast)) != 0)
            goto cleanup;
#endif

        ret = ipnet_if_link_ioctl(netif, IP_SIOCXADDMULTI_IN, &ipaddr_n);
        if (ret >= 0)
            IP_BIT_SET(addr->flags, IPNET_IP4_ADDR_FLAG_MCAST);
        break;

    case IPNET_ADDR_TYPE_UNICAST:
        ret = ipnet_ip4_change_addr_mask(addr, netmask_n);
        break;

    case IPNET_ADDR_TYPE_NETBROADCAST:
        addr->netmask_n = ~0u;
        break;

    default:
        IP_PANIC2();
        ret = -IP_ERRNO_EINVAL;
        goto cleanup;
    }

    if (ret < 0)
        goto cleanup;

    if (addr_type == IPNET_ADDR_TYPE_UNICAST || addr_type == IPNET_ADDR_TYPE_MULTICAST)
    {
        IPCOM_LOG2(INFO, "IPv4: added %s to %s",
                   ipcom_inet_ntop(IP_AF_INET, &ipaddr_n, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   netif->ipcom.name);
    }
    else
    {
        IPCOM_LOG2(DEBUG, "IPv4: added %s to %s",
                   ipcom_inet_ntop(IP_AF_INET, &ipaddr_n, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   netif->ipcom.name);
    }

    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
        return ipnet_ip4_assign_addr(addr);
    /* else: adding routes will be done when the interface is set UP */
    return 0;

 cleanup:
    (void) ipnet_ip4_remove_addr(netif, ipaddr_n);
    return ret;

}


/*
 *===========================================================================
 *                    ipnet_ip4_kioevent
 *===========================================================================
 * Description: IPv4 handler for IO events on network interfaces.
 * Parameters:  netif - The network interface the event happened on.
 *              event - The event.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip4_kioevent(Ipnet_netif *netif, int event)
{
    Ipnet_ip4_addr_entry *addr;

    switch (event)
    {
    case IP_EIOXUP:
    case IP_EIOXRUNNING:
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
        {
            netif->igmp_robustness_variable = IPNET_MCAST_DEFAULT_ROBUSTNESS_VARIABLE;
            netif->igmp_query_interval = IPNET_MCAST_DEFAULT_QUERY_INTERVAL;

            for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
                if (ipnet_ip4_assign_addr(addr) < 0)
                    /* Start over since the current address has has been removed */
                    addr = netif->inet4_addr_list;
            if (event == IP_EIOXUP)
                ipnet_ip4_if_configure(netif);
#ifdef IPNET_USE_RFC1256
            else
                ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_GENERIC_STATE_INITIALIZING);
#endif
        }
        break;
    case IP_EIOXDOWN:
        ipnet_ip4_if_unconfigure(netif);
        (void)ipnet_route_remove_all(IP_AF_INET, netif);
        for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
        {
            if (IP_BIT_ISSET(addr->flags, IPNET_IP4_ADDR_FLAG_LOOPBACK_RT))
                (void) ipnet_route_delete2(IP_AF_INET,
                                           netif->vr_index,
                                           IPCOM_ROUTE_TABLE_DEFAULT,
                                           &addr->ipaddr_n,
                                           IP_NULL,
                                           IP_NULL,
                                           0,
                                           0,
                                           0,
                                           IP_TRUE);
            /* Clear this bits since all routes to this interface has been removed */
            IP_BIT_CLR(addr->flags, IPNET_IP4_ADDR_FLAG_NETWORK_RT | IPNET_IP4_ADDR_FLAG_LOOPBACK_RT);
        }
        break;
    case IP_EIOXSTOP:
#ifdef IPNET_USE_RFC1256
        ipnet_ip4_rfc1256_state_change(netif, IPNET_RFC1256_GENERIC_STATE_INITIALIZING);
#endif
        break;
    default:
        break;
    }
}


/*
 *===========================================================================
 *                    ipnet_ip4_get_src_addr
 *===========================================================================
 * Description: Returns the address that is most resonable to use by default
 *              for a route.
 *              The actual address used can be changed by ipcom_bind().
 * Parameters:  vr - The virtual route table to use.
 *              dst_addr - The destination addres
 *              rt - The route for 'dst_addr' or IP_NULL if this is not known.
 *              netif - The interface that MUST be used regardless of what
 *                      the route table says or IP_NULL if this should be
 *                      extracted from the route table.
 * Returns:     The 'best' match address or IP_NULL if no address was found
 *              that matches the criteria.
 *
 */
IP_GLOBAL IP_CONST struct Ip_in_addr *
ipnet_ip4_get_src_addr(Ip_u16                       vr,
                       IP_CONST struct Ip_in_addr   *dst_addr,
                       Ipnet_route_entry            *rt,
                       Ipnet_netif                  *netif)
{
    Ip_u32                      dst_n;
    Ipnet_ip4_addr_entry       *addr;
    IP_CONST struct Ip_in_addr *selected_addr;
    Ipnet_route_policy_selector rps;
    Ipnet_netif                *cnetif  = netif;

    /* In case they did not provide an RPS, we assign an empty one. */
    rps.fields = 0;

    /* Verify */
    rps.fields |= IPNET_PR_RULE_DADDR|IPNET_PR_RULE_SADDR;
    rps.daddr  = dst_addr;
    rps.saddr  = &ip_inaddr_any;

    if (rt == IP_NULL
        && ipnet_route_lookup_ecmp(IP_AF_INET,
                                   vr,
                                   0,
                                   dst_addr,
                                   0,
                                   0,
                                   &rps,
                                   &rt,
                                   IP_NULL) < 0)
    {
        if (IP_UNLIKELY(ipnet->ip4.globals != IP_NULL) && netif == IP_NULL)
            return ipnet_ip4_get_global_src_addr(vr, dst_addr);
        return IP_NULL;
    }

    if (cnetif == IP_NULL)
        cnetif = rt->netif;

#ifdef IPMPLS
    if (cnetif != IP_NULL && cnetif->ipcom.type == IP_IFT_MPLS_FIBIF)
        return (struct Ip_in_addr*)ipnet_mpls_get_src_addr(IP_AF_INET, cnetif, (void*)dst_addr);
#endif

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY)
        && ipnet_route_lookup_ecmp(IP_AF_INET,
                                   vr,
                                   IPNET_RTL_FLAG_DONTCLONE,
                                   &((struct Ip_sockaddr_in *) rt->gateway)->sin_addr,
                                   0,
                                   0,
                                   &rps,
                                   &rt,
                                   IP_NULL) < 0)
        rt = IP_NULL;

    /* If no network interface was explicitly specified, check the
       globals; unless destination is LOOPBACK */
    if (netif == IP_NULL && (cnetif == IP_NULL || (IP_BIT_ISFALSE(cnetif->ipcom.flags, IP_IFF_LOOPBACK) && IP_BIT_ISFALSE(cnetif->flags2, IPNET_IFF2_NO_GLOBAL_SRC))))
    {
        /* Check the global list */
        for (addr = ipnet->ip4.globals; addr != IP_NULL; addr = addr->global_next)
        {
            /* Don't bother with anything but Unicast */
            if (addr->type != IPNET_ADDR_TYPE_UNICAST)
                continue;

            if (IP_BIT_ISFALSE(addr->flags, IPNET_IP4_ADDR_FLAG_PREFERRED))
                continue;

            /* Must be up */
            if (IP_BIT_ISSET(addr->netif->ipcom.flags, IP_IFF_UP))
                return (struct Ip_in_addr *) &addr->ipaddr_n;
        }
    }

    if (rt != IP_NULL && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE))
    {
        if (rt->rt_template)
            rt = rt->rt_template;

        if (rt->gateway != IP_NULL
            && rt->gateway->sa_family == IP_AF_INET
            && IP_BIT_ISFALSE(rt->hdr.flags,
                              IPNET_RTF_TUNNELEND | IPNET_RTF_GATEWAY))
            /* The default source address was specified in the route
               entry */
            return &((struct Ip_sockaddr_in *) rt->gateway)->sin_addr;
    }

    dst_n = IP_GET_32ON16(dst_addr);
    if (!IP_IN_CLASSD(dst_n) && dst_n != IP_INADDR_BROADCAST && dst_n != IP_INADDR_ANY)
    {
        if (cnetif != IP_NULL && IP_BIT_ISSET(cnetif->ipcom.flags, IP_IFF_LOOPBACK))
        {
            Ipnet_netif *this_netif;
            Ip_u32       i;

            IPNET_NETIF_FOR_EACH_ON_VR(this_netif, vr, i)
            {
                /* The dst_addr might be one of the addresses on one of the local interface,
                   so all interfaces is searched for a perfect address match */
                for (addr = this_netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
                {
                    if (addr->ipaddr_n == dst_n)
                        return (struct Ip_in_addr *) &addr->ipaddr_n;
                }
            }
        }

        /* Try to find a address on the same subnet as the destination
           address */
        selected_addr = IP_NULL;
        if (cnetif != IP_NULL)
        {
            for (addr = cnetif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
            {
                if (addr->type != IPNET_ADDR_TYPE_UNICAST)
                    /* No more unicast address available on this interface */
                    break;

                if (ipcom_route_key_cmp(32, &dst_n, &addr->ipaddr_n, &addr->netmask_n))
                    selected_addr = (struct Ip_in_addr *) &addr->ipaddr_n;
            }
        }
        if (selected_addr != IP_NULL
            && IPNET_IP4_IS_LINK_LOCAL(selected_addr) == IP_FALSE)
            return selected_addr;
    }

    /* Return any unicast address on the interface */
    if (cnetif != IP_NULL)
    {
        addr = cnetif->inet4_addr_list;
        if (addr != IP_NULL && addr->type == IPNET_ADDR_TYPE_UNICAST)
            return (struct Ip_in_addr *) &addr->ipaddr_n;
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip4_add_route_table
 *===========================================================================
 * Description: Adds a new route table.
 * Parameters:  vr - The virtual router the table should be added for.
 *              table - The ID of the table.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_add_route_table(Ip_u16 vr, Ip_u32 table)
{
    int                          ret;
    Ipcom_route                 *rt_head;

    rt_head = ipcom_route_new_table(sizeof(struct Ip_in_addr) * 8,
                                    ipnet_route_notify_func);

    ip_assert(rt_head != IP_NULL);
    if (rt_head == IP_NULL)
        /* Can't initialize the stack */
        return -IP_ERRNO_ENOMEM;

    ret = ipnet_route_set_rtab(IP_AF_INET, vr, table, rt_head);
    if (ret < 0)
        ipcom_route_free_table(rt_head);
    else
        IPCOM_LOG2(NOTICE, "Adding new IPv4 route table, virtual router %u, table ID %u",
                   vr, table);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip4_configure_route_table
 *===========================================================================
 * Description: Configures a new route table.
 * Parameters:  vr - The virtual router the table should be added for.
 *              table - The ID of the table.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_configure_route_table(Ip_u16 vr, Ip_u32 table)
{
    struct Ipnet_route_add_param param;
    struct Ipnet_rt_metrics      metrics;
    struct Ip_in_addr            addr_n;
    struct Ip_in_addr            mask_n;
    int                          ret;

    (void) ipcom_inet_pton(IP_AF_INET, "255.255.255.255", &addr_n);
    ipcom_memset(&metrics, 0, sizeof(metrics));
    metrics.rmx_hopcount = IPNET_ROUTE_AUTOMATIC_ENTRY_HOPCOUNT;
    metrics.rmx_expire   = IPCOM_ADDR_INFINITE;
    ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
    param.domain  = IP_AF_INET;
    param.vr      = vr;
    param.table   = table;
    param.key     = &addr_n;
    param.flags   = IPNET_RTF_X_BCAST_RO | IPNET_RTF_DONE | IPNET_RTF_REJECT | IPNET_RTF_HOST;
    param.metrics = &metrics;
    ret = ipnet_route_add(&param);
    if (ret < 0)
        return ret;

    (void) ipcom_inet_pton(IP_AF_INET, "224.0.0.0", &addr_n);
    (void) ipcom_inet_pton(IP_AF_INET, "240.0.0.0", &mask_n);
    param.netmask = &mask_n;
    param.flags   = IPNET_RTF_X_MCAST_RO | IPNET_RTF_REJECT | IPNET_RTF_DONE;
    ret = ipnet_route_add(&param);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip4_init
 *===========================================================================
 * Description: Initializes the IPv4 module.
 * Parameters:
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_init(void)
{
#ifdef IPNET_DEBUG
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_ip4_fragment_timeout,
                            (Ipnet_timeout_to_string_f) ipnet_ip4_fragment_tmo_to_string);
#ifdef IPNET_USE_RFC1256
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_ip4_rfc1256_advertise,
                            (Ipnet_timeout_to_string_f) ipnet_ip4_rfc1256_advertise_tmo_to_string);
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_ip4_rfc1256_solicit,
                            (Ipnet_timeout_to_string_f) ipnet_ip4_rfc1256_solicit_tmo_to_string);
#endif
#endif /* IPNET_DEBUG */

    ipnet->ip4.addr_slab
        = ipcom_slab_new("IPNET IPv4 address",
                         IPCOM_SLAB_F_NO_LOCKING | IPCOM_SLAB_F_ZERO | IPCOM_SLAB_F_FEW,
                         sizeof(Ipnet_ip4_addr_entry) + sizeof(Ipnet_mcast_addr),
                         0,
                         IP_NULL,
                         IP_NULL,
                         ipnet->memory_pool);

    ipnet->ip4.addrs = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_ip4_addr_obj_func,
                                      (Ipcom_hash_key_func) ipnet_ip4_addr_key_func,
                                      (Ipcom_hash_cmp_func) ipnet_ip4_addr_cmp_func);

    ipnet->ip4.addrs_ignore_if = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_ip4_addr_ignore_if_obj_func,
                                                (Ipcom_hash_key_func) ipnet_ip4_addr_ignore_if_key_func,
                                                (Ipcom_hash_cmp_func) ipnet_ip4_addr_ignore_if_cmp_func);

    if (ipnet->ip4.addrs == IP_NULL
        && ipnet->ip4.addrs_ignore_if == IP_NULL)
        return -IP_ERRNO_ENOMEM;

#ifdef IPNET_USE_NETLINK
    ipnet_rtnetlink_ip4_init();
#endif

    return 0;
}


/*
 *===========================================================================
 *                         ipnet_ip4_get_addr_type
 *===========================================================================
 * Description: Returns type of the specified address.
 * Parameters:  ipaddr_n - The address for which the type will be returned.
 *              vr - The virtual router the address must be assiged to.
 *              netif - The network interface the address should be assigned
 *              to or IP_NULL if any interface might do.
 * Returns:     IPNET_ADDR_TYPE_xxx constant.
 */
IP_GLOBAL enum Ipnet_addr_type
ipnet_ip4_get_addr_type(Ip_u32 ipaddr_n, Ip_u16 vr, Ipnet_netif *netif)
{
    Ipnet_ip4_addr_entry *addr;

    addr = ipnet_ip4_get_addr_entry(ipaddr_n, vr, netif);
    while (addr != IP_NULL)
    {
        if (IP_BIT_ISSET(addr->netif->ipcom.flags, IP_IFF_UP))
            return addr->type;

        do {
            /* Move to the next interface and skip all entries using the wrong interface if netif != IP_NULL */
            addr = addr->next_dup_addr;
        } while (addr != IP_NULL && netif != IP_NULL && addr->netif != netif);
    }

    /* 255.255.255.255 broadcast. */
    if (ipaddr_n == IP_INADDR_BROADCAST)
        return IPNET_ADDR_TYPE_BROADCAST;

    /* any. */
    if (ipaddr_n == IP_INADDR_ANY)
        return IPNET_ADDR_TYPE_ANY;

    /* not to me. */
    return IPNET_ADDR_TYPE_NOT_LOCAL;
}



/*
 *===========================================================================
 *                         ipnet_ip4_get_addr_type2
 *===========================================================================
 * Description: Returns type of the specified address, interface must be specified.
 * Parameters:  ipaddr_n - The address for which the type will be returned.
 *              netif - The network interface the address must be assigned.
 * Returns:     IPNET_ADDR_TYPE_xxx constant.
 */
IP_GLOBAL enum Ipnet_addr_type
ipnet_ip4_get_addr_type2(Ip_u32 ipaddr_n, Ipnet_netif *netif)
{
    ip_assert(netif != IP_NULL);
    return ipnet_ip4_get_addr_type(ipaddr_n, netif->vr_index, netif);
}


/*
 *===========================================================================
 *                         ipnet_ip4_get_addr_entry
 *===========================================================================
 * Description: Returns the address entry for the specified address.
 * Parameters:  ipaddr_n - The address for which the type will be returned.
 *              vr - The virtual router the address must be assignd to.
 *              netif - The network interface the address should be assigned
 *              to or IP_NULL if any interface might do.
 * Returns:     The address entry or IP_NULL if no such address is assigned
 *              to the interface.
 */
IP_GLOBAL Ipnet_ip4_addr_entry *
ipnet_ip4_get_addr_entry(Ip_u32 ipaddr_n, Ip_u16 vr, Ipnet_netif *netif)
{
    Ipnet_ip4_addr_lookup l;

    l.addr.s_addr = ipaddr_n;
    l.vr          = vr;

    if (netif == IP_NULL)
        return ipcom_hash_get(ipnet->ip4.addrs_ignore_if, &l);

    l.ifindex     = netif->ipcom.ifindex;
    return ipcom_hash_get(ipnet->ip4.addrs, &l);
}


/*
 *===========================================================================
 *                  ipnet_ip4_schedule_host_unreachable
 *===========================================================================
 * Description: Schedules sending of a host unreachable message to the
 *              source of the passed packet.
 * Parameters:  pkt - packet that could not be sent.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip4_schedule_host_unreachable(Ipcom_pkt *pkt)
{
    ipnet_schedule_pkt_report(pkt, (Ipnet_softirq_handler) ipnet_ip4_send_host_unreachable);
}


/*
 *===========================================================================
 *                    ipnet_ip4_pseudo_header_checksum_update
 *===========================================================================
 * Description: Calculates the internet checksum on IPv4 pseudo header.
 * Parameters:  [in] src - The source address.
 *              [in] dst - The destination address.
 *              [in] proto - The protcol ID of the upper layer protocol.
 *              [in] len - The number of octets in the payload for which this is
 *              pseudo header.
 * Returns:     The accumulated 32 checksum including the pseudo header.
 *
 */
IP_STATIC Ip_u32
ipnet_ip4_pseudo_header_checksum_update(IP_CONST struct Ip_in_addr *src,
                                        IP_CONST struct Ip_in_addr *dst,
                                        Ip_u16 proto, Ip_u16 len)
{
    Ip_u32   sum;

    sum  = ((Ip_u16 *) src)[0];
    sum += ((Ip_u16 *) src)[1];
    sum += ((Ip_u16 *) dst)[0];
    sum += ((Ip_u16 *) dst)[1];
    sum += ip_htons(len);
    sum += ip_htons(proto);

    return sum;
}


/*
 *===========================================================================
 *                    ipnet_ip4_sendto
 *===========================================================================
 * Description: Finds out the source and destination address and dispatches
 *              the packet to ipnet_ip4_output(). The transport header
 *              checksum is also finished (if used).
 * Parameters:  sock - The socket to use when sending.
 *              to - The destination address address, ignored if the socket
 *              is in connected state.
 *              pkt - The packet data to send.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip4_sendto(Ipnet_socket *sock, IP_CONST struct Ip_msghdr *msg, Ipcom_pkt *pkt)
{
    Ipnet_ip4_socket       *sock_ip4 = sock->ip4;
    Ipnet_ip4_output_param  param;
    int                     ret;
    enum Ipnet_addr_type    addr_type;
    struct Ip_in_addr       hdrinc_src;

    pkt->fd           = sock->ipcom.fd;
    pkt->vr_index     = sock->vr_index;
    pkt->recurs_level = 0;
#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    IPNET_ETH_PKT_SET_VLAN_TAG(pkt, sock->vlan_tag);
#endif


    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED | IPNET_SOCKET_FLAG_CONNECTING))
        param.to = (struct Ip_in_addr *) &sock_ip4->daddr_n;
    else
    {
        param.to = &((struct Ip_sockaddr_in *) msg->msg_name)->sin_addr;
        if (param.to->s_addr == IP_INADDR_ANY)
        {
            ret = -IP_ERRNO_ENETUNREACH;
            goto  errout;
        }
#ifdef IPIPSEC2
        /* Clear cached flow since destination may have changed. */
        sock->ipcom.ipsec_flow_cache = IP_NULL;
#endif
    }

#ifdef IPCOM_USE_HW_CHECKSUM_TX
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
    {
        pkt->chk = 0;
        pkt->tlstart = pkt->start;
    }
#endif /* IPCOM_USE_HW_CHECKSUM_TX */

    if (IP_LIKELY(ipnet_route_cache_is_valid(sock_ip4->cache.rt_id))
        && IP_LIKELY(param.to->s_addr == *(Ip_u32*) sock_ip4->cache.ip_hdr.dst)
        && (msg == IP_NULL || IP_LIKELY(msg->msg_control == IP_NULL))
        && IP_LIKELY(sock->bind_to_ifindex == 0)
        && IP_LIKELY(IP_BIT_ISFALSE(sock_ip4->flags, IPNET_SOCKET_FLAG_IP4_HDRINCL)))
    {
        Ipnet_netif *netif = sock_ip4->cache.rt_next_hop->netif;
        Ipnet_route_entry *rt_dst = sock_ip4->cache.rt_dst;

        if ((Ip_u32) (ipcom_pkt_get_length(pkt) + IPNET_IP_HDR_SIZE)
            <= (rt_dst->metrics.rmx_mtu ? rt_dst->metrics.rmx_mtu : netif->ipcom.mtu)
#ifdef IPCOM_USE_TSO
            || (pkt->ops->is_tso_active(pkt)
                && IP_BIT_ISSET(netif->ipcom.drv_capabilities, IPCOM_IF_DRV_CAP_IP4_TSO))
#endif
            )
        {
            Ipnet_route_entry *rt_next_hop = sock_ip4->cache.rt_next_hop;
            Ipnet_pkt_ip      *iphdr;

            /* Use the send fast path */
            IPNET_IF_LOCK(netif);
            pkt->ifindex = netif->ipcom.ifindex;

#ifdef IPCOM_ZEROCOPY
            if (((sock->proto == IP_IPPROTO_UDP) && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ZBUF)))
            {
                Ip_u16 *chksum = ipcom_pkt_get_data(pkt, sock->chk_offset);
                *chksum = 0;
            }
            else
#endif

            if (IP_LIKELY(IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM)))
            {
                Ip_u16 *chksum = ipcom_pkt_get_data(pkt, sock->chk_offset);
                pkt->chk += ipnet_ip4_pseudo_header_checksum_update((struct Ip_in_addr *) sock_ip4->cache.ip_hdr.src,
                                                                    (struct Ip_in_addr *) sock_ip4->cache.ip_hdr.dst,
                                                                    sock_ip4->cache.ip_hdr.p,
                                                                    (Ip_u16) ipcom_pkt_get_length(pkt));
                *chksum = ipcom_in_checksum_finish(pkt->chk);
                /* Fix-up the UDP checksum if it was calculated to 0 */
                if (IP_UNLIKELY(*chksum == 0) && sock->proto == IP_IPPROTO_UDP)
                    *chksum = 0xFFFF;

#ifdef IPCOM_USE_HW_CHECKSUM_TX
                if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
                {
                    pkt->chk = sock->chk_offset;
                    *chksum = ~*chksum;
                }
#endif /* IPCOM_USE_HW_CHECKSUM_TX */
            }

            /* Add a cached IPv4 header */
            pkt->start -= IPNET_IP_HDR_SIZE;
            pkt->ipstart = pkt->start;
            iphdr = (Ipnet_pkt_ip *) &pkt->data[pkt->start];
            ipcom_memcpy(iphdr, &sock_ip4->cache.ip_hdr, IPNET_IP_HDR_SIZE);
            iphdr->len = (Ip_u16) ip_htons(ipcom_pkt_get_length(pkt));
            iphdr->id  = (Ip_u16) ipcom_atomic_add_and_return(&ipnet->ip4.id, 1);
            iphdr->sum = 0;
            iphdr->sum = ipcom_in_checksum(iphdr, IPNET_IP_HDR_SIZE);

            IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 6, IPCOM_WV_NETDEVENT_START,
                               ipnet_ip4_sendto, IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_output++);
            IPCOM_MIB2(ipOutRequests++);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutRequests, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutRequests, 1, netif);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutTransmits, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutTransmits, 1, netif);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutOctets, pkt->end - pkt->start, netif);

            ++rt_next_hop->use;

#ifdef IPCOM_USE_HW_CHECKSUM_TX
            if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM)
                && IP_BIT_ISFALSE(netif->ipcom.drv_capabilities, IPCOM_IF_DRV_CAP_IP4_TRANSPORT_CSUM_TX))
                ipnet_finish_transport_checksum(pkt);
#endif

#ifdef IPFIREWALL_USE_IP_FILTER
            if (ipfirewall != IP_NULL
                && ipfirewall_output_hook(&netif->ipcom, pkt, IP_NULL))
            {
                /* Packet blocked. */
                IPNET_IF_UNLOCK(netif);
                ipcom_pkt_free(pkt);
                return -IP_ERRNO_EFIREWALL;
            }
#endif /* IPFIREWALL_USE_IP_FILTER */

            /* Up the recursion level */
            ++pkt->recurs_level;

#ifdef IPROHC
            if (iprohc.opened
                && (netif->ipcom.type != IP_IFT_MPLS_TUNNEL)
                && (netif->ipcom.type != IP_IFT_TUNNEL))
            {
                /*
                 * Do ROHC on a not (MPLS and TUNNEL) interface and
                 * packet has not been processed by MPLS.
                 */
                ret = iprohc_output_hook(&netif->ipcom, pkt);
                if (ret != IPCOM_SUCCESS)
                {
                    /* ROHC failed */
                    IPCOM_LOG1(WARNING,
                               "Discarding IPv4 datagram on %s, ROHC failed.",
                               netif->ipcom.name);
                    IPNET_IF_UNLOCK(netif);
                    ipcom_pkt_free(pkt);
                    return -IP_ERRNO_EROHC;
                }
            }
#endif /* IPROHC */

            ret = netif->link_ip4_output(netif, rt_next_hop, pkt);
            if (ret)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 7, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                                  ipcom_pkt_input, IPCOM_WV_NETD_NETDOWN,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(ip4_output_link_err++);
            }
            IPNET_IF_UNLOCK(netif);
            return ret;
        }
    }

    /* Cache is invalid or cannot be used */
    param.netif       = IP_NULL;
    param.proto       = (Ip_u8) sock->proto;
    param.chksum_ptr  = IP_NULL;
    param.cache       = &sock_ip4->cache;
    param.nexthop     = IP_NULL;
    param.opts        = sock_ip4->opts;
    *(Ip_u32*) sock_ip4->cache.ip_hdr.dst = IP_INADDR_ANY;

    if (IP_IN_CLASSD(param.to->s_addr))
        addr_type = IPNET_ADDR_TYPE_MULTICAST;
    else
    {
        addr_type = ipnet_ip4_get_addr_type(param.to->s_addr,
                                            sock->vr_index,
                                            IP_NULL);
        if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
        {
            /* Check if it is network or subnetwork broadcast */
            Ip_u32               i;
            Ipnet_netif         *netif;
            enum Ipnet_addr_type at = IPNET_ADDR_TYPE_NOT_LOCAL;
            IPNET_NETIF_FOR_EACH(netif, i)
            {
                at = ipnet_ip4_get_addr_type(param.to->s_addr,
                                             sock->vr_index,
                                             netif);
                addr_type = at;
                if (at != IPNET_ADDR_TYPE_NOT_LOCAL)
                    break;
            }
        }
    }

    if (addr_type == IPNET_ADDR_TYPE_UNICAST || addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
        /* Unicast */
        param.ttl = sock->uni_hop_limit;
    else
    {
        /* Multicast or Broadcast */
        if (param.to->s_addr == IP_INADDR_BROADCAST
            && IP_BIT_ISFALSE(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_ALLOW_BROADCAST))
            /* This socket does not allow sending packets to the broadcast address */
        {
            IPCOM_LOG1(NOTICE, "IPv4: socket %d is not allowed to send broadcast packets",
                       sock->ipcom.fd);
            ret = -IP_ERRNO_EACCES;
            goto errout;
        }

        if (addr_type == IPNET_ADDR_TYPE_MULTICAST || param.to->s_addr == IP_INADDR_BROADCAST)
            param.ttl = sock->multi_hop_limit;
        else
            param.ttl = 1;
    }

    if (addr_type == IPNET_ADDR_TYPE_BROADCAST
        || addr_type == IPNET_ADDR_TYPE_NETBROADCAST)
    {
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST);
    }
    else if (IP_IN_CLASSD(param.to->s_addr) && sock_ip4->multicast_if != 0)
    {
        /* A specific interface was set with IP_IP_MULTICAST_IF socket option */
        param.netif = ipnet_if_indextonetif(sock->vr_index, sock_ip4->multicast_if);
        if (param.netif == IP_NULL)
        {
            ret = -IP_ERRNO_ENETUNREACH;
            goto errout;
        }
    }

    if (sock->bind_to_ifindex != 0)
    {
        /* Bound to a specific device */
        param.netif = ipnet_if_indextonetif(sock->vr_index, sock->bind_to_ifindex);
        if (param.netif == IP_NULL)
        {
            ret = -IP_ERRNO_ENETUNREACH;
            goto errout;
        }
    }

    if (sock_ip4->saddr_n != IP_INADDR_ANY
        && ipnet_ip4_get_addr_type(sock_ip4->saddr_n,
                                   sock->vr_index,
                                   IP_NULL)
        == IPNET_ADDR_TYPE_UNICAST)
        /* Bound to a specific unicast address */
        param.from = (IP_CONST struct Ip_in_addr *)&sock_ip4->saddr_n;
    else
        /* Let the stack choose the source address */
        param.from = IP_NULL;

    param.tos = sock_ip4->type_of_service;

    param.flags = 0;
    if (sock_ip4->router_alert)
        IP_BIT_SET(param.flags, IPNET_IP4_OPF_ROUTER_ALERT);
    if (sock_ip4->dont_frag)
        IP_BIT_SET(param.flags, IPNET_IP4_OPF_DONT_FRAG);

    if (msg != IP_NULL && msg->msg_control != IP_NULL)
    {
        /* ipnet_ip4_apply_ancillary_data() may change the param.from address */
        ret = ipnet_ip4_apply_ancillary_data(sock, msg, &param, pkt);
        if (ret < 0)
            goto errout;
    }

    if (IP_UNLIKELY(IP_BIT_ISSET(sock_ip4->flags, IPNET_SOCKET_FLAG_IP4_HDRINCL)))
    {
        /* The user supplies the IP header */
        Ipnet_pkt_ip *ip4;
        int           hdr_len;

        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);

        pkt->ipstart = pkt->start;
        ip4 = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

        /* We're doing hdrinc; */
        if (param.from == IP_NULL)
        {
            /* Check the value in the provided header */
            hdrinc_src.s_addr = IPNET_IP4_GET_IPADDR(ip4->src);

            /* If it's set, specify the 'from', otherwise keep it as
             * NULL in order to force the ip4_output routine to properly
             * set the correct source address based on routing */
            if (hdrinc_src.s_addr != IP_INADDR_ANY)
                param.from = &hdrinc_src;

            if (ip4->id == 0)
                /* User did not specify a ID, set the next available
                   one */
                ip4->id = (Ip_u16) ipcom_atomic_add_and_return(&ipnet->ip4.id, 1);
        }

        /* Do get the protocol from the IP header; in case of raw sockets
         * this is required */
        param.proto = ip4->p;

        hdr_len = IPNET_IP4_GET_HDR_OCTET_LEN(ip4);
        if (hdr_len < IPNET_IP_HDR_SIZE)
        {
            ret = -IP_ERRNO_EINVAL;
            goto errout;
        }

        /* Note: strangely enough the IP length is set in host endian! */
        ip4->len = (Ip_u16)ip_htons(pkt->end - pkt->ipstart);
    }


    if (IP_LIKELY(IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM)))
        param.chksum_ptr = ipcom_pkt_get_data(pkt, sock->chk_offset);

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST))
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_LOOP_MCAST);

#ifdef IPNET_USE_RFC2890
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_GRE_KEY))
        ipcom_pkt_set_info(pkt, IPNET_PKT_INFO_GRE_KEY_OUTPUT, sizeof(sock->gre_key), &sock->gre_key);
#endif

#ifdef IPIPSEC2
    if (IP_UNLIKELY(IP_BIT_ISSET(sock_ip4->flags, IPNET_SOCKET_FLAG_IP4_BYPASS_FLOW)))
    {
        /* Set the flow to bypass */
        sock->ipcom.ipsec_flow_cache = IPIPSEC_BYPASS_FLOW;
    }
#endif

    return ipnet_ip4_output(&param, pkt);

 errout:
    ipcom_pkt_free(pkt);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip4_restore_options
 *===========================================================================
 * Description: Restores IPv4 option to packets for which the header has
 *              been removed for some reason (IPSec, NAT, etc.).
 * Parameters:  pkt - The packet to add the options to.
 *              optbuf - The buffer in wich all options is stored.
 *              optlen - The length of 'optbuf'.
 *              is_first_frag - IP_TRUE if this is the first fragment of a IPv4 packet
 * Returns:     The size of the restored options.
 *
 */
IP_STATIC Ip_u16
ipnet_ip4_restore_options(Ipcom_pkt *pkt, void *optbuf, int optbuflen, Ip_bool is_first_frag)
{
    Ipnet_pkt_ip_opt *opt;
    int               offset = 0;
    int               total_optlen = 0;

    while (offset < optbuflen)
    {
        opt = (Ipnet_pkt_ip_opt *) ((Ip_u8 *) optbuf + offset);

        if (opt->flag_class_num == IP_IPOPT_END)
            break;

        if (opt->flag_class_num == IP_IPOPT_NOOP)
        {
            offset++;
            continue;
        }

        offset += opt->len;

        if (is_first_frag || IP_IPOPT_COPIED(opt->flag_class_num))
        {
            pkt->start -= opt->len;
            total_optlen += opt->len;
            ipcom_memcpy(&pkt->data[pkt->start], opt, opt->len);
        }
    }

    /* Pad up to the next multiple of 4 */
    while (total_optlen & 0x3)
    {
        pkt->data[--pkt->start] = IP_IPOPT_NOOP;
        total_optlen++;
    }

    return (Ip_u16) total_optlen;
}


/*
 *===========================================================================
 *                    ipnet_ip4_add_options
 *===========================================================================
 * Description: Adds any IPv4 options that should be included in the packet.
 * Parameters:  param - Parameters to add to the IPv4 header.
 *              pkt - The packet.
 * Returns:     The total length of all added options.
 *
 */
IP_STATIC int
ipnet_ip4_add_options(Ipnet_ip4_output_param *param, Ipcom_pkt *pkt)
{
    if (param->opts != IP_NULL)
    {
        ip_assert((param->opts->len & 0x3) == 0);

        if (pkt->start < param->opts->len)
            return -IP_ERRNO_ENOSPC;

        pkt->start -= param->opts->len;
        ipcom_memcpy(&pkt->data[pkt->start], param->opts->opts, param->opts->len);
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
        {
            int i;
            Ipnet_pkt_ip_opt_timestamp *ts;
            Ip_u8 *opts;

            /* Add local timestamp if timestamp option is present */
            opts = &pkt->data[pkt->start];
            for (i = 0; i < param->opts->len;)
            {
                ts = (Ipnet_pkt_ip_opt_timestamp  *) (&opts[i]);
                switch (ts->flag_class_num)
                {
                case IP_IPOPT_TIMESTAMP:
                    (void)ipnet_ip4_process_ts_option(ts,
                                                      (Ip_u32 *) param->from,
                                                      pkt);
                    /* Fall through */
                case IP_IPOPT_END:
                    i = param->opts->len;
                    break;
                case IP_IPOPT_NOOP:
                    i++;
                    break;
                default:
                    i += ts->len;
                    break;
                }
            }
        }
        return param->opts->len;
    }
    else if (IP_BIT_ISSET(param->flags, IPNET_IP4_OPF_ROUTER_ALERT))
    {
        /*
           The Router Alert option has the following format:

                 +--------+--------+--------+--------+
                 |10010100|00000100|  2 octet value  |
                 +--------+--------+--------+--------+

            Type:
             Copied flag:  1 (all fragments must carry the option)
             Option class: 0 (control)
             Option number: 20 (decimal)

            Value:  A two octet code with the following values:
              0 - Router shall examine packet
              1-65535 - Reserved
        */
        Ipnet_pkt_ip_opt *ra_opt;

        pkt->start -= 4;
        ra_opt = (Ipnet_pkt_ip_opt *) (&pkt->data[pkt->start]);

        ra_opt->flag_class_num  = IP_IPOPT_RA;
        ra_opt->len             = 0x04;
        ra_opt->data[0]         = 0x00;
        ra_opt->data[1]         = 0x00;
        return 4;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_add_header
 *===========================================================================
 * Description: Adds an IPv4 header, including options (if any).
 * Parameters:  param - Parameters to add to the IPv4 header.
 *              pkt - The packet.
 *              opt_len - Length of IPv4 options.
 *              ipv4_id - The value of the ID field for this IP datagram.
 * Returns:     0 failure else header length.
 *
 */
IP_STATIC Ip_u16
ipnet_ip4_add_header(Ipnet_ip4_output_param *param, Ipcom_pkt *pkt, int opt_len, Ip_u16 ipv4_id)
{
    Ipnet_pkt_ip *ip_pkt;
    int           ipv4_hdr_len;

    /* Not enough space for IP header and 32-bit link cookie. */
    if (pkt->start - 4 < IPNET_IP_HDR_SIZE)
        return 0;

    /* Options has already been added */
    pkt->start  -= IPNET_IP_HDR_SIZE,
    pkt->ipstart = pkt->start;

    ipv4_hdr_len = IPNET_IP_HDR_SIZE + opt_len;

    /* Fill in IP header. */
    ip_pkt = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    ip_pkt->v_hl = (Ip_u8)(0x40 | (ipv4_hdr_len >> 2));
    ip_pkt->tos  = param->tos;
    ip_pkt->len  = (Ip_u16) ip_htons(ipcom_pkt_get_length(pkt));
    ip_pkt->id   = ipv4_id;
    ip_pkt->ttl  = param->ttl;
    ip_pkt->p    = param->proto;
    IPNET_IP4_SET_IPADDR(ip_pkt->src, param->from->s_addr);
    IPNET_IP4_SET_IPADDR(ip_pkt->dst, param->to->s_addr);
    ip_pkt->off  = (Ip_u16) ip_htons(pkt->offset >> 3);
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MORE_FRAG))
        IP_BIT_SET(ip_pkt->off, IPNET_OFF_MF);
    if (IP_BIT_ISSET(param->flags, IPNET_IP4_OPF_DONT_FRAG))
        IP_BIT_SET(ip_pkt->off, IPNET_OFF_DF);

    ip_pkt->sum = 0;
    ip_pkt->sum = ipcom_in_checksum(ip_pkt, ipv4_hdr_len);

    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
    return (Ip_u16)ipv4_hdr_len;
}


/*
 *===========================================================================
 *                    ipnet_ip4_remove_header
 *===========================================================================
 * Description: Removes IPv4 header + any IP options.
 * Parameters:  pkt - The packet which has a IPv4 header.
 *              ipv4_id - Will contain the IP ID field from the removed packet.
 * Returns:     The length of the remove IPv4 header.
 *
 */
IP_STATIC int
ipnet_ip4_remove_header(Ipcom_pkt *pkt, Ip_u16 *ipv4_id)
{
    int           ipv4_opt_len;
    Ipnet_pkt_ip *ip_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR));
    /* The IP header must be recreated */
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
    ipv4_opt_len = IPNET_IP4_GET_OPTS_OCTET_LEN(ip_hdr);
    *ipv4_id = ip_hdr->id;
    pkt->start += ipv4_opt_len + IPNET_IP_HDR_SIZE;
    return ipv4_opt_len;
}


/*
 *===========================================================================
 *                      ipnet_ip4_is_on_same_subnet
 *===========================================================================
 * Description: Checks if "from" and "to" is located on the same subnet
 *              and if that subnet is link-local to "netif".
 * Parameters:  netif -
 *              from -
 *              to -
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_is_on_same_subnet(Ipnet_netif *netif,
                            IP_CONST struct Ip_in_addr *pfrom,
                            IP_CONST struct Ip_in_addr *pto)
{
    Ipnet_ip4_addr_entry *addr;
    Ip_u32                from_n;
    Ip_u32                to_n;

    from_n = IPNET_IP4_GET_IPADDR(pfrom);
    to_n = IPNET_IP4_GET_IPADDR(pto);

    for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
    {
        Ip_u32 net_n = addr->ipaddr_n & addr->netmask_n;

        if (net_n == (from_n & addr->netmask_n))
            return net_n == (to_n & addr->netmask_n);
    }

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_ip4_output
 *===========================================================================
 * Description: Creates a IPv4 header and dispatch the packet to the
 *              link layer.
 * Parameters:  param - Parameters for this packet.
 *              pkt - The packet with IPv4 header (IPCOM_PKT_FLAG_HAS_IP_HDR set)
 *              or without IPv4 header (IPCOM_PKT_FLAG_HAS_IP_HDR not set).
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_FASTTEXT IP_GLOBAL int
ipnet_ip4_output(Ipnet_ip4_output_param *param, Ipcom_pkt *pkt)
{
    Ipcom_pkt         *pkt_next_fragment;
    Ip_bool            is_first_frag;
    int                ipv4_opt_len;
    Ip_u8              ipv4_opt[40];
    Ipnet_netif       *netif = IP_NULL;
    Ipnet_route_entry *rt_next_hop;
    Ipnet_route_entry *rt;
    Ipcom_pkt         *next_binding_pkt = IP_NULL;
    Ipnet_icmp_param   icmp_param;
    int                ret = 0;
    int                path_mtu;
    Ip_u16             ipv4_id = (Ip_u16) ipcom_atomic_get(&ipnet->ip4.id);
#ifdef IPIPSEC2
    Ipipsec_param      ipsec_param;
#endif
#ifdef IPNET_USE_NAT
    Ip_s32 natret = IPNET_NAT_NOMATCH;
    Ip_u32 natold = 0, natnew = 0;
#endif
    Ip_u32             input_if_index = pkt->ifindex;
    Ipnet_route_policy_selector rps;
    Ip_bool            dont_cache = IP_FALSE;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 8, IPCOM_WV_NETDEVENT_START,
                       ipnet_ip4_output, IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
    IPNET_STATS(ip4_output++);
    ip_assert(pkt != IP_NULL);
    ip_assert(param != IP_NULL);
    ip_assert(param->to != IP_NULL);
    ip_assert(param->to->s_addr != IP_INADDR_ANY);
    IPNET_DEBUG_LINE(ip_assert(IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_FREEQ));
    IPNET_DEBUG_LINE(ip_assert(IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_INQ));
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IP4_OUTPUT);
    IPNET_DEBUG_LINE(ipnet_pkt_check(pkt));


    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED)
        && ipnet_ip4_get_addr_type(param->from->s_addr, pkt->vr_index, IP_NULL) == IPNET_ADDR_TYPE_UNICAST)
        /* Packet beeing forward with one of the local addresses, drop it */
        goto cleanup;

    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_OUTPUT);

    if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_DONTROUTE)))
    {
        /* Cannot use the cache when don't route is used */
        dont_cache = IP_TRUE;
        ret = ipnet_route_lookup_ip4_dont_route(IP_FALSE,
                                                pkt->vr_index,
                                                param->to,
                                                param->netif ? param->netif->ipcom.ifindex : 0,
                                                &rt);
        if (ret < 0)
            goto cleanup;
    }
    else if (IP_UNLIKELY(param->nexthop != IP_NULL))
    {
        rt = param->nexthop;
        rt_next_hop = param->nexthop;
    }
    else
    {
        /* Find the route to the final destination */
        Ipnet_netif *forced_netif;

restart_lookup:
        rps.fields      = IPNET_PR_RULE_DS | IPNET_PR_RULE_PROTO | IPNET_PR_RULE_SADDR |
            IPNET_PR_RULE_PKTFLAGS | IPNET_PR_RULE_DADDR | IPNET_PR_RULE_PKTPTR;
        rps.ds          = param->tos;
        rps.proto       = param->proto;
        rps.saddr       = param->from != IP_NULL ? param->from : &ip_inaddr_any;
        rps.daddr       = param->to;
        rps.pkt_flags   = pkt->flags;
        rps.pkt         = pkt;

        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
            forced_netif = param->netif;
        else
            forced_netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);

        if (forced_netif != IP_NULL)
        {
            IP_BIT_SET(rps.fields, IPNET_PR_RULE_IFINDEX);
            rps.ifindex = forced_netif->ipcom.ifindex;
        }

        ret = ipnet_route_lookup_ecmp(IP_AF_INET,
                                      pkt->vr_index,
                                      0,
                                      param->to,
                                      0,
                                      param->netif == IP_NULL ? 0 : param->netif->ipcom.ifindex,
                                      &rps,
                                      &rt,
                                      &dont_cache);
        if (ret < 0)
        {
            if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
            {
                /* This packet is beeing routed, tell the source that the packet cannot be delivered */
                ipnet_icmp4_param_init(&icmp_param, pkt, param);
                icmp_param.type = IPNET_ICMP4_TYPE_DST_UNREACHABLE;

                switch (-ret)
                {
                case IP_ERRNO_EHOSTUNREACH:
                    icmp_param.code = IPNET_ICMP4_CODE_DST_UNREACH_HOST;
                    break;
                case IP_ERRNO_ENETUNREACH:
                    icmp_param.code = IPNET_ICMP4_CODE_DST_UNREACH_NET;
                    break;
                case IP_ERRNO_EACCES:
                    if (rt == IP_NULL || IP_BIT_ISSET(rt->hdr.flags,IPNET_RTF_HOST))
                        icmp_param.code = IPNET_ICMP4_CODE_DST_PROHIBITED_HOST;
                    else
                        icmp_param.code = IPNET_ICMP4_CODE_DST_PROHIBITED_NET;
                    break;
                default:
                    icmp_param.code = IPNET_ICMP4_CODE_DST_UNREACH_NET;
                    break;
                }

                (void) ipnet_icmp4_send(&icmp_param, IP_FALSE);
            }
            else
            {
                if (IP_UNLIKELY(ipnet->ip4.globals != IP_NULL) && param->from == IP_NULL)
                {
                    param->from = ipnet_ip4_get_global_src_addr(pkt->vr_index,
                                                                param->to);
                    if (param->from != IP_NULL)
                        goto restart_lookup;
                }

                if (ret == -IP_ERRNO_EHOSTUNREACH || ret == -IP_ERRNO_ENETUNREACH)
                {
                    IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsOutNoRoutes, 1);
                }
            }

            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 9, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                              ipnet_ip4_output, IPCOM_WV_NETD_NOROUTE,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_output_noroute++);
            IPCOM_MIB2(ipOutNoRoutes++);
            goto cleanup;
        }
        else
        {
            /* Have to resolve from if we've got policy routes available */
            if (param->from == IP_NULL && IP_UNLIKELY(ipnet->policy_routing_rules != IP_NULL))
            {
                param->from = ipnet_ip4_get_src_addr(pkt->vr_index,
                                                     param->to,
                                                     rt,
                                                     param->netif);

                /* No address found, use 0 address. */
                if (param->from == IP_NULL)
                    param->from = &ip_inaddr_any;
                else
                    goto restart_lookup;
            }
        }
    }

    if (IP_UNLIKELY(rt->rt_func_info != IP_NULL))
    {
        dont_cache = IP_TRUE;

        ret = (*rt->rt_func_info->rt_info) (&rt, pkt, param, &dont_cache);
        if (IP_UNLIKELY(ret != 0))
        {
            if (ret < 0)
                goto cleanup;
            goto restart_lookup;
        }
    }

mbinding:

    if (IP_UNLIKELY(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_BLACKHOLE)))
    {
        /* Silently drop all packets to this route */
        ret = 0;
        ++rt->use;
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 10, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                          ipnet_ip4_output, IPCOM_WV_NETD_INFO_DROPPED,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_output_blackhole++);
        goto cleanup;
    }

    /* Set the network interface the packet will be sent on */
    if (IP_LIKELY(param->netif == IP_NULL))
        netif = rt->netif;
    else
        netif = param->netif;

    if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
    {
        IPCOM_MIB2(ipOutRequests++);
        IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutRequests, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutRequests, 1, netif);
    }
    else
    {
        IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutForwDatagrams, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutForwDatagrams, 1, netif);
    }
    IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutTransmits, 1);
    IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutTransmits, 1, netif);
    IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
    IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutOctets, pkt->end - pkt->start, netif);

    /* Lock interface. */
    IPNET_IF_LOCK(netif);

    if (IP_UNLIKELY(netif->ipcom.ifindex == pkt->ifindex)
#ifdef IPMPLS
        && netif->ipcom.type != IP_IFT_MPLS_FIBIF
#endif
        )
    {
        Ip_bool forward_pkt;
        Ip_bool send_redirect;

        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
        {
            IPCOM_LOG4(WARNING, "IPv4: Route loop detected to %s on interface %s when using route %s on VR %d",
                       ipcom_inet_ntop(IP_AF_INET, param->to,
                                       ipnet->log_buf, sizeof(ipnet->log_buf)),
                       netif->ipcom.name,
                       ipcom_inet_ntop(IP_AF_INET,
                                       rt->hdr.key,
                                       ipnet->log_buf + IP_INET_ADDRSTRLEN,
                                       sizeof(ipnet->log_buf) - IP_INET_ADDRSTRLEN),
                       pkt->vr_index);
            ret = -IP_ERRNO_EHOSTUNREACH;
            goto cleanup;
        }

        if (IP_FALSE == ipnet_ip4_is_on_same_subnet(netif, param->from, param->to))
        {
            /* Forward this packet even though the ingress and egress
               interface is the same one. The "from" or "to" address
               is on different subnetworks on the same link. */
            forward_pkt = IP_TRUE;
            send_redirect = IP_FALSE;
        }
        else
        {
            /* The final destination was on the same (sub)network as the sender of the packet,
               check which action to perform. */
            switch (ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "IcmpRedirectSend", 2))
            {
            case -1:
                forward_pkt = IP_FALSE;
                send_redirect = IP_FALSE;
                break;
            case 0:
                forward_pkt = IP_TRUE;
                send_redirect = IP_FALSE;
                break;
            case 1:
                forward_pkt = IP_FALSE;
                send_redirect = IP_TRUE;
                break;
            default:
                forward_pkt = IP_TRUE;
                send_redirect = IP_TRUE;
                break;
            }
        }

        if (send_redirect
            && ipnet_ip4_get_addr_type2(param->from->s_addr, rt->netif) != IPNET_ADDR_TYPE_UNICAST)
        {
            /* Must restore the TTL of the header to the value it had
               when the IP datagram was received */
            ++((Ipnet_pkt_ip *)&pkt->data[pkt->ipstart])->ttl;
            ipnet_icmp4_param_init(&icmp_param, pkt, param);
            icmp_param.type = IPNET_ICMP4_TYPE_REDIRECT;
            icmp_param.code = IPNET_ICMP4_CODE_RED_HOST;
            if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
                /* Use another gateway */
                icmp_param.data.gateway_addr = &((struct Ip_sockaddr_in *) rt->gateway)->sin_addr;
            else
                /* Target is on link */
                icmp_param.data.gateway_addr = param->to;
            (void) ipnet_icmp4_send(&icmp_param, IP_FALSE);
        }

        if (!forward_pkt)
        {
            ++rt->use;
            goto cleanup;
        }
    }
    pkt->ifindex = netif->ipcom.ifindex;

    if (param->from == IP_NULL)
    {
        param->from = ipnet_ip4_get_src_addr(pkt->vr_index,
                                             param->to,
                                             rt,
                                             param->netif);

        /* No address found, use 0 address. */
        if (param->from == IP_NULL)
            param->from = &ip_inaddr_any;
    }

    if (param->chksum_ptr)
#ifdef IPCOM_ZEROCOPY
        if (((param->proto == IP_IPPROTO_UDP) && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ZBUF)))
        {
            *param->chksum_ptr = 0;
        }
        else
#endif
    {
        /* Should never need to calculate transport header checksum on
           forwarded packets */
        ip_assert(IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR));

#ifdef IPCOM_USE_HW_CHECKSUM_TX
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
        {
            /* This is the slow path, the packet can get fragmented
               so using HW checksum might fail. Revert back to software */
            IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
            pkt->chk = ipcom_in_checksum_update(&pkt->data[pkt->start],
                                                pkt->end - pkt->start);
        }
#endif /* IPCOM_USE_HW_CHECKSUM_TX */

        pkt->chk += ipnet_ip4_pseudo_header_checksum_update(param->from,
                                                            param->to,
                                                            param->proto,
                                                            (Ip_u16) (ipcom_pkt_get_length(pkt)));
        *param->chksum_ptr = ipcom_in_checksum_finish(pkt->chk);
        /* Fix-up the UDP checksum if it was calculated to 0 */
        if (param->proto == IP_IPPROTO_UDP && *param->chksum_ptr == 0)
            *param->chksum_ptr = 0xFFFF;
    }

 ip_header_check:
    pkt->ipstart = pkt->start;
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
    {
        Ipnet_pkt_ip *ip4;

        pkt->ipstart = pkt->start;
        ip4 = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

        ret = IPNET_IP4_GET_HDR_OCTET_LEN(ip4);
        ipv4_opt_len = ret - IPNET_IP_HDR_SIZE;

        if (ip4->src[0] == 0 && ip4->src[1] == 0)
            IPNET_IP4_SET_IPADDR(ip4->src, param->from->s_addr);
        ip4->sum = 0;
        ip4->sum = ipcom_in_checksum(ip4, ret);
    }
    else
    {
        /* Add IPv4 header and options. */
        ipv4_opt_len = ipnet_ip4_add_options(param, pkt);
        if (IP_LIKELY(ipv4_opt_len >= 0))
            ret = ipnet_ip4_add_header(param, pkt, ipv4_opt_len, ipv4_id);
        else
            ret = ipv4_opt_len;
    }

    if (IP_UNLIKELY(ret <= 0) || IP_UNLIKELY(pkt->start < netif->ipcom.link_hdr_size))
    {
        pkt->start = pkt->ipstart;
        ret = ipnet_increase_hdr_space(&pkt, IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR));
        if (ret < 0)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 11, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                              ipnet_ip4_output, IPCOM_WV_NETD_INFO_DROPPED,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_output_hdr_space++);
            goto cleanup;
        }
        goto ip_header_check;
    }

    ip_assert(pkt->start == pkt->ipstart);
    ip_assert((pkt->data[pkt->ipstart] & 0xf0) == 0x40);  /* Version 4. */

    if (IP_UNLIKELY(ipv4_opt_len > 0))
        /* The packet has options */
        ipcom_memcpy(ipv4_opt, &pkt->data[pkt->ipstart + IPNET_IP_HDR_SIZE], ipv4_opt_len);

#ifdef IPFIREWALL_USE_IP_FILTER
    if (ipfirewall != IP_NULL
        && ipfirewall_output_hook(&netif->ipcom, pkt, IP_NULL))
    {
        /* Packet blocked. */
        ret = -IP_ERRNO_EFIREWALL;
        goto cleanup;
    }
#endif /* IPFIREWALL_USE_IP_FILTER */

#ifdef IPNET_USE_NAT
    if (IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED)
        && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_NAT)
        && netif->ipcom.ifindex != input_if_index)
    {
        natold = IP_GET_NTOHL(&pkt->data[pkt->ipstart+12]);
        ipnet->nat.origfrom = (struct Ip_in_addr *)param->from;
        natret = ipnet_nat_output_hook(&pkt, IP_FALSE);
        switch (natret)
        {
            case IPNET_NAT_TRANSLATED:
                natnew = IP_GET_NTOHL(&pkt->data[pkt->ipstart+12]);
                ipnet->nat.newsource.s_addr = ip_htonl(natnew);
                param->from = &ipnet->nat.newsource;
                IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NAT);
                break;
            case IPNET_NAT_NOMATCH:
            case IPNET_NAT_INVALID:
                break;
            case IPNET_NAT_DROP:
                ret = -IP_ERRNO_ENAT;
                goto cleanup;
            default:
                break;
        }
    }
#endif /* IPNET_USE_NAT */

#ifdef IPIPSEC2
    if (pkt->ipsec_skip == 0)
    {
        ipsec_param.key.src.in.s_addr = IP_GET_32ON16(&param->from->s_addr);
        ipsec_param.key.dst.in.s_addr = IP_GET_32ON16(&param->to->s_addr);
        ipsec_param.key.proto  = param->proto;
        ipsec_param.key.domain = IP_AF_INET;
        ipsec_param.ttl        = param->ttl;
#ifdef IPIPSEC_USE_DAEMONS
        ret = ipipsec_send_daemon_pipe(pkt, &ipsec_param, 0);
#else
        ret = ipipsec_output(&pkt, &ipsec_param);
#endif
        if (ret == IPIPSEC_KEPT)
        {
            /* IPSEC kept the packet for later transmission */
            goto async;
        }
        else if (ret < IPIPSEC_OK)
        {
            switch (ret)
            {
            case -IP_ERRNO_EINVAL:
                /* EINVAL means silent drop (require flow packet drops). */
                ret = 0;
                break;
            case -IP_ERRNO_EPERM:
                if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
                {
                    /* 5.1.1 RFC4301 s5.1.1 send ICMP DST UNREACH, admin prohib */
                    ipnet_icmp4_param_init(&icmp_param, pkt, param);
                    icmp_param.type = IPNET_ICMP4_TYPE_DST_UNREACHABLE;
                    icmp_param.code = IPNET_ICMP4_CODE_DST_PROHIBITED_ADM;
                    (void)ipnet_icmp4_send(&icmp_param, IP_FALSE);
                }
                break;
            default:
                break;
            }
            goto cleanup;
        }

        /* IPv4 packet tunneled in an IPv6 packet. */
        if (ipsec_param.key.domain == IP_AF_INET6)
        {
#ifdef IPCOM_USE_INET6
            Ipnet_ip6_output_param  ip6_param;

            ipcom_memset(&ip6_param, 0, sizeof(Ipnet_ip6_output_param));
            ip6_param.from     = (IP_CONST struct Ip_in6_addr *)&ipsec_param.key.src.in6;
            ip6_param.to       = (IP_CONST struct Ip_in6_addr *)&ipsec_param.key.dst.in6;
            ip6_param.final_to = ip6_param.to;
            ip6_param.nxt_hdr  = ipsec_param.key.proto;
            ip6_param.hlim     = param->ttl;
            pkt->ifindex       = 0; /* must clear to avoid loop-detection check */

            ret = ipnet_ip6_output(&ip6_param, pkt);
            pkt = IP_NULL;
#endif /* IPCOM_USE_INET6 */
            goto cleanup;
        }

        /* Reroute datagram if packet was tunneled. */
        ip_assert(ipsec_param.key.domain == IP_AF_INET);
        if (ret == IPIPSEC_TUNNELED || param->proto == IP_IPPROTO_ESP || param->proto == IP_IPPROTO_AH)
            dont_cache = IP_TRUE;

        if (param->nexthop == IP_NULL && ret == IPIPSEC_TUNNELED)
        {
            if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_DONTROUTE))
                ret = ipnet_route_lookup_ip4_dont_route(IP_FALSE,
                                                        pkt->vr_index,
                                                        &ipsec_param.key.dst.in,
                                                        0,
                                                        &rt);
            else
            {
                rps.fields =    IPNET_PR_RULE_DS | IPNET_PR_RULE_PROTO | IPNET_PR_RULE_SADDR |
                    IPNET_PR_RULE_DADDR;
                rps.ds     = param->tos;
                rps.proto  = ipsec_param.key.proto;
                rps.saddr  = &ipsec_param.key.src.in.s_addr;
                rps.daddr  = &ipsec_param.key.dst.in.s_addr;

                ret = ipnet_route_lookup_ecmp(IP_AF_INET,
                                              pkt->vr_index,
                                              0,
                                              &ipsec_param.key.dst.in.s_addr,
                                              0,
                                              0,
                                              &rps,
                                              &rt,
                                              &dont_cache);
            }
            if (ret < 0)
            {
                IPCOM_LOG1(DEBUG, "IPSEC: no route for tunneled IPv4 packet '%s' ",
                           ipcom_inet_ntop(IP_AF_INET, &ipsec_param.key.dst.in.s_addr,
                                           ipnet->log_buf, sizeof(ipnet->log_buf)));
                goto cleanup;
            }

            if (netif != rt->netif)
            {
                /* IPSec tunneling has changed the outgoinging interface */
                IPNET_IF_UNLOCK(netif);
                netif = rt->netif;
                IPNET_IF_LOCK(netif);
            }

            /* Update IPv4 source and destination and discard header options. */
            ipv4_id = (Ip_u16) ipcom_atomic_get(&ipnet->ip4.id);
            param->to    = (IP_CONST struct Ip_in_addr *)&ipsec_param.key.dst.in.s_addr;
            param->from  = (IP_CONST struct Ip_in_addr *)&ipsec_param.key.src.in.s_addr;
            param->cache = IP_NULL;
            IP_BIT_CLR(param->flags, IPNET_IP4_OPF_ROUTER_ALERT);
            ipv4_opt_len = 0;
        }

        /* Update IPv4 protocol (prevent caching to avoid bad param contents) */
        if (param->proto != ipsec_param.key.proto)
            param->cache = IP_NULL;
        param->proto = ipsec_param.key.proto;
    }
#endif /* IPIPSEC2 */

    if (IP_UNLIKELY(IP_IN_CLASSD(IP_GET_32ON16(param->to))))
    {
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST);
        netif->omcasts++;
    }
    else if (IP_UNLIKELY(ipnet_ip4_get_addr_type2(param->to->s_addr,
                                                  netif) == IPNET_ADDR_TYPE_NETBROADCAST))
    {

        if (IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED)
            && (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                                 netif,
                                                 "DontForwardBroadcast",
                                                 IP_TRUE,
                                                 ipnet_bool_map) != 0)
            )
        {

            IPCOM_LOG1(DEBUG, "Drop broadcast forwarding to destination '%s' ",
                       ipcom_inet_ntop(IP_AF_INET, &param->to,
                                       ipnet->log_buf, sizeof(ipnet->log_buf)));
            goto cleanup;

        }


        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST);
    }

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_GATEWAY)
        || IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST)
        || IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST))
    {
        /* Destination is on link */
        rt_next_hop = rt;
    }
    else
    {
        /* Destination is off link */
        struct Ip_sockaddr_in *gw4 = (struct Ip_sockaddr_in *) rt->gateway;

        ip_assert(gw4->sin_family == IP_AF_INET);
        ret = ipnet_route_lookup_ecmp(IP_AF_INET,
                                      IPNET_ROUTE_GET_VR(rt->head),
                                      IPNET_RTL_FLAG_LINK_LOCAL,
                                      &gw4->sin_addr,
                                      0,
                                      0,
                                      &rps,
                                      &rt_next_hop,
                                      &dont_cache);
        if (ret < 0)
        {
            /* The gateway was link local to this node */
            if (ipnet_ip4_get_addr_type2(gw4->sin_addr.s_addr,
                                         rt->netif) != IPNET_ADDR_TYPE_UNICAST)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 12, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                  ipnet_ip4_output, IPCOM_WV_NETD_INFO_DROPPED,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(ip4_output_noroute++);
                goto cleanup;
            }
            /* Destination is on link, but with the source address specified by the route table */
            rt_next_hop = rt;
        }
        else
        {
            if (IP_UNLIKELY(IP_BIT_ISSET(rt_next_hop->hdr.flags, IPNET_RTF_BLACKHOLE)))
            {
                /* Silently drop all packets to this route */
                ret = 0;
                ++rt_next_hop->use;
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 13, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                  ipnet_ip4_output, IPCOM_WV_NETD_INFO_DROPPED,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(ip4_output_blackhole++);
                goto cleanup;
            }
        }
    }

    ip_assert(rt_next_hop != IP_NULL);

    if (param->proto == IP_IPPROTO_ICMP
        && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR)
        && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
    {
        /* Don't create a fast path for locally generated ICMP message
           since they are not performance critical and the MIB
           statistics will not be correct if ICMP messages is not sent
           trough the normal path */
        dont_cache = IP_TRUE;
        if(pkt->data[pkt->start+IPNET_IP_HDR_SIZE+ipv4_opt_len] == IPNET_ICMP4_TYPE_ECHO_REQUEST)
        {
            IPCOM_MIB2(icmpOutMsgs++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsOutMsgs, 1);
            IPCOM_MIB2(icmpOutEchos++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpMsgStatsOutPkts[IPNET_ICMP4_TYPE_ECHO_REQUEST], 1);
        }
    }

    if (IP_UNLIKELY(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_MBINDING))
        && ipnet_ip4_get_next_binding_rt(rt) != IP_NULL)
        /* Using a multiple binding route and the current one is not the last mbind route,
           clone the packet before fragmentation */
        next_binding_pkt = ipnet_pkt_clone(pkt, IP_TRUE);

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_MBINDING)
        || IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST | IPCOM_PKT_FLAG_BROADCAST))
        dont_cache = IP_TRUE;

#ifdef IPNET_FASTPATH
    if (dont_cache == IP_FALSE
        && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED)
        && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
    {
        /* Forwarded packet, lets create a fast path for it */
        Ipnet_ip4_rt_cache *cache;
        Ipnet_pkt_ip       *ip_hdr = (Ipnet_pkt_ip *)&pkt->data[pkt->start];

#ifdef IPIPSEC2
        ipsec_param.key.src.in.s_addr = IP_GET_32ON16(ip_hdr->src);
        ipsec_param.key.dst.in.s_addr = IP_GET_32ON16(ip_hdr->dst);
        ipsec_param.key.proto  = ip_hdr->p;
        ipsec_param.key.domain = IP_AF_INET;
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE)
            && ipipsec_3tuple_match(pkt, &ipsec_param) == IP_FALSE)
#endif
        {
#ifdef IPNET_USE_NAT
            if (natret == IPNET_NAT_TRANSLATED)
                IP_SET_HTONL(ip_hdr->src, natold);
#endif
            IPNET_IP4_GET_FORWARD_CACHE_ENTRY(cache, ip_hdr);
            cache->ifindex     = input_if_index;
            cache->rt_next_hop = rt_next_hop;
            ipnet_route_cache_set_valid(&cache->rt_id);
            cache->tos         = ip_hdr->tos;
            cache->p           = ip_hdr->p;
            cache->src[0]      = ip_hdr->src[0];
            cache->src[1]      = ip_hdr->src[1];
            cache->dst[0]      = ip_hdr->dst[0];
            cache->dst[1]      = ip_hdr->dst[1];

#ifdef IPNET_USE_NAT
            if (natret == IPNET_NAT_TRANSLATED)
                IP_SET_HTONL(ip_hdr->src, natnew);
#endif

        }
    }

#endif /* IPNET_FASTPATH */

    /* Check if the packet must be fragmented */
    path_mtu = (rt->metrics.rmx_mtu == 0) ? netif->ipcom.mtu : IP_MIN(netif->ipcom.mtu, rt->metrics.rmx_mtu);
    if (IP_LIKELY(ipcom_pkt_get_length(pkt) <= path_mtu))
    {
        if (dont_cache || pkt->data[pkt->start] != 0x45)
            param->cache = IP_NULL;
        if (param->cache != IP_NULL)
        {
            /* No ip options, lets cache this header */
            param->cache->rt_dst      = rt;
            param->cache->rt_next_hop = rt_next_hop;
            ipcom_memcpy(&param->cache->ip_hdr, &pkt->data[pkt->start], IPNET_IP_HDR_SIZE);
            ipnet_route_cache_set_valid(&param->cache->rt_id);
        }
    }
    else
    {
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsOutFragReqds, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsOutFragReqds, 1, netif);

        if (IP_BIT_ISSET(param->flags, IPNET_IP4_OPF_DONT_FRAG)
            && (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED) || pkt->ipsec_hlen != 0)
            && rt->rt_func_info == IP_NULL)
        {
            /* This packet is not allowed to be fragmented */
            if (pkt->ipsec_hlen != 0)
            {
                Ipcom_pkt    *pkt_orig;
                Ipnet_pkt_ip *ip4;

                /* Reduce MTU for IPSEC */
                path_mtu -= (int)pkt->ipsec_hlen;

                /* Recycle the original packet */
                pkt_orig = pkt->next_original;
                ip_assert(pkt_orig != IP_NULL);
                pkt->next_original = IP_NULL;
                ipcom_pkt_free(pkt);
                pkt = pkt_orig;
                pkt->start = pkt->ipstart;

                /* Reset output parameters */
                ip4 = (Ipnet_pkt_ip *)&pkt->data[pkt->start];
                param->from  = (struct Ip_in_addr *)&ip4->src;
                param->to    = (struct Ip_in_addr *)&ip4->dst;
                param->proto = ip4->p;
            }

#ifdef IPNET_USE_NAT
            /* Restore original source address */
            if (natret == IPNET_NAT_TRANSLATED)
                param->from = ipnet->nat.origfrom;
#endif
            ipnet_icmp4_param_init(&icmp_param, pkt, param);
            icmp_param.type = IPNET_ICMP4_TYPE_DST_UNREACHABLE;
            icmp_param.code = IPNET_ICMP4_CODE_DST_NEEDFRAG;
            icmp_param.data.max_path_mtu = path_mtu;
            (void) ipnet_icmp4_send(&icmp_param, IP_FALSE);
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 14, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                              ipnet_ip4_output, IPCOM_WV_NETD_INFO_DROPPED,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_input_need_frag++);
            IPCOM_MIB2(ipFragFails++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsOutFragFails, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsOutFragFails, 1, netif);
            ret = -IP_ERRNO_EMSGSIZE;
            goto cleanup;
        }

        if (IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
            ipv4_opt_len = ipnet_ip4_remove_header(pkt, &ipv4_id);

#ifdef IPCOM_ZEROCOPY
        pkt = ipcom_pkt_make_linear(pkt);
#endif
        pkt = ipnet_fragment_packet(pkt,
                                    IPNET_IP_HDR_SIZE + ipv4_opt_len,
                                    0,
                                    path_mtu,
                                    ipnet_ip4_more_fragments);
        if (pkt == IP_NULL)
        {
            /* The original packet is free'd by ipnet_fragment_packet() */
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                              1, 15, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                              ipnet_ip4_output, IPCOM_WV_NETD_NOBUFS,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_output_enobufs++);
            IPCOM_MIB2(ipFragFails++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsOutFragFails, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsOutFragFails, 1, netif);
            IPCOM_MIB2(ipOutDiscards++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsOutDiscards, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsOutDiscards, 1, netif);
            ret = -IP_ERRNO_ENOBUFS;
            goto cleanup;
        }
        IPCOM_MIB2(ipFragOKs++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsOutFragOKs, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsOutFragOKs, 1, netif);

#ifdef IP_PORT_INTEGRITY
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING))
        {
            Ipnet_pkt_queue *snd_queue = netif->snd_queue;

            /*
             * Special treatment for fragmented datagrams only if a fifo
             * queue is installed. If no fifo queue, an immediate error is
             * issued to prevent hard-to-diagnose out-of-buffer conditions.
             */
            if (snd_queue == IP_NULL || ipcom_strcmp(snd_queue->type, "fifo") != 0)
            {
                IPCOM_LOG0(WARNING, "Sending large datagrams requires a fifo interface queue,");
                IPCOM_LOG0(WARNING, "please refer to ipnet_conf_boot_cmd[] in ipnet_config.c,");
                IPCOM_LOG0(WARNING, "e.g.: \"qc queue add dev eth0 root handle 1 fifo limit 128\"");
                ret = -IP_ERRNO_ENOBUFS;
                goto cleanup;
            }
            else
            {
                union Ipnet_ifqueue_data q;
                Ipcom_pkt *p;
                int pnum = 0;

                /* Count number of fragments */
                for (p = pkt; p != IP_NULL; p = p->next_fragment)
                {
                    pnum++;
                }

                /*
                 * Check that all packets will fit in the send queue. Otherwise,
                 * issue EWOULDBLOCK before any transmissions are made. This will
                 * ensure that all fragments eventually can be buffered.
                 */
                snd_queue->dump(snd_queue, &q);
                if (pnum > (q.fifo.fifo_limit - snd_queue->count(snd_queue)))
                {
                    if (pnum > q.fifo.fifo_limit)
                    {
                        IPCOM_LOG1(WARNING, "Interface queue too small for datagram, required size is %d,", pnum);
                        IPCOM_LOG0(WARNING, "please refer to ipnet_conf_boot_cmd[] in ipnet_config.c");
                        ret = -IP_ERRNO_ENOBUFS;
                    }
                    else
                    {
                        ret = -IP_ERRNO_EWOULDBLOCK;
                    }
                    goto cleanup;
                }
            }
        }
#endif /* IP_PORT_INTEGRITY */
    }

    /* Loop and output each packet to lower layers. */
    is_first_frag = IP_TRUE;
    while (pkt != IP_NULL)
    {
        /* Add IPv4 header to all IP packets, including fragments */
        pkt_next_fragment  = pkt->next_fragment;
        pkt->next_fragment = IP_NULL;

        if (IP_UNLIKELY(IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR)))
        {
            Ip_u16 copied_opt_len;
            int    start = pkt->start;

            for (;;)
            {
                copied_opt_len = ipnet_ip4_restore_options(pkt,
                                                           ipv4_opt,
                                                           ipv4_opt_len,
                                                           is_first_frag);
                ret = ipnet_ip4_add_header(param, pkt, copied_opt_len, ipv4_id);
                if (ret > 0 && pkt->start >= netif->ipcom.link_hdr_size)
                    break;
                /* else: Could not fit all headers */
                pkt->start = start;

                ret = ipnet_increase_hdr_space(&pkt, IP_FALSE);
                if (ret < 0)
                {
                    pkt->next_fragment = pkt_next_fragment;
                    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                      1, 16, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                      ipnet_ip4_output, IPCOM_WV_NETD_INFO_DROPPED,
                                      IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                    IPNET_STATS(ip4_output_hdr_space++);
                    goto cleanup;
                }
            }
        }
        is_first_frag = IP_FALSE;

        /* Set the if index for all packets */
        pkt->ifindex = netif->ipcom.ifindex;

        /* Clear the IP header flag to avoid problems for loopback, retransmits etc. */
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);

        ip_assert2((pkt->data[pkt->start] & 0xf0) == 0x40);

        if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST | IPCOM_PKT_FLAG_MULTICAST))
            && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOP_MCAST)
            && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_SIMPLEX))
        {
            /* Send the packet to local host via loopback. */
            Ipcom_pkt   *clone;
            Ipnet_netif *loopback;

            loopback = ipnet_loopback_get_netif(pkt->vr_index);
            ip_assert(loopback != IP_NULL);
            clone = ipnet_pkt_clone(pkt, IP_TRUE);
            if (clone != IP_NULL)
            {
                clone->ifindex = netif->ipcom.ifindex;
                ++rt_next_hop->use;
                (void) loopback->link_ip4_output(loopback, rt_next_hop, clone);
            }
        }

        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST))
        {
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutBcastPkts, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutBcastPkts, 1, netif);
        }
        else if(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST))
        {
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutMcastPkts, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutMcastPkts, 1, netif);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutMcastOctets, pkt->end - pkt->start);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutMcastOctets, pkt->end - pkt->start, netif);
        }

        ++rt->use;
        if (rt != rt_next_hop)
            ++rt_next_hop->use;

        /* Now finally output the IPv4 packet to the lower layers (Ethernet, PPP etc.) */
#ifdef IPCOM_USE_HW_CHECKSUM_TX
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM)
            && IP_BIT_ISFALSE(netif->ipcom.drv_capabilities, IPCOM_IF_DRV_CAP_IP4_TRANSPORT_CSUM_TX))
            ipnet_finish_transport_checksum(pkt);
#endif
        ipnet_route_lock(rt_next_hop);

        if (pkt->recurs_level > IPCOM_PKT_MAX_RECURSE_LEVEL)
        {
            IPCOM_LOG1(WARNING, "Discarding IPv4 datagram on %s, too deep recurs level.", netif->ipcom.name);
            ipcom_pkt_free(pkt);
            ret = -IP_ERRNO_EHOSTUNREACH;
        }
        else
        {
            ++pkt->recurs_level;
#ifdef IPROHC
            if (iprohc.opened
                && (netif->ipcom.type != IP_IFT_MPLS_TUNNEL)
                && (netif->ipcom.type != IP_IFT_TUNNEL)
                && (pkt->ipstart == pkt->start))
            {
                /*
                 * Do ROHC on a not (MPLS and TUNNEL) interface and
                 * packet has not been processed by MPLS.
                 */
                ret = iprohc_output_hook(&netif->ipcom, pkt);
                if (ret != IPCOM_SUCCESS)
                {
                    /* ROHC failed */
                    IPCOM_LOG1(WARNING, "Discarding IPv4 datagram on %s, ROHC failed.", netif->ipcom.name);
                    ipcom_pkt_free(pkt);
                    pkt = pkt_next_fragment;
                    ret = -IP_ERRNO_EROHC;
                    goto cleanup;
                }
            }
#endif /* IPROHC */

            ret = netif->link_ip4_output(netif, rt_next_hop, pkt);
        }

        pkt = pkt_next_fragment;
        if (ipnet_route_unlock(rt_next_hop))
            /* The route has been deleted */
            goto cleanup;

        if (IP_UNLIKELY(ret < 0))
        {
            /* The packet was released by link_ip4_output() */
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 17, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                              ipnet_ip4_output, IPCOM_WV_NETD_NETDOWN,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_output_link_err++);
            goto cleanup;
        }
    }

#ifdef IPIPSEC2
async:
#endif

    IPNET_IF_UNLOCK(netif);
    /* Increment ID last because same IP ID for NAT, IPSec and cleartext must be used. */
    ipcom_atomic_inc(&ipnet->ip4.id);
    if (IP_UNLIKELY(next_binding_pkt != IP_NULL))
    {
        /* This packet should be sent to multiple destinatin even if it is unicast */
        netif = IP_NULL;
        pkt = next_binding_pkt;
        pkt->ifindex = 0;
        next_binding_pkt = IP_NULL;
        rt_next_hop = IP_NULL;
        param->cache      = IP_NULL;
        param->chksum_ptr = IP_NULL;
        rt = ipnet_ip4_get_next_binding_rt(rt);
        if (rt == IP_NULL)
            goto cleanup;

        goto mbinding;
    }

    return 0;

 cleanup:
    if (netif != IP_NULL)
        IPNET_IF_UNLOCK(netif);
    if (pkt != IP_NULL)
    {
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
        ipcom_pkt_free(pkt);
    }
    if (next_binding_pkt != IP_NULL)
        ipcom_pkt_free(next_binding_pkt);
    return ret;
}



/*
 *===========================================================================
 *                    ipnet_icmp4_dst_create_rt
 *===========================================================================
 * Description: Creates a host route the the specific destination.
 * Parameters:  netif - The interface that should be used when sending packets
 *              to the final destination.
 *              dst - The key for the route.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_PUBLIC Ipnet_route_entry *
ipnet_icmp4_dst_create_rt(Ipnet_netif *netif,
                          struct Ip_in_addr *dst,
                          struct Ip_in_addr *gw)
{
    struct Ipnet_route_add_param rt_param;
    struct Ipnet_rt_metrics      rt_metrics;
    struct Ip_sockaddr_in        sin_gw;
    Ipnet_route_entry           *rt;
    Ip_u16                       vr = netif->vr_index;
    int                          ret;

    if (gw != IP_NULL && ipcom_memcmp(gw, dst, sizeof(struct Ip_in_addr)) == 0)
        /* The router that sent the redirect says this host is link local to us,
           assume that the one configured this host knows what they are doing
           and keep sending the packets through that router */
        return IP_NULL;

    ret = ipnet_route_lookup(IP_AF_INET,
                             vr,
                             IPCOM_ROUTE_TABLE_DEFAULT,
                             IPNET_RTL_FLAG_DONTCLONE,
                             dst,
                             0,
                             0,
                             &rt);

    ipcom_memset(&sin_gw, 0, sizeof(sin_gw));
    if (gw != IP_NULL)
    {
        /* Use the supplied gateway */
        ipcom_memset(&sin_gw, 0, sizeof(sin_gw));
        sin_gw.sin_family = IP_AF_INET;
        IPCOM_SA_LEN_SET(&sin_gw, sizeof(sin_gw));
        ipcom_memcpy(&sin_gw.sin_addr, gw, sizeof(sin_gw.sin_addr));
    }
    else if (ret >= 0 && rt->gateway != IP_NULL)
        /* Copy the gateway from the current route */
        ipcom_memcpy(&sin_gw, rt->gateway, sizeof(sin_gw));

    if (ret == IPNET_ROUTE_PERFECT_MATCH)
    {
        /* Already exist */
        if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_DYNAMIC))
            /* Not added in response to a redirect, keep this entry */
            return rt;
        /* Remove the current entry */
        (void) ipnet_route_delete2(IP_AF_INET, vr, IPCOM_ROUTE_TABLE_DEFAULT,
                                   rt->hdr.key, rt->hdr.mask, rt->gateway,
                                   rt->netif->ipcom.ifindex, 0, 0, IP_FALSE);
    }

    ipcom_memset(&rt_metrics, 0, sizeof(rt_metrics));
    rt_metrics.rmx_expire = (Ip_u32) ipcom_sysvar_get_as_int0("ipnet.inet.DstCacheToLive", 300);

    ipcom_memset(&rt_param, 0, sizeof(rt_param));
    rt_param.domain  = IP_AF_INET;
    rt_param.vr      = vr;
    rt_param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
    rt_param.netif   = netif;
    rt_param.key     = dst;
    rt_param.flags   = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DYNAMIC | IPNET_RTF_DONE;
    rt_param.metrics = &rt_metrics;
    if (sin_gw.sin_family)
    {
        rt_param.gateway = (struct Ip_sockaddr *) &sin_gw;
        rt_param.flags |= IPNET_RTF_GATEWAY;
    }

    ret = ipnet_route_add(&rt_param);
    if (ret >= 0)
    {
        ret = ipnet_route_lookup(IP_AF_INET,
                                 vr,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_DONTCLONE,
                                 dst,
                                 0,
                                 netif->ipcom.ifindex,
                                 &rt);
        if (ret == IPNET_ROUTE_PERFECT_MATCH)
            return rt;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_icmp4_input
 *===========================================================================
 * Description: Processes incoming ICMPv4 packets.
 * Parameters:  netif - The interface the packet was received on.
 *              pkt - The received data, pkt->start is the offset to the ICMP
 *              header.
 *              ip4_param - Cached IP parameters for the packet.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_icmp4_input(Ipnet_netif *netif, Ipcom_pkt *pkt, Ipnet_ip4_output_param *ip4_param)
{
    Ipnet_icmp_param icmp_param;
    Ipnet_pkt_icmp  *icmp_hdr;
    int              icmp_len;
    int              ret;
    int              addr_type;
    Ip_u16           chksum;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 18, IPCOM_WV_NETDEVENT_START,
                       ipnet_icmp4_input, IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
    IPNET_STATS(icmp4_input++);
    IPCOM_MIB2(icmpInMsgs++);
    IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsInMsgs, 1);
    icmp_len = pkt->end - pkt->start;
    icmp_hdr = (Ipnet_pkt_icmp *) &pkt->data[pkt->start];

    /* check that the message is big enough to hold the icmp header. */
    if (icmp_len < IPNET_ICMP_HDR_SIZE)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 19, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_icmp4_input, IPCOM_WV_NETD_BADHLEN,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(icmp4_input_hdrsize++);
        IPCOM_MIB2(icmpInErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsInErrors, 1);
        goto cleanup;
    }

    /* ICMP checksum must be ok. */
#ifdef IPCOM_USE_HW_CHECKSUM_RX
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
        chksum = 0;
    else if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_TL_CHECKSUM))
        chksum = ipcom_in_checksum_finish(pkt->chk);
    else
#endif /* IPCOM_USE_HW_CHECKSUM_RX */
        chksum = ipcom_in_checksum(icmp_hdr, icmp_len);

    if (chksum != 0)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 20, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_icmp4_input, IPCOM_WV_NETD_BADSUM,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(icmp4_input_badchksum++);
        IP_PANIC2();
        IPCOM_MIB2(icmpInErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsInErrors, 1);
        goto cleanup;
    }

    IPCOM_MIB2(ipInDelivers++);

    if (ip4_param->proto == IP_IPPROTO_IGMP)
        return ipnet_igmp_input(netif, pkt, ip4_param);

    if (icmp_hdr->type == IPNET_ICMP4_TYPE_ECHO_REPLY)
    {
        IPCOM_MIB2(icmpInEchoReps++);
        return ipnet_ip4_deliver_to_raw_sock(pkt, ip4_param, IP_TRUE);
    }
    else if (icmp_hdr->type != IPNET_ICMP4_TYPE_ECHO_REQUEST)
        /* Give a copy of the packet to all raw sockets. */
        (void)ipnet_ip4_deliver_to_raw_sock(pkt, ip4_param, IP_FALSE);

    if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET, netif, "IcmpIgnoreBroadcast", 1, ipnet_bool_map) != 0)
    {
        addr_type = ipnet_ip4_get_addr_type2(ip4_param->to->s_addr, IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
                                             ? ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex):
                                             netif);

        if (addr_type == IPNET_ADDR_TYPE_NETBROADCAST
            || addr_type == IPNET_ADDR_TYPE_BROADCAST)
        {
            IPCOM_LOG2(DEBUG,"SMURF attack detected! ignoring icmp from %s on %s.. ",
                       ipcom_inet_ntop(IP_AF_INET, ip4_param->from, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       netif->ipcom.name);
            goto cleanup;

        }
    }


    ipnet_icmp4_param_init(&icmp_param, pkt, ip4_param);

    IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpMsgStatsInPkts[icmp_hdr->type], 1);

    switch (icmp_hdr->type)
    {
    case IPNET_ICMP4_TYPE_ECHO_REQUEST:
        if (ipcom_sysvar_get_as_int0("ipnet.inet.IcmpIgnoreEchoRequest", 0))
        {
            IPCOM_LOG2(DEBUG, "ignore echo request from %s on %s",
                       ipcom_inet_ntop(IP_AF_INET, ip4_param->from, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       netif->ipcom.name);
            goto cleanup;
        }
        /* Send a echo reply message */
        IPCOM_LOG3(DEBUG, "echo request from %s to %s on %s",
                   ipcom_inet_ntop(IP_AF_INET, ip4_param->from, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   ipcom_inet_ntop(IP_AF_INET, ip4_param->to,
                                   ipnet->log_buf + IP_INET_ADDRSTRLEN,
                                   sizeof(ipnet->log_buf)- IP_INET_ADDRSTRLEN),
                   netif->ipcom.name);
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 21, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_icmp4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(icmp4_input_echo_request++);
        IPCOM_MIB2(icmpInEchos++);
        icmp_param.type = IPNET_ICMP4_TYPE_ECHO_REPLY;
        icmp_param.code = 0;
        ret = ipnet_icmp4_send(&icmp_param, IP_FALSE);
        if (ret < 0 && icmp_param.recv_pkt != IP_NULL)
            /* ipnet_icmp4_send() did not free the orginal packet */
            goto cleanup;
        return ret;

    case IPNET_ICMP4_TYPE_DST_UNREACHABLE:
    case IPNET_ICMP4_TYPE_TIME_EXCEEDED:
        {
            Ipnet_socket *sock;
            Ipnet_pkt_ip *failing_ip_pkt;
            Ip_u32        fsrc_addr_n;
            Ip_u32        fdst_addr_n;
            Ip_u16        fsport;
            Ip_u16        fdport;
            Ip_u16        proto;
            void         *transport_hdr;

            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 22, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_icmp4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(icmp4_input_dst_unreach++);
            if(icmp_hdr->type == IPNET_ICMP4_TYPE_DST_UNREACHABLE)
                IPCOM_MIB2(icmpInDestUnreachs++);
            else if(icmp_hdr->type == IPNET_ICMP4_TYPE_TIME_EXCEEDED)
                IPCOM_MIB2(icmpInTimeExcds++);
            failing_ip_pkt = (Ipnet_pkt_ip *) icmp_hdr->data.failing_pkt.ip;

            if (icmp_len < IPNET_ICMP_HDR_SIZE + IPNET_IP_HDR_SIZE + 8)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 23, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                  ipnet_icmp4_input, IPCOM_WV_NETD_BADHLEN,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(icmp4_input_hdrsize2++);
                IPCOM_MIB2(icmpInErrors++);
                IPCOM_MIB2_SYSWI_U32_ADD(v4, icmpStatsInErrors, 1);
                goto cleanup;
            }

            fdst_addr_n = IPNET_IP4_GET_IPADDR(failing_ip_pkt->dst);
            fsrc_addr_n = IPNET_IP4_GET_IPADDR(failing_ip_pkt->src);
            proto = failing_ip_pkt->p;
            transport_hdr = (Ip_u8 *)failing_ip_pkt + IPNET_IP4_GET_HDR_OCTET_LEN(failing_ip_pkt);

#ifdef IPSCTP
            if (proto == IP_IPPROTO_SCTP)
            {
                /* notify the sctp,  process path mtu change and other error notifications*/
                ipsctp_icmp4_cb(netif, pkt);
                return 0;
            }
#endif

            if (proto == IP_IPPROTO_UDP || proto == IP_IPPROTO_TCP)
            {
                Ipnet_pkt_udp  *udp_pkt = transport_hdr;
                fsport = ip_ntohs(udp_pkt->sport);
                fdport = ip_ntohs(udp_pkt->dport);
            }
            else
            {
                fsport = 0;
                fdport = 0;
            }

            /* Find the affected socket */
            sock = ipnet_sock_ip4_lookup(pkt->vr_index, proto,
                                         &fsrc_addr_n, fsport,
                                         &fdst_addr_n, fdport);
            if (sock != IP_NULL
                && IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_CONNECTED | IPNET_SOCKET_FLAG_CONNECTING))
                /* Notify only connected sockets */
                sock = IP_NULL;

            if (icmp_hdr->type == IPNET_ICMP4_TYPE_DST_UNREACHABLE
                && icmp_hdr->code == IPNET_ICMP4_CODE_DST_NEEDFRAG
                && ipcom_sysvar_get_as_int0("ipnet.inet.EnablePathMtuDiscovery", 1))
            {
                /* Lower the path MTU to the destination */
                Ipnet_route_entry *rt;
                Ip_u32             max_path_mtu;

                max_path_mtu = IP_MAX(IP_GET_NTOHL(icmp_hdr->data.failing_pkt.next_hop_mtu), ipnet_conf_ip4_min_mtu);
                rt = ipnet_icmp4_dst_create_rt(netif, (struct Ip_in_addr *) &fdst_addr_n, IP_NULL);
                if (rt != IP_NULL && rt->metrics.rmx_mtu != max_path_mtu)
                {
                    rt->metrics.rmx_mtu = max_path_mtu;
                    IP_BIT_SET(rt->hdr.flags, IPNET_RTF_MODIFIED);
                    IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));
                }
#ifdef IPTCP
                if (sock != IP_NULL && sock->tcb != IP_NULL)
                    sock->notify_cb(sock, IPTCP_SOCKET_EVENT_PATH_MTU);
#endif
                goto cleanup;
            }

            if (sock != IP_NULL)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 24, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                                  ipnet_icmp4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(icmp4_input_dst_unreach_match++);

#ifdef IPTCP
                if (sock->tcb != IP_NULL)
                {
                    if (pkt->end - pkt->start >= IPTCP_TCP_HDR_SIZE)
                        iptcp_icmp4_report(sock, icmp_hdr->type, icmp_hdr->code, transport_hdr);
                    break;
                }
#endif

                if (icmp_hdr->type == IPNET_ICMP4_TYPE_TIME_EXCEEDED)
                {
                    switch (icmp_hdr->code)
                    {
                    case IPNET_ICMP4_CODE_TIM_TTL:
                    case IPNET_ICMP4_CODE_TIM_REASSEMBLY:
                        sock->ipcom.so_errno = IP_ERRNO_EHOSTUNREACH;
                        break;
                    default:
                        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                          1, 25, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                          ipnet_icmp4_input, IPCOM_WV_NETD_BADTYPE,
                                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                        IPNET_STATS(icmp4_input_dst_unreach_badcode++);
                        goto cleanup;
                    }
                }
                else
                {
                    switch (icmp_hdr->code)
                    {
                    case IPNET_ICMP4_CODE_DST_UNREACH_NET:
                        sock->ipcom.so_errno = IP_ERRNO_ENETUNREACH;
                        break;
                    case IPNET_ICMP4_CODE_DST_UNREACH_HOST:
                        sock->ipcom.so_errno = IP_ERRNO_EHOSTUNREACH;
                        break;
                    case IPNET_ICMP4_CODE_DST_UNREACH_PROTO:
                        sock->ipcom.so_errno = IP_ERRNO_EPROTONOSUPPORT;
                        break;
                    case IPNET_ICMP4_CODE_DST_UNREACH_PORT:
                        sock->ipcom.so_errno = IP_ERRNO_ECONNREFUSED;
                        break;
                    default:
                        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                          1, 26, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                          ipnet_icmp4_input, IPCOM_WV_NETD_BADTYPE,
                                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                        IPNET_STATS(icmp4_input_dst_unreach_badcode++);
                        goto cleanup;
                    }
                }

                /* Wake up processes sleeping on the socket. */
                ipnet_sock_data_avail(sock, sock->ipcom.so_errno, IP_SHUT_RDWR);
            }
            else
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 27, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                                  ipnet_icmp4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(icmp4_input_dst_unreach_nomatch++);
            }
        }
        break;

    case IPNET_ICMP4_TYPE_REDIRECT:
            IPCOM_MIB2(icmpInRedirects++);

            switch (icmp_hdr->code)
            {
            case IPNET_ICMP4_CODE_RED_NETWORK:
            case IPNET_ICMP4_CODE_RED_HOST:
                break;
            default:
                /* Invalid or unsupported code */
                goto cleanup;
            }

            if (ipnet_sysvar_netif_get_as_int(IP_AF_INET, netif, "IcmpRedirectReceive", 1))
            {
                (void) ipnet_icmp4_dst_create_rt(netif,
                                                 (struct Ip_in_addr *)((Ipnet_pkt_ip *) icmp_hdr->data.redirect.ip)->dst,
                                                 (struct Ip_in_addr *)icmp_hdr->data.redirect.gateway_addr);
            }
        break;

    case IPNET_ICMP4_TYPE_TSTAMP_REQUEST:
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET, netif, "IcmpIgnoreTimestampRequest", 0, ipnet_bool_map))
        {
            IPCOM_LOG2(DEBUG, "ignore timestamp request from %s on %s",
                       ipcom_inet_ntop(IP_AF_INET, ip4_param->from, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       netif->ipcom.name);
            goto cleanup;
        }
        IPCOM_MIB2(icmpInTimestamps++);
        icmp_param.type = IPNET_ICMP4_TYPE_TSTAMP_REPLY;
        icmp_param.code = 0;
        ret = ipnet_icmp4_send(&icmp_param, IP_FALSE);
        if (ret < 0 && icmp_param.recv_pkt != IP_NULL)
            /* ipnet_icmp4_send() did not free the orginal packet */
            goto cleanup;
        return ret;

    case IPNET_ICMP4_TYPE_ROUTER_ADVERT:
#ifdef IPNET_USE_RFC1256
        ipnet_ip4_rfc1256_advertise_input(netif, icmp_hdr, icmp_len, ip4_param);
#endif
        break;

    case IPNET_ICMP4_TYPE_ROUTER_SOLICIT:
#ifdef IPNET_USE_RFC1256
        ipnet_ip4_rfc1256_solicit_input(netif, icmp_hdr, icmp_len, ip4_param);
#endif
        break;

    case IPNET_ICMP4_TYPE_MASK_REQUEST:
        IPCOM_MIB2(icmpInAddrMasks++);
        break;

    case IPNET_ICMP4_TYPE_MASK_REPLY:
        IPCOM_MIB2(icmpInAddrMaskReps++);
        break;

    case IPNET_ICMP4_TYPE_SOURCEQUENCH:
        IPCOM_MIB2(icmpInSrcQuenchs++);
        break;

    case IPNET_ICMP4_TYPE_TSTAMP_REPLY:
        IPCOM_MIB2(icmpInTimestampReps++);
        break;

    case IPNET_ICMP4_TYPE_PARAMPROB:
        IPCOM_MIB2(icmpInParmProbs++);
        break;

    default:
        break;
    }

 cleanup:
    ipcom_pkt_free(pkt);
    return 0;
}



/*
 *===========================================================================
 *                    ipnet_ip4_martian_addr_filtering
 *===========================================================================
 * Description: Performs Martian Address Filtering as descibed by
 *              RFC1812, chapter 5.3.7
 * Parameters:  netif - ingress interface
 *              src_n - source address of IP datagram
 *              dst_n - destination address of IP datagram
 *              dst_addr_type - type of destination address.
 * Returns:     IP_FALSE addresses failed the Martian filter and the
 *                    IP datagram should be discarded.
 *              IP_TRUE - addresses looks OK
 */
IP_STATIC Ip_bool
ipnet_ip4_martian_addr_filtering(Ipnet_netif *netif,
                                 Ip_u32 src_n,
                                 Ip_u32 dst_n,
                                 enum Ipnet_addr_type dst_addr_type)
{
    enum Ipnet_addr_type src_addr_type;

    /* An IP destination address is invalid if it is among those
     * defined as illegal destinations in 4.2.3.1, or is a Class E
     * address (except 255.255.255.255).
     */
    if (dst_addr_type == IPNET_ADDR_TYPE_ANY
        || (dst_addr_type != IPNET_ADDR_TYPE_BROADCAST
            && IP_IN_EXPERIMENTAL(dst_n)))
        return IP_FALSE;

    /*
     * A router SHOULD NOT forward, except over a loopback interface,
     * any packet that has a source address on network 127.
     */
    if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_LOOPBACK)
        && (*(Ip_u8*) &src_n) == 127)
        return IP_FALSE;


    /* If not not local, this packet is destined for this host - 
     * skip router sanity
     */
    if (dst_addr_type != IPNET_ADDR_TYPE_NOT_LOCAL)
        return IP_TRUE;

    /*
     * A router SHOULD NOT forward any packet that has an invalid IP
     * source address or a source address on network 0.
     */
    src_addr_type = ipnet_ip4_get_addr_type(src_n,
                                            netif->vr_index,
                                            netif);
    return (!IP_IN_CLASSD(src_n)
            && !IP_IN_EXPERIMENTAL(src_n)
            && (src_addr_type == IPNET_ADDR_TYPE_UNICAST
                || src_addr_type == IPNET_ADDR_TYPE_NOT_LOCAL));
}



/*
 *===========================================================================
 *                     ipnet_ip4_src_addr_validation
 *===========================================================================
 * Description: Verifies that the source address would be reached
 *              through the passed network interface.
 * Parameters:  netif - ingress interface for IP-datafram with source
 *                     'src_n'
 *              src_n - an IP address
 * Returns:     IP_TRUE - correct interface
 *              IP_FALSE - wrong interface.
 *
 */
#ifdef IPNET_USE_RFC1812_SRC_ADDR_VALIDATION
IP_STATIC Ip_bool
ipnet_ip4_src_addr_validation(Ipnet_netif *netif, Ip_u32 src_n)
{
    Ipnet_route_entry *rt;

    /*
     * RFC 1812, chapter 5.3.8
     * If this filtering is enabled, the router MUST silently discard
     * a packet if the interface on which the packet was received is
     * not the interface on which a packet would be forwarded to reach
     * the address contained in the source address.  In simpler terms,
     * if a router wouldn't route a packet containing this address
     * through a particular interface, it shouldn't believe the
     * address if it appears as a source address in a packet read from
     * this interface.

     */
    return ipnet_route_lookup(IP_AF_INET,
                              netif->vr_index,
                              IPCOM_ROUTE_TABLE_DEFAULT,
                              0,
                              &src_n,
                              0,
                              netif->ipcom.ifindex,
                              &rt) >= 0;
}
#endif /* IPNET_USE_RFC1812_SRC_ADDR_VALIDATION */


/*
 *===========================================================================
 *                         ipnet_ip4_need_address
 *===========================================================================
 * Description: Returns if a network interface need an IP address.
 * Parameters:  netif - a network interface
 * Returns:     IP_TRUE if no "normal" unicast IP address is assigned
 *                         to the interface
 *              IP_FALSE if at least one normal unicast IP address is
 *                         assigned.
 *
 */
IP_STATIC Ip_bool
ipnet_ip4_need_address(Ipnet_netif *netif)
{
    Ipnet_ip4_addr_entry *addr;

    addr = netif->inet4_addr_list;

    if (addr == IP_NULL)
        /* No addresses assigned to the interface at all */
        return IP_TRUE;

    if (addr->type != IPNET_ADDR_TYPE_UNICAST)
        /* No unicast addresses assigned to this interface */
        return IP_TRUE;

#ifdef IPNET_USE_RFC3927
    if (IPNET_IP4_IS_LINK_LOCAL(&addr->ipaddr_n))
        /*
         * The only assigned unicast IP address is an automatically
         * created IPv4 link local address.
         */
        return IP_TRUE;
#endif /* IPNET_USE_RFC3927 */

    return IP_FALSE;
}



/*
 *===========================================================================
 *                    ipnet_ip4_input
 *===========================================================================
 * Description: Handler for received IPv4 packets.
 * Parameters:  pkt - Received IPv4 packet. (pkt->start is offset to IPv4 header)
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_FASTTEXT IP_GLOBAL int
ipnet_ip4_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip        *ip_pkt;
    Ipnet_icmp_param     icmp_param;
    struct Ip_in_addr    src;
    struct Ip_in_addr    dst;
    enum Ipnet_addr_type addr_type;
    int                  ret = -IP_ERRNO_EINVAL;
    int                  ip_datagram_len;
    int                  ip_hdr_len;
    int                  nat_t    = IP_UDP_NAT_T_DISABLED;
    Ipnet_ip4_output_param ip4_param;
#ifdef IPIPSEC2
    int                ipsec_ret = IPIPSEC_OK;
    Ipipsec_param      ipsec_param;
#endif
#ifdef IPFIREWALL_USE_IP_FILTER
    Ip_bool            fw_checked = IP_FALSE;
#endif

    IPCOM_PROF("B: IPv4 input");

    ip_assert(netif != IP_NULL);
    ip_assert(pkt != IP_NULL);
    ip_assert(pkt->data != IP_NULL);
    ip_assert(pkt->end <= pkt->maxlen);
    ip_assert(pkt->start <= pkt->end);
    ip_assert2((pkt->data[pkt->start] & 0xf0) == 0x40);

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 28, IPCOM_WV_NETDEVENT_START,
                       ipnet_ip4_input, IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
    IPNET_STATS(ip4_input++);
    IPCOM_MIB2(ipInReceives++);
    IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInReceives, 1);
    IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInReceives, 1, netif);
    IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInOctets, pkt->end - pkt->start);
    IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInOctets, pkt->end - pkt->start, netif);

#if defined(IPIPSEC2) || defined(IPNET_USE_NAT)
 parse_ip_datagram:
#endif
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IP4_INPUT);

    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPV4);
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_IPV6);

    /* Packet must minimum contain an IP header. */
    if (IP_UNLIKELY(pkt->end - pkt->start < IPNET_IP_HDR_SIZE))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 29, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADHLEN,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_hdr_trunc++);
        IPCOM_MIB2(ipInHdrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInHdrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInHdrErrors, 1, netif);
        goto cleanup;
    }

    pkt->ipstart = pkt->start;

#ifdef IPROHC
    if (iprohc.opened)
    {
        ret = iprohc_input_hook(&netif->ipcom, pkt);
        if (ret != IPCOM_SUCCESS)
        {
            /* Decompress fails, discard the packet */
            IPCOM_LOG1(WARNING,
                       "Discarding rcv'ed IPv4 datagram on %s, ROHC failed.",
                       netif->ipcom.name);
            ret = -IP_ERRNO_EROHC;
            goto cleanup;
        }
    }
#endif /* IPROHC */

    ip_pkt = (Ipnet_pkt_ip *)&pkt->data[pkt->ipstart];
    dst.s_addr = IPNET_IP4_GET_IPADDR(ip_pkt->dst);
    src.s_addr = IPNET_IP4_GET_IPADDR(ip_pkt->src);
    ip_hdr_len = IPNET_IP4_GET_HDR_OCTET_LEN(ip_pkt);
    ip_datagram_len = ip_ntohs(ip_pkt->len);
    if (pkt->end != pkt->start + ip_datagram_len)
    {
        if (pkt->end < pkt->start + ip_datagram_len)
        {
            /* Either a bogus packet or the packet was truncated at some point */
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInTruncatedPkts, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInTruncatedPkts, 1, netif);
            goto cleanup;
        }

        pkt->end = pkt->start + ip_datagram_len;
#ifdef IPCOM_USE_HW_CHECKSUM_RX
        /* The hardware calculated checksum has included padding
           so it cannot be used, revert back to software checksumming */
        pkt->chk = 0;
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_TL_CHECKSUM);
#endif
    }

    pkt->tlstart = pkt->start + ip_hdr_len;

    if (IP_LIKELY(netif->inet4_addr_type_cache.rt_id == ipnet->route_cache_id)
        && netif->inet4_addr_type_cache.addr.s_addr == dst.s_addr)
        addr_type = netif->inet4_addr_type_cache.type;
    else
    {
        /* Address destination check. */
        addr_type = ipnet_ip4_get_addr_type2(dst.s_addr,
                                             IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
                                             ? ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex)
                                             : netif);
        if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
        {
#ifdef IPNET_STRICT_MODE
            if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK))
                /* In strict mode, check all addresses if Rx interface is loopback. */
#endif /* IPNET_STRICT_MODE */
                /* In loose mode, always check all addresses */
                addr_type = ipnet_ip4_get_addr_type(dst.s_addr, pkt->vr_index, IP_NULL);

            if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_X_DHCPRUNNING)
                && addr_type == IPNET_ADDR_TYPE_NOT_LOCAL
                && ip_pkt->p == IP_IPPROTO_UDP
                && IP_BIT_ISFALSE(pkt->flags, (IPCOM_PKT_FLAG_LINK_BROADCAST
                                               | IPCOM_PKT_FLAG_LINK_MULTICAST
                                               | IPCOM_PKT_FLAG_LINK_OTHER))
                && ipnet_ip4_need_address(netif))
                {
                    /*
                     * A DHCP client is running on this interface, the
                     * payload of the IP datagram is UDP and no
                     * address has been assigned to this interface
                     * yet.
                     * Accept This IP datagram as local since it
                     * might be a DHCP offer.
                     */
                    addr_type = IPNET_ADDR_TYPE_UNICAST;
                }

        }
    }

    if (!ipnet_ip4_martian_addr_filtering(netif, src.s_addr, dst.s_addr, addr_type))
    {
        /*
         * This is a Martian address. This packet SHOULD be discarded
         * (RFC 1812, chapter 5.3.7)
         */

        /*
         * RFC 1812, chapter 5.3.7
         * If a router discards a packet because of these rules, it
         * SHOULD log at least the IP source address, the IP
         * destination address, and, if the problem was with the
         * source address, the physical interface on which the packet
         * was received and the Link Layer address of the host or
         * router from which the packet was received.
         */
        IPCOM_LOG3(INFO,
                   "IPv4: packet from %s to %s received on %s failed sanity check",
                   ipcom_inet_ntop(IP_AF_INET, &src, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   ipcom_inet_ntop(IP_AF_INET, &dst, ipnet->log_buf + IP_INET_ADDRSTRLEN, sizeof(ipnet->log_buf)),
                   netif->ipcom.name);
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 30, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADADDR,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_not_to_me++);
        IPCOM_MIB2(ipInAddrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInAddrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInAddrErrors, 1, netif);
        goto cleanup;
    }

#ifdef IPNET_USE_RFC1812_SRC_ADDR_VALIDATION
    if (!ipnet_ip4_src_addr_validation(netif, src.s_addr))
    {
        /*
         * RFC 1812, chapter 5.3.8
         * If this filtering is enabled, the router MUST silently
         * discard a packet if the interface on which the packet was
         * received is not the interface on which a packet would be
         * forwarded to reach the address contained in the source
         * address.
         */
        IPCOM_MIB2(ipInAddrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInAddrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInAddrErrors, 1, netif);
        goto cleanup;
    }
#endif /* IPNET_USE_RFC1812_SRC_ADDR_VALIDATION */


    /* Add this address type lookup to the cache */
    netif->inet4_addr_type_cache.rt_id = ipnet->route_cache_id;
    netif->inet4_addr_type_cache.addr  = dst;
    netif->inet4_addr_type_cache.type  = addr_type;

    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
    {
#ifdef IPNET_FASTPATH
        /* Try fast forward path */
        switch (ipnet_ip4_try_fast_forward_path(netif, pkt, ip_pkt))
        {
        case IPNET_FORWARD_CACHE_TAKEN:
            /* Fast forwarded */
            return 0;
#ifdef IPFIREWALL_USE_IP_FILTER
        case IPNET_FORWARD_CACHE_BLOCKED_BY_FW:
            /* Blocked by firewall */
            ret = -IP_ERRNO_EFIREWALL;
            goto cleanup;
#endif
#ifdef IPNET_USE_NAT
        case IPNET_FORWARD_CACHE_BLOCKED_BY_NAT:
            /* Blocked by NAT */
            ret = -IP_ERRNO_ENAT;
            goto cleanup;
#endif
        default:
            /* Use normal path */
            break;
        }
#endif /* IPNET_FASTPATH */
    }
    else
    {
        if (ipnet->ip4.rx_cache.sock != IP_NULL
            && IPNET_ISFRAG(ip_pkt->off) == IP_FALSE
            && dst.s_addr == ipnet->ip4.rx_cache.dst.s_addr
            && src.s_addr == ipnet->ip4.rx_cache.src.s_addr
            && ip_pkt->p == ipnet->ip4.rx_cache.proto
            && ip_hdr_len == IPNET_IP_HDR_SIZE
            && IP_GET_32ON16(&pkt->data[pkt->tlstart]) == ipnet->ip4.rx_cache.tldata
            && ipnet->ip4.rx_cache.sock->vr_index == pkt->vr_index)
        {
            Ip_u32        chk = pkt->chk;
            Ip_bool       fast_path_taken;
            Ipnet_socket *cache_sock = ipnet->ip4.rx_cache.sock;
            Ipnet_rx_cache_fast_input_f fast_input_func = ipnet->ip4.rx_cache.fast_input_func;

#ifdef IPFIREWALL_USE_IP_FILTER
            if (ipfirewall != IP_NULL)
            {
                if (ipfirewall_input_hook(&netif->ipcom, pkt, IP_NULL))
                {
                    ret = -IP_ERRNO_EFIREWALL;
                    goto cleanup;
                }
                else
                    fw_checked = IP_TRUE;
            }
#endif /* IPFIREWALL_USE_IP_FILTER */

            pkt->chk += ipnet_ip4_pseudo_header_checksum_update(&src,
                                                                &dst,
                                                                ip_pkt->p,
                                                                (Ip_u16) (pkt->end - pkt->tlstart));

            fast_path_taken = fast_input_func(cache_sock, pkt, &nat_t);
            if (fast_path_taken)
                return 0;

            pkt->chk = chk;

            if (IP_UNLIKELY(nat_t != 0))
            {
                switch (nat_t)
                {
#ifdef IPNET_USE_TUNNEL
                case IP_UDP_NAT_T_RFC3519:
                    /* Fast forward the UDP encap'ed packet */
                    pkt->start += ip_hdr_len;
                    ipnet_tunnel_ip4_rfc3519_input(pkt);
                    return 0;
                case IP_UDP_NAT_T_DSMIPV6:
                    pkt->start += ip_hdr_len;
                    ipnet_tunnel_dsmipv6_input(pkt);
                    return 0;
#endif
#ifdef IPIPSEC2
                case IP_UDP_NAT_T_RFC3948:
                    /* Flag it as unicast; go to ipipsec decap */
                    addr_type = IPNET_ADDR_TYPE_UNICAST;
                    pkt->chk = 0;
#ifdef IPFIREWALL_USE_IP_FILTER
                    fw_checked = IP_FALSE;
#endif
                    pkt->start += ip_hdr_len;
                    goto ipipsec_process;
#endif
                }
            }
        }
    }

    /* Use the normal input path since non of the fast forwarding
       path or the fast termination paths was taken */
    pkt->start += ip_hdr_len;

    /* Invalid IP header length check. */
    if (IP_UNLIKELY(ip_hdr_len < IPNET_IP_HDR_SIZE)
        || IP_UNLIKELY(ip_hdr_len > ip_datagram_len))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 31, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADLEN,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_inval_len++);
        IPCOM_MIB2(ipInHdrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInHdrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInHdrErrors, 1, netif);
        goto cleanup;
    }

    /* Version check version and length. */
    if (IP_UNLIKELY((ip_pkt->v_hl & 0xF0) != 0x40))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 32, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADVERS,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_inval_version++);
        IPCOM_MIB2(ipInHdrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInHdrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInHdrErrors, 1, netif);
        goto cleanup;
    }

    /* Init ip4_param, i.e. cache src&dst at native alignments. */
    ip4_param.flags   = 0;
    ip4_param.from    = &src;
    ip4_param.to      = &dst;
    ip4_param.tos     = ip_pkt->tos;
    ip4_param.nexthop = IP_NULL;
    ip4_param.proto   = ip_pkt->p;
    ip4_param.opts    = IP_NULL;

    if (IP_LIKELY(!IP_IN_CLASSD(dst.s_addr))
        && IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_MULTICAST)))
        /* Multicast at the link layer but sent to a unicast IP address,
           discard this packet */
        goto cleanup;

#ifdef IPNET_USE_VRRP
    if (netif->ipcom.type != IP_IFT_ETHER && netif->ipcom.type != IP_IFT_L2VLAN)
        /* VRRP is currently not implemented for other link layers */
        IPCOM_PKT_SET_VRID(pkt, 0);
#endif

    if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER)))
    {
#ifdef IPNET_USE_VRRP
        /* Do cleanup if this is not a VRRP packet or if this host is not
           master of the specific VRID.  */
        if (IPCOM_PKT_GET_VRID(pkt) == 0
            || ipnet_vrrp_get_addr_entry(netif, IPCOM_PKT_GET_VRID(pkt)) == IP_NULL)
#endif
            /* Do not process packets that was sent to a link layer unicast
               address that is not ours, AF_PACKET sockets will get this packet
               if the interface is promiscuous mode */
            goto cleanup;
#ifdef IPNET_USE_VRRP
        /* else: this host owns this VRRP address on the VRID passed by the link layer */
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER);
#endif
    }

    if (IP_UNLIKELY(addr_type == IPNET_ADDR_TYPE_ANY))
    {
        /* Invalid destination address */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 33, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADADDR,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_not_to_me++);
        IPCOM_MIB2(ipInAddrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInAddrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInAddrErrors, 1, netif);
        goto cleanup;
    }

    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
    {
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_PROMISC)
            && IP_IN_CLASSD(dst.s_addr))
            /* Must process all multicast packets when running in promiscuous mode */
            addr_type = IPNET_ADDR_TYPE_MULTICAST;
#ifdef IPNET_USE_VRRP
        if (ipnet_vrrp_owns_addr(netif, IPCOM_PKT_GET_VRID(pkt), dst))
            /* This host currently owns this virtual router address */
            addr_type = IPNET_ADDR_TYPE_UNICAST;
#endif
    }

    /* IP checksum must be ok. */
    if (IP_UNLIKELY(ipcom_in_checksum(ip_pkt, ip_hdr_len) != 0))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 34, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADSUM,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_iph_badchksum++);
        IPCOM_MIB2(ipInHdrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInHdrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInHdrErrors, 1, netif);
        goto cleanup;
    }

    if (IP_UNLIKELY(ip_hdr_len != IPNET_IP_HDR_SIZE))
    {
        /* IP header contains options */
        ret = ipnet_ip4_process_options(pkt,
                                        ip_hdr_len - IPNET_IP_HDR_SIZE,
                                        &ip4_param);
        if (ret < 0)
        {
            IPCOM_MIB2(ipInHdrErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInHdrErrors, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInHdrErrors, 1, netif);
            goto cleanup;
        }
    }

    if (IP_UNLIKELY(IPNET_ISFRAG(ip_pkt->off)))
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_FRAGMENT | IPCOM_PKT_FLAG_NO_RX_CACHE);

    if (addr_type != IPNET_ADDR_TYPE_NOT_LOCAL && addr_type != IPNET_ADDR_TYPE_UNICAST)
    {
        netif->imcasts++;
    }

    if (addr_type == IPNET_ADDR_TYPE_MULTICAST)
    {
        IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInMcastPkts, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInMcastPkts, 1, netif);
        IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInMcastOctets, pkt->end - pkt->start);
        IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInMcastOctets, pkt->end - pkt->start, netif);
    }
    else if (addr_type == IPNET_ADDR_TYPE_NETBROADCAST
             || addr_type == IPNET_ADDR_TYPE_BROADCAST)
    {
        IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInBcastPkts, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInBcastPkts, 1, netif);
    }

    if (IP_IN_CLASSD(dst.s_addr))
    {
#ifdef IPNET_USE_MCAST_ROUTING
        if (ip4_param.proto != IP_IPPROTO_IGMP
            && ((ip_ntohl(dst.s_addr) & 0xffffff00) != 0xe0000000))
        {
            /* Input packets for multicast routing unless they are IGMP packets
               or in the 224.0.0.0/24 network */
            if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FRAGMENT)))
                pkt->offset = (Ip_u16) ipnet_ip4_get_offset(pkt);
            ipnet_ip4_mcast_input(netif, pkt);
        }
#endif /* IPNET_USE_MCAST_ROUTING */

        if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 35, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_ip4_input, IPCOM_WV_NETD_BADADDR,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_input_not_to_me++);
            IPCOM_MIB2(ipInAddrErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInAddrErrors, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInAddrErrors, 1, netif);
            goto cleanup;
        }
    }

    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL
        && (IP_BIT_ISFALSE(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT)
            || IP_BIT_ISSET(netif->flags2, IPNET_IFF2_NO_IPV4_FORWARD)
            || IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_BROADCAST)))
    {
        /* Packet not to me and not configured as a router or packet was
           a broadcast on the link layer */
        IPCOM_MIB2(ipInAddrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInAddrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInAddrErrors, 1, netif);
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 36, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADADDR,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_not_to_me++);
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_LINK_BROADCAST))
            IPCOM_LOG2(DEBUG2,
                       "IPv4: Not configured as a router, will not forward packet from %s to %s",
                       ipcom_inet_ntop(IP_AF_INET, &src, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       ipcom_inet_ntop(IP_AF_INET, &dst, ipnet->log_buf + IP_INET_ADDRSTRLEN, sizeof(ipnet->log_buf)));
        goto cleanup;
    }

    /* Packet to me -> Reassemble, IPSEC and/or NAT */
    if (addr_type != IPNET_ADDR_TYPE_NOT_LOCAL)
    {
        /* Reassemble fragments. */
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FRAGMENT))
        {
            Ipcom_pkt *reassembly_list;

            IPCOM_MIB2(ipReasmReqds++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsReasmReqds, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsReasmReqds, 1, netif);
            IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IP4_REASSEMBLY);

            reassembly_list = ipnet_reassembly(&ipnet->ip4.reassembly_list,
                                               pkt,
                                               ipnet_ip4_is_part_of_same_pkt,
                                               ipnet_ip4_get_offset,
                                               ipnet_ip4_more_fragments,
                                               (Ipnet_timeout_handler) ipnet_ip4_fragment_timeout,
                                               0);

            if (reassembly_list == IP_NULL)
                /* More fragments needed */
                return 0;

            ip_datagram_len = reassembly_list->start - reassembly_list->ipstart;

            pkt = ipnet_create_reassembled_packet(reassembly_list,
                                                  ip_datagram_len,
                                                  0,
                                                  ipnet_ip4_update_ip_header,
                                                  IP_NULL);
            if (pkt == IP_NULL)
            {
                IPCOM_MIB2(ipInDiscards++);
                IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInDiscards, 1);
                IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInDiscards, 1, netif);
                IPCOM_MIB2(ipReasmFails++);
                IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsReasmFails, 1);
                IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsReasmFails, 1, netif);
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                                  1, 37, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_RECV,
                                  ipnet_ip4_input, IPCOM_WV_NETD_NOBUFS,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(ip4_input_reasm_enobufs++);
                return -IP_ERRNO_ENOBUFS;
            }

            IPCOM_MIB2(ipReasmOKs++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsReasmOKs, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsReasmOKs, 1, netif);

            pkt->start += ip_datagram_len;
            ip_pkt      = (Ipnet_pkt_ip *)&pkt->data[pkt->ipstart];
            ip_hdr_len  = IPNET_IP4_GET_HDR_OCTET_LEN(ip_pkt);
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPV4);
        }

#ifdef IPIPSEC2
    ipipsec_process:
        if (ip_pkt->p == IP_IPPROTO_ESP
            || ip_pkt->p == IP_IPPROTO_AH
            /*|| ip_pkt->p == IP_IPPROTO_IPIP*/
            || nat_t == IP_UDP_NAT_T_RFC3948)
        {
            /* Reset any NAT-T */
            nat_t = IP_UDP_NAT_T_DISABLED;

            ipsec_param.key.src.in.s_addr = src.s_addr;
            ipsec_param.key.dst.in.s_addr = dst.s_addr;
            ipsec_param.key.proto         = ip_pkt->p;
            ipsec_param.key.domain        = IP_AF_INET;
#ifdef IPIPSEC_USE_DAEMONS
            ipsec_ret = ipipsec_send_daemon_pipe(pkt, &ipsec_param, 1);
#else
            ipsec_ret = ipipsec_input(pkt, &ipsec_param);
#endif
            if (ipsec_ret >= IPIPSEC_OK)
            {
                if (ipsec_ret == IPIPSEC_KEPT)
                {
                    /* IPSEC kept the packet for later reception */
                    return 0;
                }
                IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE | IPCOM_PKT_FLAG_NO_RX_CACHE);
                /* return value 1 -> packet is tunneled. */
                if (ipsec_ret == IPIPSEC_TUNNELED)
                {
                    if (ipsec_param.key.domain == IP_AF_INET6)
                    {
                        /* IPV6 tunneled. */
#ifdef IPCOM_USE_INET6
                        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_IPV4);
                        return ipnet_ip6_input(netif, pkt);
#else
                        goto cleanup;
#endif
                    }
                    /* IPIP tunneled. */
                    goto parse_ip_datagram;
                }

                ip_assert(src.s_addr ==  ipsec_param.key.src.in.s_addr);
                ip_assert(dst.s_addr == ipsec_param.key.dst.in.s_addr);
                ip_assert(ipsec_param.key.proto != IP_IPPROTO_ESP);
                ip_assert(ipsec_param.key.proto != IP_IPPROTO_AH);

                ip4_param.proto = ipsec_param.key.proto;

                /* IPv4 header may have moved. */
                ip_pkt = (Ipnet_pkt_ip *)&pkt->data[pkt->ipstart];
                ip_assert((ip_pkt->v_hl & 0xf0) == 0x40);
            }
        }
#endif /* IPIPSEC2 */

#ifdef IPNET_USE_NAT
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_NAT))
        {
            Ip_s32 natret;
#ifdef IPNET_USE_NAT_PT
            Ipnet_ip6_output_param param6;
            natret = ipnet_nat_input_hook(&pkt, &param6);
#else
            natret = ipnet_nat_input_hook(&pkt, IP_NULL);
#endif
            switch (natret)
            {
#ifdef IPNET_USE_NAT_PT
            case IPNET_NAT_PT_TRANSLATED:
                return ipnet_ip6_output(&param6, pkt);
#endif
            case IPNET_NAT_TRANSLATED:
                pkt->start -= ip_hdr_len;
                /* Avoid NAT input hook to be called again and avoid
                   NAT output hook to be called later */
                IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NAT);
                goto parse_ip_datagram;
            case IPNET_NAT_NOMATCH:
            case IPNET_NAT_INVALID:
                break;
            case IPNET_NAT_DROP:
                ret = -IP_ERRNO_ENAT;
                goto cleanup;
            default:
                break;
            }
        }
#endif  /* #ifdef IPNET_USE_NAT */

#ifdef IPIPSEC2
        if (ipsec_ret < IPIPSEC_OK)
            goto cleanup;    /* discard ipsec datagram. */
#endif
    }

#ifdef IPIPSEC2
    if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE))
    {
        ipsec_param.key.src.in.s_addr = src.s_addr;
        ipsec_param.key.dst.in.s_addr = dst.s_addr;
        ipsec_param.key.proto         = ip_pkt->p;
        ipsec_param.key.domain        = IP_AF_INET;

        /* Check policy for non ipsec'd traffic */
        ipsec_ret = ipipsec_policy_check(pkt, &ipsec_param);
        if (ipsec_ret < IPIPSEC_OK)
            goto cleanup;    /* discard ipsec datagram. */
    }
#endif /* IPIPSEC2 */

#ifdef IPFIREWALL_USE_IP_FILTER
    if (ipfirewall != IP_NULL
        && fw_checked == IP_FALSE
        && ipfirewall_input_hook(&netif->ipcom, pkt, IP_NULL))
    {
        ret = -IP_ERRNO_EFIREWALL;
        goto cleanup;
    }
#endif /* IPFIREWALL_USE_IP_FILTER */

    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
    {
        /********** BEGIN FORWARDING **********/
        IPCOM_PROF_TYPE(prof_forward);
        IPCOM_PROF_PUSH(prof_forward, "IPv4 Forward");

#ifdef IPNET_STRICT_MODE
        if (ipnet_ip4_get_addr_type(dst.s_addr, pkt->vr_index, IP_NULL) != IPNET_ADDR_TYPE_NOT_LOCAL)
            /* Destination address is local, but not on this interface. Do not route packets to ourselves */
            goto cleanup;
#endif

        if (IP_UNLIKELY(IP_BIT_ISSET(ip4_param.flags, IPNET_IP4_OPF_ROUTER_ALERT)))
        {
            /* Send a copy of this packet to all matching socket */
            if (ipnet_ip4_deliver_to_raw_sock(pkt, &ip4_param, IP_FALSE) == 0)
                /* Packet matched at least one socket */
                goto cleanup;
            /* else: use the normal forwarding path for this packet */
        }

        ip4_param.ttl = ip_pkt->ttl;
        if (IP_UNLIKELY(ip4_param.ttl-- <= 1))
        {
            /* The time to live field reached 0, send a time exceeded message */
            ipnet_icmp4_param_init(&icmp_param, pkt, &ip4_param);
            icmp_param.type = IPNET_ICMP4_TYPE_TIME_EXCEEDED;
            icmp_param.code = IPNET_ICMP4_CODE_TIM_TTL;
            ret = ipnet_icmp4_send(&icmp_param, IP_FALSE);
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 38, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_ip4_input, IPCOM_WV_NETD_INFO_TIMEOUT,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(ip4_input_time_exceeded++);
            IPCOM_MIB2(ipInHdrErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInHdrErrors, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInHdrErrors, 1, netif);
            goto cleanup;
        }
        ip_pkt->ttl = ip4_param.ttl;

        if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FRAGMENT)))
            pkt->offset = (Ip_u16) ipnet_ip4_get_offset(pkt);

        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED);
        pkt->start = pkt->ipstart;
        ip_assert2((pkt->data[pkt->start] & 0xf0) == 0x40);

#ifdef IPNET_USE_DIFFSERV
        /* Classifier, meter and marker step */
        ipnet_diffserv_input(pkt);
#endif /* IPNET_USE_DIFFSERV */

        if (IP_BIT_ISSET(ip_pkt->off, IPNET_OFF_DF))
            IP_BIT_SET(ip4_param.flags, IPNET_IP4_OPF_DONT_FRAG);
        ip4_param.netif      = IP_NULL;
        ip4_param.chksum_ptr = IP_NULL;

        IPCOM_MIB2(ipForwDatagrams++);
        IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInForwDatagrams, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInForwDatagrams, 1, netif);

        /* No socket cache when doing forwardning */
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
        ip4_param.cache = IP_NULL;
        ret = ipnet_ip4_output(&ip4_param, pkt);

        IPCOM_PROF_POP(prof_forward);
        return ret;
        /********** END FORWARDING **********/
    }

    IP_PREPARE_FOR_READ(&pkt->data[pkt->start]);
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_LINK_MULTICAST);

    /* Act on the IP protocol. */
    switch (ip4_param.proto)
    {
    case IP_IPPROTO_ICMP:
    case IP_IPPROTO_IGMP:
        ret = ipnet_icmp4_input(netif, pkt, &ip4_param);
        if (ret == 0)
        {
            IPCOM_MIB2(ipInDelivers++);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInDelivers, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInDelivers, 1, netif);
        }
        return ret;
    case IP_IPPROTO_UDP:
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 39, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(udp4_input++);

        pkt->chk += ipnet_ip4_pseudo_header_checksum_update(&src,
                                                            &dst,
                                                            IP_IPPROTO_UDP,
                                                            (Ip_u16) (pkt->end - pkt->start));

        ip_assert(addr_type != IPNET_ADDR_TYPE_NOT_LOCAL);
        ret = ipnet_udp_input(pkt,
                              addr_type != IPNET_ADDR_TYPE_UNICAST,
                              ip4_param.from,
                              ip4_param.to,
                              (Ipnet_sock_lookup_f) ipnet_sock_ip4_lookup,
                              &nat_t);
        if (ret < 0)
        {
            if (ret == -IP_ERRNO_ECONNREFUSED && addr_type == IPNET_ADDR_TYPE_UNICAST)
            {
                /* No one listen on the destination port, send back a Port Unreachable error */
                ipnet_icmp4_param_init(&icmp_param, pkt, &ip4_param);
                icmp_param.type = IPNET_ICMP4_TYPE_DST_UNREACHABLE;
                icmp_param.code = IPNET_ICMP4_CODE_DST_UNREACH_PORT;
                (void) ipnet_icmp4_send(&icmp_param, IP_FALSE);
            }
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 40, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_ip4_input, IPCOM_WV_NETD_NOPORT,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(udp4_input_err++);
            goto cleanup;
        }
        else if (IP_UNLIKELY(ret == 0))
        {
            switch (nat_t)
            {
#ifdef IPIPSEC2
            case IP_UDP_NAT_T_RFC3948:
                /* Do ipsec processing for this frame; its NAT-Traversal */
                pkt->chk = 0;
                goto ipipsec_process;
#endif
#ifdef IPNET_USE_TUNNEL
            case IP_UDP_NAT_T_RFC3519:
                /* Do RFC3519 processing */
                ipnet_tunnel_ip4_rfc3519_input(pkt);
                break;
            case IP_UDP_NAT_T_DSMIPV6:
                ipnet_tunnel_dsmipv6_input(pkt);
                break;
#endif
            }
        }

        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 41, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPCOM_MIB2(ipInDelivers++);
        IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInDelivers, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInDelivers, 1, netif);
        IPNET_STATS(udp4_input_ok++);
        return 0;

#ifdef IPTCP
    case IP_IPPROTO_TCP:
        {
            IPCOM_PROF_TYPE(prof_tcp);

            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 42, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_ip4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPNET_STATS(tcp4_input++);

            if (addr_type != IPNET_ADDR_TYPE_UNICAST)
                /* Address must be unicast for TCP */
                goto cleanup;

            IPCOM_PROF_PUSH(prof_tcp, "IPv4 TCP input");

            pkt->chk += ipnet_ip4_pseudo_header_checksum_update(&src,
                                                                &dst,
                                                                IP_IPPROTO_TCP,
                                                                (Ip_u16) (pkt->end - pkt->start));

            ret = iptcp_input(pkt,
                              ip4_param.from,
                              ip4_param.to,
                              (Iptcp_socklookup) ipnet_sock_ip4_lookup);
            IPCOM_PROF_POP(prof_tcp);
            if (ret < 0)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 43, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                  ipnet_ip4_input, IPCOM_WV_NETD_NOPORT,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(tcp4_input_err++);
                goto cleanup;
            }

            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 44, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_ip4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPCOM_MIB2(ipInDelivers++);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInDelivers, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInDelivers, 1, netif);
            IPNET_STATS(tcp4_input_ok++);
            IPCOM_PROF("E: IPv4 input");
            return 0;
        }
#endif /* IPTCP */

#ifdef IPSCTP
    case IP_IPPROTO_SCTP:
        {
            IPCOM_PROF_TYPE(prof_sctp);

            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 46, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_ip4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            /*IPNET_STATS(tcp4_input++);*/

            if (addr_type != IPNET_ADDR_TYPE_UNICAST)
                /* Address must be unicast for SCTP */
                goto cleanup;

            IPCOM_PROF_PUSH(prof_sctp, "IPv4 SCTP input");

            /*pkt->chk += ipnet_ip4_pseudo_header_checksum_update(&src,
                                                                &dst,
                                                                IP_IPPROTO_TCP,
                                                                (Ip_u16) (pkt->end - pkt->start));*/

            ret = ipsctp_input(pkt,
                              ip4_param.from,
                              ip4_param.to);
            IPCOM_PROF_POP(prof_sctp);
            if (ret < 0)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 47, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                  ipnet_ip4_input, IPCOM_WV_NETD_NOPORT,
                                  IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
                /*IPNET_STATS(tcp4_input_err++);*/
                goto cleanup;
            }

            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 48, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_ip4_input, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
            IPCOM_MIB2(ipInDelivers++);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInDelivers, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInDelivers, 1, netif);
            /*IPNET_STATS(tcp4_input_ok++);*/
            IPCOM_PROF("E: IPv4 input");
            return 0;
        }
#endif /* IPSCTP */

#ifdef IPNET_USE_TUNNEL
    case IP_IPPROTO_IPIP:
    case IP_IPPROTO_MOBILE:
# ifdef IPCOM_USE_INET6
    case IP_IPPROTO_IPV6:
# endif
# ifdef IPNET_USE_GRE
    case IP_IPPROTO_GRE:
# endif
        return ipnet_tunnel_ip4_input(pkt);
#endif /* IPNT_USE_TUNNEL */

#ifdef IPNET_USE_MCAST_ROUTING
    case IP_IPPROTO_PIM:
        return ipnet_pim_ip4_input(pkt);
#endif
    default:
        {
            Ipnet_pkt_ip ipv4_hdr;

            /* Unknown proto */
            ipcom_memcpy(&ipv4_hdr, ip_pkt, sizeof(Ipnet_pkt_ip));
            ipnet_icmp4_param_init(&icmp_param, pkt, &ip4_param);
            ret = ipnet_ip4_deliver_to_raw_sock(pkt, &ip4_param, IP_TRUE);
            pkt = IP_NULL;

            if (ret == -IP_ERRNO_EPROTONOSUPPORT
                && (IP_IN_CLASSA(dst.s_addr)
                    || IP_IN_CLASSB(dst.s_addr)
                    || IP_IN_CLASSC(dst.s_addr)))
            {
                /* No protocol handler for this protcol */
                IPCOM_MIB2(ipInUnknownProtos++);
                IPCOM_MIB2_SYSWI_U32_ADD(v4, ipSystemStatsInUnknownProtos, 1);
                IPCOM_MIB2_PERIF_U32_ADD(v4, ipIfStatsInUnknownProtos, 1, netif);
                icmp_param.type     = IPNET_ICMP4_TYPE_DST_UNREACHABLE;
                icmp_param.code     = IPNET_ICMP4_CODE_DST_UNREACH_PROTO;
                pkt = ipcom_pkt_malloc(sizeof(sizeof(Ipnet_pkt_ip)),
                                       IP_FLAG_FC_STACKCONTEXT);
                if (pkt != IP_NULL)
                {
                    pkt->end     = pkt->maxlen;
                    pkt->start   = pkt->end - sizeof(Ipnet_pkt_ip);
                    pkt->ipstart = pkt->start;
                    ipcom_memcpy(&pkt->data[pkt->ipstart], &ipv4_hdr, sizeof(ipv4_hdr));
                    icmp_param.recv_pkt = pkt;
                    (void) ipnet_icmp4_send(&icmp_param, IP_FALSE);
#ifdef IP_PORT_LKM
                    /* The LKM port will always free the icmp_param.recv_pkt */
                    pkt = IP_NULL;
#endif
                }
            }
            else if (ret == 0)
            {
                IPCOM_MIB2(ipInDelivers++);
                IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInDelivers, 1);
                IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInDelivers, 1, netif);
            }
        }
        break;
    }

 cleanup:
    if (ret < 0)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 45, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip4_input, IPCOM_WV_NETD_BADBUF,
                          IPCOM_WV_IPNET_IP4_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(ip4_input_err++);
    }
    if (pkt != IP_NULL)
    {
        ip_assert(pkt->data != IP_NULL);
        ipcom_pkt_free(pkt);
    }
    return ret < 0 ? ret : 0;
}


/*
 *===========================================================================
 *                    ipnet_ip4_get_mss
 *===========================================================================
 * Description: Returns the MSS for the connected socket 'sock'.
 * Parameters:  sock - The TCP socket to calculate MSS for.
 * Returns:     The MSS.
 *
 */
#ifdef IPTCP
IP_GLOBAL Ip_u32
ipnet_ip4_get_mss(Ipnet_socket *sock)
{
    Ipnet_route_entry *rt;
    int                ret;
    Ip_u32             mtu;
    Ip_u32             mss;
    Ipnet_route_policy_selector rps;

    /* In case they did not provide an RPS, we assign an empty one. */
    rps.fields = 0;

    /* Verify */
    rps.fields |= IPNET_PR_RULE_DADDR|IPNET_PR_RULE_SADDR|IPNET_PR_RULE_PROTO;
    rps.daddr  = &sock->ip4->daddr_n;
    rps.proto  = IP_IPPROTO_TCP;

    /* Determine source address to use in policy lookup */
    if (IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_BOUND))
        rps.saddr = &sock->ip4->saddr_n;
    else
        rps.saddr  = &ip_inaddr_any;

    ret = ipnet_route_lookup_ecmp(IP_AF_INET,
                                  sock->vr_index,
                                  0,
                                  &sock->ip4->daddr_n,
                                  0,
                                  0,
                                  &rps,
                                  &rt,
                                  IP_NULL);

    if (ret < 0)
        mtu = ipnet_conf_ip4_min_mtu;
    else
        mtu = (rt->metrics.rmx_mtu == 0)
            ? rt->netif->ipcom.mtu
            : IP_MIN(rt->netif->ipcom.mtu, rt->metrics.rmx_mtu);

#ifdef IPIPSEC2
    {
        Ipipsec_param    ipsec_param;

        ipsec_param.key.src.in.s_addr = sock->ip4->saddr_n;
        ipsec_param.key.dst.in.s_addr = sock->ip4->daddr_n;
        ipsec_param.key.proto  = sock->proto;
        ipsec_param.key.domain = IP_AF_INET;
        ipsec_param.key.ports.udptcp_srcdst_n[0] = ip_htons(sock->sport);
        ipsec_param.key.ports.udptcp_srcdst_n[1] = ip_htons(sock->dport);

        mtu -= ipipsec_output_hdrspace(&ipsec_param);
    }
#endif /* IPIPSEC2 */

    mss = mtu - sock->max_hdrspace;
    return IP_MIN((Ip_u32) sock->recv_max_bytes, mss);
}
#endif /* IPTCP */


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */
#else
int ipnet_ip4_empty_file;
#endif /* IPCOM_USE_INET */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
