/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ip6.c,v $ $Revision: 1.626.14.3 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ip6.c,v $
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
#ifdef IPCOM_USE_INET6

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_clib.h>
#include <ipcom_sysvar.h>
#include <ipcom_inet.h>
#include <ipcom_syslog.h>

#include "ipnet_h.h"
#include "ipnet.h"
#include "ipnet_loopback.h"
#include "ipnet_icmp6.h"

#ifdef IPNET_USE_RFC3971
#include "ipnet_send.h"
#endif

#ifdef IPNET_USE_TUNNEL
#include "ipnet_tunnel.h"
#endif

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPSCTP
#include <ipsctp.h>
#endif
#ifdef IPMPLS
#include "ipnet_mpls.h"
#endif

#ifdef IPNET_USE_ROUTESOCK
#include "ipnet_routesock.h"
#endif

#ifdef IPNET_USE_NETLINK
#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"
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

#define IPNET_IP6_ADDR_SOLICITED_NODE_CMP(addr1, addr2) \
    ((IP_GET_32ON16(&(addr1)->in6.addr32[3]) & ip_htonl(0x00ffffff)) == \
     (IP_GET_32ON16(&(addr2)->in6.addr32[3]) & ip_htonl(0x00ffffff)))


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Search key for IPv6 addresses */
typedef struct Ipnet_ip6_addr_lookup_struct
{
    Ip_u32                       ifindex;
    Ip_u16                       vr_index;
    IP_CONST struct Ip_in6_addr *addr;
}
Ipnet_ip6_addr_lookup;


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

IP_STATIC void ipnet_ip6_addr_timeout(Ipnet_ip6_addr_entry *addr);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* Conversion from enum Ipnet_ip6_scope to string */
static char *ipnet_ip6_scope_str[16] = {
    IP_NULL,  /* 0 */
    "node",   /* 1 */
    "link",   /* 2 */
    "subnet", /* 3 */
    "admin",  /* 4 */
    "site",   /* 5 */
    IP_NULL,  /* 6 */
    IP_NULL,  /* 7 */
    "org",    /* 8 */
    IP_NULL,  /* 9 */
    IP_NULL,  /* a */
    IP_NULL,  /* b */
    IP_NULL,  /* c */
    IP_NULL,  /* d */
    "global", /* e */
    IP_NULL   /* f */
};


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                      ipnet_ip6_addr_key_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip6_addr_key_func(Ipnet_ip6_addr_lookup *key)
{
    unsigned v;
    v = ipcom_hash_update(&key->ifindex, sizeof(key->ifindex), key->vr_index);
    return ipcom_hash_update(key->addr, sizeof(*key->addr), v);
}


/*
 *===========================================================================
 *                      ipnet_ip6_addr_obj_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip6_addr_obj_func(Ipnet_ip6_addr_entry *addr)
{
    unsigned v;
    v = ipcom_hash_update(&addr->netif->ipcom.ifindex, sizeof(addr->netif->ipcom.ifindex), addr->netif->vr_index);
    return ipcom_hash_update(&addr->addr, sizeof(addr->addr), v);
}


/*
 *===========================================================================
 *                      ipnet_ip6_addr_cmp_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_ip6_addr_cmp_func(Ipnet_ip6_addr_entry *addr, Ipnet_ip6_addr_lookup *key)
{
    return addr->netif->ipcom.ifindex == key->ifindex
        && addr->netif->vr_index == key->vr_index
        && IP_IN6_ARE_ADDR_EQUAL(&addr->addr, key->addr);
}


/*
 *===========================================================================
 *                    ipnet_ip6_addr_ignore_if_key_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip6_addr_ignore_if_key_func(Ipnet_ip6_addr_lookup *key)
{
    return ipcom_hash_update(key->addr, sizeof(*key->addr), key->vr_index);
}


/*
 *===========================================================================
 *                    ipnet_ip6_addr_ignore_if_obj_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ip6_addr_ignore_if_obj_func(Ipnet_ip6_addr_entry *addr)
{
    return ipcom_hash_update(&addr->addr, sizeof(addr->addr), addr->netif->vr_index);
}


/*
 *===========================================================================
 *                    ipnet_ip6_addr_ignore_if_cmp_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_ip6_addr_ignore_if_cmp_func(Ipnet_ip6_addr_entry *addr, Ipnet_ip6_addr_lookup *key)
{
    return addr->netif->vr_index == key->vr_index
        && IP_IN6_ARE_ADDR_EQUAL(&addr->addr, key->addr);
}


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                     ipnet_ip6_initialize_mapped_ip4
 *===========================================================================
 * Description: Initializes the socket for IPv6 mapped IPv4 address operation.
 * Parameters:  sock - The socket that will be used in IPv6 mapped IPv4
 *                     address operation.
 *              to - The destination address.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_initialize_mapped_ip4(Ipnet_socket *sock, IP_CONST struct Ip_in6_addr *to)
{
    if (IP_IN6_IS_ADDR_V4MAPPED(to) == IP_FALSE
        || (IP_IN6_IS_ADDR_V4MAPPED(&sock->ip6->saddr) == IP_FALSE
            && IP_IN6_IS_ADDR_UNSPECIFIED(&sock->ip6->saddr) == IP_FALSE))
    {
        IPCOM_LOG2(DEBUG, "IPv6: Trying to use IPv6 mapped IPv4 addresses with wrong type of addresses %s %s",
                   ipcom_inet_ntop(IP_AF_INET6, &sock->ip6->saddr, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   ipcom_inet_ntop(IP_AF_INET6, &sock->ip6->daddr, ipnet->log_buf + IP_INET6_ADDRSTRLEN, sizeof(ipnet->log_buf)));
        return -IP_ERRNO_EINVAL;
    }

    ip_assert(sock->ip4 == IP_NULL);
    sock->ip4 = ipcom_calloc(1, sizeof(Ipnet_ip4_socket));
    if (sock->ip4 == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    ipcom_memcpy(&sock->ip4->saddr_n, &sock->ip6->saddr.in6.addr32[3], sizeof(Ip_u32));
    ipcom_memcpy(&sock->ip4->daddr_n, &sock->ip6->daddr.in6.addr32[3], sizeof(Ip_u32));
    return 0;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                     ipnet_ip6_set_addr_lifetime
 *===========================================================================
 * Description: Sets the lifetime on an address.
 * Parameters:  addr_entry - The address entry to set lifetime for.
 *              preferred_sec - The time when the address will start being
 *              deprecated or IPCOM_ADDR_INFINITE if it should never be deprecated.
 *              expires_sec - The time when the address expires and will be
 *              removed or IPCOM_ADDR_INFINITE if it should never be removed.
 *              is_delta_time - IP_TRUE if the time parameters are number of
 *              seconds or IP_FALSE if they are an absolute time.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_set_addr_lifetime(Ipnet_ip6_addr_entry *addr_entry,
                            Ip_u32 preferred_sec,
                            Ip_u32 expires_sec,
                            Ip_bool is_delta_time)
{
    Ip_u32 next_timeout_sec = 0;

    ipnet_timeout_cancel(addr_entry->tmo);

    if (is_delta_time)
    {
        if (preferred_sec != IPCOM_ADDR_INFINITE)
        {
            preferred_sec += IPNET_SECONDS_ABS;
            if (preferred_sec == IPCOM_ADDR_INFINITE)
                preferred_sec++;
        }
        if (expires_sec != IPCOM_ADDR_INFINITE)
        {
            expires_sec += IPNET_SECONDS_ABS;
            if (expires_sec == IPCOM_ADDR_INFINITE)
                expires_sec++;
        }
    }

    IP_BIT_CLR(addr_entry->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED);
    addr_entry->preferred_sec = preferred_sec;
    addr_entry->expires_sec   = expires_sec;

    if (preferred_sec != IPCOM_ADDR_INFINITE  && IPCOM_IS_GT(preferred_sec, IPNET_SECONDS_ABS))
        next_timeout_sec = preferred_sec;
    else
    {
        if (preferred_sec != IPCOM_ADDR_INFINITE)
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED);
        if (expires_sec != IPCOM_ADDR_INFINITE)
        {
            if (IPCOM_IS_GT(expires_sec, IPNET_SECONDS_ABS))
                next_timeout_sec = expires_sec;
            else
                next_timeout_sec = (IPNET_SECONDS_ABS ? IPNET_SECONDS_ABS : 1);
        }
    }

    if (next_timeout_sec)
        return ipnet_ip6_addr_set_timeout(addr_entry,
                                          next_timeout_sec - IPNET_SECONDS_ABS,
                                          IP_FALSE);
    return 0;
}


/*
 *===========================================================================
 *                  ipnet_ip6_create_subnet_anycast_addr
 *===========================================================================
 * Description: Creates the subnet anycast address that corresponds to the
 *              specific unicast address.
 * Parameters:  addr_entry - The uncast address entry to return the anycast
 *                           address for.
 *              subnet_anycast - Will contain the subnet anycast address.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_create_subnet_anycast_addr(Ipnet_ip6_addr_entry *addr_entry,
                                     struct Ip_in6_addr *subnet_anycast)
{
    struct Ip_in6_addr mask = IP_IN6ADDR_ANY_INIT;

    ip_assert(addr_entry->type == IPNET_ADDR_TYPE_UNICAST);
    IPNET_IP6_SET_ADDR(subnet_anycast, &addr_entry->addr);
    ipnet_route_create_mask(&mask, addr_entry->prefixlen);
    ipnet_route_apply_mask(subnet_anycast, &mask, sizeof(struct Ip_in6_addr) * 8);
}


/*
 *===========================================================================
 *                      ipnet_ip6_add_sol_mcast_addr
 *===========================================================================
 * Description: Adds the corresponding solicitated multicast address, if
 *              needed.
 * Parameters:  addr_entry - the address entry to check if a solicitated
 *                           multicast address must be added for.
 * Returns:     0 = success,
 *             <0 = error code
 *
 */
IP_STATIC int
ipnet_ip6_add_sol_mcast_addr(Ipnet_ip6_addr_entry *addr_entry)
{
    int ret = 0;
    Ipnet_netif *netif = addr_entry->netif;

    if (IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_SOL_MCAST)
        && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_MULTICAST))
    {
        struct Ip_in6_addr sol_addr;
        /*
         * Join the solicited-node multicast link address for this
         * address.
         */
        IPNET_IP6_SET_SOLICITED_ADDR(&sol_addr, &addr_entry->addr);
        ret = ipnet_ip6_add_addr(netif, &sol_addr, 0, IPCOM_ADDR_INFINITE, IPCOM_ADDR_INFINITE, 16);
        if (ret >= 0)
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_SOL_MCAST);
    }
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_assign_addr
 *===========================================================================
 * Description: Adds the address to the list of assigned addresses and updates
 *              the routing table to route packet from this node to the new
 *              address via the loopback interface.
 * Parameters:  addr_entry - The address entry to add.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_assign_addr(Ipnet_ip6_addr_entry *addr_entry)
{
    struct Ipnet_route_add_param param;
    struct Ipnet_ipv6_key        key;
    struct Ipnet_ipv6_key        mask;
    Ipnet_netif                 *netif = addr_entry->netif;
    int                          ret = 0;
    struct Ip_sockaddr_in6       gw6;

    ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));

    ipcom_memset(&mask, 0 , sizeof(mask));
    ipnet_route_create_mask(&mask.addr, addr_entry->prefixlen);

    /* Invalidate the route cache tag since assigning this address
       might have affects on the outcome of lookups */
    ipnet_route_cache_invalidate();

    /* PPP interfaces should not have the network added automatically,
       but tunnel interfaces (which is also a kind of point to point) should */
    if (IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_DONT_ADD_NETWORK_RT)
        && IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_NETWORK_RT))
    {
        /* Add network route for this address */
        IPNET_IP6_SET_ADDR(&key.addr, &addr_entry->addr);
        key.scope_id = ipnet_ip6_get_zone(netif->ipcom.ifindex,
                                          &addr_entry->addr);

        if (ipnet_ip6_is_scoped_addr(&addr_entry->addr))
            mask.scope_id = 0xffffffff;
        ipnet_route_apply_mask(&key, &mask, sizeof(struct Ipnet_ipv6_key) * 8);

        /* Add the network route for this address */
        param.domain  = IP_AF_INET6;
        param.vr      = netif->vr_index;
        param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
        param.netif   = netif;
        param.key     = &key;
        param.netmask = &mask;
        param.flags   = IPNET_RTF_UP | IPNET_RTF_DONE | IPNET_RTF_MASK;
        if (addr_entry->type == IPNET_ADDR_TYPE_UNICAST)
            /* The gateway field will is the default source address
               for packets sent to the network for this address */
            param.gateway = ipnet_ip6_create_sockaddr(&gw6,
                                                      &addr_entry->addr,
                                                      netif->ipcom.ifindex);

        if (IP_IN6_IS_ADDR_MULTICAST(&addr_entry->addr))
            /* Hide multicast routes for GateD to work (since they have no gateway) */
            param.flags |= IPNET_RTF_X_HIDDEN;
        else
            param.flags |= IPNET_RTF_CLONING;
        ret = ipnet_route_add(&param);
        if (ret >= 0)
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_NETWORK_RT);
    }

    if (IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_LOOPBACK_RT)
        && ret >= 0
        && addr_entry->type == IPNET_ADDR_TYPE_UNICAST)
    {
        /* Add route to loopback packets sent from this node to
           this unicast address */
        IPNET_IP6_SET_ADDR(&key.addr, &addr_entry->addr);
        key.scope_id = ipnet_ip6_get_zone(netif->ipcom.ifindex,
                                          &addr_entry->addr);

        ipnet_route_create_mask(&mask.addr, 128);
        if (ipnet_ip6_is_scoped_addr(&addr_entry->addr))
            mask.scope_id = 0xffffffff;

        ipnet_route_apply_mask(&key, &mask, sizeof(struct Ipnet_ipv6_key) * 8);

        ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
        if (netif->ipcom.type == IP_IFT_LOOP)
            param.netif = netif;
        else
            param.netif = ipnet_loopback_get_netif(netif->vr_index);
        param.domain  = IP_AF_INET6;
        param.vr      = netif->vr_index;
        param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
        param.key     = &key;
        param.flags   = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DONE;
        param.gateway = ipnet_ip6_create_sockaddr(&gw6,
                                                  &addr_entry->addr,
                                                  netif->ipcom.ifindex);
        ret = ipnet_route_add(&param);
        if (ret >= 0)
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_LOOPBACK_RT);
    }

    if (ret >= 0
        && addr_entry->type == IPNET_ADDR_TYPE_UNICAST
        && IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT)
        && IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD)
        && IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE)
        && IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_SUBNET_ANYCAST)
        && netif->ipcom.link_addr_size > 0)
    {
        struct Ip_in6_addr subnet_anycast;

        ipnet_ip6_create_subnet_anycast_addr(addr_entry, &subnet_anycast);
        ret = ipnet_ip6_add_addr(netif,
                                 &subnet_anycast,
                                 IPNET_IP6_ADDR_FLAG_DONT_ADD_NETWORK_RT | IPNET_IP6_ADDR_FLAG_ANYCAST,
                                 IPCOM_ADDR_INFINITE,
                                 IPCOM_ADDR_INFINITE,
                                 addr_entry->prefixlen);
        if (ret >= 0)
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_SUBNET_ANYCAST);
    }

    if (IP_BIT_ISFALSE(addr_entry->flags,
                       IPNET_IP6_ADDR_FLAG_NEWADDR_DONE
                       | IPNET_IP6_ADDR_FLAG_TENTATIVE))
    {
        /* Notify all listener that this address is now ready to be used */
        IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_NEWADDR_DONE);
        ipnet_kioevent(netif, IP_EIOXNEWADDR, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        IPNET_ROUTESOCK(ipnet_routesock_addr_add(netif, IP_AF_INET6, addr_entry));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_ip6_addr_add(netif,addr_entry));
    }

    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_RUNNING))
    {
        if (ret >= 0 && IP_BIT_ISSET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
        {
            /* Start DAD */
            ipnet_timeout_cancel(addr_entry->tmo);

            IPCOM_LOG2(INFO, "IPv6: performing DAD for %s at %s",
                       ipcom_inet_ntop(IP_AF_INET6, &addr_entry->addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       netif->ipcom.name);
            addr_entry->dad_count = 0;
            /* Initiate DAD probes */

            /*
             * RFC 4862 chapter 5.4.2
             *
             * Even if the Neighbor Solicitation is not going to be
             * the first message sent, the node SHOULD delay joining
             * the solicited-node multicast address by a random delay
             * between 0 and MAX_RTR_SOLICITATION_DELAY if the address
             * being checked is configured by a router advertisement
             * message sent to a multicast address.  The delay will
             * avoid similar congestion when multiple nodes are going
             * to configure addresses by receiving the same single
             * multicast router advertisement.
             */
            if (IP_FALSE && IP_BIT_ISSET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS))
                /* Address configured by RA */
                ipnet_ip6_addr_set_timeout(addr_entry,
                                           ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                                         netif,
                                                                         "MaxRtrSolicitationDelay",
                                                                         1),
                                           IP_TRUE);
            else
                ipnet_ip6_addr_set_timeout(addr_entry, 0, IP_FALSE);

            return 0;
        }

        if (ret >= 0)
        {
            if (addr_entry->type == IPNET_ADDR_TYPE_MULTICAST)
            {
                addr_entry->mcast.filter_change_resend_count = (Ip_u8) netif->mld_robustness_variable;
                ipnet_mld_report_filter_change(addr_entry);
            }
            else
                /* This is a unicast or anycast address, lets join the
                   corresponding solicited multicast address so
                   neighbor solicitation is received for this
                   address */
                ret = ipnet_ip6_add_sol_mcast_addr(addr_entry);
        }
    }


    if (ret >= 0 && addr_entry->tmo == IP_NULL)
        /* Reinitialize address timeouts */
        ret = ipnet_ip6_set_addr_lifetime(addr_entry,
                                          addr_entry->preferred_sec,
                                          addr_entry->expires_sec,
                                          IP_FALSE);

    if (ret < 0)
    {
        /* Failed to assign address */
        IPCOM_LOG3(NOTICE,
                   "IPv6: Failed to assign address %s to interface %s, code=%d",
                   ipcom_inet_ntop(IP_AF_INET6, &addr_entry->addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   netif->ipcom.name,
                   -ret);
        (void)ipnet_ip6_remove_addr(netif, &addr_entry->addr);
        return ret;
    }

    if (addr_entry->type == IPNET_ADDR_TYPE_UNICAST
        && IP_IN6_IS_ADDR_LINK_LOCAL(&addr_entry->addr)
        && IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE)
        && IP_BIT_ISSET(netif->flags2, IPNET_IFF2_ADV_ACCEPT))
    {
        ipnet_icmp6_send_router_sol_msg(netif);
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_addr_timeout
 *===========================================================================
 * Description: Removes addresses that has passed their expired timestamp.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_addr_timeout(Ipnet_ip6_addr_entry *addr)
{
    Ipnet_netif *netif = addr->netif;

    if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE)
        && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP)
        && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_RUNNING))
    {
        (void)ipnet_ip6_add_sol_mcast_addr(addr);

        if (IP_BIT_ISSET(netif->flags2, IPNET_IFF2_FIRST_NS))
        {
            /*
             * RFC 2462, chapter 5.4.2
             *
             * If the Neighbor Solicitation is the first message to be
             * sent from an interface after interface
             * (re)initialization, the node should delay sending the
             * message by a random delay between 0 and
             * MAX_RTR_SOLICITATION_DELAY as specified in [DISCOVERY].
             */
            IP_BIT_CLR(netif->flags2, IPNET_IFF2_FIRST_NS);
            ipnet_ip6_addr_set_timeout(addr,
                                       ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                                     netif,
                                                                     "MaxRtrSolicitationDelay",
                                                                     1),
                                       IP_TRUE);
            return;
        }

        if (addr->dad_count++ < ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                              netif,
                                                              "DupAddrDetectTransmits",
                                                              1))
            (void)ipnet_icmp6_send_dad(addr);
        else
        {
#ifdef IPNET_USE_RFC4941
            if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY))
            {
                Ipnet_route_entry *prefix;

                prefix = ipnet_icmp6_prefix_get(netif,
                                                &addr->addr,
                                                addr->prefixlen);
                if (prefix)
                    prefix->temp_idgen_retries = 0;
            }
#endif /* IPNET_USE_RFC4941 */

            /* The address has not been removed during duplicate address
               detection, assign the address to the interface */
            IPCOM_LOG2(INFO, "IPv6: DAD successful for %s on %s",
                       ipcom_inet_ntop(IP_AF_INET6, &addr->addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       netif->ipcom.name);
            IP_BIT_CLR(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE);
            /* Must invalidate the caches since the source address selection can now change */
            ipnet_route_cache_invalidate();
            (void)ipnet_ip6_assign_addr(addr);
        }
    }
    else
    {
        if (addr->expires_sec != IPCOM_ADDR_INFINITE
            && IPCOM_IS_GEQ(IPNET_SECONDS_ABS, addr->expires_sec))
            /* Address has expired */
            (void)ipnet_ip6_remove_addr(netif, &addr->addr);
        else
            (void)ipnet_ip6_set_addr_lifetime(addr,
                                              addr->preferred_sec,
                                              addr->expires_sec,
                                              IP_FALSE);
    }
}


/*
 *===========================================================================
 *                    ipnet_ip6_addr_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_ip6_addr_to_string(Ipnet_ip6_addr_entry *addr, char *buf, int buf_len)
{
    int offset;

    offset = ipcom_snprintf(buf,
                            buf_len,
                            "IPv6 addr %s  preferred ",
                            ipcom_inet_ntop(IP_AF_INET6, &addr->addr, ipnet->log_buf, sizeof(ipnet->log_buf)));

    if (addr->preferred_sec == IPCOM_ADDR_INFINITE)
        offset += ipcom_snprintf(&buf[offset], buf_len - offset, "INFINITE");
    else if (addr->preferred_sec < IPNET_SECONDS_ABS)
        offset += ipcom_snprintf(&buf[offset], buf_len - offset, "NO");
    else
        offset += ipcom_snprintf(&buf[offset], buf_len - offset, "%u s",
                                 (unsigned) (addr->preferred_sec - IPNET_SECONDS_ABS));

    offset += ipcom_snprintf(&buf[offset], buf_len - offset, "  expires ");

    if (addr->expires_sec == IPCOM_ADDR_INFINITE)
        offset += ipcom_snprintf(&buf[offset], buf_len - offset, "INFINITE");
    else
        offset += ipcom_snprintf(&buf[offset], buf_len - offset, "%u s",
                                 (unsigned) (addr->expires_sec - IPNET_SECONDS_ABS));

    offset += ipcom_snprintf(&buf[offset],
                             buf_len - offset,
                             "  %s%s",
                             IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE) ? "TENTATIVE " : "",
                             IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY) ? "TEMPORARY " : "");
    return offset;
}
#endif /* IPNET_DEBUG */


/*
 *===========================================================================
 *                    ipnet_ip6_add_fragmentation_hdr
 *===========================================================================
 * Description: Adds a fragmentation header to all packets in the fragmentation
 *              list.
 * Parameters:  fragment_list - The first fragment in the list.
 *              next_header - The orginal next header value, will contain the value
 *              to use on the next header.
 * Returns:     The header value to use in the next header.
 */
IP_STATIC void
ipnet_ip6_add_fragmentation_hdr(Ipcom_pkt *fragment_list, Ip_u8 *next_hdr)
{
    Ipcom_pkt       *frag_pkt;
    Ip_pkt_ip6_frag *frg;
    int              frag_id;

    while (0 == (frag_id = ipnet->ip6.id++))
        /* Do nothing */;

    for (frag_pkt = fragment_list; frag_pkt != IP_NULL; frag_pkt = frag_pkt->next_fragment)
    {
        ip_assert(frag_pkt->start > (int) IP_IP6F_SIZE);
        frag_pkt->start -= IP_IP6F_SIZE;
        frg = (Ip_pkt_ip6_frag *) &frag_pkt->data[frag_pkt->start];
        frg->ip6f_nxt      = *next_hdr;
        frg->ip6f_reserved = 0;
        frg->ip6f_offlg    = (Ip_u16) ip_htons(frag_pkt->offset);
        if (frag_pkt->next_fragment != IP_NULL)
            IP_BIT_SET(frg->ip6f_offlg, IP_IP6F_MORE_FRAG);
        IP_SET_32ON16(&frg->ip6f_ident, frag_id);
    }
    *next_hdr = IP_IPPROTO_FRAGMENT;
}


/*
 *===========================================================================
 *                    ipnet_ip6_is_part_of_same_pkt
 *===========================================================================
 * Description: Returns if the two fragments are parts in the same packet.
 * Parameters:  frag1 - A IPv6 fragment.
 *              frag2 - Another IPv6 fragment.
 * Returns:     IP_TRUE or IP_FALSE.
 *
 */
IP_GLOBAL Ip_bool
ipnet_ip6_is_part_of_same_pkt(Ipcom_pkt *frag1, Ipcom_pkt *frag2)
{
    Ipnet_pkt_ip6   *ip6_hdr1;
    Ipnet_pkt_ip6   *ip6_hdr2;
    Ip_pkt_ip6_frag *frag_hdr1;
    Ip_pkt_ip6_frag *frag_hdr2;

    frag_hdr1 = (Ip_pkt_ip6_frag *) &frag1->data[frag1->start];
    frag_hdr2 = (Ip_pkt_ip6_frag *) &frag2->data[frag2->start];

    if (IP_GET_32ON16(&frag_hdr1->ip6f_ident) != IP_GET_32ON16(&frag_hdr2->ip6f_ident))
        return IP_FALSE;

    ip6_hdr1 = (Ipnet_pkt_ip6 *) &frag1->data[frag1->ipstart];
    ip6_hdr2 = (Ipnet_pkt_ip6 *) &frag2->data[frag2->ipstart];

    return IP_IN6_ARE_ADDR_EQUAL(&ip6_hdr1->src, &ip6_hdr2->src)
        && IP_IN6_ARE_ADDR_EQUAL(&ip6_hdr1->dst, &ip6_hdr2->dst);
}


/*
 *===========================================================================
 *                    ipnet_ip6_get_offset
 *===========================================================================
 * Description: Returns the offset for this segment into the IP packet.
 * Parameters:  frag - A IPv6 fragment.
 * Returns:     The offset.
 *
 */
IP_STATIC int
ipnet_ip6_get_offset(Ipcom_pkt *frag)
{
    return IP_IP6F_GET_OFFSET((Ip_pkt_ip6_frag *) &frag->data[frag->start]);
}


/*
 *===========================================================================
 *                    ipnet_ip6_more_fragments
 *===========================================================================
 * Description: Returns if this is the last fragment or not.
 * Parameters:  frag - A IPv6 fragment.
 * Returns:     IP_TRUE or IP_FALSE.
 *
 */
IP_STATIC Ip_bool
ipnet_ip6_more_fragments(Ipcom_pkt *frag)
{
    Ip_pkt_ip6_frag *frag_hdr;

    frag_hdr = (Ip_pkt_ip6_frag *) &frag->data[frag->start];
    return IP_BIT_ISSET(frag_hdr->ip6f_offlg, IP_IP6F_MORE_FRAG);
}


/*
 *===========================================================================
 *                    ipnet_ip6_update_ip_header
 *===========================================================================
 * Description: Sets the next header and plen field in the IPv6 header for
 *              the reassembled packet.
 * Parameters:  pkt - The reassembled IPv6 packet.
 *              frag_list - The individual fragments that was received.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_update_ip_header(Ipcom_pkt *pkt, Ipcom_pkt *frag_list)
{
    Ipnet_pkt_ip6   *ip6_hdr;
    Ip_pkt_ip6_frag *frag_hdr;
    Ip_u8            next_header;

    frag_hdr = (Ip_pkt_ip6_frag *) &frag_list->data[frag_list->start - IP_IP6F_SIZE];
    next_header = frag_hdr->ip6f_nxt;

    ip6_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];
    ip6_hdr->nxt = next_header;
    ip6_hdr->plen = (Ip_u16) ip_htons(pkt->end - pkt->start - IPNET_IP6_HDR_SIZE);
}


/*
 *===========================================================================
 *                    ipnet_ip6_fragment_timeout
 *===========================================================================
 * Description: Called if reassembly of the packet times out.
 * Parameters:  frag_head - The first packet in the list of fragments.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_fragment_timeout(Ipcom_pkt *frag_head)
{
    Ipnet_icmp6_param icmp6_param;
    Ip_pkt_ip6_frag  *frag_hdr;
    Ipnet_pkt_ip6    *ip_hdr;
    Ipcom_pkt        *f;
#ifdef IPCOM_USE_MIB2
    Ipnet_netif *netif = ipnet_if_indextonetif(frag_head->vr_index, frag_head->ifindex);

    if (netif != IP_NULL)
    {
        IPCOM_MIB2_NETIF(ipv6IfStatsReasmFails++);
        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsReasmFails, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsReasmFails, 1, netif);
    }
#endif /* IPCOM_USE_MIB2 */

    /* Unhook the fragment list from the reassembly list */
    ip_assert(ipnet->ip6.reassembly_list == frag_head);
    ipnet->ip6.reassembly_list = frag_head->next;

    ip_hdr = (Ipnet_pkt_ip6 *) &frag_head->data[frag_head->ipstart];
    frag_hdr = (Ip_pkt_ip6_frag *) &frag_head->data[frag_head->start];
    if (IP_IN6_IS_ADDR_MULTICAST(&ip_hdr->dst) == IP_FALSE
        && (frag_hdr->ip6f_offlg & IP_IP6F_OFF_MASK) == 0)
    {
        /* First fragment was received, send Time Exceeded Message */
        ipnet_icmp6_param_init(&icmp6_param, frag_head);
        icmp6_param.type = IPNET_ICMP6_TIME_EXCEEDED;
        icmp6_param.code = IPNET_ICMP6_TIME_EXCEEDED_FRAG;
        (void) ipnet_icmp6_send(&icmp6_param);
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 1, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_ip6_fragment_timeout, IPCOM_WV_NETD_INFO_TIMEOUT,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_input_time_exceeded++);
    }

    ipcom_pkt_free(frag_head);

    /* Add a timeout for the next entry (if any) */
    f = ipnet->ip6.reassembly_list;
    if (f != IP_NULL)
    {
        (void)ipnet_timeout_schedule(IPNET_PKT_GET_TIMEOUT(f) * 1000,
                                     (Ipnet_timeout_handler) ipnet_ip6_fragment_timeout,
                                     f,
                                     IPNET_PKT_GET_TMO_PTR(f));
    }
}


/*
 *===========================================================================
 *                    ipnet_ip6_fragment_tmo_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_ip6_fragment_tmo_to_string(Ipcom_pkt *frag_head, char *buf, int buf_len)
{
    Ip_pkt_ip6_frag  *frag_hdr;
    Ipnet_pkt_ip6    *ip_hdr;
    char              dst[IP_INET6_ADDRSTRLEN];
    char              src[IP_INET6_ADDRSTRLEN];

    ip_hdr = (Ipnet_pkt_ip6 *) &frag_head->data[frag_head->ipstart];
    frag_hdr = (Ip_pkt_ip6_frag *) &frag_head->data[frag_head->start];
    return ipcom_snprintf(buf,
                          buf_len,
                          "IPv6 fragment id %u  src %s  dst %s",
                          (unsigned) ip_ntohl(frag_hdr->ip6f_ident),
                          ipcom_inet_ntop(IP_AF_INET6, &ip_hdr->src, src, sizeof(src)),
                          ipcom_inet_ntop(IP_AF_INET6, &ip_hdr->dst, dst, sizeof(dst)));
}
#endif /* IPNET_DEBUG */

/*
 *===========================================================================
 *                    ipnet_ip6_reassembly_err_hnd
 *===========================================================================
 * Description: Sends a parameter problem since the fragments could not
 *              be reassebled since they exceed the maximum MTU.
 * Parameters:  frag_list - The individual fragments that was received.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ip6_reassembly_err_hnd(Ipcom_pkt *frag_list)
{
    Ipnet_icmp6_param icmp6_param;
    Ipcom_pkt        *pkt = frag_list;

    while (pkt->next_fragment)
        pkt = pkt->next_fragment;

    ipnet_icmp6_param_init(&icmp6_param, pkt);
    icmp6_param.type = IPNET_ICMP6_PARAM_PROB;
    icmp6_param.code = IPNET_ICMP6_PARAM_PROB_ERR_HDR;
    icmp6_param.data.param_prob_pointer = pkt->start - pkt->ipstart
        - sizeof(struct Ip_ip6_frag) + ip_offsetof(struct Ip_ip6_frag, ip6f_offlg);
    (void)ipnet_icmp6_send(&icmp6_param);
}


/*
 *===========================================================================
 *                    ipnet_ip6_process_ext_options
 *===========================================================================
 * Description: Processes all options in a extension header.
 * Parameters:  extension_type - the type of the extension header containing
 *              the options
 *              first_opt - pointer to the first option
 *              opts_len - the total length of all options
 *              param - Parameters needed to process extension headers.
 * Returns:     IP_TRUE - continue processing the packet
 *              IP_FALSE - discard the packet
 */
IP_STATIC Ip_bool
ipnet_ip6_process_ext_options(int extension_type,
                              Ip_pkt_ip6_opt *first_opt,
                              int opts_len,
                              Ipnet_process_ext_param *param)
{
    Ip_pkt_ip6_opt      *opt;
    int                 opt_type;
    int                 action;
    int                 offset = 0;
    struct Ip_in6_addr  temp_addr;

    while (opts_len > offset)
    {
        opt = (Ip_pkt_ip6_opt *) ((Ip_u8 *) first_opt + offset);

        opt_type = opt->ip6o_type & 0x1F;
        action = opt->ip6o_type & 0xC0;

        if (opt_type != IP_IP6OPT_PAD1
            && opts_len < (int) (opt->ip6o_len + sizeof(Ip_pkt_ip6_opt)))
        {
            param->icmp6_type = IPNET_ICMP6_PARAM_PROB;
            param->icmp6_code = IPNET_ICMP6_PARAM_PROB_ERR_HDR;
            param->param_prob_start += offset + sizeof(Ip_pkt_ip6_ext_hdr);
            return IP_FALSE;
        }

        switch (opt_type)
        {
        case IP_IP6OPT_PAD1:
            offset += 1;
            continue;

        case IP_IP6OPT_PADN:
            break;

        case IP_IP6OPT_DSTOPT_HOME_ADDRESS:
            /* Only valid in the DSTOPT extension type */
            if (extension_type != IP_IPPROTO_DSTOPTS)
                goto unknown;

            /* Swap source address and the dst opt address */
            IPNET_IP6_SET_ADDR(&temp_addr, (opt + 1));
            IPNET_IP6_SET_ADDR((opt + 1), &param->ip6_hdr->src);
            IPNET_IP6_SET_ADDR(&param->ip6_hdr->src, &temp_addr);
            break;
        case IP_IP6OPT_ROUTER_ALERT:
            if (extension_type == IP_IPPROTO_HOPOPTS)
            {
                IP_BIT_SET((*param->ppkt)->flags, IPCOM_PKT_FLAG_ROUTER_ALERT);
                break;
            }
            /* else: fall through */
        default:
unknown:
            /* Unknown option, check the action */
            switch (action)
            {
            case IP_IP6OPT_TYPE_SKIP:
                break;

            case IP_IP6OPT_TYPE_DISCARD:
                return IP_FALSE;

            case IP_IP6OPT_TYPE_ICMP:
                if (IP_IN6_IS_ADDR_MULTICAST(&param->ip6_hdr->dst))
                    return IP_FALSE;
                /* Fall through */
            case IP_IP6OPT_TYPE_FORCEICMP:
                param->icmp6_type = IPNET_ICMP6_PARAM_PROB;
                param->icmp6_code = IPNET_ICMP6_PARAM_PROB_OPT;
                param->param_prob_start += offset + sizeof(Ip_pkt_ip6_ext_hdr);
                return IP_FALSE;
            default:
                break;
            }
        }

        offset += opt->ip6o_len + sizeof(Ip_pkt_ip6_opt);
    }

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_ip6_process_rthdr
 *===========================================================================
 * Description: Processes all the extension headers up to, but not including,
 *              any upper layer protocol.
 * Parameters:  param - Parameters needed to process extension headers.
 * Returns:     >0 Forward packet to next host in the list (IPv6 dst has been changed).
 *              <0 Send ICMP error message (param has been updated)
 *              0 Keep processing this packet (final destination reached).
 */
IP_STATIC int
ipnet_ip6_process_rthdr(Ip_pkt_ip6_rthdr        *rthdr,
                        Ipnet_process_ext_param *param)
{
    Ip_pkt_ip6_rthdr0  *rthdr0;
    struct Ip_in6_addr *rtaddr;
    struct Ip_in6_addr  temp_addr;
    int                 num_addresses;
    Ipcom_pkt          *pkt = *param->ppkt;
    Ip_bool             process_rthdr_type_0;

    /* Routing header type 0 is considered a possible security threat
       by RFC5095. It is up to the system administrator to determine
       if routing header type 0 should be processed or not. */
    process_rthdr_type_0 =
        ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6,
                                         ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex),
                                         "RtHdr0",
                                         IP_FALSE,
                                         ipnet_bool_map);

    if (rthdr->ip6r_type != IP_IPV6_RTHDR_TYPE_0
        || process_rthdr_type_0 == IP_FALSE)
    {
        /* Is this a mobile IPv6 routing header type 2 packet */
        if (rthdr->ip6r_type == IP_IPV6_RTHDR_TYPE_2)
        {
            Ip_pkt_ip6_rthdr2  *rthdr2 = (Ip_pkt_ip6_rthdr2 *) rthdr;

            /* Sanity. Must be length 2 & segments 1 */
            if (rthdr2->ip6r2_len  == 2 && rthdr2->ip6r2_segleft == 1)
            {
                IP_GLOBAL Ipnet_ip6_addr_entry  *addr = ipnet_ip6_get_addr_entry(&rthdr2->ip6r2_home,
                                                                                 pkt->vr_index,
                                                                                 IP_NULL);
                /* Is it a home address? */
                if (addr != IP_NULL && IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                {
                    /* Update */
                    param->ip6_dst_entry    = addr;

                    /* Do some processing. */
                    --rthdr2->ip6r2_segleft;

                    /* Swap destination address and the routing header address */
                    IPNET_IP6_SET_ADDR(&temp_addr, &rthdr2->ip6r2_home);
                    IPNET_IP6_SET_ADDR(&rthdr2->ip6r2_home, &param->ip6_hdr->dst);
                    IPNET_IP6_SET_ADDR(&param->ip6_hdr->dst, &temp_addr);

                    /* Ok, go next */
                    return 0;
                }
            }
        }

        /* Unsupported routing header
           RFC 2460, chapter 4.4 Routing header
           ...
           If, while processing a received packet, a node encounters a Routing
           header with an unrecognized Routing Type value, the required behavior
           of the node depends on the value of the Segments Left field, as
           follows:

             If Segments Left is zero, the node must ignore the Routing header
             and proceed to process the next header in the packet, whose type
             is identified by the Next Header field in the Routing header.

             If Segments Left is non-zero, the node must discard the packet and
             send an ICMP Parameter Problem, Code 0, message to the packet's
             Source Address, pointing to the unrecognized Routing Type.
           ...
        */
        if (rthdr->ip6r_segleft == 0)
            return 0;

        param->icmp6_type = IPNET_ICMP6_PARAM_PROB;
        param->icmp6_code = IPNET_ICMP6_PARAM_PROB_ERR_HDR;
        param->param_prob_start += ip_offsetof(Ip_pkt_ip6_rthdr, ip6r_type);
        return -1;
    }

    /* Route header type 0 */
    if (rthdr->ip6r_segleft == 0)
        /* Final destination reached, process next header */
        return 0;

    rthdr0 = (Ip_pkt_ip6_rthdr0 *) rthdr;
    if (rthdr0->ip6r0_len & 0x1)
    {
        /* Invalid length of routing header (set error pointer to length field) */
        param->icmp6_type = IPNET_ICMP6_PARAM_PROB;
        param->icmp6_code = IPNET_ICMP6_PARAM_PROB_ERR_HDR;
        param->param_prob_start += ip_offsetof(Ip_pkt_ip6_rthdr0, ip6r0_len);
        return -1;
    }

    num_addresses = rthdr0->ip6r0_len / 2;
    if (rthdr0->ip6r0_segleft > num_addresses)
    {
        /* Segments left cannot be greater total number of addresses (set error pointer to segleft field) */
        param->icmp6_type = IPNET_ICMP6_PARAM_PROB;
        param->icmp6_code = IPNET_ICMP6_PARAM_PROB_ERR_HDR;
        param->param_prob_start += ip_offsetof(Ip_pkt_ip6_rthdr0, ip6r0_segleft);
        return -1;
    }

    rtaddr = (struct Ip_in6_addr *) (rthdr0 + 1);
    rtaddr = &rtaddr[num_addresses - rthdr0->ip6r0_segleft];

    if (IP_IN6_IS_ADDR_MULTICAST(rtaddr)
        || IP_IN6_IS_ADDR_MULTICAST(&param->ip6_hdr->dst))
        /* RFC 2460, chapter 4.3
           ...
           Multicast addresses must not appear in a Routing header of Type 0, or
           in the IPv6 Destination Address field of a packet carrying a Routing
           header of Type 0.
           ...
        */
        return -1;

    if (ipnet_ip6_is_link_local_addr(rtaddr)
        && !ipnet_ip6_is_link_local_addr(&param->ip6_hdr->dst))
    {
        /* RFC 4007, chapter 9
         ...
         A node that receives a packet addressed to itself and
         containing a Routing Header with more than zero Segments Left
         (Section 4.4 of [3]) first checks the scope of the next
         address in the Routing Header.  If the scope of the next
         address is smaller than the scope of the original destination
         address, the node MUST discard the packet.
         ...
         Multicast addresses is discared above so the only thing that
         needs to be checked is if the scopes goes from link-local to
         global.
        */
        return -1;
    }

    --rthdr0->ip6r0_segleft;
    /* Swap destination address and the routing header address */
    IPNET_IP6_SET_ADDR(&temp_addr, rtaddr);
    IPNET_IP6_SET_ADDR(rtaddr, &param->ip6_hdr->dst);
    IPNET_IP6_SET_ADDR(&param->ip6_hdr->dst, &temp_addr);
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_ip6_exthdr_find_option
 *===========================================================================
 * Description: Locates a given option in an extension header-
 * Parameters:  type - The option type
 *              first_opt - pointer to the first option
 *              opts_len - the total length of all options
 * Returns:     The option if it exists, or IP_NULL if it doesn't.
 */
IP_STATIC Ip_pkt_ip6_opt *
ipnet_ip6_exthdr_find_option(int type,
                             Ip_pkt_ip6_opt *first_opt,
                             int opts_len)
{
    Ip_pkt_ip6_opt *opt;
    int             opt_type;
    int             offset = 0;

    while (opts_len > offset)
    {
        opt = (Ip_pkt_ip6_opt *) ((Ip_u8 *) first_opt + offset);

        opt_type = opt->ip6o_type & 0x1F;

        if (opt_type != IP_IP6OPT_PAD1
            && opts_len < (int) (opt->ip6o_len + sizeof(Ip_pkt_ip6_opt)))
        {
            /* Error */
            return IP_NULL;
        }

        switch (opt_type)
        {
        case IP_IP6OPT_PAD1:
            offset += 1;
            continue;

        case IP_IP6OPT_PADN:
            break;

        default:
            /* Is it the one we're looking for? */
            if (opt_type == type)
                return opt;
            break;
        }

        /* Try next */
        offset += opt->ip6o_len + sizeof(Ip_pkt_ip6_opt);
    }

    /* Not found */
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip6_apply_exthdrs_dstopt
 *===========================================================================
 * Description: Go through the dstopts, locate any home addresses, verify
 *              them and assign them as 'real from'
 * Parameters:  dstopt      - Destination options
 *              param       - Output parameters.
 *              vr          - The virtual router
 * Returns:     IP_TRUE if dst opt was found.
 *
 */
IP_STATIC Ip_bool
ipnet_ip6_apply_exthdrs_dstopt(Ip_pkt_ip6_dest          *dstopt,
                               Ipnet_ip6_output_param   *param,
                               Ip_u16                   vr)
{
    (void)vr;
    /* Locate any Home Address Destination Options */
    if (param != IP_NULL)
    {
        Ip_pkt_ip6_opt *opt;

        /* Try locate it in the payload */
        opt = ipnet_ip6_exthdr_find_option(IP_IP6OPT_DSTOPT_HOME_ADDRESS,
                                           (Ip_pkt_ip6_opt *) (dstopt + 1),
                                           ((dstopt->ip6d_len + 1) << 3) - sizeof(Ip_pkt_ip6_opt));

        /* Found it? */
        if (opt != IP_NULL)
        {
            /* Sanity check the length of the option */
            if (opt->ip6o_len == sizeof(struct Ip_in6_addr))
            {
                /* Store the from value */
                param->real_from = (const struct Ip_in6_addr *) (opt + 1);

                /* Return */
                return IP_TRUE;
            }
        }
    }

    /* No option */
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_ip6_apply_exthdrs
 *===========================================================================
 * Description: Applies the correct extension header to the parameters for
 *              the outgoing packet.
 * Parameters:  [in] sock - The socket to use when sending.
 *              [in] msg - Message data (can be IP_NULL).
 *              [out] has_exthdr - Will contain IP_TRUE if at least 1 extension
 *              header is present
 *              [out] exthdrs - Will contain pointers to each extension header.
 *              [in,out] param - Output parameters. 'to' may be changed to the
 *              first hop in the routing header. The final destination is stored
 *              in the output param 'rthdr0_to' field.
 *              real_from may be changed to the value of an existing home
 *              option. Binding may be set to the corresponding binding.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_apply_exthdrs(Ipnet_socket *sock,
                        const struct Ip_msghdr *msg,
                        Ip_u8 *has_exthdr,
                        Ipnet_ip6_ext_hdrs *exthdrs,
                        Ipnet_ip6_output_param *param)
{
    struct Ip_cmsghdr   *cmsg;

    ipcom_memset(exthdrs, 0, sizeof(Ipnet_ip6_ext_hdrs));

    /* Use any extension header set as sticky by setsockopt() */
    exthdrs->hbh      = sock->ip6->exthdr_hbh;
    exthdrs->dst      = sock->ip6->exthdr_dst;
    exthdrs->rthdr0   = sock->ip6->exthdr_rthdr;
    exthdrs->rthdrdst = sock->ip6->exthdr_rthdrdst;
    exthdrs->rthdr2   = IP_NULL;

    if (msg != IP_NULL && msg->msg_controllen > 0)
    {
        /* Set extensions header specified to sendmsg() (this overrides any sticky options) */
        ip_assert(msg->msg_control != IP_NULL);
        for (cmsg = IP_CMSG_FIRSTHDR(msg);
             cmsg != IP_NULL;
             cmsg = IP_CMSG_NXTHDR(msg, cmsg))
        {
            if (cmsg->cmsg_level != IP_IPPROTO_IPV6)
                continue;

            switch (cmsg->cmsg_type)
            {
#ifdef IPNET_RFC2292_LEGACY_SUPPORT
            case IP_IPV6_2292HOPOPTS:
#endif
            case IP_IPV6_HOPOPTS:
                exthdrs->hbh = IP_CMSG_DATA(cmsg);
                break;
#ifdef IPNET_RFC2292_LEGACY_SUPPORT
            case IP_IPV6_2292DSTOPTS:
#endif
            case IP_IPV6_DSTOPTS:
                exthdrs->dst = IP_CMSG_DATA(cmsg);
                break;
#ifdef IPNET_RFC2292_LEGACY_SUPPORT
            case IP_IPV6_2292RTHDR:
#endif
            case IP_IPV6_RTHDR:
                {
                    Ip_pkt_ip6_rthdr    *rthdr = IP_CMSG_DATA(cmsg);

                    /* Verify that it's Type 0 */
                    if (rthdr->ip6r_type == IP_IPV6_RTHDR_TYPE_0)
                    {
                        /* Store RT Header type 0 */
                        exthdrs->rthdr0 = rthdr;
                    }
                    else if (rthdr->ip6r_type == IP_IPV6_RTHDR_TYPE_2)
                    {
                        /* Should do some sanity checking here */
                        /* Type 2 Routing Header */
                        exthdrs->rthdr2 = rthdr;
                    }
                    else
                    {
                        /* Unknown RT type */
                        /* Bad indata */
                        return -IP_ERRNO_EINVAL;
                    }
                }
                break;
            case IP_IPV6_RTHDRDSTOPTS:
                exthdrs->rthdrdst = IP_CMSG_DATA(cmsg);
                break;
            default:
                break;
            }
        }
    }

    if (exthdrs->rthdr0 == IP_NULL)
        /* Silent drop any destination option placed after routing header
           if no routing header was specified (9.2 in RFC3542) */
        exthdrs->rthdrdst = IP_NULL;
    else if (param != IP_NULL)
    {
        Ip_pkt_ip6_rthdr0 *rthdr0 = (Ip_pkt_ip6_rthdr0 *) exthdrs->rthdr0;
        struct Ip_in6_addr *addr = (struct Ip_in6_addr *) (rthdr0 + 1);

        if (rthdr0->ip6r0_type != IP_IPV6_RTHDR_TYPE_0)
            /* Unsupported routing header type */
            return -IP_ERRNO_EINVAL;

        /* Store */
        exthdrs->rthdr0_to  = param->to;
        param->to = addr;
    }

    if (exthdrs->hbh != IP_NULL)
        exthdrs->total_len += (exthdrs->hbh->ip6h_len + 1) * 8;
    if (exthdrs->dst != IP_NULL)
    {
        exthdrs->total_len += (exthdrs->dst->ip6d_len + 1) * 8;
        /* Process any home options */
        ipnet_ip6_apply_exthdrs_dstopt(exthdrs->dst, param, sock->vr_index);
    }

    /* Routing header type 0 */
    if (exthdrs->rthdr0 != IP_NULL)
        exthdrs->total_len += (exthdrs->rthdr0->ip6r_len + 1) * 8;

    /* Append routing header type 2 */
    if (exthdrs->rthdr2 != IP_NULL)
    {
        Ip_pkt_ip6_rthdr2 *rthdr2 = (Ip_pkt_ip6_rthdr2 *) exthdrs->rthdr2;

        /* Need to alter final_to */
        exthdrs->total_len += (exthdrs->rthdr2->ip6r_len + 1) * 8;

        /* The rthdr2 specifies the final destination */
        param->final_to = &rthdr2->ip6r2_home;
    }

    /* Trailing destination options */
    if (exthdrs->rthdrdst != IP_NULL)
    {
        exthdrs->total_len += (exthdrs->rthdrdst->ip6d_len + 1) * 8;
        /* Process any home options */
        ipnet_ip6_apply_exthdrs_dstopt(exthdrs->rthdrdst, param, sock->vr_index);
    }

    *has_exthdr = (Ip_u8) (exthdrs->total_len != 0);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_apply_ancillary_data
 *===========================================================================
 * Description: Applies ancillary data and sticky options to paramters
 *              of the outgoing packet.
 * Parameters:  [in] sock - The socket to use when sending.
 *              [in] msg - Message data (can be IP_NULL).
 *              [in, out] param - The IPv6 send parameters.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_apply_ancillary_data(Ipnet_socket *sock,
                               IP_CONST struct Ip_msghdr *msg,
                               Ipnet_ip6_output_param *param)
{
    struct Ip_in6_pktinfo  *pktinfo;
    struct Ip_sockaddr_in6 *nexthop;
    int                     val;
    struct Ip_cmsghdr      *cmsg;
    int                     ret;

    pktinfo = sock->ip6->pktinfo;
    nexthop = sock->ip6->nexthop;

    if (msg != IP_NULL && msg->msg_controllen > 0)
    {
        /* Set extensions header specified to sendmsg() (this overrides any sticky options) */
        ip_assert(msg->msg_control != IP_NULL);
        for (cmsg = IP_CMSG_FIRSTHDR(msg);
             cmsg != IP_NULL;
             cmsg = IP_CMSG_NXTHDR(msg, cmsg))
        {
            if (cmsg->cmsg_level != IP_IPPROTO_IPV6)
                continue;

            switch (cmsg->cmsg_type)
            {
#ifdef IPNET_RFC2292_LEGACY_SUPPORT
            case IP_IPV6_2292HOPLIMIT:
#endif
            case IP_IPV6_HOPLIMIT:
                val = *(int *) IP_CMSG_DATA(cmsg);
                param->hlim = (Ip_u8)val;
                break;
            case IP_IPV6_TCLASS:
                val = *(int *) IP_CMSG_DATA(cmsg);
                param->tclass = (Ip_u8)val;
                break;
            case IP_IPV6_ADDR_PREFERENCES:
                val = *(int *) IP_CMSG_DATA(cmsg);
                param->src_flags = (Ip_u8)val;
                break;
            case IP_IPV6_NEXTHOP:
                nexthop = IP_CMSG_DATA(cmsg);
                break;
#ifdef IPNET_RFC2292_LEGACY_SUPPORT
            case IP_IPV6_2292PKTINFO:
#endif
            case IP_IPV6_PKTINFO:
                pktinfo = IP_CMSG_DATA(cmsg);
                break;

#ifdef IPNET_RFC2292_LEGACY_SUPPORT
            case IP_IPV6_2292HOPOPTS:
            case IP_IPV6_2292DSTOPTS:
            case IP_IPV6_2292RTHDR:
#endif
            case IP_IPV6_HOPOPTS:
            case IP_IPV6_DSTOPTS:
            case IP_IPV6_RTHDR:
            case IP_IPV6_RTHDRDSTOPTS:
                break;

            default:
                IPCOM_LOG1(DEBUG, "IPv6: unsupported ancillary data type (%d)",
                           cmsg->cmsg_type);
                return -IP_ERRNO_EOPNOTSUPP;
            }
        }
    }

    if (sock->bind_to_ifindex)
    {
        /* Bound to a specific device */
        param->netif = ipnet_if_indextonetif(sock->vr_index, sock->bind_to_ifindex);
        if (param->netif == IP_NULL
            || (param->scope_id != 0
                && param->scope_id != param->netif->ipcom.ifindex))
            return -IP_ERRNO_ENETUNREACH;
    }

    if (pktinfo != IP_NULL)
    {
        if (pktinfo->ipi6_ifindex != 0)
        {
            if (!ipnet_ip6_is_link_local_addr(param->to))
            {
                param->netif = ipnet_if_indextonetif(sock->vr_index, pktinfo->ipi6_ifindex);
                if (param->netif == IP_NULL)
                    /* Invalid interface */
                    return -IP_ERRNO_ENXIO;
            }
            else
            {
#ifdef IP_PORT_LKM
                if (param->scope_id == 0)
                    /* Several Linux base network utilities (like ping6) does not set
                       scope_id, but uses pktinfo instead.
                       So this fix is required for the LKM port */
                    param->scope_id = pktinfo->ipi6_ifindex;
#endif
                if (param->scope_id != pktinfo->ipi6_ifindex)
                    /* Cannot send to the 'to' link local address since it is not
                       valid on the outgoing link */
                    return -IP_ERRNO_ENETUNREACH;
                /* Do not set the "param->netif" since it fixed by the scope_id anyway.
                   Setting the "param->netif" will also cause route lookup failure
                   when sending to on of the nodes own link-local addresses not assigned
                   to the loopback interface */
            }
        }

        if (IP_IN6_IS_ADDR_UNSPECIFIED(&pktinfo->ipi6_addr) == IP_FALSE)
        {
            Ipnet_netif *netif = IP_NULL;

            if (IP_IN6_IS_ADDR_LINK_LOCAL(&pktinfo->ipi6_addr))
            {
                /* The ipnet_ip6_get_addr_type() call will fail for link local
                   address unless the interface is specified */
                if (param->netif != IP_NULL)
                    netif = param->netif;
                else
                    netif = ipnet_if_indextonetif(sock->vr_index, param->scope_id);
            }

            /* Try locate it in the required interface */
            if (!IP_IN6_IS_ADDR_MULTICAST(param->to)
                && ipnet_ip6_get_addr_type(&pktinfo->ipi6_addr,
                                           sock->vr_index,
                                           netif) != IPNET_ADDR_TYPE_UNICAST)
            {
                /* Must allow pktinfo to specify home addresses; even though they're not local to
                 * the interface */
                Ipnet_ip6_addr_entry *addr = ipnet_ip6_get_addr_entry(&pktinfo->ipi6_addr,
                                                                      sock->vr_index,
                                                                      IP_NULL);
                /* Is it a home address? */
                if (addr == IP_NULL || IP_BIT_ISFALSE(addr->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                    return -IP_ERRNO_EADDRNOTAVAIL;
            }

            param->from = &pktinfo->ipi6_addr;
        }
    }

    if (nexthop != IP_NULL)
    {
        Ipnet_route_entry *rt_next_hop;

        ret = ipnet_route_lookup(IP_AF_INET6,
                                 sock->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_LINK_LOCAL,
                                 &nexthop->sin6_addr,
                                 nexthop->sin6_scope_id,
                                 param->netif ? param->netif->ipcom.ifindex : 0,
                                 &rt_next_hop);
        if (ret < 0)
            return ret;

        /* Only link local will be returned */
        param->nexthop = rt_next_hop;
    }

    return 0;
}


/*
 *===========================================================================
 *                        ipnet_ip6_get_addr_scope
 *===========================================================================
 * Description: Returns the scope of the specified address.
 * Parameters:  addr - an IPv6 address.
 * Returns:     The scope of the address.
 *
 */
IP_STATIC enum Ipnet_ip6_scope
ipnet_ip6_get_addr_scope(IP_CONST struct Ip_in6_addr *addr)
{
    /* Possible todo: IPv6 mapped IPv4 address might be treated as
       link local.

       RFC 4007, chapter 4
       ...
       [1] defines IPv6 addresses with embedded IPv4 addresses as
       being part of global addresses.  Thus, those addresses have
       global scope, with regard to the IPv6 scoped address
       architecture.  However, an implementation may use those
       addresses as if they had other scopes for convenience.  For
       instance, [6] assigns link-local scope to IPv4 auto-configured
       link-local addresses (the addresses from the prefix
       169.254.0.0/16 [7]) and converts those addresses into
       IPv4-mapped IPv6 addresses in order to perform destination
       address selection among IPv4 and IPv6 addresses.  This would
       implicitly mean that the IPv4-mapped IPv6 addresses equivalent
       to the IPv4 auto-configuration link-local addresses have
       link-local scope.  This document does not preclude such a
       usage, as long as it is limited within the implementation.
       ...
    */
    if (IP_IN6_IS_ADDR_MULTICAST(addr))
        return IPNET_IP6_MC_SCOPE(addr);
    else
    {
        if (IP_IN6_IS_ADDR_LINK_LOCAL(addr))
            return IPNET_IP6_SCOPE_LINK_LOCAL;
        if (IP_IN6_IS_ADDR_LOOPBACK(addr))
            return IPNET_IP6_SCOPE_NODE_LOCAL;
    }

    /* Site local address should no longer be treated as scoped
       according to RFC3879 - Deprecating Site Local Addresses */
    return IPNET_IP6_SCOPE_GLOBAL;
}


/*
 *===========================================================================
 *                      ipnet_ip6_get_addr_scope
 *===========================================================================
 * Description: Returns the list head of the addresses with the specified scope.
 * Parameters:  scope - The scope the addresses must have.
 *              ae - The head will be stored in this parameter if != IP_NULL
 * Returns:     Pointer to where the list head is stored or IP_NULL if the scope
 *              was unknown.
 */
IP_STATIC Ipnet_ip6_addr_entry **
ipnet_ip6_get_addr_scope_head(enum Ipnet_ip6_scope scope, Ipnet_ip6_addr_entry **ae)
{
    Ipnet_ip6_addr_entry **scope_head;

    switch (scope)
    {
    case IPNET_IP6_SCOPE_NODE_LOCAL:
        scope_head = &ipnet->ip6.scope.node_local;
        break;
    case IPNET_IP6_SCOPE_LINK_LOCAL:
        scope_head = &ipnet->ip6.scope.link_local;
        break;
    case IPNET_IP6_SCOPE_SITE_LOCAL:
        scope_head = &ipnet->ip6.scope.site_local;
        break;
    case IPNET_IP6_SCOPE_GLOBAL:
        scope_head = &ipnet->ip6.scope.global;
        break;
    default:
        /* Unknown scope or organization local scope,
           no unicast addresses has organization local scope */
        scope_head = IP_NULL;
        break;
    }

    if (ae != IP_NULL)
    {
        if (scope_head == IP_NULL)
            *ae = IP_NULL;
        else
            *ae = *scope_head;
    }
    return scope_head;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_ip6_if_configure
 *===========================================================================
 * Description: Adds automatic IPv6 addresses for interfaces that just
 *              entered running state.
 * Parameters:  netif - The interface that is going to be configured.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_if_configure(Ipnet_netif *netif)
{
    struct Ip_in6_addr addr;

    IP_BIT_CLR(netif->flags2, IPNET_IFF2_IPV6_DISABLED);

    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_MULTICAST))
    {
        /* RFC 4861, chapter 7.2.1
           ...
           When a multicast-capable interface becomes enabled, the node MUST
           join the all-nodes multicast address on that interface, as well as
           the solicited-node multicast address corresponding to each of the IP
           addresses assigned to the interface.
           ...
           The solicited-node multicast addresses are joined when the
           unicast addresses is assigned to the interface.
        */
        (void)ipcom_inet_pton(IP_AF_INET6, "FF02::1", &addr);
        (void)ipnet_ip6_add_addr(netif,
                                 &addr,
                                 IPNET_IP6_ADDR_FLAG_AUTOMATIC,
                                 IPCOM_ADDR_INFINITE,
                                 IPCOM_ADDR_INFINITE,
                                 16);
    }

    /* Add link-local address to the interface, but skip PPP interfaces since
       the link-local address is negotiated in that case */
    if (netif->link_ip6_create_addr != IP_NULL
        && netif->ipcom.type != IP_IFT_PPP
        && netif->ipcom.type != IP_IFT_PPPOE
        && ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6, netif, "AutoConf", 1, ipnet_bool_map))
    {
        (void) ipcom_inet_pton(IP_AF_INET6, "FE80::", &addr);
        if (netif->link_ip6_create_addr(netif, &addr, &addr) >= 0)
            (void)ipnet_ip6_add_addr(netif, &addr,
                                     (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK) ? 0 : IPNET_IP6_ADDR_FLAG_TENTATIVE)
                                     | IPNET_IP6_ADDR_FLAG_AUTOMATIC,
                                     IPCOM_ADDR_INFINITE,
                                     IPCOM_ADDR_INFINITE,
                                     64);
    }

#ifdef IP_PORT_LKM
    if (netif->ipcom.type == IP_IFT_PPP)
    {
        Ipnet_ppp_peer  *p = netif->private_data;

        /* Setup the peer route */
        (void)ipnet_if_set_ipv6_ppp_peer(p,&p->peer6);
    }
#endif

}


/*
 *===========================================================================
 *                    ipnet_ip6_if_unconfigure
 *===========================================================================
 * Description: Removes all automatic IPv6 addresses.
 * Parameters:  netif - The interface that is going to be unconfigured.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_if_unconfigure(Ipnet_netif *netif)
{
    Ipnet_ip6_addr_entry *addr_entry;

    ipnet_timeout_cancel(netif->inet6_rs_tmo);

    for (addr_entry = netif->inet6_addr_list;
         addr_entry != IP_NULL;
         addr_entry = addr_entry->next)
        IP_BIT_CLR(addr_entry->flags, IPNET_IP6_ADDR_FLAG_UPDATE_DONE);

    addr_entry = netif->inet6_addr_list;
    while (addr_entry != IP_NULL)
    {
        if (IP_BIT_ISSET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_UPDATE_DONE)
            || IP_BIT_ISFALSE(addr_entry->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS | IPNET_IP6_ADDR_FLAG_AUTOMATIC))
        {
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_UPDATE_DONE);
            addr_entry = addr_entry->next;
        }
        else
        {
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_UPDATE_DONE);
            (void )ipnet_ip6_remove_addr(netif, &addr_entry->addr);
            addr_entry = netif->inet6_addr_list;
        }
    }

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    ipnet_timeout_cancel(netif->mldv1_querier_present_tmo);
#endif
}


/*
 *===========================================================================
 *                    ipnet_ip6_checksum
 *===========================================================================
 * Description: Calculates the internet checksum for a IPv6 packet.
 * Parameters:  src - The 128-bit IPv6 source address.
 *              dst - The 128-bit IPv6 destination address.
 *              proto - The protcol ID of the upper layer protocol.
 *              bytes - The number of bytes in the payload for which this is
 *              pseudo header.
 *              pkt_flags - The flags for the packet containing the data to
 *                          checksum.
 * Returns:     The one complement internet checksum.
 *
 */
IP_GLOBAL Ip_u16
ipnet_ip6_checksum(IP_CONST struct Ip_in6_addr *src,
                   IP_CONST struct Ip_in6_addr *dst,
                   Ip_u16 proto,
                   Ip_u16 *data,
                   Ip_u16 bytes,
                   Ipcom_pkt *pkt)
{
    Ip_u32 sum;

    (void)pkt;

#ifdef IPCOM_USE_HW_CHECKSUM_RX
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
        return 0;

    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_TL_CHECKSUM))
        sum = pkt->chk;
    else
#endif /* IPCOM_USE_HW_CHECKSUM_RX */
        sum = ipcom_in_checksum_update(data, bytes);
    sum += ipnet_ip6_pseudo_header_checksum_update(src, dst, proto, bytes);
    return ipcom_in_checksum_finish(sum);
}


/*
 *===========================================================================
 *                    ipnet_ip6_create_sockaddr
 *===========================================================================
 * Description: Creates a socket address structure for the specified
 *              IPv6 address
 * Parameters:  sin6 - buffer to store the socket address.
 *              addr - an IPv6 address.
 *              ifindex - interface that will be use with this address
 * Returns:     The socket address for the specified IPv6 address.
 *
 */
IP_GLOBAL struct Ip_sockaddr *
ipnet_ip6_create_sockaddr(struct Ip_sockaddr_in6 *sin6,
                          IP_CONST struct Ip_in6_addr *addr,
                          Ip_u32 ifindex)
{
    ipcom_memset(sin6, 0, sizeof(struct Ip_sockaddr_in6));
    sin6->sin6_family = IP_AF_INET6;
    IPCOM_SA_LEN_SET(sin6, sizeof(struct Ip_sockaddr_in6));
    IPNET_IP6_SET_ADDR(&sin6->sin6_addr, addr);
    if (ipnet_ip6_is_scoped_addr(addr))
        sin6->sin6_scope_id = ipnet_ip6_get_zone(ifindex, addr);
    return (struct Ip_sockaddr *) sin6;
}


/*
 *===========================================================================
 *                    ipnet_ip6_addr_set_timeout
 *===========================================================================
 * Description: Sets the time to the next address event.
 * Parameters:  addr_entry - The address to set the time for.
 *              next_timeout_sec
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_addr_set_timeout(Ipnet_ip6_addr_entry *addr_entry,
                           Ip_u32 next_timeout_sec,
                           Ip_bool is_range)
{
    return ipnet_timeout_schedule((is_range
                                   ? ipcom_rand() % (next_timeout_sec * 1000)
                                   : next_timeout_sec * 1000),
                                  (Ipnet_timeout_handler) ipnet_ip6_addr_timeout,
                                  addr_entry,
                                  &addr_entry->tmo);
}


/*
 *===========================================================================
 *                    ipnet_ip6_create_mapped_addr
 *===========================================================================
 * Description: Creates a IPv6 mapped IPv4 address.
 * Parameters:  ipv4_addr - An IPv4 address.
 *              ipv4_mapped_ipv6_addr - The resulting IPv6 address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_create_mapped_addr(IP_CONST struct Ip_in_addr *ipv4_addr,
                             struct Ip_in6_addr *ipv4_mapped_ipv6_addr)
{
    ipcom_memset(&ipv4_mapped_ipv6_addr->in6.addr16[0], 0, 10);
    ipcom_memset(&ipv4_mapped_ipv6_addr->in6.addr16[5], 0xff, 2);
    ipcom_memcpy(&ipv4_mapped_ipv6_addr->in6.addr16[6], ipv4_addr, 4);
}


/*
 *===========================================================================
 *                    ipnet_ip6_add_route_table
 *===========================================================================
 * Description: Adds a new route table.
 * Parameters:  vr - The index of the route table to add.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_add_route_table(Ip_u16 vr, Ip_u32 table)
{
    Ipcom_route                 *rt_head;
    int                          ret;

    /* The key for IPv6 is 128-bit IPv6 address + 32-bit interface id */
    rt_head = ipcom_route_new_table((sizeof(struct Ip_in6_addr) + sizeof(Ip_u32)) * 8,
                                    ipnet_route_notify_func);

    ip_assert(rt_head != IP_NULL);
    if (rt_head == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    ret = ipnet_route_set_rtab(IP_AF_INET6, vr, table, rt_head);
    if (ret < 0)
        ipcom_route_free_table(rt_head);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_configure_route_table
 *===========================================================================
 * Description: Configures a new route table.
 * Parameters:  vr - The index of the route table to add.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_configure_route_table(Ip_u16 vr, Ip_u32 table)
{
    int                          ret    = IPCOM_SUCCESS;
    if (table == IPCOM_ROUTE_TABLE_DEFAULT)
    {
        struct Ipnet_route_add_param param;
        struct Ipnet_ipv6_key        key;
        struct Ipnet_ipv6_key        mask;

        /* Add route reject route for all multicast (will be overridden
           for networks for which multicasting is possible) */
        ipcom_memset(&key, 0, sizeof(key));
        key.addr.in6.addr16[0] = ip_htons(0xff00);

        ipcom_memset(&mask, 0, sizeof(mask));
        ipnet_route_create_mask(&mask.addr, 8);

        /* All packets matching ff00::/8 will use one of the
           multicast capable interfaces */
        ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
        param.domain  = IP_AF_INET6;
        param.flags   = IPNET_RTF_X_MCAST_RO | IPNET_RTF_REJECT | IPNET_RTF_DONE;
        param.key     = &key;
        param.netmask = &mask;
        param.vr      = vr;
        param.table   = table;
        ret = ipnet_route_add(&param);
        if (ret < 0)
            return ret;

#ifdef IPNET_USE_LOOPBACK
        /* All packets matching ff02::/16 with no scope specified
           must be rejected */
        key.addr.in6.addr16[0] = ip_htons(0xff02);
        ipnet_route_create_mask(&mask.addr, 16);
        param.flags   = IPNET_RTF_X_HIDDEN | IPNET_RTF_REJECT | IPNET_RTF_DONE;
        param.netif   = ipnet_loopback_get_netif(vr);
        ret = ipnet_route_add(&param);
#endif
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_insert_addr_cache
 *===========================================================================
 * Description: Adds an address to a set of hash tables to make it possible
 *              to find the entry in constant time for all types of addresses.
 * Parameters:  addr - The address entry to cache.
 * Returns:     0 = success
 *              -IP_ERRNO_ENOMEM = not enough memory
 *
 */
IP_GLOBAL int
ipnet_ip6_insert_addr_cache(Ipnet_ip6_addr_entry *addr)
{
    Ipnet_ip6_addr_lookup  l;
    Ipnet_ip6_addr_entry  *a;
    Ipnet_ip6_addr_entry **scope_head;

    if (addr->type == IPNET_ADDR_TYPE_UNICAST)
    {
        /* Insert this address in the list of local addresses */
        scope_head = ipnet_ip6_get_addr_scope_head(ipnet_ip6_get_addr_scope(&addr->addr), IP_NULL);
        ip_assert(scope_head != IP_NULL);
        addr->scope_prev = IP_NULL;
        addr->scope_next = *scope_head;
        *scope_head = addr;
        if (addr->scope_next != IP_NULL)
            addr->scope_next->scope_prev = addr;
    }

    l.ifindex  = addr->netif->ipcom.ifindex;
    l.addr     = &addr->addr;
    l.vr_index = addr->netif->vr_index;

    ip_assert(ipcom_hash_get(ipnet->ip6.addrs, &l) == IP_NULL);

    /* Direct match on interface, route table and address */
    if (ipcom_hash_add(ipnet->ip6.addrs, addr) != IPCOM_SUCCESS)
        return -IP_ERRNO_ENOMEM;

    if (IP_IN6_IS_ADDR_MULTICAST(&addr->addr)
        || IP_IN6_IS_ADDR_LINK_LOCAL(&addr->addr))
        return 0;

    /* Match on route table and address */
    a = ipcom_hash_get(ipnet->ip6.addrs_ignore_if, &l);
    if (a == IP_NULL)
    {
        if (ipcom_hash_add(ipnet->ip6.addrs_ignore_if, addr) != IPCOM_SUCCESS)
            return -IP_ERRNO_ENOMEM;
    }
    else
    {
        addr->next_dup_addr = a->next_dup_addr;
        a->next_dup_addr = addr;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_remove_addr_cache
 *===========================================================================
 * Description: Removes an address from hash lookup tables.
 * Parameters:  addr - The address entry to remove the cache for.
 * Returns:
 *
 */
IP_GLOBAL void ipnet_ip6_remove_addr_cache(Ipnet_ip6_addr_entry *addr)
{
    Ipnet_ip6_addr_entry  *a;
    Ipnet_ip6_addr_lookup  l;
    Ipnet_ip6_addr_entry **scope_head;

    if (addr->type == IPNET_ADDR_TYPE_UNICAST)
    {
        /* Remove this address in the list of local addresses */
        scope_head = ipnet_ip6_get_addr_scope_head(ipnet_ip6_get_addr_scope(&addr->addr), IP_NULL);
        ip_assert(scope_head != IP_NULL);

        if (addr->scope_prev == IP_NULL)
            /* First entry, make the head point to the next element */
            *scope_head = addr->scope_next;
        else
            addr->scope_prev->scope_next = addr->scope_next;

        if (addr->scope_next != IP_NULL)
            addr->scope_next->scope_prev = addr->scope_prev;

        addr->scope_prev = IP_NULL;
        addr->scope_next = IP_NULL;
    }

    (void)ipcom_hash_remove(ipnet->ip6.addrs, addr);

    if (IP_IN6_IS_ADDR_MULTICAST(&addr->addr)
        || IP_IN6_IS_ADDR_LINK_LOCAL(&addr->addr))
        return;

    /* Match on route table and address */
    l.ifindex  = addr->netif->ipcom.ifindex;
    l.vr_index = addr->netif->vr_index;
    l.addr     = &addr->addr;
    a = ipcom_hash_get(ipnet->ip6.addrs_ignore_if, &l);
    ip_assert(a != IP_NULL);
    if (a == addr)
    {
        (void)ipcom_hash_remove(ipnet->ip6.addrs_ignore_if, a);
        if (a->next_dup_addr != IP_NULL)
            (void)ipcom_hash_add(ipnet->ip6.addrs_ignore_if, a->next_dup_addr);
    }
    else
    {
        while (a->next_dup_addr != addr)
            a = a->next_dup_addr;
        a->next_dup_addr = addr->next_dup_addr;
    }
    addr->next_dup_addr = IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ip6_add_addr
 *===========================================================================
 * Description: Adds an IPv6 address to the interface.
 * Parameters:  netif - The interface the address should be added to.
 *              addr - The address to add.
 *              flags - IPNET_IP6_ADDR_FLAG_xxx flags.
 *              preferred - Number of seconds this address should be preferred.
 *              Set to IPCOM_ADDR_INFINITE for inifinity.
 *              valid - Number of seconds this address should be valid.
 *              Set to IPCOM_ADDR_INFINITE for inifinity.
 *              prefixlen - The number of bits used as network ID in this
 *              address.
 * Returns:     0 = success, <0 = error code.
 *              NOTE: the "netif" pointer might be invalid if this function
 *              returns failure.
 *
 */
IP_GLOBAL int
ipnet_ip6_add_addr(Ipnet_netif *netif, IP_CONST struct Ip_in6_addr *addr,
                   Ip_u32 flags, Ip_u32 preferred, Ip_u32 valid, int prefixlen)
{
    Ipnet_ip6_addr_entry *addr_entry;
    int                   ret = 0;

    if (prefixlen < 0 || prefixlen > 128)
    {
        IPCOM_LOG1(ERR, "IPv6: invalid prefixlen (%d) in add address", prefixlen);
        return -IP_ERRNO_EINVAL;
    }

    if (IP_IN6_IS_ADDR_V4COMPAT(addr) || IP_IN6_IS_ADDR_V4MAPPED(addr))
    {
        IPCOM_LOG1(ERR,
                   "IPv6: %s is a IPv4 compatibility or IPv6-mapped-IPv4 address. "
                   "Such address cannot be added since they represents IPv4 "
                   "addresses in the IPv6 domain",
                   ipcom_inet_ntop(IP_AF_INET6, addr, ipnet->log_buf, sizeof(ipnet->log_buf)));
        return -IP_ERRNO_EINVAL;

    }

    if (IP_IN6_IS_ADDR_MC_SITELOCAL(addr) || IP_IN6_IS_ADDR_SITE_LOCAL(addr))
        IPCOM_LOG1(NOTICE,
                   "IPv6: %s is a site local address, their usage is deprecated by RFC 3879. "
                   "The address will be treated as a global unicast address",
                   ipcom_inet_ntop(IP_AF_INET6, addr, ipnet->log_buf, sizeof(ipnet->log_buf)));

    /* Check if the address already is assigned to the interface */
    addr_entry = ipnet_ip6_get_addr_entry(addr, netif->vr_index, netif);
    if (addr_entry != IP_NULL)
    {
        /* Don't reset the timer */
        if (IP_BIT_ISSET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
            return 0;

        /* Update address lifetimes */
        ret = ipnet_ip6_set_addr_lifetime(addr_entry, preferred, valid, IP_TRUE);
        if (ret < 0)
            return ret;

        if (addr_entry->type != IPNET_ADDR_TYPE_UNICAST)
            /* Reference count non-unicast addresses */
            addr_entry->ref_count++;
        return 0;
    }

    addr_entry = ipcom_slab_alloc(ipnet->ip6.addr_slab);
    if (addr_entry == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    addr_entry->ref_count = 1;
    addr_entry->prefixlen = prefixlen;
    addr_entry->flags     = flags & IPNET_IP6_ADDR_FLAG_RW_MASK;
#ifdef IPNET_USE_RFC4941
    addr_entry->added_sec = IPNET_SECONDS_ABS;
#endif

    if (IP_IN6_IS_ADDR_MULTICAST(addr))
    {
        addr_entry->type = IPNET_ADDR_TYPE_MULTICAST;
        IP_BIT_CLR(flags, IPNET_IP6_ADDR_FLAG_TENTATIVE);
    }
    else if (IP_BIT_ISSET(flags, IPNET_IP6_ADDR_FLAG_ANYCAST))
        addr_entry->type = IPNET_ADDR_TYPE_ANYCAST;
    else
        addr_entry->type = IPNET_ADDR_TYPE_UNICAST;

    if (prefixlen == 0 || prefixlen == 128)
        IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_DONT_ADD_NETWORK_RT);

    IPNET_IP6_SET_ADDR(&addr_entry->addr, addr);
    addr_entry->netif = netif;

    /* Addresses are stored in this order in the netif->inet6_addr_list
     *  1. Unicast
     *  2. Multicast
     *  3. Anycast
     */
    if (addr_entry->type == IPNET_ADDR_TYPE_UNICAST
        || netif->inet6_addr_list == IP_NULL
        || netif->inet6_addr_list->type != IPNET_ADDR_TYPE_UNICAST)
    {
        /* Put unicast addresses in-front of non-unicast addresses */
        addr_entry->next = netif->inet6_addr_list;
        netif->inet6_addr_list = addr_entry;
    }
    else /* Take care of multicast and anycast addresses */
    {
        Ipnet_ip6_addr_entry *ae = netif->inet6_addr_list;

        while (ae->next != IP_NULL && ae->next->type != IPNET_ADDR_TYPE_ANYCAST)
            ae = ae->next;
        addr_entry->next = ae->next;
        ae->next = addr_entry;
    }

    ret = ipnet_ip6_set_addr_lifetime(addr_entry, preferred, valid, IP_TRUE);
    if (ret < 0)
        goto errout;

    ret = ipnet_ip6_insert_addr_cache(addr_entry);
    if (ret < 0)
        goto errout;

    if (addr_entry->type == IPNET_ADDR_TYPE_MULTICAST)
    {
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        if ((ret = ipnet_mcast_init(&addr_entry->mcast)) != 0)
            goto errout;
#endif

        ret = ipnet_if_link_ioctl(netif, IP_SIOCXADDMULTI_IN6, (void*) addr);
        if (ret >= 0)
            IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_MCAST);
    }

    if (ret < 0)
        goto errout;

    IPCOM_LOG2(INFO, "IPv6: added %s to %s",
               ipcom_inet_ntop(IP_AF_INET6, &addr_entry->addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
               netif->ipcom.name);

    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
        return ipnet_ip6_assign_addr(addr_entry);
    /* else: DAD and adding routes will be done when the interface is set UP */
    return 0;

 errout:
    (void)ipnet_ip6_remove_addr(netif, addr);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_remove_addr
 *===========================================================================
 * Description: Removes an IPv6 address from a network interface.
 * Parameters:  netif - Network interface to which the address is assigned.
 *              addr - The address to remove.
 * Returns:     >=0 = success (returned value is the ref. count), <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_remove_addr(Ipnet_netif *netif, IP_CONST struct Ip_in6_addr *addr)
{
    struct Ipnet_ipv6_key  ipv6_key;
    struct Ipnet_ipv6_key  ipv6_mask;
    Ipnet_ip6_addr_entry **addr_it;
    Ipnet_ip6_addr_entry  *del_addr;
    int                    ret;
    Ip_u16                 vr = netif->vr_index;

    del_addr = IP_NULL;
    ret = 0;
    addr_it = &netif->inet6_addr_list;

    /* Find the address */
    while (*addr_it != IP_NULL)
    {
        if (IP_IN6_ARE_ADDR_EQUAL(addr, &(*addr_it)->addr))
        {
            del_addr = *addr_it;
            if (--del_addr->ref_count > 0)
                return del_addr->ref_count;

            break;
        }
        addr_it = &(*addr_it)->next;
    }
    if (del_addr == IP_NULL)
        /* Address not found */
        return -IP_ERRNO_EADDRNOTAVAIL;

    /* Invalidate the route cache tag since assigning this address
       might have affects on the outcome of lookups */
    ipnet_route_cache_invalidate();
    ipnet_ip6_remove_addr_cache(del_addr);

    /* Remove the address from the list of assigned addresses */
    *addr_it = del_addr->next;

    if (del_addr->type == IPNET_ADDR_TYPE_MULTICAST)
    {
        ipnet_mld_done(del_addr);
        ipnet_mcast_free(&del_addr->mcast);
    }

    ipnet_timeout_cancel(del_addr->tmo);

    if (del_addr->socket_list != IP_NULL)
    {
        Ipnet_ip6_addr_entry *addr_entry;

        /* Clients can bind to multicast addresses not joined by IPNET
           since that some UNIX application expects that to be possible */
        ip_assert(del_addr->type != IPNET_ADDR_TYPE_MULTICAST);
        /* One or more sockets are bound to this address */
        addr_entry = ipnet_ip6_get_addr_entry(&del_addr->addr, netif->vr_index, IP_NULL);
        if (addr_entry == IP_NULL)
            /* No other interface has this address assigned, notify all sockets */
            ipnet_sock_bind_addr_removed(del_addr->socket_list);
        else
            ipnet_sock_change_addr_entry(del_addr->socket_list, &addr_entry->socket_list, addr_entry);
    }

    ipcom_memset(&ipv6_mask, 0 , sizeof(ipv6_mask));
    ipnet_route_create_mask(&ipv6_mask.addr, del_addr->prefixlen);

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP6_ADDR_FLAG_LOOPBACK_RT))
    {
        /* Remove the loopback route for this (local) address */
        IPNET_IP6_SET_ADDR(&ipv6_key.addr, &del_addr->addr);
        ipv6_key.scope_id = netif->ipcom.ifindex;
        /* Do not check return value since this route can have been removed already */
        (void) ipnet_route_delete2(IP_AF_INET6,
                                   vr,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &ipv6_key,
                                   IP_NULL, IP_NULL, 0, 0, 0, IP_TRUE);
    }

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP6_ADDR_FLAG_NETWORK_RT))
    {
        /* Remove the network route for this (local) address */
        IPNET_IP6_SET_ADDR(&ipv6_key.addr,  &del_addr->addr);
        ipv6_key.scope_id = netif->ipcom.ifindex;

        if (ipnet_ip6_is_scoped_addr(&del_addr->addr))
            ipv6_mask.scope_id = ~0u;
        else
            ipv6_mask.scope_id = 0;
        ipnet_route_apply_mask(&ipv6_key, &ipv6_mask, sizeof(struct Ipnet_ipv6_key) * 8);

        (void) ipnet_route_delete2(IP_AF_INET6,
                                   vr,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &ipv6_key,
                                   &ipv6_mask,
                                   IP_NULL,
                                   netif->ipcom.ifindex,
                                   0,
                                   0,
                                   IP_TRUE);
    }

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP6_ADDR_FLAG_SUBNET_ANYCAST))
    {
        struct Ip_in6_addr subnet_anycast;

        ipnet_ip6_create_subnet_anycast_addr(del_addr, &subnet_anycast);
        (void) ipnet_ip6_remove_addr(netif, &subnet_anycast);
    }

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP6_ADDR_FLAG_MCAST))
        /* leave the multicast link address for this address */
        ret = ipnet_if_link_ioctl(netif, IP_SIOCXDELMULTI_IN6, (void*) addr);

    if (IP_BIT_ISSET(del_addr->flags, IPNET_IP6_ADDR_FLAG_SOL_MCAST))
    {
        /* leave the solicitated node multicast group */
        struct Ip_in6_addr tentative_sol_addr;

        IPNET_IP6_SET_SOLICITED_ADDR(&tentative_sol_addr, addr);
        ret = ipnet_ip6_remove_addr(netif, &tentative_sol_addr);
    }

    if (IP_BIT_ISFALSE(del_addr->flags, IPNET_IP6_ADDR_FLAG_NEWADDR_DONE))
    {
        /* No corresponding add event has been sent unless the
           IPNET_IP6_ADDR_FLAG_NEWADDR_DONE is set, send add event now
           directly followed by a delete so that all listener gets notification
           about the removal of the address */
        ipnet_kioevent(netif, IP_EIOXNEWADDR, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        IPNET_ROUTESOCK(ipnet_routesock_addr_add(netif, IP_AF_INET6, del_addr));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_ip6_addr_add(netif, del_addr));
    }

    ipnet_kioevent(netif, IP_EIOXDELADDR, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
    IPNET_ROUTESOCK(ipnet_routesock_addr_delete(netif, IP_AF_INET6, del_addr));
    IPNET_NETLINKSOCK(ipnet_rtnetlink_ip6_addr_del(netif, del_addr));

    IPCOM_LOG2(INFO, "IPv6: removed %s from %s",
               ipcom_inet_ntop(IP_AF_INET6, addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
               netif->ipcom.name);

    ipcom_slab_free(ipnet->ip6.addr_slab, del_addr);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_kioevent
 *===========================================================================
 * Description: IPv6 handler for IO events on network interfaces.
 * Parameters:  netif - The network interface the event happened on.
 *              command - The event.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_kioevent(Ipnet_netif *netif, int event)
{
    Ipnet_ip6_addr_entry *addr;

    switch (event)
    {
    case IP_EIOXRUNNING:
    case IP_EIOXUP:
#ifdef IPNET_USE_RFC4941
        /* RFC4941, chapter 3.5
           ...
           Finally, when an interface connects to a new link, a new
           randomized interface identifier SHOULD be generated
           immediately together with a new set of temporary addresses.
           If a device moves from one ethernet to another, generating
           a new set of temporary addresses from a different
           randomized interface identifier ensures that the device
           uses different randomized interface identifiers for the
           temporary addresses associated with the two links, making
           it more difficult to correlate addresses from the two
           different links as being from the same node.
           ...
        */
        /* Implement the above by declaring the randomized host ID as
           uninitialized. A new randomized host ID will then be
           regenerated before any new address is formed
        */
        IP_BIT_CLR(ipnet_flags, IPNET_FLAG_IPV6_RFC4941_INIT);
#endif /* IPNET_USE_RFC4941 */

        IP_BIT_SET(netif->flags2, IPNET_IFF2_FIRST_NS);

        /* Assign the multicast addresses first to make sure that DAD can be performed */
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
        {
            netif->mld_robustness_variable = IPNET_MCAST_DEFAULT_ROBUSTNESS_VARIABLE;
            netif->mld_query_interval      = IPNET_MCAST_DEFAULT_QUERY_INTERVAL;
            netif->inet6_rs_retrans_count  = IPNET_INET6_RS_RETRANS_COUNT_NEED_INIT;
            netif->inet6_nd_curhoplimit    = (Ip_u8) ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                                                   netif,
                                                                                   "BaseHopLimit",
                                                                                   64);

            for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
                if (addr->type == IPNET_ADDR_TYPE_MULTICAST)
                    if (ipnet_ip6_assign_addr(addr) < 0)
                        /* Start over since the current address has has been removed */
                        addr = netif->inet6_addr_list;
            for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
                if (addr->type != IPNET_ADDR_TYPE_MULTICAST)
                    if (ipnet_ip6_assign_addr(addr) < 0)
                        /* Start over since the current address has has been removed */
                        addr = netif->inet6_addr_list;
            if (event == IP_EIOXUP)
                ipnet_ip6_if_configure(netif);
        }
        break;

    case IP_EIOXDOWN:
        ipnet_ip6_if_unconfigure(netif);
        (void)ipnet_route_remove_all(IP_AF_INET6, netif);
        for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
        {
            struct Ipnet_ipv6_key  ipv6_key;
            if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_LOOPBACK_RT))
            {
                /* Remove the loopback route for this (local) address */
                IPNET_IP6_SET_ADDR(&ipv6_key.addr, &addr->addr);
                ipv6_key.scope_id = netif->ipcom.ifindex;
                /* Do not check return value since this route can have been removed already */
                (void) ipnet_route_delete2(IP_AF_INET6,
                                           netif->vr_index,
                                           IPCOM_ROUTE_TABLE_DEFAULT,
                                           &ipv6_key,
                                           IP_NULL, IP_NULL, 0, 0, 0, IP_TRUE);
            }
            /* Clear this bit since all routes to this interface has been removed */
            IP_BIT_CLR(addr->flags, IPNET_IP6_ADDR_FLAG_NETWORK_RT | IPNET_IP6_ADDR_FLAG_LOOPBACK_RT);

            if (addr->type == IPNET_ADDR_TYPE_UNICAST)
                /* Mark the addresses as tentative since the host cannot know
                   what is happening on the link when in DOWN state */
                IP_BIT_SET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE);
        }
        break;

    case IP_EIOXSTOP:
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
            for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
                if (addr->type == IPNET_ADDR_TYPE_UNICAST)
                    /* Mark the addresses as tentative since the host cannot know
                       what is happening on the link when detacted from it */
                    IP_BIT_SET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE);
        break;

    default:
        break;
    }
}


/*
 *===========================================================================
 *                    ipnet_ip6_get_addr_label
 *===========================================================================
 * Description: Returns the address label as described in RFC 3484, chapter 2.1
 * Parameters:  addr - The address to return the label for.
 * Returns:     The address label.
 *
 */
IP_STATIC int
ipnet_ip6_get_addr_label(IP_CONST struct Ip_in6_addr *addr)
{
    if (IP_IN6_IS_ADDR_LOOPBACK(addr))
        return 0;

    if (addr->in6.addr16[0] == ip_htons(0x2002))
        return 2;

    if (addr->in6.addr16[0] == 0 && addr->in6.addr16[1] == 0
        && addr->in6.addr16[2] == 0 && addr->in6.addr16[3] == 0
        && addr->in6.addr16[4] == 0)
    {
        if (addr->in6.addr16[5] == 0)
            return 3;
        if (addr->in6.addr16[5] == 0xffff)
            return 4;
    }

    return 1;
}


/*
 *===========================================================================
 *                       ipnet_ip6_ifindex_to_zone
 *===========================================================================
 * Description: Returns the zone a specified interface is assigned to.
 * Parameters:  vr - virtual router index.
 *              scope - scope of the zone.
 *              ifindex - interface that has to be included in the zone.
 * Returns:     The zone the interface is assigned to.
 *
 */
IP_STATIC Ip_u32
ipnet_ip6_ifindex_to_zone(Ip_u16 vr, enum Ipnet_ip6_scope scope, Ip_u32 ifindex)
{
    char sysvar_name[40];

    if (scope <= IPNET_IP6_SCOPE_MIN || scope >= IPNET_IP6_SCOPE_MAX)
    {
        IPCOM_LOG2(NOTICE, "Got a invalid scope (%d) on ifindex %d.", scope, ifindex);
        return IPNET_IP6_DEFAULT_ZONE;
    }

    while (IP_UNLIKELY(ipnet_ip6_scope_str[scope] == IP_NULL))
        /* The current scope does not have a string mapping, select
           the next higher scope instead. */
        scope++;

    if (scope == IPNET_IP6_SCOPE_GLOBAL || ifindex == 0 || ifindex == ~0u)
        /* Global scope or invalid ifindex, use the default zone */
        return IPNET_IP6_DEFAULT_ZONE;

    /* RFC 4007, chapter 5
       ...
       Zones of the different scopes are instantiated as follows:

       o  Each interface on a node comprises a single zone of interface-
          local scope (for multicast only).

       o  Each link and the interfaces attached to that link comprise a
          single zone of link-local scope (for both unicast and multicast).

       o  There is a single zone of global scope (for both unicast and
          multicast) comprising all the links and interfaces in the
          Internet.

       o  The boundaries of zones of a scope other than interface-local,
          link-local, and global must be defined and configured by network
          administrators.
       ...
    */
    /* All "scope_id = ifindex" assignments must be replaced with
       "scope_id = ipnet_ip6_ifindex_to_zone(vr,
       IPNET_IP6_SCOPE_LINK_LOCAL, ifindex)" to support that more than
       one interface is attached to the same link local zone. The
       check below makes sure that this function returns the "right"
       value for the current implementation */
    ipcom_strcpy(sysvar_name, "zone.");
    ipcom_strcat(sysvar_name, ipnet_ip6_scope_str[scope]);
    ipcom_strcat(sysvar_name, ".Id");
    return ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                         ipnet_if_indextonetif(vr, ifindex),
                                         sysvar_name,
                                         (scope == IPNET_IP6_SCOPE_LINK_LOCAL
                                          ? ifindex
                                          : 0));
}


/*
 *===========================================================================
 *                           ipnet_ip6_get_zone
 *===========================================================================
 * Description: Returns the zone for the specified interface and address.
 * Parameters:  netif - interface that must be part of the zone.
 *              addr - address that must be part of the zone.
 * Returns:     A zone ID.
 *
 */
IP_GLOBAL Ip_u32
ipnet_ip6_get_zone(Ip_u32 ifindex, IP_CONST struct Ip_in6_addr *addr)
{
    Ipnet_netif *netif = ipnet_if_indextonetif(IPCOM_VR_ANY, ifindex);

    ip_assert(addr != IP_NULL);
    if (netif == IP_NULL)
        /* Invalud ifindex, use default zone */
        return IPNET_IP6_DEFAULT_ZONE;
    return ipnet_ip6_ifindex_to_zone(netif->vr_index,
                                     ipnet_ip6_get_addr_scope(addr),
                                     netif->ipcom.ifindex);
}


/*
 *===========================================================================
 *                       ipnet_ip6_get_default_zone
 *===========================================================================
 * Description: Returns the default zone for a specific scope on a specific
 *              virtual router.
 * Parameters:  vr - virtual router index.
 *              scope - scope to fetch zone for.
 * Returns:     The default zone for the specified VR and scope.
 *
 */
IP_STATIC Ip_u32
ipnet_ip6_get_default_zone(Ip_u16 vr, enum Ipnet_ip6_scope scope)
{
    char   sysvar_name[40];
    Ip_u32 zone;

    if (scope <= IPNET_IP6_SCOPE_MIN || scope >= IPNET_IP6_SCOPE_MAX)
    {
        IPCOM_LOG1(NOTICE,
                   "Invalid scope (%d) while mapping to default zone",
                   scope);
        /* Use the global zone for now */
        return 0;
    }

    while (IP_UNLIKELY(ipnet_ip6_scope_str[scope] == IP_NULL))
        /* The current scope does not have a string mapping, select
           the next higher scope instead. */
        scope++;

    if (scope == IPNET_IP6_SCOPE_GLOBAL)
        /* RFC 4007, chapter 5
           ...
           o  There is a single zone of global scope (for both unicast and
              multicast) comprising all the links and interfaces in the
              Internet.
           ...
        */
        return 0;

    ipcom_strcpy(sysvar_name, "ipnet.inet6.zone.");
    ipcom_strcat(sysvar_name, ipnet_ip6_scope_str[scope]);
    ipcom_strcat(sysvar_name, ".Default");

    zone = (Ip_u32) ipcom_sysvar_get_as_int0(sysvar_name, 0);
    if (zone == 0)
    {
        Ip_u32       i;
        Ip_u32       best_ifindex = ~0u;
        Ipnet_netif *netif;

        /* No default zone defined for the specified scope
           RFC 4007, chapter 6
           ...
       ---------------------------------------------------------------
      | a node                                                        |
      |                                                               |
      |                                                               |
      |                                                               |
      |                                                               |
      |                                                               |
      |  /--link1--\ /--link2--\ /--link3--\ /--link4--\ /--link5--\  |
      |                                                               |
      |  /--intf1--\ /--intf2--\ /--intf3--\ /--intf4--\ /--intf5--\  |
       ---------------------------------------------------------------
              :           |           |           |           |
              :           |           |           |           |
              :           |           |           |           |
          (imaginary    =================      a point-       a
           loopback        an Ethernet         to-point     tunnel
             link)                               link

             Figure 2: Example of Default Zone Indices
           As well as initially assigning zone indices, as specified
           above, an implementation should automatically select a
           default zone for each scope for which there is more than
           one choice, to be used whenever an address is specified
           without a zone index (or with a zone index of zero).  For
           instance, in the example shown in Figure 2, the
           implementation might automatically select intf2 and link2
           as the default zones for each of those two scopes.  (One
           possible selection algorithm is to choose the first zone
           that includes an interface other than the loopback
           interface as the default for each scope.)  A means must
           also be provided to assign the default zone for a scope
           manually, overriding any automatic assignment.
           ...
        */
        IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
        {
            if (netif->ipcom.ifindex < best_ifindex
                && ((scope == IPNET_IP6_SCOPE_NODE_LOCAL
                     && netif->ipcom.type == IP_IFT_LOOP)
                    || (scope != IPNET_IP6_SCOPE_NODE_LOCAL
                        && netif->ipcom.type != IP_IFT_LOOP)))
                best_ifindex = netif->ipcom.ifindex;
        }

        zone = ipnet_ip6_ifindex_to_zone(vr, scope, best_ifindex);
    }

    return zone;
}


/*
 *===========================================================================
 *                  ipnet_ip6_link_local_zone_to_ifindex
 *===========================================================================
 * Description: Transforms a link local zone ID to ifindex of the interface
 *              that has the specified address.
 * Parameters:  vr - virtual router index
 *              zone - a link local zone
 *              addr - a link local address assigned to the local node.
 * Returns:     ifindex of the interface in the specified zone and owning
 *              the specified address.
 *
 */
IP_STATIC Ip_u32
ipnet_ip6_link_local_zone_to_ifindex(Ip_u16 vr, Ip_u32 zone, IP_CONST struct Ip_in6_addr *addr)
{
    Ipnet_netif *netif = ipnet_if_indextonetif(vr, zone);
    Ip_u32       i;

    ip_assert(ipnet_ip6_is_link_local_addr(addr));

    if (netif != IP_NULL
        && zone == ipnet_ip6_ifindex_to_zone(vr,
                                             IPNET_IP6_SCOPE_LINK_LOCAL,
                                             netif->ipcom.ifindex))
        /* Mapping was zone == ifindex which is the default configuration */
        return netif->ipcom.ifindex;

    /* Must pass a valid "netif" pointer in order to find link local
       addresses. So this is unfortunately the only way to do this. */
    IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
    {
        if (ipnet_ip6_get_addr_entry(addr, vr, netif) != IP_NULL
            && zone == ipnet_ip6_ifindex_to_zone(vr,
                                                 IPNET_IP6_SCOPE_LINK_LOCAL,
                                                 netif->ipcom.ifindex))
            return netif->ipcom.ifindex;
    }

    /* Could not map the zone to an ifindex */
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ip6_neighbor_cache_find
 *===========================================================================
 * Description: Returns the neighbor cache entry matching the address. A new
 *              entry is create if the request neigbor entry do not extist.
 * Parameters:  netif - The link to the neighbor is located on.
 *              addr - The address of the neighbor to find.
 *              create - IP_TRUE if entry should be created if not exists.
 * Returns:     The neighbor cache entry or IP_NULL if out of memory or if no
 *              ND information exist for that address.
 *
 */
IP_GLOBAL Ipnet_ip6_nc_entry *
ipnet_ip6_neighbor_cache_find(Ipnet_netif *netif,
                              IP_CONST struct Ip_in6_addr *addr,
                              Ip_bool create)
{
    int                ret;
    Ipnet_route_entry *rt;

    ret = ipnet_route_raw_lookup(IP_AF_INET6,
                                 netif->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_LINK_LOCAL | (create ? 0 : IPNET_RTL_FLAG_DONTCLONE),
                                 addr,
                                 ipnet_ip6_ifindex_to_zone(netif->vr_index,
                                                           IPNET_IP6_SCOPE_LINK_LOCAL,
                                                           netif->ipcom.ifindex),
                                 netif->ipcom.ifindex,
                                 &rt);
    if (ret != IPNET_ROUTE_PERFECT_MATCH
        || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO)
        || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_REJECT))
        return IP_NULL;
    return IPNET_IP6_NC(rt);
}


/*
 *===========================================================================
 *                    ipnet_ip6_neighbor_cache_update
 *===========================================================================
 * Description: Update an existing neigbor cache entry with data just
 *              received.
 * Parameters:  neighbor - The neighbor cache entry to update.
 *              state - The new state.
 *              link_addr_size - The size, in bytes, of the 'link_addr' buffer.
 *              link_addr - Pointer to the link address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_neighbor_cache_update(Ipnet_ip6_nc_entry *neighbor,
                                Ipnet_nd_state_t state,
                                IP_CONST void *link_addr,
                                Ip_bool        update_static)
{
    if (IP_UNLIKELY(!update_static && IP_BIT_ISSET(neighbor->ne.rt->hdr.flags, IPNET_RTF_STATIC)))
        return;

    /* Dont update static entries -- leave them be */
    ipnet_neigh_set_state(IP_AF_INET6,
                          &neighbor->ne,
                          state,
                          link_addr);

    if (state == IPNET_ND_REACHABLE)
    {
        Ipnet_route_entry *rt = neighbor->ne.rt;
        (void)ipnet_icmp6_default_router_change(rt->netif,
                                                rt->hdr.key,
                                                IP_FALSE,
                                                0,
                                                IP_TRUE,
                                                IP_TRUE);
    }
}


/*
 *===========================================================================
 *                    ipnet_ip6_pseudo_header_checksum_update
 *===========================================================================
 * Description: Calculates the internet checksum on IPv6 pseudo header.
 * Parameters:  [in] src - The source address.
 *              [in] dst - The destination address.
 *              [in] proto - The protcol ID of the upper layer protocol.
 *              [in] len - The number of octets in the payload for which this is
 *                   pseudo header.
 * Returns:     The 32-bit internet checksum.
 *
 */
IP_GLOBAL Ip_u32
ipnet_ip6_pseudo_header_checksum_update(IP_CONST struct Ip_in6_addr *src,
                                        IP_CONST struct Ip_in6_addr *dst,
                                        Ip_u16 proto, Ip_u16 len)
{
    Ip_u32 sum;

    sum  = src->in6.addr16[0];
    sum += src->in6.addr16[1];
    sum += src->in6.addr16[2];
    sum += src->in6.addr16[3];
    sum += src->in6.addr16[4];
    sum += src->in6.addr16[5];
    sum += src->in6.addr16[6];
    sum += src->in6.addr16[7];

    sum += dst->in6.addr16[0];
    sum += dst->in6.addr16[1];
    sum += dst->in6.addr16[2];
    sum += dst->in6.addr16[3];
    sum += dst->in6.addr16[4];
    sum += dst->in6.addr16[5];
    sum += dst->in6.addr16[6];
    sum += dst->in6.addr16[7];

    sum += ip_htons(len);
    sum += ip_htons(proto);

    return sum;
}


/*
 *===========================================================================
 *                        ipnet_ip6_is_scoped_addr
 *===========================================================================
 * Description: Returns if the specified address is scoped or not.
 * Parameters:  addr - an IPv6 address
 * Returns:     IP_TRUE if the passed address is scoped.
 *
 */
IP_GLOBAL Ip_bool
ipnet_ip6_is_scoped_addr(IP_CONST struct Ip_in6_addr *addr)
{
    enum Ipnet_ip6_scope scope;

    scope = ipnet_ip6_get_addr_scope(addr);
    return scope > IPNET_IP6_SCOPE_NODE_LOCAL
        && scope < IPNET_IP6_SCOPE_GLOBAL;
}


/*
 *===========================================================================
 *                      ipnet_ip6_is_link_local_addr
 *===========================================================================
 * Description: Returns if the specified address is link local or not.
 * Parameters:  addr - an IPv6 address
 * Returns:     IP_TRUE if the passed address is link local.
 *
 */
IP_GLOBAL Ip_bool
ipnet_ip6_is_link_local_addr(IP_CONST struct Ip_in6_addr *addr)
{
    return ipnet_ip6_get_addr_scope(addr) == IPNET_IP6_SCOPE_LINK_LOCAL;
}


/*
 *===========================================================================
 *                    ipnet_ip6_extract_scoped_addr
 *===========================================================================
 * Description:  Extract scope-id from gateway link-local address.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_extract_scoped_addr(struct Ip_sockaddr *sa, Ip_u32 ifindex)
{
    struct Ip_sockaddr_in6 *in6;

    if (sa == IP_NULL || sa->sa_family != IP_AF_INET6)
        return;

    in6 = (struct Ip_sockaddr_in6 *)sa;

    if (IP_IN6_IS_ADDR_LINK_LOCAL(&in6->sin6_addr)
        && in6->sin6_scope_id == 0)
    {
        /* Extract scope-id from gateway link-local address. */
#ifdef IP_PORT_LKM
        /* Linux saves scope index in 32-bit. */
        in6->sin6_scope_id = IP_GET_NTOHL(&in6->sin6_addr.in6.addr16[1]);
        in6->sin6_addr.in6.addr16[1] = 0;
        in6->sin6_addr.in6.addr16[2] = 0;
#else
        /* BSD saves scope index in 16-bit. */
        in6->sin6_scope_id = IP_GET_NTOHS(&in6->sin6_addr.in6.addr16[1]);
        in6->sin6_addr.in6.addr16[1] = 0;
#endif /* IP_PORT_LKM */

        if (in6->sin6_scope_id == 0)
            in6->sin6_scope_id = ifindex;
    }
}


/*
 *===========================================================================
 *                    ipnet_ip6_get_mapped_ip4_src_addr
 *===========================================================================
 * Description: Returns the address that is most resonable to use by default
 *              for a route when the destination address is a IPv6 mapped
 *              IPv4 address.
 * Parameters:  netif - The interface that will using the address.
 *              dst_addr - The destination addres
 * Returns:     A pointer to the matching address or IP_NULL if no address
 *              match.
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC IP_CONST struct Ip_in6_addr *
ipnet_ip6_get_mapped_ip4_src_addr(Ip_u16 vr,
                                  IP_CONST struct Ip_in6_addr *dst_addr,
                                  Ipnet_netif *netif)
{
    static struct Ip_in6_addr   src_addr_buf;
    IP_CONST struct Ip_in_addr *ipv4_addr;

    ipv4_addr = ipnet_ip4_get_src_addr(vr,
                                       (struct Ip_in_addr *) &dst_addr->in6.addr32[3],
                                       IP_NULL,
                                       netif);
    if (ipv4_addr == IP_NULL)
        return IP_NULL;

    /* NOTE: the ONLY reason it is possible to use a static
       "src_addr_buf" here is because there is only a single task that
       will ever run this code. */
    ip_assert(ipnet_is_stack_task());
    ipnet_ip6_create_mapped_addr(ipv4_addr, &src_addr_buf);
    return &src_addr_buf;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                  ipnet_ip6_get_src_addr_by_rules
 *===========================================================================
 * Description: Returns the best address, according to RFC 3484, to use
 *              when sending packets to the specified address.
 * Parameters:  vr - The route table to use.
 *              dst_addr - The destination address
 *              netif - The network interface that must be used when sending.
 *              flags - IP_IPV6_PREFER_SRC_xxx and/or IP_IPV6_REQUIRE_SRC_xxx
 *                      flags to tweak source address selection.
 * Returns:     A pointer to the matching address or IP_NULL if no address
 *              match.
 *
 */
IP_STATIC IP_CONST struct Ip_in6_addr *
ipnet_ip6_get_src_addr_by_rules(Ip_u16                       vr,
                                IP_CONST struct Ip_in6_addr *dst_addr,
                                Ipnet_netif                 *netif,
                                Ip_u16                       flags)
{
    Ipnet_ip6_addr_entry        *addr;
    Ipnet_ip6_addr_entry        *best = IP_NULL;
    int                          best_label = -1;
    unsigned                     best_longest_match = 0;
    int                          dst_label;
    enum Ipnet_ip6_scope         dst_scope;
    enum Ipnet_ip6_scope         src_scope;
    enum Ipnet_ip6_scope         start_scope;
    enum Ipnet_ip6_scope         stop_scope;
    int                          scope_step = 1;
    unsigned                     b;
    int                          search_count;
    int                          search_count_max = 2;

#ifdef IPCOM_USE_INET
    if (IP_IN6_IS_ADDR_V4MAPPED(dst_addr))
        return ipnet_ip6_get_mapped_ip4_src_addr(vr, dst_addr, netif);
#endif /* IPCOM_USE_INET */

    if (ipnet_ip6_get_addr_type(dst_addr, vr, IP_NULL) == IPNET_ADDR_TYPE_UNICAST)
        /* RULE 1: Prefer same address */
        return dst_addr;

    start_scope = dst_scope = ipnet_ip6_get_addr_scope(dst_addr);
    dst_label = ipnet_ip6_get_addr_label(dst_addr);

    /* RULE 2: Prefer appropriate scope */
    if (IP_BIT_ISSET(flags, IP_IPV6_PREFER_SRC_LARGESCOPE)
        && start_scope < IPNET_IP6_SCOPE_GLOBAL)
    {
        /* Override rule 2; prefer address with higher scope */
        start_scope++;
    }

    if (IP_BIT_ISSET(flags, IP_IPV6_PREFER_SRC_SMALLSCOPE)
        && start_scope > IPNET_IP6_SCOPE_NODE_LOCAL)
    {
        /* Override rule 2; prefer address with higher scope */
        start_scope--;
        scope_step = -1;
    }

    for (search_count = 1; search_count <= search_count_max; search_count++)
    {
        stop_scope = (scope_step == 1 ? IPNET_IP6_SCOPE_GLOBAL : IPNET_IP6_SCOPE_NODE_LOCAL);

        if (IP_BIT_ISSET(flags, IPNET_IPV6_REQUIRE_SRC_LINKLOCAL))
        {
            /* Override rule 2; accept only link local addresses */
            start_scope = stop_scope = IPNET_IP6_SCOPE_LINK_LOCAL;
            /* No reason to go through this loop more than once */
            search_count_max = 1;
        }
        else if (IP_BIT_ISSET(flags, IPNET_IPV6_REQUIRE_SRC_HOMEADDRESS))
        {
            if (start_scope != IPNET_IP6_SCOPE_GLOBAL)
                return IP_NULL;
        }

        for (src_scope = start_scope;
             (scope_step == 1 ? src_scope <= stop_scope : src_scope >= stop_scope)
                 && best == IP_NULL;
             src_scope += scope_step)
        {
            Ipnet_ip6_addr_entry *head;

            (void)ipnet_ip6_get_addr_scope_head(src_scope, &head);
            if (head == IP_NULL)
                continue;

            for (addr = head; addr != IP_NULL; addr = addr->scope_next)
            {
                if (addr->netif->vr_index != vr)
                    /* Wrong virtual router */
                    continue;

                if (IP_BIT_ISTRUE(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
                    /* Don't use a tentative address */
                    continue;

                /* Only home addresses allowed? */
                if (IP_BIT_ISSET(flags, IPNET_IPV6_REQUIRE_SRC_HOMEADDRESS) && IP_BIT_ISFALSE(addr->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                    continue;

                if (addr->netif != netif
                    && (src_scope <= IPNET_IP6_SCOPE_LINK_LOCAL || IP_IN6_IS_ADDR_MULTICAST(dst_addr)))
                    /* Must use the correct interface for link local addresses and multicast addresses
                       RFC 3484, chapter 4 */
                    continue;

                if ((best == IP_NULL || IP_BIT_ISSET(best->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED))
                    && IP_BIT_ISFALSE(addr->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED))
                    /* RULE 3: Avoid deprecated addresses */
                    goto select_as_best;

                if ((best != IP_NULL && IP_BIT_ISSET(best->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                    && IP_BIT_ISFALSE(addr->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                    continue;

                if ((best == IP_NULL || IP_BIT_ISFALSE(best->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                    && IP_BIT_ISTRUE(addr->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                {
                    /* Avoid Rule 4 if a netif has been explicitly specified; we end up in some really nasty
                     * issues with one hop ICMP replies using large scoped addresses otherwise */
                    if (IP_BIT_ISFALSE(flags, IPNET_IPV6_EXPLICIT_NETIF))
                        /* RULE 4: Prefer home addresses */
                        goto select_as_best;
                }

                if (best != IP_NULL && best->netif != netif && addr->netif == netif)
                    /* RULE 5: Prefer outgoing interface */
                    goto select_as_best;

#ifdef IPNET_USE_RFC3971
                /* Rule 5.5, use CGA address if available */
                if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_CGA))
                    goto select_as_best;
#endif
                if (best != IP_NULL
                    && best_label != dst_label
                    && dst_label == ipnet_ip6_get_addr_label(&addr->addr))
                    /* RULE 6: Prefer matching label */
                    goto select_as_best;

                if (IP_BIT_ISSET(flags, IP_IPV6_PREFER_SRC_TMP))
                {
                    if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY)
                        && (best == IP_NULL || IP_BIT_ISFALSE(best->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY)))
                    {
                        /* Inverse of rule RULE 7: Prefer public addresses  */
                        goto select_as_best;
                    }
                }
                else
                {
                    if (IP_BIT_ISFALSE(addr->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY)
                        && (best == IP_NULL || IP_BIT_ISSET(best->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY)))
                    {
                        /* RULE 7: Prefer public addresses  */
                        goto select_as_best;
                    }
                }

                if (best != IP_NULL
                    && best_longest_match < addr->prefixlen)
                {
                    for (b = 0; b < sizeof(struct Ip_in6_addr) / sizeof(Ip_u16); b++)
                        if (dst_addr->in6.addr16[b] != addr->addr.in6.addr16[b])
                            break;

                    if ((b << 4) > best_longest_match)
                        /* RULE 8: Prefer longest prefix match */
                        goto select_as_best;
                }
                continue;

            select_as_best:
                best = addr;
                best_label = ipnet_ip6_get_addr_label(&best->addr);
                for (b = 0; b < sizeof(struct Ip_in6_addr) / sizeof(Ip_u16); b++)
                    if (dst_addr->in6.addr16[b] != best->addr.in6.addr16[b])
                        break;
                best_longest_match = b << 4;
            }
        }

        if (best != IP_NULL)
            break;

        scope_step = -scope_step;
        start_scope = dst_scope;
    }

    if (best == IP_NULL)
        return IP_NULL;
    return &best->addr;
}


/*
 *===========================================================================
 *                    ipnet_ip6_get_src_addr
 *===========================================================================
 * Description: Returns the address that is most resonable to use by default
 *              for a route. (ex. if the destination is a link local IPv6,
 *              then use the link local address of the interface).
 *              The actual address used can be changed by ipcom_bind().
 * Parameters:  vr          - The virtual router to use
 *              dst_addr    - The destination address
 *              scope_id    - Scope ID for the address (if the address is scoped).
 *              rt          - The route to the destination or IP_NULL if
 *                            not known.
 *              rps         - Route policy selector
 *              arg_netif   - The network interface that must be used when
 *                            sending.
 *              flags - IP_IPV6_PREFER_SRC_xxx and/or IP_IPV6_REQUIRE_SRC_xxx
 *                      flags to tweak source address selection.
 * Returns:     A pointer to the matching address or IP_NULL if no address
 *              match.
 *
 */
IP_GLOBAL IP_CONST struct Ip_in6_addr *
ipnet_ip6_get_src_addr(Ip_u16                       vr,
                       IP_CONST struct Ip_in6_addr *dst_addr,
                       int                          scope_id,
                       Ipnet_route_entry           *rt,
                       Ipnet_route_policy_selector *rps,
                       Ipnet_netif                 *arg_netif,
                       Ip_u16                       flags)
{
    struct Ip_sockaddr_in6      *gw;
    Ipnet_netif                 *netif;
    Ipnet_route_policy_selector  rps_empty;
    IP_CONST struct Ip_in6_addr *src_addr;

    /* In case they did not provide an RPS, we assign an empty one. */
    if (rps == IP_NULL)
    {
        rps_empty.fields = 0;
        rps = &rps_empty;

        rps->fields |= IPNET_PR_RULE_DADDR;
        rps->daddr  = dst_addr;
    }

#ifdef IPCOM_USE_INET
    if (IP_IN6_IS_ADDR_V4MAPPED(dst_addr))
        return ipnet_ip6_get_mapped_ip4_src_addr(vr, dst_addr, arg_netif);
#endif /* IPCOM_USE_INET */

    /*  */
    if (rt == IP_NULL
        && ipnet_route_lookup_ecmp(IP_AF_INET6, vr, IPNET_RTL_FLAG_DONTCLONE, dst_addr, scope_id, 0, rps, &rt, IP_NULL) < 0)
    {
        /* If we aint got an explicit netif, do a homeaddress search */
        if (arg_netif == IP_NULL)
            return ipnet_ip6_get_src_addr_by_rules(vr, dst_addr, IP_NULL, flags|IPNET_IPV6_REQUIRE_SRC_HOMEADDRESS);
        return IP_NULL;
    }

    gw = (struct Ip_sockaddr_in6 *) rt->gateway;
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY)
        && ipnet_route_lookup_ecmp(IP_AF_INET6, vr, IPNET_RTL_FLAG_DONTCLONE,
                                   &gw->sin6_addr, gw->sin6_scope_id, 0, rps, &rt, IP_NULL) < 0)
    {
        if (arg_netif == IP_NULL)
            /* Cannot determine interface to use */
            return IP_NULL;
        rt = IP_NULL;
    }

    /* Resolve the interface to use. */
    if (arg_netif != IP_NULL)
    {
        flags |= IPNET_IPV6_EXPLICIT_NETIF;
        /* Select an address from the specified interface */
        netif = arg_netif;
    }
    else
    {
        netif = rt->netif;
        if (netif != IP_NULL)
        {
            if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK))
            {
                if (ipnet_ip6_get_addr_scope(dst_addr) != IPNET_IP6_SCOPE_LINK_LOCAL)
                    /* Look for any global unicast address on the node */
                    netif = IP_NULL;
                else
                {
                    /* Look for a link local unicast address on the
                       node that matches the specified scope */
                    Ip_u32 ifindex;

                    ifindex = ipnet_ip6_link_local_zone_to_ifindex(vr,
                                                                   scope_id,
                                                                   dst_addr);
                    netif = ipnet_if_indextonetif(vr, ifindex);
                }

                /* Destination is one of the local unicast addresses */
                if (ipnet_ip6_get_addr_type(dst_addr,
                                            vr,
                                            netif) == IPNET_ADDR_TYPE_UNICAST)

                /* RULE 1: Prefer same address */
                return dst_addr;
            }
            netif = rt->netif;
        }
    }

    /* Resolve by preferred interface */
    src_addr = ipnet_ip6_get_src_addr_by_rules(vr,
                                               dst_addr,
                                               netif,
                                               flags);

    if (src_addr == IP_NULL
        && rt != IP_NULL
        && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE))
    {
        if (rt->rt_template)
            rt = rt->rt_template;

        if (rt->gateway != IP_NULL
            && rt->gateway->sa_family == IP_AF_INET6
            && IP_BIT_ISFALSE(rt->hdr.flags,
                              IPNET_RTF_TUNNELEND | IPNET_RTF_GATEWAY))
            /* Default source address was specified in the route table */
            src_addr = &((struct Ip_sockaddr_in6 *) rt->gateway)->sin6_addr;
    }

    return src_addr;
}


/*
 *===========================================================================
 *                    ipnet_ip6_init
 *===========================================================================
 * Description: Initializes the routing table(s) for IPv6.
 * Parameters:
 * Returns:     0 = success, 0< = error code.
 *
 */
#include "ipnet_eth.h"
IP_GLOBAL int
ipnet_ip6_init(void)
{
    ipnet->ip6.addr_slab
        = ipcom_slab_new("IPNET IPv6 address",
                         IPCOM_SLAB_F_NO_LOCKING | IPCOM_SLAB_F_ZERO | IPCOM_SLAB_F_FEW,
                         sizeof(Ipnet_ip6_addr_entry) + sizeof(Ipnet_mcast_addr),
                         0,
                         IP_NULL,
                         IP_NULL,
                         ipnet->memory_pool);

    ipnet->ip6.addrs = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_ip6_addr_obj_func,
                                      (Ipcom_hash_key_func) ipnet_ip6_addr_key_func,
                                      (Ipcom_hash_cmp_func) ipnet_ip6_addr_cmp_func);

    ipnet->ip6.addrs_ignore_if = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_ip6_addr_ignore_if_obj_func,
                                                (Ipcom_hash_key_func) ipnet_ip6_addr_ignore_if_key_func,
                                                (Ipcom_hash_cmp_func) ipnet_ip6_addr_ignore_if_cmp_func);

    if (ipnet->ip6.addrs == IP_NULL && ipnet->ip6.addrs_ignore_if == IP_NULL)
        return -IP_ERRNO_ENOMEM;

#ifdef IPNET_USE_RFC4941
    /* RFC 4941, chapter 5. Defined Constants

       DESYNC_FACTOR -- A random value within the range 0 -
       MAX_DESYNC_FACTOR.  It is computed once at system start (rather
       than each time it is used) and must never be greater than
       (TEMP_VALID_LIFETIME - REGEN_ADVANCE).
    */
    /* Both TEMP_VALID_LIFETIME and REGEN_ADVANCE can be configured
       per interface, so the calculated DESYNC_FACTOR is checked
       against those values each time it is used.
    */
    ipnet->ip6.desync_factor = (ipcom_random()
                                % (IPNET_IPV6_RFC4941_MAX_DESYNC_FACTOR + 1));
#endif

#ifdef IPNET_DEBUG
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_ip6_fragment_timeout,
                            (Ipnet_timeout_to_string_f) ipnet_ip6_fragment_tmo_to_string);
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_ip6_addr_timeout,
                            (Ipnet_timeout_to_string_f) ipnet_ip6_addr_to_string);
    ipnet_icmp6_init();
#endif

#ifdef IPNET_USE_NETLINK
    ipnet_rtnetlink_ip6_init();
#endif
    return 0;
}


/*
 *===========================================================================
 *                         ipnet_ip6_get_addr_type
 *===========================================================================
 * Description: Returns type of the specified address.
 * Parameters:  ipaddr - The address for which the type will be returned.
 *              vr - The route tab the address must be assigned to.
 *              netif - The network interface the address should be assigned
 *              to or IP_NULL if any interface might do.
 *
 * Returns:     IPNET_ADDR_TYPE_xxx constant
 */


/*
 *===========================================================================
 *                   ipnet_ip6_get_addr_type_and_entry
 *===========================================================================
 * Description: Returns the type and the address entry.
 * Parameters:  ipaddr - address for which the address entry should be returned
 *              vr - virtual router index
 *              netif - interface the address must be assigned to or IP_NULL
 *              addrp - location where the address entry should be stored.
 * Returns:     The address type.
 *
 */
IP_STATIC enum Ipnet_addr_type
ipnet_ip6_get_addr_type_and_entry(IP_CONST struct Ip_in6_addr *ipaddr,
                                  Ip_u16 vr,
                                  Ipnet_netif *netif,
                                  Ipnet_ip6_addr_entry **addrp)
{
    Ipnet_ip6_addr_entry *addr;

    *addrp = IP_NULL;

#ifdef IPCOM_USE_INET
    if (IP_IN6_IS_ADDR_V4MAPPED(ipaddr))
    {
        Ip_u32 ipv4_addr;

        ipcom_memcpy(&ipv4_addr, &ipaddr->in6.addr32[3], sizeof(ipv4_addr));
        return ipnet_ip4_get_addr_type(ipv4_addr, vr, netif);
    }
#endif /* IPCOM_USE_INET */

    addr = ipnet_ip6_get_addr_entry(ipaddr, vr, netif);
    while (addr != IP_NULL)
    {
        if (IP_BIT_ISSET(addr->netif->ipcom.flags, IP_IFF_UP))
        {
            *addrp = addr;
            if (IP_UNLIKELY(IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE)))
                return IPNET_ADDR_TYPE_TENTATIVE;
            return addr->type;
        }

        do {
            /* Move to the next interface and skip all entries using the wrong interface if netif != IP_NULL */
            addr = addr->next_dup_addr;
        } while (addr != IP_NULL && netif != IP_NULL && addr->netif != netif);
    }

    if (IP_IN6_IS_ADDR_UNSPECIFIED(ipaddr))
        return IPNET_ADDR_TYPE_ANY;

    return IPNET_ADDR_TYPE_NOT_LOCAL;
}


/*
 *===========================================================================
 *                   ipnet_ip6_get_addr_type_and_entry2
 *===========================================================================
 * Description: Returns the type and the associated address entry if the
 *              address is assigned to the local node.
 * Parameters:  ipaddr - an IPv6 address
 *              netif - interface that the address must be assigned to or
 *                      IP_NULL if the address can be assigned to any
 *                      interface.
 *              addrp - location where the address entry will be stored.
 * Returns:     Type of address, IPNET_ADDR_TYPE_xxx constant.
 *
 */
IP_STATIC enum Ipnet_addr_type
ipnet_ip6_get_addr_type_and_entry2(IP_CONST struct Ip_in6_addr *ipaddr,
                                   Ipnet_netif *netif,
                                   Ipnet_ip6_addr_entry **addrp)
{
    ip_assert(netif != IP_NULL);
    return ipnet_ip6_get_addr_type_and_entry(ipaddr, netif->vr_index, netif, addrp);
}


/*
 *===========================================================================
 *                         ipnet_ip6_get_addr_type2
 *===========================================================================
 * Description: Returns type of the specified address.
 * Parameters:  ipaddr - The address for which the type will be returned.
 *              netif - The network interface the address must be assigned to
 * Returns:     Type of address, IPNET_ADDR_TYPE_xxx constant.
 */
IP_GLOBAL enum Ipnet_addr_type
ipnet_ip6_get_addr_type2(IP_CONST struct Ip_in6_addr *ipaddr,
                        Ipnet_netif *netif)
{
    Ipnet_ip6_addr_entry *addr;
    return ipnet_ip6_get_addr_type_and_entry2(ipaddr, netif, &addr);
}


/*
 *===========================================================================
 *                         ipnet_ip6_get_addr_type
 *===========================================================================
 * Description: Returns type of the specified address.
 * Parameters:  ipaddr - The address for which the type will be returned.
 *              vr - The route tab the address must be assigned to.
 *              netif - The network interface the address should be assigned
 *                      to or IP_NULL if any interface might do.
 * Returns:     Type of address, IPNET_ADDR_TYPE_xxx constant.
 */
IP_GLOBAL enum Ipnet_addr_type
ipnet_ip6_get_addr_type(IP_CONST struct Ip_in6_addr *ipaddr,
                        Ip_u16 vr,
                        Ipnet_netif *netif)
{
    Ipnet_ip6_addr_entry *addr;
    return ipnet_ip6_get_addr_type_and_entry(ipaddr, vr, netif, &addr);
}


/*
 *===========================================================================
 *                         ipnet_ip6_get_addr_entry
 *===========================================================================
 * Description: Returns the address entry for the specified address.
 * Parameters:  ipaddr - The address for which the type will be returned.
 *              vr - The route tab the address must be located at
 *              netif - The network interface the address should be assigned
 *              to or IP_NULL if any interface might do.
 * Returns:     The address entry or IP_NULL if the address does not exists.
 */
IP_GLOBAL Ipnet_ip6_addr_entry *
ipnet_ip6_get_addr_entry(IP_CONST struct Ip_in6_addr *ipaddr,
                         Ip_u16 vr,
                         Ipnet_netif *netif)
{
    Ipnet_ip6_addr_lookup l;

    l.addr     = ipaddr;
    l.vr_index = vr;

    if (netif == IP_NULL)
        return ipcom_hash_get(ipnet->ip6.addrs_ignore_if, &l);

    l.ifindex     = netif->ipcom.ifindex;
    return ipcom_hash_get(ipnet->ip6.addrs, &l);
}


/*
 *===========================================================================
 *                    ipnet_ip6_process_exthdr
 *===========================================================================
 * Description: Processes all the extension headers up to, but not including,
 *              any transport layer protocol.
 * Parameters:  param - Parameters needed to process extension headers.
 * Returns:     >0 The upper layer protocol starting at (*ppkt)->start.
 *              <0 Error code.
 *              0  Discard packet
 */
IP_GLOBAL int
ipnet_ip6_process_exthdr(Ipnet_process_ext_param *param)
{
    Ipcom_pkt          *reassembly_list;
    Ip_pkt_ip6_ext_hdr *ext_hdr;
    Ip_u8               this_hdr;
    int                 payload_len;
    Ipcom_pkt          *pkt;
    int                 ret;
    int                 prev_nxt_start;
#ifdef IPCOM_USE_MIB2
    Ipnet_netif *netif = ipnet_if_indextonetif((*param->ppkt)->vr_index, (*param->ppkt)->ifindex);
#endif

    param->icmp6_code       = 0;
    param->icmp6_type       = 0;
    param->param_prob_start = (*param->ppkt)->start + IPNET_IP6_HDR_SIZE;
    param->ipsec_tunnel     = 0;

    for (;;)
    {
        pkt = *param->ppkt;
        /* NOTE: do not refer to the IPv6 header via the pkt->ipstart
           since that points to IPv6 header used to deliver the packet,
           but the caller might want to process an embedded IPv6 header
           (at offset pkt->start) while still retaining a pointer to
           the real IPv6 header */
        param->ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->start];

        this_hdr = param->ip6_hdr->nxt;
        payload_len = ip_ntohs(param->ip6_hdr->plen);
        prev_nxt_start = pkt->start + ip_offsetof(Ipnet_pkt_ip6, nxt);
        pkt->start += IPNET_IP6_HDR_SIZE;

        for (;;)
        {
            unsigned ext_hdr_len;

            if (pkt->start > pkt->end || pkt->end - pkt->start > payload_len)
            {
                /* Malformed packet */
#ifdef IPCOM_USE_MIB2
                if (netif != IP_NULL)
                {
                    IPCOM_MIB2_NETIF(ipv6IfStatsInTruncatedPkts++);
                    IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInTruncatedPkts, 1);
                    IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInTruncatedPkts, 1, netif);
                }
#endif
                return -IP_ERRNO_EINVAL;
            }

            if (param->route == IP_TRUE && this_hdr != IP_IPPROTO_HOPOPTS)
            {
                /* No other extension header should be examined by routers */
                return (int)this_hdr;
            }

            ext_hdr = (Ip_pkt_ip6_ext_hdr*) &pkt->data[pkt->start];

            switch (this_hdr)
            {
            case IP_IPPROTO_HOPOPTS:
            case IP_IPPROTO_DSTOPTS:
            case IP_IPPROTO_ROUTING:
            case IP_IPPROTO_FRAGMENT:
            case IP_IPPROTO_MH:
                /* Verify that the options is within the boundary of
                   the IP datagram. Smallest possible extension header
                   length is 8 bytes */
                if (payload_len < 8
                    || pkt->end < pkt->start + ((ext_hdr->len + 1) << 3))
                {
#ifdef IPCOM_USE_MIB2
                    if (netif != IP_NULL)
                    {
                        IPCOM_MIB2_NETIF(ipv6IfStatsInTruncatedPkts++);
                        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInTruncatedPkts, 1);
                        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInTruncatedPkts, 1, netif);
                    }
#endif
                    return -IP_ERRNO_EINVAL;
                }
                break;
            default:
                /* Transport layer protocols does not use the extension header format */
                break;
            }

            switch (this_hdr)
            {
            case IP_IPPROTO_HOPOPTS:
            case IP_IPPROTO_DSTOPTS:
                if (ipnet_ip6_process_ext_options(this_hdr,
                                                  (Ip_pkt_ip6_opt *) (ext_hdr + 1),
                                                  ((ext_hdr->len + 1) << 3) - sizeof(Ip_pkt_ip6_opt),
                                                  param) == IP_FALSE)
                    /* Discard this packet */
                    return 0;
                break;

            case IP_IPPROTO_ROUTING:
                ret  = ipnet_ip6_process_rthdr((Ip_pkt_ip6_rthdr *) ext_hdr, param);
                if (ret > 0)
                {
                    /* Forward the packet */
                    pkt->start = pkt->ipstart;
                    return IP_IPPROTO_ROUTING;
                }
                if (ret < 0)
                    /* Send ICMPv6 error message */
                    return 0;
                /* ==0 continue processing */
                break;

            case IP_IPPROTO_FRAGMENT:
                /* Dont do any parsing of fragments when we're in error mode */
                if (param->error)
                    return IP_IPPROTO_FRAGMENT;
                if (ip_ntohs(param->ip6_hdr->plen) & 0x7
                    && IP_BIT_ISSET(((Ip_pkt_ip6_frag *) ext_hdr)->ip6f_offlg, IP_IP6F_MORE_FRAG))
                {
                    /* Not a multiple of 8, send ICMP parameter problem */
                    param->icmp6_type = IPNET_ICMP6_PARAM_PROB;
                    param->icmp6_code = IPNET_ICMP6_PARAM_PROB_ERR_HDR;
                    param->param_prob_start = pkt->ipstart + ip_offsetof(Ipnet_pkt_ip6, plen);
                    return 0;
                }
                IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IP6_REASSEMBLY);

                reassembly_list = ipnet_reassembly(&ipnet->ip6.reassembly_list,
                                                   pkt,
                                                   ipnet_ip6_is_part_of_same_pkt,
                                                   ipnet_ip6_get_offset,
                                                   ipnet_ip6_more_fragments,
                                                   (Ipnet_timeout_handler) ipnet_ip6_fragment_timeout,
                                                   IP_IP6F_SIZE);

                if (reassembly_list == IP_NULL)
                    return IP_IPPROTO_FRAGMENT;

                pkt = ipnet_create_reassembled_packet(reassembly_list,
                                                      IPNET_IP6_HDR_SIZE,
                                                      IP_IP6F_SIZE,
                                                      ipnet_ip6_update_ip_header,
                                                      ipnet_ip6_reassembly_err_hnd);
                *param->ppkt = pkt;
                if (pkt == IP_NULL)
                {
#ifdef IPCOM_USE_MIB2
                    if (netif != IP_NULL)
                    {
                        IPCOM_MIB2_NETIF(ipv6IfStatsInDiscards++);
                        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInDiscards, 1);
                        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInDiscards, 1, netif);
                        IPCOM_MIB2_NETIF(ipv6IfStatsReasmFails++);
                        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsReasmFails, 1);
                        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsReasmFails, 1, netif);
                    }
#endif
                    return -IP_ERRNO_ENOBUFS;
                }
                param->ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->start];
                break;

#ifdef IPIPSEC2
            case IP_IPPROTO_ESP:
            case IP_IPPROTO_AH:
/*
            case IP_IPPROTO_IPV6:
*/

                {
                    int  ipsec_ret;
                    Ipipsec_param   ipsec_param;
                    int pkt_org_start = pkt->start;
                    int pkt_org_end   = pkt->end;

                    /* Dont do any parsing of ipsec when we're in error mode */
                    if (param->error)
                        return (int)this_hdr;

                    /* Fill in ipsec_param. */
                    ipcom_memcpy(&ipsec_param.key.src.in6, &param->ip6_hdr->src, sizeof(struct Ip_in6_addr));
                    ipcom_memcpy(&ipsec_param.key.dst.in6, &param->ip6_hdr->dst, sizeof(struct Ip_in6_addr));
                    ipsec_param.key.proto  = this_hdr;
                    ipsec_param.key.domain = IP_AF_INET6;

#ifdef IPIPSEC_USE_DAEMONS
                    ipsec_ret = ipipsec_send_daemon_pipe(pkt, &ipsec_param, 1);
#else
                    ipsec_ret = ipipsec_input(pkt, &ipsec_param);
#endif
                    param->ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->ipstart];

                    if (ipsec_ret < IPIPSEC_OK)
                        return ipsec_ret;  /* discard ipsec datagram. */
                    else if (ipsec_ret == IPIPSEC_KEPT)
                        return -IP_ERRNO_EIPSEC; /* async */
                    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE | IPCOM_PKT_FLAG_NO_RX_CACHE);
                    if (ipsec_ret == IPIPSEC_TUNNELED)
                    {
                        /* Tunneled packet. */
                        param->ipsec_tunnel = 1;
                        if (ipsec_param.key.domain == IP_AF_INET6)
                            return IP_IPPROTO_IPV6;
                        else
                            return IP_IPPROTO_IPIP;
                    }
                    this_hdr = ipsec_param.key.proto;
                    payload_len -= (pkt->start - pkt_org_start);
                    payload_len -= (pkt_org_end - pkt->end);
                }
                continue; /* Do not update payload_len & pkt->start outside the switch below! */
#endif /* IPIPSEC2 */

            case IP_IPPROTO_MH:
            default:
                /* Application layer protocol (or unsupported protocol/extension header) */
                param->param_prob_start = prev_nxt_start;
                return (int)this_hdr;
            }

            if (this_hdr == IP_IPPROTO_FRAGMENT)
            {
                /* All fragments received, continue process the reassembled packet */
#ifdef IPCOM_USE_MIB2
                if (netif != IP_NULL)
                {
                    IPCOM_MIB2_NETIF(ipv6IfStatsReasmOKs++);
                    IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsReasmOKs, 1);
                    IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsReasmOKs, 1, netif);
                }
#endif
                break;
            }

            if (ext_hdr->nxt == IP_IPPROTO_HOPOPTS)
            {
                /* Hop-by-hop must be the first extension header or not
                   present at all */
                param->icmp6_type = IPNET_ICMP6_PARAM_PROB;
                param->icmp6_code = IPNET_ICMP6_PARAM_PROB_NXT_HDR;
                param->param_prob_start += ip_offsetof(Ip_pkt_ip6_ext_hdr, nxt);
                return 0;
            }

            this_hdr = ext_hdr->nxt;
            ext_hdr_len = (ext_hdr->len + 1) << 3;
            payload_len -= ext_hdr_len;
            prev_nxt_start = pkt->start + ip_offsetof(Ip_pkt_ip6_ext_hdr, nxt);
            pkt->start += ext_hdr_len;
            param->param_prob_start += ext_hdr_len;
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_ip6_is_icmp6_error
 *===========================================================================
 * Description: Returns if this is a ICMPv6 error message.
 * Parameters:  pkt - An incoming packet.
 * Returns:     IP_TRUE if this is an ICMPv6 packet.
 *
 */
IP_STATIC Ip_bool
ipnet_ip6_is_icmp6_error(Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip6      *ip6_hdr;
    Ip_pkt_ip6_ext_hdr *ext_hdr;
    Ipnet_pkt_icmp6    *icmp6;
    Ip_u8               this_hdr;
    int                 start = pkt->ipstart + IPNET_IP6_HDR_SIZE;
    Ip_bool             at_application_hdr = IP_FALSE;

    ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->ipstart];
    this_hdr = ip6_hdr->nxt;

    while (!at_application_hdr)
    {
        if (pkt->end - start < (int) IPNET_ICMPV6_HDR_SIZE)
            return IP_FALSE;

        ext_hdr = (Ip_pkt_ip6_ext_hdr*) &pkt->data[start];
        switch (this_hdr)
        {
        case IP_IPPROTO_HOPOPTS:
        case IP_IPPROTO_DSTOPTS:
        case IP_IPPROTO_FRAGMENT:
        case IP_IPPROTO_ROUTING:
        case IP_IPPROTO_AH:
        case IP_IPPROTO_MH:
            this_hdr = ext_hdr->nxt;
            start += (ext_hdr->len + 1) << 3;
            break;
        default:
            at_application_hdr = IP_TRUE;
            break;
        }
    }
    if (this_hdr != IP_IPPROTO_ICMPV6)
        return IP_FALSE;

    icmp6 = (Ipnet_pkt_icmp6 *) &pkt->data[start];
    return IP_BIT_ISFALSE(icmp6->icmp6_type, IPNET_ICMP6_INFOMSG_MASK);
}


/*
 *===========================================================================
 *                    ipnet_ip6_try_fast_forward_path
 *===========================================================================
 * Description: Fast packet forward path.
 * Parameters:  innetif - Incoming network interface.
 *              pkt - The incoming packet.
 *              ip_hdr - The IPv6 header for the packet.
 * Returns:     One of the IPNET_FORWARD_CACHE_xxx constants.
 *
 */
#ifdef IPNET_FASTPATH
IP_STATIC int
ipnet_ip6_try_fast_forward_path(Ipnet_netif *innetif, Ipcom_pkt *pkt, Ipnet_pkt_ip6 *ip_hdr)
{
    Ipnet_netif        *netif;
    Ip_u32              path_mtu;
    Ip_u32              ver_class_flow = IP_GET_32ON16(&ip_hdr->ver_class_flow);
    Ipnet_ip6_rt_cache *cache = innetif->inet6_last_forward_cache;
    int                 pass;

    /* Try the last used entry in the first pass and look in the
       forward cache in the 2:nd pass */
    for (pass = 0; pass < 2; pass++)
    {
        if (ipnet_route_cache_is_valid(cache->rt_id)
            && cache->ifindex == innetif->ipcom.ifindex
            && cache->ver_class_flow == ver_class_flow
            && cache->nxt == ip_hdr->nxt
            && IP_IN6_ARE_ADDR_EQUAL(&cache->dst, &ip_hdr->dst)
            && IP_IN6_ARE_ADDR_EQUAL(&cache->src, &ip_hdr->src)
            && ip_hdr->hlim > 1)
        {
            netif = cache->rt_next_hop->netif;
            if (cache->rt_next_hop->metrics.rmx_mtu)
                path_mtu = cache->rt_next_hop->metrics.rmx_mtu;
            else
                path_mtu = netif->ipcom.mtu;

            if (IP_LIKELY(path_mtu >= (Ip_u32) (pkt->end - pkt->start)
                          && pkt->recurs_level < IPCOM_PKT_MAX_RECURSE_LEVEL
                          && pkt->start >= netif->ipcom.link_hdr_size))
            {
#ifdef IPFIREWALL_USE_IP_FILTER
                if (ipfirewall != IP_NULL
                    && ipfirewall_input_hook(&innetif->ipcom, pkt, (void *)cache))
                    return IPNET_FORWARD_CACHE_BLOCKED_BY_FW;
#endif
                --ip_hdr->hlim;
                pkt->ifindex = netif->ipcom.ifindex;
#ifdef IPFIREWALL_USE_IP_FILTER
                if (ipfirewall != IP_NULL
                    && ipfirewall_output_hook(&netif->ipcom, pkt, (void *)cache))
                    return IPNET_FORWARD_CACHE_BLOCKED_BY_FW;
#endif
                ++cache->rt_next_hop->use;
                IPCOM_MIB2_NETIF(ipv6IfStatsOutForwDatagrams++);
                IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInForwDatagrams, 1);
                IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInForwDatagrams, 1, innetif);
                IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutForwDatagrams, 1);
                IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutForwDatagrams, 1, netif);
                IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutTransmits, 1);
                IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutTransmits, 1, netif);
                IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
                IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutOctets, pkt->end - pkt->start, netif);

                ++pkt->recurs_level;
                IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
                IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED);
#ifdef IPROHC
                if (iprohc.opened
                    && (netif->ipcom.type != IP_IFT_MPLS_TUNNEL)
                    && (netif->ipcom.type != IP_IFT_TUNNEL))
                {                 /*
                                   * Do ROHC on a not (MPLS and TUNNEL) interface and
                                   * packet has not been processed by MPLS.
                                   */
                    if (iprohc_output_hook(&netif->ipcom, pkt) != IPCOM_SUCCESS)
                    {
                        /* ROHC failed */
                        IPCOM_LOG1(WARNING, "Discarding IPv6 datagram on %s, ROHC failed.", netif->ipcom.name);
                        return IPNET_FORWARD_CACHE_BLOCKED_BY_ROHC;
                    }
                }
#endif /* IPROHC */

                if (netif->link_ip6_output(netif, cache->rt_next_hop, pkt))
                    IPCOM_MIB2(ipv6DiscardedRoutes++);

                innetif->inet6_last_forward_cache = cache;
                return IPNET_FORWARD_CACHE_TAKEN;
            }
        }
        /* The per interface one-behind cache did not match, lets try
           the shared forwarding cache. */
        IPNET_IP6_GET_FORWARD_CACHE_ENTRY(cache, ip_hdr, ver_class_flow);
    }

    return IPNET_FORWARD_CACHE_MISS;
}
#endif /* IPNET_FASTPATH */


/*
 *===========================================================================
 *                    ipnet_ip6_input
 *===========================================================================
 * Description: Handler for IPv6 packets. (pkt->start is offset to IPv6 header).
 * Parameters:  pkt - Received IPv6 packet.
 * Returns:     0 = succcess, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_icmp6_param       icmp6_param;
    Ipnet_process_ext_param ext_param;
    Ipnet_pkt_ip6          *ip6_hdr;
    int                     proto;
    enum Ipnet_addr_type    addr_type;
    int                     ret = -IP_ERRNO_EINVAL;
#ifdef IPFIREWALL_USE_IP_FILTER
    Ip_bool                 fw_checked = IP_FALSE;
#endif
    int                     nat_t    = IP_UDP_NAT_T_DISABLED;


    ip_assert(netif != IP_NULL);
    ip_assert(pkt != IP_NULL);
    ip_assert(pkt->data != IP_NULL);
    ip_assert(pkt->end <= pkt->maxlen);
    ip_assert(pkt->start <= pkt->end);
    ip_assert2((pkt->data[pkt->start] & 0xf0) == 0x60);

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 2, IPCOM_WV_NETDEVENT_START,
                       ipnet_ip6_input, IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
    IPNET_STATS(ip6_input++);
    IPCOM_MIB2_NETIF(ipv6IfStatsInReceives++);
    IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInReceives, 1);
    IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInReceives, 1, netif);
    IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInOctets, pkt->end - pkt->start);
    IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInOctets, pkt->end - pkt->start, netif);

    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPV6);
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_IPV4);

    if (IP_UNLIKELY(IP_BIT_ISSET(netif->flags2, IPNET_IFF2_IPV6_DISABLED)))
        goto cleanup;

    if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER)))
        /* Do not process packets that was sent to a link layer unicast
           address that is not ours, AF_PACKET sockets will get this packet
           if the interface is promiscuous mode */
        goto cleanup;

    /* Set the route table index in the packet. */
    if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_LOOPBACK))
        pkt->ifindex = netif->ipcom.ifindex;
    /* else: use the preset values for ifindex and */

#ifdef IPIPSEC2
 parse_ip_datagram:
#endif
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IP6_INPUT);

    pkt->ipstart = pkt->start;

#ifdef IPROHC
    if (iprohc.opened)
    {
        ret = iprohc_input_hook(&netif->ipcom, pkt);
        if (ret != IPCOM_SUCCESS)
        {
            /* Decompress fails, discard the packet */
            IPCOM_LOG1(WARNING, "Discarding rcv'ed IPv6 datagram on %s, ROHC failed.", netif->ipcom.name);
            ret = -IP_ERRNO_EROHC;
            goto cleanup;
        }
    }
#endif /* IPROHC */

    ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->start];

    if (IP_UNLIKELY(pkt->end - pkt->start < IPNET_IP6_HDR_SIZE)
        || IP_UNLIKELY(IPNET_IP6_GET_VER(ip6_hdr) != 6))
    {
        /* Can not be a valid IPv6 packet */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 3, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip6_input, IPCOM_WV_NETD_BADHLEN,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_input_hdr_trunc++);
        IPCOM_MIB2_NETIF(ipv6IfStatsInHdrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInHdrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInHdrErrors, 1, netif);
        goto cleanup;
    }

    if (IP_UNLIKELY(pkt->end < pkt->ipstart + (int)ip_ntohs(ip6_hdr->plen) + IPNET_IP6_HDR_SIZE))
    {
        /* Did not receive whole packet */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 4, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip6_input, IPCOM_WV_NETD_BADLEN,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_input_inval_len++);
        IPCOM_MIB2_NETIF(ipv6IfStatsInHdrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInHdrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInHdrErrors, 1, netif);
        goto cleanup;
    }
    pkt->end = pkt->ipstart + (int)ip_ntohs(ip6_hdr->plen) + IPNET_IP6_HDR_SIZE;
    pkt->tlstart = pkt->ipstart + IPNET_IP6_HDR_SIZE;

    if (IP_LIKELY(netif->inet6_addr_type_cache.rt_id == ipnet->route_cache_id)
        && IP_IN6_ARE_ADDR_EQUAL(&netif->inet6_addr_type_cache.addr, &ip6_hdr->dst))
    {
        addr_type               = netif->inet6_addr_type_cache.type;
        ext_param.ip6_dst_entry = netif->inet6_addr_type_cache.addr_entry;
    }
    else
    {
        addr_type = ipnet_ip6_get_addr_type_and_entry2(&ip6_hdr->dst,
                                                       IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
                                                       ? ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex)
                                                       : netif,
                                                       &ext_param.ip6_dst_entry);

        if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
        {
            if (IP_IN6_IS_ADDR_LOOPBACK(&ip6_hdr->dst))
            {
                /* Destination address is ::1 and the IP datagram was
                   received on an interface that does have that address
                   assigned. Must be a spoofed IP datagram. */
                goto cleanup;
            }
#ifdef IPNET_STRICT_MODE
            if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK))
                /* In strict mode, check all addresses if Rx interface is loopback. */
#endif
                /* In loose mode, always check all addresses */
                addr_type = ipnet_ip6_get_addr_type_and_entry(&ip6_hdr->dst, pkt->vr_index, IP_NULL, &ext_param.ip6_dst_entry);
        }

        /* Add this address type lookup to the cache */
        netif->inet6_addr_type_cache.rt_id = ipnet->route_cache_id;
        IPNET_IP6_SET_ADDR(&netif->inet6_addr_type_cache.addr, &ip6_hdr->dst);
        netif->inet6_addr_type_cache.type = addr_type;
        netif->inet6_addr_type_cache.addr_entry = ext_param.ip6_dst_entry;
    }

    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
    {
#ifdef IPNET_FASTPATH
        switch (ipnet_ip6_try_fast_forward_path(netif, pkt, ip6_hdr))
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
        default:
            break;
        }
#endif /* IPNET_FASTPATH */
    }
    else
    {
#ifndef IPMIP6
        if (ipnet->ip6.rx_cache.sock != IP_NULL
            && ip6_hdr->nxt == ipnet->ip6.rx_cache.proto
            && IP_GET_32ON16(&pkt->data[pkt->tlstart]) == ipnet->ip6.rx_cache.tldata
            && IP_IN6_ARE_ADDR_EQUAL(&ip6_hdr->dst, &ipnet->ip6.rx_cache.dst)
            && IP_IN6_ARE_ADDR_EQUAL(&ip6_hdr->src, &ipnet->ip6.rx_cache.src)
            && ipnet->ip6.rx_cache.sock->vr_index == pkt->vr_index)
        {
            Ip_u32        chk = pkt->chk;
            Ipnet_socket *cache_sock = ipnet->ip6.rx_cache.sock;
            Ipnet_rx_cache_fast_input_f fast_input_func = ipnet->ip6.rx_cache.fast_input_func;

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

            /* Calculate the 'pseudo header' part of the checksum */
            pkt->chk += ipnet_ip6_pseudo_header_checksum_update(&ip6_hdr->src,
                                                                &ip6_hdr->dst,
                                                                ip6_hdr->nxt,
                                                                (Ip_u16) (pkt->end - pkt->tlstart));
            if (fast_input_func(cache_sock, pkt, &nat_t))
                return 0;

            if (IP_UNLIKELY(nat_t != 0))
            {
                switch (nat_t)
                {
#ifdef IPNET_USE_TUNNEL
                case IP_UDP_NAT_T_DSMIPV6:
                    pkt->start += IPNET_IP6_HDR_SIZE;
                    ipnet_tunnel_dsmipv6_input(pkt);
                    return 0;
#endif
                }
            }


            pkt->chk = chk;
        }
#endif
    }

    if ((addr_type == IPNET_ADDR_TYPE_NOT_LOCAL
         && (IP_IN6_IS_ADDR_LINK_LOCAL(&ip6_hdr->dst)
             || IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)))
        || addr_type == IPNET_ADDR_TYPE_ANY)
    {
        /* Not to me */
        ret = -IP_ERRNO_ENOTOME;
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 5, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip6_input, IPCOM_WV_NETD_BADADDR,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_input_not_to_me++);
        IPCOM_MIB2_NETIF(ipv6IfStatsInAddrErrors++);
        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInAddrErrors, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInAddrErrors, 1, netif);
        goto cleanup;
    }

    if (addr_type == IPNET_ADDR_TYPE_MULTICAST)
    {
        netif->imcasts++;
        IPCOM_MIB2_NETIF(ipv6IfStatsInMcastPkts++);
        IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInMcastPkts, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInMcastPkts, 1, netif);
        IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInMcastOctets, pkt->end - pkt->start);
        IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInMcastOctets, pkt->end - pkt->start, netif);
    }

    ext_param.ppkt  = &pkt;
    ext_param.route = (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL);
    ext_param.error = IP_FALSE;

#ifdef IPNET_USE_MCAST_ROUTING
    if (IP_IN6_IS_ADDR_MULTICAST(&ip6_hdr->dst))
    {
        Ip_bool skip = IP_FALSE;

        if (ip6_hdr->nxt == IP_IPPROTO_HOPOPTS)
        {
            /* This is a temporary solution to a problem that requires
               some restruction of how the stack processes IPv6
               extension headers. Hop-by-hop option must be processed
               before multicast and/or unicast forwarding takes place,
               while all other should only be processed if the packet
               should be delivered locally. ipnet_ip6_process_exthdr()
               can do that for the unicast case. Multicast packets
               might be delivered locally AND forwarded, that case is
               not covered. */
            Ip_pkt_ip6_ext_hdr *ext_hdr = (Ip_pkt_ip6_ext_hdr *) (ip6_hdr + 1);

            ext_param.ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->start];
            skip = !ipnet_ip6_process_ext_options(IP_IPPROTO_HOPOPTS,
                                                  (Ip_pkt_ip6_opt *) (ext_hdr + 1),
                                                  ((ext_hdr->len + 1) << 3) - sizeof(Ip_pkt_ip6_opt),
                                                  &ext_param);
        }

        if (!skip && IPNET_IP6_MC_SCOPE(&ip6_hdr->dst) > IPNET_IP6_SCOPE_LINK_LOCAL)
        {
            Ipnet_pkt_icmp6 *icmp6 = (Ipnet_pkt_icmp6*)&pkt->data[pkt->start];

            if (ip6_hdr->nxt != IP_IPPROTO_ICMPV6)
                ipnet_ip6_mcast_input(netif, pkt);
            else
            {
                switch (icmp6->icmp6_type)
                {
                case IPNET_MLD_LISTENER_QUERY:
                case IPNET_MLD_V1_LISTENER_REPORT:
                case IPNET_MLD_V2_LISTENER_REPORT:
                case IPNET_MLD_LISTENER_REDUCTION:
                    break;
                default:
                    ipnet_ip6_mcast_input(netif, pkt);
                    break;
                }
            }
        }
    }
#endif /* IPNET_USE_MCAST_ROUTING */

    proto = ipnet_ip6_process_exthdr(&ext_param);

    if (proto < 0)
    {
        if (proto == -IP_ERRNO_EIPSEC)
        {
            /* IPSEC kept the packet for later reception */
            return 0;
        }
        ret = proto;
        goto cleanup;
    }

    if (IP_UNLIKELY(addr_type == IPNET_ADDR_TYPE_TENTATIVE))
    {
        Ipnet_pkt_icmp6 *icmp6 = (Ipnet_pkt_icmp6*) &pkt->data[pkt->start];

        /* Silently drop all but ICMPv6 NS and NA packages while
           in tentative mode */
        ret = 0;
        if (proto != IP_IPPROTO_ICMPV6)
            goto cleanup;
        if (icmp6->icmp6_type != IPNET_ND_NEIGHBOR_SOLICIT
            && icmp6->icmp6_type != IPNET_ND_NEIGHBOR_ADVERT)
            goto cleanup;
    }

    if (proto == 0)
    {
        if (ext_param.icmp6_type && ipnet_ip6_is_icmp6_error(pkt) == IP_FALSE)
        {
            /* Send back ICMP message */
            ipnet_icmp6_param_init(&icmp6_param, pkt);
            icmp6_param.type = ext_param.icmp6_type;
            icmp6_param.code = ext_param.icmp6_code;
            icmp6_param.data.param_prob_pointer = ext_param.param_prob_start - pkt->ipstart;
            (void)ipnet_icmp6_send(&icmp6_param);
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 6, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_ip6_input, IPCOM_WV_NETD_BADFLAGS,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(ip6_input_inval_exthdr++);
        }

        /* else: Silent drop this packet */
        ret = 0;
        goto cleanup;
    }

    ip6_hdr = ext_param.ip6_hdr;

    if (proto == IP_IPPROTO_ROUTING)
        /* Routing header changed the final destination, route the packet */
        addr_type = IPNET_ADDR_TYPE_NOT_LOCAL;

    IP_BIT_CLR(pkt->flags, IPNET_IP6_PKT_SENT_TO_SOCK);
    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL
        && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ROUTER_ALERT))
    {
        (void) ipnet_raw_input(pkt,
                               IP_FALSE,
                               proto, &ip6_hdr->src, 0, &ip6_hdr->dst, 0,
                               (Ipnet_sock_lookup_f) ipnet_sock_ip6_lookup);
        IP_BIT_SET(pkt->flags, IPNET_IP6_PKT_SENT_TO_SOCK);
    }

    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL
        && IP_IN6_IS_ADDR_MULTICAST(&ip6_hdr->dst))
    {
        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_PROMISC))
            goto cleanup;
        addr_type = IPNET_ADDR_TYPE_MULTICAST;
    }

#ifdef IPIPSEC2
    if (addr_type != IPNET_ADDR_TYPE_NOT_LOCAL && ext_param.ipsec_tunnel)
    {
        if (proto == IP_IPPROTO_IPV6)
            goto parse_ip_datagram;
        if (proto == IP_IPPROTO_IPIP)
        {
#ifdef IPCOM_USE_INET
            return ipnet_ip4_input(netif, pkt);
#else
            ret = -IP_ERRNO_EINVAL;  /* Drop packet, no IPv4 support. */
            goto cleanup;
#endif
        }
    }
#endif /* IPIPSEC2 */

    if (proto == IP_IPPROTO_FRAGMENT && addr_type != IPNET_ADDR_TYPE_NOT_LOCAL)
    {
        /* More fragments needed */
        IPCOM_MIB2_NETIF(ipv6IfStatsReasmReqds++);
        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsReasmReqds, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsReasmReqds, 1, netif);
        return 0;
    }

#ifdef IPIPSEC2
    if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE))
    {
        Ipipsec_param ipsec_param;

        ipcom_memcpy(&ipsec_param.key.src.in6, &ip6_hdr->src, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&ipsec_param.key.dst.in6, &ip6_hdr->dst, sizeof(struct Ip_in6_addr));
        ipsec_param.key.proto  = proto;
        ipsec_param.key.domain = IP_AF_INET6;

        /* Check policy for non ipsec traffic */
        ret = ipipsec_policy_check(pkt, &ipsec_param);
        if (ret < 0)
            goto cleanup;
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
        /* Route the packet */
        Ipnet_ip6_output_param param;

        if (IP_BIT_ISFALSE(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT)
            || IP_BIT_ISTRUE(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD))
        {
            IPCOM_LOG2(DEBUG,
                       "IPv6: Not configured as a router, will not forward packet from %s to %s",
                       ipcom_inet_ntop(IP_AF_INET6, &ip6_hdr->src, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       ipcom_inet_ntop(IP_AF_INET6, &ip6_hdr->dst, ipnet->log_buf + IP_INET6_ADDRSTRLEN, sizeof(ipnet->log_buf)));
            IPCOM_MIB2_NETIF(ipv6IfStatsInAddrErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInAddrErrors, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInAddrErrors, 1, netif);
            goto cleanup;
        }

        if (IP_UNLIKELY(IP_IN6_IS_ADDR_UNSPECIFIED(&ip6_hdr->src)))
            /* Packet sent with unspecified source address must never
               be forwaded. */
            goto cleanup;

        if (IP_UNLIKELY(IP_IN6_IS_ADDR_LINK_LOCAL(&ip6_hdr->src)))
        {
            /* Packet sent with link-local source address must never
               be forwaded.

               RFC 4443, chapter 3.1
               ...
               If the reason for the failure to deliver is that the
               destination is beyond the scope of the source address,
               the Code field is set to 2.  This condition can occur
               only when the scope of the source address is smaller
               than the scope of the destination address (e.g., when a
               packet has a link-local source address and a
               global-scope destination address) and the packet cannot
               be delivered to the destination without leaving the
               scope of the source address.
               ...
            */
            ipnet_icmp6_param_init(&icmp6_param, pkt);
            icmp6_param.type = IPNET_ICMP6_DST_UNREACH;
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_BEYONDSCOPE;
            ret = ipnet_icmp6_send(&icmp6_param);
            goto cleanup;
        }

#ifdef IPNET_STRICT_MODE
        if (ipnet_ip6_get_addr_type(&ip6_hdr->dst, pkt->vr_index, IP_NULL) != IPNET_ADDR_TYPE_NOT_LOCAL)
            /* Destination address is local, but not on this
               interface. Do not route packets to ourselves */
            goto cleanup;
#endif

        param.from                = param.real_from = &ip6_hdr->src;
        param.to                  = param.final_to = &ip6_hdr->dst;
        param.netif               = IP_NULL;
        param.scope_id            = netif->ipcom.ifindex;
        param.chksum_ptr          = IP_NULL;
        param.has_exthdr          = IP_FALSE;
        param.send_icmp6_redirect = (Ip_u8)(proto != IP_IPPROTO_ROUTING);
        param.hlim                = ip6_hdr->hlim;
        param.nexthop             = IP_NULL;

        if (param.hlim-- <= 1)
        {
            /* Send Time Exceeded Message */
            ipnet_icmp6_param_init(&icmp6_param, pkt);
            icmp6_param.type = IPNET_ICMP6_TIME_EXCEEDED;
            icmp6_param.code = IPNET_ICMP6_TIME_EXCEEDED_HLIM;
            ret = ipnet_icmp6_send(&icmp6_param);
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 7, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_ip6_input, IPCOM_WV_NETD_BADFLAGS,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(ip6_input_time_exceeded++);
            IPCOM_MIB2_NETIF(ipv6IfStatsInHdrErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInHdrErrors, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInHdrErrors, 1, netif);
            goto cleanup;
        }
        ip6_hdr->hlim = param.hlim;

        param.flowinfo = IPNET_IP6_GET_FLOW(ip6_hdr);
        param.nxt_hdr  = ip6_hdr->nxt;
        param.tclass   = (Ip_u8)IPNET_IP6_GET_CLASS(ip6_hdr);
        param.cache    = IP_NULL;

        /* The packet will not be fragmented, reuse the IP header */
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR | IPCOM_PKT_FLAG_FORWARDED);
        pkt->start = pkt->ipstart;

#ifdef IPNET_USE_NAT_PT
        {
            Ipnet_ip4_output_param param4;
            Ip_s32 natret;

            natret = ipnet_nat_pt_output_hook(&param4, &pkt);
            switch (natret)
            {
            case IPNET_NAT_PT_TRANSLATED:
                return ipnet_ip4_output(&param4, pkt);
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
#endif

#ifdef IPNET_USE_DIFFSERV
        /* Classifier, meter and marker step */
        ipnet_diffserv_input(pkt);
#endif /* IPNET_USE_DIFFSERV */

        IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInForwDatagrams, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInForwDatagrams, 1, netif);
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
        return ipnet_ip6_output(&param, pkt);
    }

    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_LINK_MULTICAST);

#ifdef IPMIP6
    if (ext_param.ip6_dst_entry != IP_NULL && IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_LOOPBACK))
    {
        /**/
        ext_param.ip6_dst_entry->in_bytes       += pkt->end - pkt->start;
        ext_param.ip6_dst_entry->in_pkts        += 1;
        ext_param.ip6_dst_entry->in_stamp_sec   = ipnet->sec_now;
        ext_param.ip6_dst_entry->in_stamp_msec  = ipnet->msec_now - ipnet->msec_last;
    }
#endif

    /* Calculate the 'pseudo header' part of the checksum */
    switch (proto)
    {
    case IP_IPPROTO_UDP:
#ifdef IPTCP
    case IP_IPPROTO_TCP:
#endif
#ifdef IPNET_USE_MCAST_ROUTING
    case IP_IPPROTO_PIM:
#endif /* IPNET_USE_MCAST_ROUTING */
        pkt->chk += ipnet_ip6_pseudo_header_checksum_update(&ip6_hdr->src,
                                                            &ip6_hdr->dst,
                                                            (Ip_u16) proto,
                                                            (Ip_u16) (pkt->end - pkt->start));
        break;
    default:
        break;
    }

    /* Act on the transport protocol: */
    switch (proto)
    {
    case IP_IPPROTO_ICMPV6:
        ret = ipnet_icmp6_input(netif, &ip6_hdr->src, &ip6_hdr->dst, ip6_hdr->hlim, pkt);
        if (ret < 0)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 8, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_ip6_input, IPCOM_WV_NETD_BADBUF,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(icmp6_input_err++);
            if (ret == -IP_ERRNO_EINVAL)
            {
                IPCOM_MIB2_NETIF(ipv6IfIcmpInErrors++);
                IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpStatsInErrors, 1);
            }
        }
        else
        {
            IPCOM_MIB2_NETIF(ipv6IfStatsInDelivers++);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInDelivers, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInDelivers, 1, netif);
        }
        return ret;

#ifdef IPSCTP
    case IP_IPPROTO_SCTP:
#endif
#ifdef IPTCP
    case IP_IPPROTO_TCP:
#endif
    case IP_IPPROTO_UDP:
        if (proto == IP_IPPROTO_UDP)
        {
            ret = ipnet_udp_input(pkt, IP_IN6_IS_ADDR_MULTICAST(&ip6_hdr->dst),
                                  &ip6_hdr->src, &ip6_hdr->dst,
                                  (Ipnet_sock_lookup_f) ipnet_sock_ip6_lookup,
                                  &nat_t);
            if (IP_UNLIKELY(ret == 0))
            {
                switch (nat_t)
                {
#ifdef IPNET_USE_TUNNEL
                case IP_UDP_NAT_T_DSMIPV6:
                    ipnet_tunnel_dsmipv6_input(pkt);
                    break;
#endif
                }
            }
        }
#ifdef IPSCTP
        else if (proto == IP_IPPROTO_SCTP)
        {
            if (!IP_IN6_IS_ADDR_MULTICAST(&ip6_hdr->dst))
            {
                ret = ipsctp_input(pkt,
                                  &ip6_hdr->src, &ip6_hdr->dst);
            }
            else
            {
                ret = -IP_ERRNO_EINVAL;
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 14, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                  ipnet_ip6_input, IPCOM_WV_NETD_BADBUF,
                                  IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
                goto cleanup;
            }
        }
#endif /* IPSCTP */
#ifdef IPTCP
        else if (!IP_IN6_IS_ADDR_MULTICAST(&ip6_hdr->dst))
        {
            ret = iptcp_input(pkt,
                              &ip6_hdr->src, &ip6_hdr->dst,
                              (Iptcp_socklookup) ipnet_sock_ip6_lookup);
        }
        else
        {
            /* TCP segments can never be sent to a multicast address */
            ret = -IP_ERRNO_EINVAL;
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 9, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_ip6_input, IPCOM_WV_NETD_BADBUF,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(ip6_input_tcp_multicast++);
            goto cleanup;
        }
#endif /* IPTCP */
        if (ret >= 0)
        {
            /* UDP or TCP input was successful */
            IPCOM_MIB2_NETIF(ipv6IfStatsInDelivers++);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInDelivers, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInDelivers, 1, netif);
            return 0;
        }
        else if (ret == -IP_ERRNO_ECONNREFUSED)
        {
            /* No one listen on the destination port, send back a Port Unreachable error */
            ipnet_icmp6_param_init(&icmp6_param, pkt);
            icmp6_param.type = IPNET_ICMP6_DST_UNREACH;
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_NOPORT;
            (void)ipnet_icmp6_send(&icmp6_param);
        }
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 10, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip6_input, IPCOM_WV_NETD_NOPORT,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_input_prot_err++);
        goto cleanup;

#ifdef IPNET_USE_TUNNEL
    case IP_IPPROTO_IPIP:
    case IP_IPPROTO_IPV6:
    case IP_IPPROTO_GRE:
        ip6_hdr->nxt = (Ip_u8) proto;
        return ipnet_tunnel_ip6_input(pkt);
#endif /* IPNET_USE_TUNNEL */

#ifdef IPNET_USE_MCAST_ROUTING
    case IP_IPPROTO_PIM:
        return ipnet_pim_ip6_input(pkt);
#endif /* IPNET_USE_MCAST_ROUTING */


    case IP_IPPROTO_NONE:
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 11, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_ip6_input, IPCOM_WV_NETD_BADBUF,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_input_hdr_no_next++);
        goto cleanup;

    default:
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 12, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_ip6_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_input_raw++);
        /* Unknown application proto, send to raw sockets */
        IPNET_STATS(raw6_input++);
        pkt->chk += ipnet_ip6_pseudo_header_checksum_update(&ip6_hdr->src,
                                                            &ip6_hdr->dst,
                                                            (Ip_u16) proto,
                                                            (Ip_u16) (pkt->end - pkt->start));

        ret = ipnet_raw_input(pkt,
                              IP_FALSE,
                              (Ip_u16) proto,
                              &ip6_hdr->src,
                              0,
                              &ip6_hdr->dst, 0,
                              (Ipnet_sock_lookup_f) ipnet_sock_ip6_lookup);

        if (ret == 0)
        {
            /* No support for this protocol, send Parameter Problem ICMP message */
            IPCOM_MIB2_NETIF(ipv6IfStatsInUnknownProtos++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInUnknownProtos, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInUnknownProtos, 1, netif);
            ipnet_icmp6_param_init(&icmp6_param, pkt);
            icmp6_param.type = IPNET_ICMP6_PARAM_PROB;
            icmp6_param.code = IPNET_ICMP6_PARAM_PROB_NXT_HDR;
            icmp6_param.from = &ip6_hdr->dst;
            icmp6_param.data.param_prob_pointer = ext_param.param_prob_start - pkt->ipstart;
            (void) ipnet_icmp6_send(&icmp6_param);
            ret = -IP_ERRNO_EPROTONOSUPPORT;
        }
        else if (ret > 0)
        {
            IPCOM_MIB2_NETIF(ipv6IfStatsInDelivers++);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInDelivers, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInDelivers, 1, netif);
        }
        break;
    }

 cleanup:
    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                      1, 13, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                      ipnet_ip6_input, IPCOM_WV_NETD_BADBUF,
                      IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
    IPNET_STATS(ip6_input_err++);
    if (pkt != IP_NULL)
        ipcom_pkt_free(pkt);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_param2policy
 *===========================================================================
 * Description: Set the correct values in the rps based on outgoing
 *              interface and param structure.
 * Parameters:  param   - The param structure
 *              netif   - The network interface.
 *              rps     - The policy selector
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ip6_param2policy(Ipnet_ip6_output_param       *param,
                       Ipnet_netif                  *netif,
                       Ipnet_route_policy_selector  *rps,
                       Ipcom_pkt                    *pkt)
{
    /* Set the basic fields */
    rps->fields =   IPNET_PR_RULE_DS | IPNET_PR_RULE_PROTO | IPNET_PR_RULE_SADDR |
                    IPNET_PR_RULE_FLOW | IPNET_PR_RULE_SCOPE | IPNET_PR_RULE_PKTFLAGS |
                    IPNET_PR_RULE_DADDR;
    rps->ds         = param->tclass;
    rps->proto      = param->nxt_hdr;
    rps->flow       = param->flowinfo;
    rps->scope      = ipnet_ip6_get_addr_scope(param->to);
    rps->daddr      = param->to;
    if (pkt != IP_NULL)
    {
        rps->fields     |= IPNET_PR_RULE_PKTPTR;
        rps->pkt        = pkt;
        rps->pkt_flags  = pkt->flags;
    }
    else
    {
        rps->pkt        = IP_NULL;
        rps->pkt_flags  = 0;
    }

    /* Use any if from is not set */
    rps->saddr  = param->from? param->from : &ip_in6addr_any;

    if (netif != IP_NULL)
    {
        IP_BIT_SET(rps->fields, IPNET_PR_RULE_IFINDEX);
        rps->ifindex = netif->ipcom.ifindex;
    }
}




/*
 *===========================================================================
 *                    ipnet_ip6_socket2param
 *===========================================================================
 * Description: Sets all the correct values in the param based on msg, to
 *              and socket.
 * Parameters:  sock    - The socket to use
 *              msg     - Message structure with (unconnected sockets) destination
 *              address and (optional) auxillary data. Can be IP_NULL
 *              if the socket is connected and no auxillary data is needed.
 *              is in connected state.
 *              to      - The destination
 *              param   - The outbound parameters.
 * Returns:     ==0 success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_socket2param(Ipnet_socket                 *sock,
                       IP_CONST struct Ip_msghdr    *msg,
                       struct Ip_sockaddr_in6       *to,
                       Ipnet_ip6_output_param       *param)
{
    int                     ret;
    Ipnet_ip6_socket        *sock_ip6 = sock->ip6;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED | IPNET_SOCKET_FLAG_CONNECTING))
    {
        /* Connected socket */
        param->to       = &sock_ip6->daddr;
        param->flowinfo = sock_ip6->flowinfo;
        param->scope_id = sock_ip6->iscope_id;
    }
    else
    {
        /* Unconnected socket */
        param->to       = &to->sin6_addr;;
        param->flowinfo = ip_ntohl(to->sin6_flowinfo);
        param->scope_id = to->sin6_scope_id;
    }
    /* Clear all but the lower 20 bits */
    param->flowinfo &= 0xfffff;

    param->nxt_hdr    = (Ip_u8) sock->proto;
    param->has_exthdr = IP_FALSE;
    param->tclass     = (Ip_u8)sock_ip6->tclass;
    param->from       = param->real_from = IP_NULL;
    param->netif      = IP_NULL;
    param->cache      = &sock_ip6->cache;
    param->chksum_ptr = IP_NULL;
    param->nexthop    = IP_NULL;
    param->src_flags  = sock_ip6->prefer_src_flags;

    /* Save the final destination since ipnet_ip6_setup_exthdrs() will change
       the first host to visit (param.to) if routing header is present */
    param->final_to  = param->to;

    if (IP_IN6_IS_ADDR_MULTICAST(param->to) == IP_FALSE)
        param->hlim = sock->uni_hop_limit;
    else
    {
        param->hlim = sock->multi_hop_limit;
        if (sock_ip6->multicast_if != 0
            && ipnet_ip6_is_link_local_addr(param->to) == IP_FALSE)
        {
            /* A specific interface was set with IP_IPV6_MULTICAST_IF socket option */
            param->netif = ipnet_if_indextonetif(sock->vr_index, sock_ip6->multicast_if);
            ip_assert(param->netif != IP_NULL);
        }
    }

    if ((msg != IP_NULL && msg->msg_control != IP_NULL)
        || IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_HAS_STICKY_OPT)
        || sock->bind_to_ifindex != 0)
    {
        ret = ipnet_ip6_apply_exthdrs(sock, msg, &param->has_exthdr, &param->exthdrs, param);
        if (ret < 0)
            goto errout;

        /* ipnet_ip6_apply_ancillary_data() may change the param.from address */
        ret = ipnet_ip6_apply_ancillary_data(sock, msg, param);
        if (ret < 0)
            goto errout;
    }

    if (param->from == IP_NULL
        && IP_IN6_IS_ADDR_UNSPECIFIED(&sock_ip6->saddr) == IP_FALSE
        && IP_IN6_IS_ADDR_MULTICAST(&sock_ip6->saddr) == IP_FALSE)
    {
        param->from = &sock_ip6->saddr;
    }

    /* Param is properly set up */
    return 0;

errout:
    /* error, failed */
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_ip6_sendto
 *===========================================================================
 * Description: Finds out the source and destination address and dispatches
 *              the packet to ipnet_ip6_output(). The transport header
 *              checksum is also finished (if used).
 * Parameters:  sock - The socket to use when sending.
 *              msg - Message structure with (unconnected sockets) destination
 *              address and (optional) auxillary data. Can be IP_NULL
 *              if the socket is connected and no auxillary data is needed.
 *              is in connected state.
 *              pkt - The packet data to send.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_sendto(Ipnet_socket *sock, IP_CONST struct Ip_msghdr *msg, Ipcom_pkt *pkt)
{
    int                     ret;
    Ipnet_ip6_output_param  param;
    Ipnet_ip6_socket       *sock_ip6 = sock->ip6;

    pkt->fd             = sock->ipcom.fd;
    pkt->recurs_level   = 0;

#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    IPNET_ETH_PKT_SET_VLAN_TAG(pkt, sock->vlan_tag);
#endif

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED | IPNET_SOCKET_FLAG_CONNECTING))
        param.to = &sock_ip6->daddr;
    else
    {
        /* Unconnected socket */
        ip_assert(msg != IP_NULL);
        ip_assert(msg->msg_name != IP_NULL);
        param.to       = &((struct Ip_sockaddr_in6 *) msg->msg_name)->sin6_addr;
        param.flowinfo = ip_ntohl(((struct Ip_sockaddr_in6 *) msg->msg_name)->sin6_flowinfo);
        param.scope_id = ((struct Ip_sockaddr_in6 *) msg->msg_name)->sin6_scope_id;
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

    if (IP_LIKELY(ipnet_route_cache_is_valid(sock_ip6->cache.rt_id))
        && IP_LIKELY(IP_IN6_ARE_ADDR_EQUAL(param.to, &sock_ip6->cache.ip_hdr.dst))
        && (msg == IP_NULL || IP_LIKELY(msg->msg_control == IP_NULL))
        && IP_LIKELY(sock->bind_to_ifindex == 0))
    {
        Ipnet_route_entry *rt_dst = sock_ip6->cache.rt_dst;
        Ipnet_netif *netif = sock_ip6->cache.rt_next_hop->netif;

        if ((pkt->end - pkt->start + IPNET_IP6_HDR_SIZE)
            <= (int) (rt_dst->metrics.rmx_mtu ? rt_dst->metrics.rmx_mtu : netif->ipcom.mtu)
#ifdef IPCOM_USE_TSO
            || (pkt->ops->is_tso_active(pkt)
                && IP_BIT_ISSET(netif->ipcom.drv_capabilities, IPCOM_IF_DRV_CAP_IP6_TSO))
#endif
            )
        {
            Ipnet_route_entry *rt_next_hop = sock_ip6->cache.rt_next_hop;
            Ipnet_pkt_ip6     *ip6hdr;

            /* Use the fast path for sending */
            IPNET_IF_LOCK(netif);

            if (IP_LIKELY(IP_BIT_ISSET(rt_next_hop->hdr.flags, IPNET_RTF_LLINFO)))
            {
                Ipnet_ip6_nc_entry *nc = IPNET_IP6_NC(rt_next_hop);
                if (nc != IP_NULL)
                {
                    if (nc->ne.state == IPNET_ND_STALE)
                        ipnet_ip6_neighbor_cache_update(nc, IPNET_ND_DELAY, IP_NULL, IP_FALSE);

                    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_PROGRESS)
                        && (nc->ne.state == IPNET_ND_REACHABLE
                            || nc->ne.state == IPNET_ND_PROBE))
                        nc->ne.is_making_forward_progress = IP_TRUE;
                }
            }

            pkt->ifindex = netif->ipcom.ifindex;
            IP_BIT_SET(pkt->flags, sock_ip6->cache.pkt_flags);

            if (IP_LIKELY(IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM)))
            {
                Ip_u16 *chksum_ptr = (Ip_u16 *)&pkt->data[pkt->start + sock->chk_offset];

                *chksum_ptr = ipcom_in_checksum_finish(pkt->chk
                                                       + sock_ip6->cache.pseudo_hdr_chksum
                                                       + ip_htons((Ip_u16) (pkt->end - pkt->start)));
                /* Fix-up the UDP checksum if it was calculated to 0 */
                if (IP_UNLIKELY(*chksum_ptr == 0) && sock->proto == IP_IPPROTO_UDP)
                    *chksum_ptr = (Ip_u16) 0xFFFF;

#ifdef IPCOM_USE_HW_CHECKSUM_TX
                if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
                {
                    pkt->chk = sock->chk_offset;
                    *chksum_ptr = ~*chksum_ptr;
                }
#endif /* IPCOM_USE_HW_CHECKSUM_TX */
            }

            /* Add a cached IPv6 header */
            pkt->start -= IPNET_IP6_HDR_SIZE;
            pkt->ipstart = pkt->start;
            ip6hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];
            ipcom_memcpy(ip6hdr, &sock_ip6->cache.ip_hdr, IPNET_IP6_HDR_SIZE);
            ip6hdr->plen = (Ip_u16) ip_htons(pkt->end - pkt->start - IPNET_IP6_HDR_SIZE);

            IPNET_STATS(ip6_output++);

            IPCOM_MIB2_NETIF(ipv6IfStatsOutRequests++);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutRequests, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutRequests, 1, netif);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutTransmits, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutTransmits, 1, netif);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutOctets, pkt->end - pkt->start, netif);

            ++rt_next_hop->use;
#ifdef IPCOM_USE_HW_CHECKSUM_TX
            if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM)
                && IP_BIT_ISFALSE(netif->ipcom.drv_capabilities, IPCOM_IF_DRV_CAP_IP6_TRANSPORT_CSUM_TX))
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
                    IPCOM_LOG1(WARNING, "Discarding IPv6 datagram on %s, ROHC failed.", netif->ipcom.name);
                    IPNET_IF_UNLOCK(netif);
                    ipcom_pkt_free(pkt);
                    return -IP_ERRNO_EROHC;
                }
            }
#endif /* IPROHC */

            ret = netif->link_ip6_output(netif, rt_next_hop, pkt);
            if (ret < 0)
                IPNET_STATS(ip6_output_link_err++);
            IPNET_IF_UNLOCK(netif);
            return ret;
        }
        /* else: need fragmentation */
    }

#ifdef IPCOM_USE_INET
    if (IP_IN6_IS_ADDR_V4MAPPED(param.to))
    {
        struct Ip_sockaddr_in ipv4_to_addr;
        struct Ip_msghdr      ipv4_msg;

        if (sock->ip4 == IP_NULL)
        {
            ret = ipnet_ip6_initialize_mapped_ip4(sock, param.to);
            if (ret < 0)
                return ret;
        }

        ipcom_memset(&ipv4_msg, 0, sizeof(ipv4_msg));
        if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_CONNECTED | IPNET_SOCKET_FLAG_CONNECTING))
        {
            ipv4_msg.msg_name    = (struct Ip_sockaddr *) &ipv4_to_addr;
            ipv4_msg.msg_namelen = sizeof(ipv4_to_addr);

            ipcom_memset(&ipv4_to_addr, 0, sizeof(ipv4_to_addr));
            IPCOM_SA_LEN_SET(&ipv4_to_addr, sizeof(ipv4_to_addr));
            ipv4_to_addr.sin_family = IP_AF_INET;
            ipv4_to_addr.sin_port   = ((struct Ip_sockaddr_in6 *) msg->msg_name)->sin6_port;
            ipcom_memcpy(&ipv4_to_addr.sin_addr,
                         &param.to->in6.addr32[3],
                         sizeof(ipv4_to_addr.sin_addr));
        }
        return ipnet_ip4_sendto(sock, &ipv4_msg, pkt);
    }
#endif /* IPCOM_USE_INET */

    if ((ret = ipnet_ip6_socket2param(sock,
                                      msg,
                                      msg ? (struct Ip_sockaddr_in6 *) msg->msg_name : IP_NULL,
                                      &param)) < 0)
        goto errout;

    /* Loop multicast */
    if (IP_IN6_IS_ADDR_MULTICAST(param.to) != IP_FALSE)
    {
        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST))
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_LOOP_MCAST);
    }

    /* Must setup the pointer for the packet */
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM))
        param.chksum_ptr = (Ip_u16 *)&pkt->data[pkt->start + sock->chk_offset];

    pkt->fd       = sock->ipcom.fd;
    pkt->vr_index = sock->vr_index;

#ifdef IPNET_USE_RFC2890
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_GRE_KEY))
        ipcom_pkt_set_info(pkt, IPNET_PKT_INFO_GRE_KEY_OUTPUT, sizeof(sock->gre_key), &sock->gre_key);
#endif

#ifdef IPIPSEC2
    if (IP_UNLIKELY(IP_BIT_ISSET(sock_ip6->flags, IPNET_SOCKET_FLAG_IP6_BYPASS_FLOW)))
    {
        /* Set the flow to bypass */
        sock->ipcom.ipsec_flow_cache = IPIPSEC_BYPASS_FLOW;
    }
#endif

    return ipnet_ip6_output(&param, pkt);

 errout:
    ipcom_pkt_free(pkt);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_add_header
 *===========================================================================
 * Description: Adds an IPv6 header.
 * Parameters:  param - Parameters to add to the IPv6 header.
 *              pkt - The packet.
 * Returns:     >=0 = success (>0 == length of added buffer)
 *               <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_add_header(Ipnet_ip6_output_param *param, Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip6 *ip6_hdr;

    if (pkt->start < IPNET_IP6_HDR_SIZE)
        return -IP_ERRNO_ENOBUFS;

    pkt->start  -= IPNET_IP6_HDR_SIZE;
    pkt->ipstart = pkt->start;

    ip6_hdr = (Ipnet_pkt_ip6 *)&pkt->data[pkt->ipstart];

    /* Add the IPv6 header */
    IPNET_IP6_SET_VER_CLASS_FLOW(ip6_hdr, 6, param->tclass, param->flowinfo);
    /* plen is the size of the IPv6 payload (excluding the size of the IPv6 header) */
    ip6_hdr->plen = (Ip_u16) ip_htons(pkt->end - pkt->start - IPNET_IP6_HDR_SIZE);
    ip6_hdr->nxt  = param->nxt_hdr;
    ip6_hdr->hlim = param->hlim;
    IPNET_IP6_SET_ADDR(&ip6_hdr->src, param->from);
    IPNET_IP6_SET_ADDR(&ip6_hdr->dst, param->to);

    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
    return IPNET_IP6_HDR_SIZE;
}


/*
 *===========================================================================
 *                    ipnet_ip6_add_inner_dest_header
 *===========================================================================
 * Description: Adds the destinaion header option that is inside the
 *              routing header option..
 * Parameters:  param - The destination header data.
 *              pkt - The packet.
 * Returns:     >=0 = success (>0 == length of added buffer)
 *               <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_add_inner_dest_header(Ipnet_ip6_output_param *param, Ipcom_pkt *pkt)
{
    int              ip6d_len;
    Ip_pkt_ip6_dest *dest;

    ip_assert(param->has_exthdr == IP_TRUE);

    if (param->exthdrs.rthdrdst == IP_NULL)
        return 0;

    ip6d_len = (param->exthdrs.rthdrdst->ip6d_len + 1) * 8;

    if (pkt->start < ip6d_len)
        return -IP_ERRNO_ENOBUFS;
    pkt->start -= ip6d_len;

    dest = (Ip_pkt_ip6_dest *) &pkt->data[pkt->start];
    ipcom_memcpy(dest, param->exthdrs.rthdrdst, ip6d_len);

    dest->ip6d_nxt = param->nxt_hdr;
    param->nxt_hdr = IP_IPPROTO_DSTOPTS;
    return ip6d_len;
}


/*
 *===========================================================================
 *                    ipnet_ip6_add_extension_headers
 *===========================================================================
 * Description: Adds hop-by-hop option, destination header and/or
 *              routing header.
 * Parameters:  param - The extension header(s) data.
 *              pkt - The packet.
 * Returns:     >=0 = success (>0 == length of added buffer)
 *               <0 = error code.
 *
 */
IP_STATIC int
ipnet_ip6_add_extension_headers(Ipnet_ip6_output_param *param, Ipcom_pkt *pkt)
{
    Ip_pkt_ip6_ext_hdr *ext;
    int                 len;
    int                 total_len = 0;
    int                 start;

    ip_assert(param->has_exthdr == IP_TRUE);

    if (IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
    {
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
        pkt->start  += IPNET_IP6_HDR_SIZE;
    }
    start = pkt->start;

    if (param->exthdrs.rthdr0 != IP_NULL)
    {
        struct Ip_in6_addr *rtaddrs;
        struct Ip_in6_addr *pktaddrs;
        Ip_pkt_ip6_rthdr0  *rthdr0;
        int                segments;
        /* The routing header data is formated like this
           rthdr
           Intermediate node 1
           Intermediate node 2
           ...
           Intermediate node N
           Final destination

           The destination address in the IPv6 header will be Intermediate node 1,
           so Intermediate node 2 to Final destination is put into the routing header.
        */
        /* Number of routing segments, excluding the final destination */
        segments = (param->exthdrs.rthdr0->ip6r_len >> 1) - 1;

        /* Total length */
        len = param->exthdrs.rthdr0->ip6r_len * 8 + sizeof(Ip_pkt_ip6_rthdr0);
        total_len += len;
        pkt->start -= len;

        if (pkt->start < 0)
            goto errout;

        /* Get the required offsets from the ancillary data object */
        rtaddrs     = (struct Ip_in6_addr *) ((Ip_pkt_ip6_rthdr0 *) param->exthdrs.rthdr0 + 1);

        /* Get the required offsets from the packet */
        rthdr0      = (Ip_pkt_ip6_rthdr0 *) &pkt->data[pkt->start];
        pktaddrs    = (struct Ip_in6_addr *) (rthdr0 + 1);

        /* Copy the routing header */
        ipcom_memcpy(rthdr0, param->exthdrs.rthdr0, sizeof(Ip_pkt_ip6_rthdr0));

        /* Are there more than one segment? */
        if (segments != 0)
        {
            /* Copy address 1 - n into the routing header at location 0 - (n - 1) */
            ipcom_memcpy(&pktaddrs[0], &rtaddrs[1], segments * sizeof(struct Ip_in6_addr));
        }

        /* Copy final destination into location n */
        ipcom_memcpy(&pktaddrs[segments], param->exthdrs.rthdr0_to, sizeof(struct Ip_in6_addr));

        rthdr0->ip6r0_nxt = param->nxt_hdr;
        param->nxt_hdr = IP_IPPROTO_ROUTING;
    }

    if (param->exthdrs.rthdr2 != IP_NULL)
    {
        Ip_pkt_ip6_rthdr2  *rthdr2;

        /* */
        len         = sizeof(Ip_pkt_ip6_rthdr2);
        total_len   += len;
        pkt->start  -= len;

        if (pkt->start < 0)
            goto errout;

        rthdr2 = (Ip_pkt_ip6_rthdr2 *) &pkt->data[pkt->start];
        ipcom_memcpy(rthdr2, param->exthdrs.rthdr2, len);

        rthdr2->ip6r2_nxt = param->nxt_hdr;
        param->nxt_hdr = IP_IPPROTO_ROUTING;
    }

    if (param->exthdrs.dst != IP_NULL)
    {
        len = (param->exthdrs.dst->ip6d_len + 1) * 8;
        total_len += len;
        pkt->start -= len;

        if (pkt->start < 0)
            goto errout;

        ext = (Ip_pkt_ip6_ext_hdr *) &pkt->data[pkt->start];
        ipcom_memcpy(ext, param->exthdrs.dst, len);

        ext->nxt = param->nxt_hdr;
        param->nxt_hdr = IP_IPPROTO_DSTOPTS;
    }

    if (param->exthdrs.hbh != IP_NULL)
    {
        len = (param->exthdrs.hbh->ip6h_len + 1) * 8;
        total_len += len;
        pkt->start -= len;

        if (pkt->start < 0)
            goto errout;

        ext = (Ip_pkt_ip6_ext_hdr *) &pkt->data[pkt->start];
        ipcom_memcpy(ext, param->exthdrs.hbh, len);

        ext->nxt = param->nxt_hdr;
        param->nxt_hdr = IP_IPPROTO_HOPOPTS;
    }

    return total_len;

 errout:
    pkt->start = start;
    return -IP_ERRNO_ENOBUFS;
}


/*
 *===========================================================================
 *                    ipnet_ip6_output
 *===========================================================================
 * Description: Sends a IPv6 packet. This function will make sure that the
 *              data is split into fragments equal to or smaller than the
 *              path MTU.
 * Parameters:  netif - The network interface to use when sending the packet.
 *              vr_entry - The routing entry for the destination.
 *              param - The IPv6 header parameters for this packet.
 *              pkt - The packet buffer.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_ip6_output(Ipnet_ip6_output_param *param, Ipcom_pkt *pkt)
{
    Ipcom_pkt          *pkt_next_fragment;
    Ipnet_netif        *netif = IP_NULL;
    Ipnet_route_entry  *rt = IP_NULL;
    Ipnet_route_entry  *rt_next_hop = IP_NULL;
    Ipnet_ip6_nc_entry *nc;
    Ipnet_icmp6_param   icmp6_param;
    Ip_u32              pseudo_hdr_chksum;
    int                 ret = 0;
#ifdef IPIPSEC2
    Ipipsec_param       ipsec_param;
#endif
    Ip_u8               frag_nxt_hdr;
    int                 path_mtu;
    Ip_bool             dont_cache = IP_FALSE;
#ifdef IPNET_IS_ROUTER
    Ip_u32              input_if_index = pkt->ifindex;
#endif
    Ipnet_route_policy_selector rps;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 14, IPCOM_WV_NETDEVENT_START,
                       ipnet_ip6_output, IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
    IPNET_STATS(ip6_output++);
    ip_assert(pkt != IP_NULL);
    IPNET_DEBUG_LINE(ip_assert(IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_FREEQ));
    IPNET_DEBUG_LINE(ip_assert(IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_INQ));
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IP6_OUTPUT);
    IPNET_DEBUG_LINE(ipnet_pkt_check(pkt));

    rps.fields = 0;
    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_OUTPUT);

    if (param->scope_id == IPNET_IP6_DEFAULT_ZONE)
    {
        Ip_u32 zone = 0;

        if (ipnet_ip6_is_link_local_addr(param->to))
            zone = ipnet_ip6_get_default_zone(pkt->vr_index, IPNET_IP6_SCOPE_LINK_LOCAL);
        else if (IP_IN6_IS_ADDR_MULTICAST(param->to))
            zone = ipnet_ip6_get_default_zone(pkt->vr_index, IPNET_IP6_MC_SCOPE(param->to));

        if (zone)
        {
            param->scope_id = zone;
            /* Cannot figure out the default zone in the send fast path */
            dont_cache = IP_TRUE;
        }
    }

    if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_DONTROUTE)))
    {
        Ip_u32 ifindex;

        /* Cannot use the cache when don't route is used */
        dont_cache = IP_TRUE;

        /* Determine the interface to use or 0 for any */
        if (param->netif != IP_NULL)
            ifindex = param->netif->ipcom.ifindex;
        else if (ipnet_ip6_is_scoped_addr(param->to))
            ifindex = param->scope_id;
        else
            ifindex = 0;

        ret = ipnet_route_lookup_ip6_dont_route(IP_FALSE,
                                                pkt->vr_index,
                                                param->to,
                                                ifindex,
                                                &rt);
        if (ret < 0)
            goto cleanup;

    }
    else if (IP_UNLIKELY(param->nexthop != IP_NULL))
    {
        /* Set the appropriate values */
        rt = param->nexthop;
        rt_next_hop = param->nexthop;
    }
    else
    {
        Ipnet_netif *grp_netif;

    reroute:
        if (rt != IP_NULL)
        {
            /* Need to verify routing, I've changed source or destination */

            /* Unlock any previous netif's */
            if (netif != IP_NULL)
            {
                IPNET_IF_UNLOCK(netif);

                /* Null it so that 'cleanup' doesn't do bad things */
                netif = IP_NULL;
            }

            /* Need to redo the routing lookup */
            rt = IP_NULL;
        }

        /* Resolve interface */
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
            grp_netif = param->netif;
        else
            grp_netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);

        /* Convert to policy */
        ipnet_ip6_param2policy(param, grp_netif, &rps, pkt);

        /* Find the route to the destination */
        ret = ipnet_route_lookup_ecmp(IP_AF_INET6,
                                      pkt->vr_index,
                                      0,
                                      param->to,
                                      param->scope_id,
                                      param->netif ? param->netif->ipcom.ifindex : 0,
                                      &rps,
                                      &rt,
                                      &dont_cache);
        if (ret < 0)
        {
            /* If its globally scoped, and we can resolve an address, re-route */
            if (param->from == IP_NULL
                && ipnet_ip6_get_addr_scope(param->final_to) == IPNET_IP6_SCOPE_GLOBAL)
            {
                param->from = ipnet_ip6_get_src_addr_by_rules(pkt->vr_index, param->final_to, param->netif, 0);
                if (param->from != IP_NULL)
                    goto reroute;
            }

            ipnet_icmp6_send_dst_unreach(pkt, -ret);
            goto cleanup;
        }
    }

    /* We've gotten a basic route, if we're stuck with a NULL 'from'
     * we need to resolve it and reroute due to policy routing */
    if (param->from == IP_NULL)
    {
        /* No from/source address specifed, select on */
        param->from = ipnet_ip6_get_src_addr(pkt->vr_index,
                                             param->final_to,
                                             param->scope_id,
                                             rt,
                                             &rps,
                                             param->netif,
                                             param->src_flags);
        if (param->from == IP_NULL)
        {
            ret = -IP_ERRNO_EHOSTUNREACH;
            goto cleanup;
        }

        /* If the dont route flag aint set, do reroute this packet */
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_DONTROUTE)
            && param->nexthop == IP_FALSE)
        {
            /* Since policy routing may be in effect, we may be forced
             * to look at were to send it once again */
            goto reroute;
        }
    }


    if (IP_UNLIKELY(rt->rt_func_info != IP_NULL))
    {
        ret = (*rt->rt_func_info->rt_info) (&rt, pkt, param, &dont_cache);
        if (IP_UNLIKELY(ret != 0))
        {
            if (ret < 0)
                goto cleanup;
            goto reroute;
        }
    }


    if (IP_UNLIKELY(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_BLACKHOLE)))
    {
        /* Silently drop all packets to this route */
        ++rt->use;
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 15, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                          ipnet_ip6_output, IPCOM_WV_NETD_INFO_DROPPED,
                          IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
        IPNET_STATS(ip6_output_blackhole++);
        ipcom_pkt_free(pkt);
        return 0;
    }

    /* Set the network interface the packet will be sent on */
    if (IP_LIKELY(param->netif == IP_NULL))
        netif = rt->netif;
    else
        netif = param->netif;

    /* Lock interface. */
    IPNET_IF_LOCK(netif);

    if (IP_UNLIKELY(IP_BIT_ISSET(netif->flags2, IPNET_IFF2_IPV6_DISABLED)))
        goto cleanup;

    if (param->hlim == IPNET_IP6_OUTPUT_PARAM_USE_DEFAULT_HLIM)
        /* No hoplimit specified, use the default value */
        param->hlim = netif->inet6_nd_curhoplimit;

#ifdef IPNET_IS_ROUTER
    if (IP_UNLIKELY(netif->ipcom.ifindex == pkt->ifindex))
    {
        Ip_bool forward_pkt;
        Ip_bool send_redirect;

        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
        {
            IPCOM_LOG4(WARNING, "IPv6: Route loop detected to %s on interface %s when using route %s on VR %d",
                       ipcom_inet_ntop(IP_AF_INET6, param->to,
                                       ipnet->log_buf, sizeof(ipnet->log_buf)),
                       netif->ipcom.name,
                       ipcom_inet_ntop(IP_AF_INET6,
                                       rt->hdr.key,
                                       ipnet->log_buf + IP_INET6_ADDRSTRLEN,
                                       sizeof(ipnet->log_buf) - IP_INET_ADDRSTRLEN),
                       pkt->vr_index);
            ret = -IP_ERRNO_EHOSTUNREACH;
            goto cleanup;
        }

        switch (ipnet_sysvar_netif_get_as_int(IP_AF_INET6, netif, "IcmpRedirectSend", 2))
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

        if (send_redirect)
            /* Only send redirect if the sender is link local to this
               node */
            send_redirect = (ipnet_ip6_neighbor_cache_find(ipnet_if_indextonetif(IPCOM_VR_ANY, input_if_index),
                                                           param->from,
                                                           IP_TRUE) != IP_NULL);

        if (send_redirect && param->send_icmp6_redirect)
        {
            /* The final destination was on the same link as the sender of the packet
               and that is not because of a routing header,
               send a ICMP redirect () */
            ipnet_icmp6_param_init(&icmp6_param, pkt);
            icmp6_param.type = IPNET_ND_REDIRECT;
            icmp6_param.code = 0;
            icmp6_param.from = IP_NULL;
            if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
            {
                /* Go through a gateway */
                icmp6_param.data.redirect.target_addr =
                    &((struct Ip_sockaddr_in6 *) rt->gateway)->sin6_addr;
                ip_assert(rt->gateway->sa_family == IP_AF_INET6);
                /* RFC 2461, section 8
                   ...
                   A router MUST be able to determine the link-local
                   address for each of its neighboring routers in
                   order to ensure that the target address in a
                   Redirect message identifies the neighbor router by
                   its link-local address.
                   ...
                */
                if (IP_IN6_IS_ADDR_LINK_LOCAL(icmp6_param.data.redirect.target_addr))
                {
                    /* Do not send redirect to rtr unless link loc */
                    icmp6_param.data.redirect.dst_addr = param->to;
                    icmp6_param.data.redirect.dst_rt   = rt;
                    (void) ipnet_icmp6_send(&icmp6_param);
                }
            }
            else
            {
                /* The end host is on this link */
                icmp6_param.data.redirect.target_addr = param->to;
                icmp6_param.data.redirect.dst_addr = param->to;
                icmp6_param.data.redirect.dst_rt   = rt;
                (void) ipnet_icmp6_send(&icmp6_param);
            }
        }

        if (!forward_pkt)
        {
            ++rt->use;
            goto cleanup;
        }
    }
#endif /* IPNET_IS_ROUTER */

    /* If the real_from param is NULL, set it to current from */
    if (param->real_from == IP_NULL)
        param->real_from = param->from;

    pkt->ifindex = netif->ipcom.ifindex;

#ifdef IPMIP6
    /* Need to resolve CN/MN optimizations */
#endif

    /* Update MIB-2 stats */
    if(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
    {
        IPCOM_MIB2_NETIF(ipv6IfStatsOutForwDatagrams++);
        IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutForwDatagrams, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutForwDatagrams, 1, netif);
    }
    else
    {
        IPCOM_MIB2_NETIF(ipv6IfStatsOutRequests++);
        IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutRequests, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutRequests, 1, netif);
    }
    IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutTransmits, 1);
    IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutTransmits, 1, netif);
    IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
    IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutOctets, pkt->end - pkt->start, netif);

    if (param->nxt_hdr == IP_IPPROTO_ICMPV6
        && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR)
        && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
    {
        switch (((Ipnet_pkt_icmp6 *) &pkt->data[pkt->start])->icmp6_type)
        {
        case IPNET_ICMP6_ECHO_REQUEST:
            IPCOM_MIB2_NETIF(ipv6IfIcmpOutEchos++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpMsgStatsOutPkts[IPNET_ICMP6_ECHO_REQUEST], 1);
            break;
        case IPNET_MLD_LISTENER_QUERY:
            IPCOM_MIB2_NETIF(ipv6IfIcmpOutGroupMembQueries++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpMsgStatsOutPkts[IPNET_MLD_LISTENER_QUERY], 1);
            break;
        case IPNET_ICMP6_PACKET_TOO_BIG:
            IPCOM_MIB2_NETIF(ipv6IfIcmpOutPktTooBigs++);
            break;
        default:
            break;
        }

        IPCOM_MIB2_NETIF(ipv6IfIcmpOutMsgs++);
        IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpStatsOutMsgs, 1);

        /* Don't create a fast path for locally generated ICMPv6
           message since they are not performance critical and the MIB
           statistics will not be correct if ICMPv6 messages is not
           sent trough the normal path */
        dont_cache = IP_TRUE;
    }

    if (param->chksum_ptr != IP_NULL)
    {
#ifdef IPCOM_USE_HW_CHECKSUM_TX
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
        {
            /* This is the slow path, the packet can get fragmented so
               using HW checksum might fail. Revert back to
               software */
            IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
            pkt->chk = ipcom_in_checksum_update(&pkt->data[pkt->start],
                                                pkt->end - pkt->start);
        }
#endif /* IPCOM_USE_HW_CHECKSUM_TX */

        /* Add the pseudo header checksum to the transport layer
           checksum.  Cannot include the length in the cached value
           since it isn't a constant value */
        pseudo_hdr_chksum = ipnet_ip6_pseudo_header_checksum_update(param->real_from,
                                                                    param->final_to,
                                                                    param->nxt_hdr,
                                                                    0);
        if (param->cache != IP_NULL)
            param->cache->pseudo_hdr_chksum = pseudo_hdr_chksum;


        *param->chksum_ptr = ipcom_in_checksum_finish(pkt->chk
                                                      + pseudo_hdr_chksum
                                                      + ip_htons((Ip_u16) (pkt->end - pkt->start)));
        /* Fix-up the UDP checksum if it was calculated to 0 */
        if (param->nxt_hdr == IP_IPPROTO_UDP && *param->chksum_ptr == 0)
            *param->chksum_ptr = 0xFFFF;
    }

 ip_header_check:
    pkt->ipstart = pkt->start;
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
        ret = IPNET_IP6_HDR_SIZE;
    else
    {
        ret = 0;
        if (param->has_exthdr)
        {
            dont_cache = IP_TRUE;
            if ((ret = ipnet_ip6_add_inner_dest_header(param, pkt)) >= 0)
                ret = ipnet_ip6_add_extension_headers(param, pkt);
        }
        if (ret >= 0)
            ret = ipnet_ip6_add_header(param, pkt);
    }

    if (IP_UNLIKELY(ret <= 0) || IP_UNLIKELY(pkt->start < netif->ipcom.link_hdr_size))
    {
        /* IPv6 header could not be fitted or not enough room for link header */
        pkt->start = pkt->ipstart;
        ret = ipnet_increase_hdr_space(&pkt, IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR));
        if (ret < 0)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 16, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                              ipnet_ip6_output, IPCOM_WV_NETD_INFO_DROPPED,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(ip6_output_hdr_space++);
            goto cleanup;
        }
        goto ip_header_check;
    }

#ifdef IPFIREWALL_USE_IP_FILTER
    if (ipfirewall != IP_NULL
        && ipfirewall_output_hook(&netif->ipcom, pkt, IP_NULL))
    {
        /* Packet blocked. */
        ret = -IP_ERRNO_EFIREWALL;
        goto cleanup;
    }
#endif /* IPFIREWALL_USE_IP_FILTER */

#ifdef IPIPSEC2
    if (pkt->ipsec_skip == 0)
    {
        IPNET_IP6_SET_ADDR(&ipsec_param.key.src.in6, param->real_from);
        IPNET_IP6_SET_ADDR(&ipsec_param.key.dst.in6, param->final_to);
        ipsec_param.key.proto  = param->nxt_hdr;
        ipsec_param.key.domain = IP_AF_INET6;
        ipsec_param.ttl        = param->hlim;
#ifdef IPIPSEC_USE_DAEMONS
        ret = ipipsec_send_daemon_pipe(pkt, &ipsec_param, 0);
#else
        ret = ipipsec_output(&pkt, &ipsec_param);
#endif
        if(ret == IPIPSEC_KEPT)
        {
            /* IPSEC kept the packet for later transmission */
            ipnet->ip6.id++; /* Next packet may come before the kept packet
                                is sent by the asyncronous task so we must
                                increment the identfier */
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
                ipnet_icmp6_send_dst_unreach(pkt, IP_ERRNO_EPERM);
                break;
            default:
                break;
            }
            goto cleanup;
        }

        /* IPv6 packet tunneled in an IPv4 packet. */
        if (ipsec_param.key.domain == IP_AF_INET)
        {
#ifdef IPCOM_USE_INET
            Ipnet_ip4_output_param  ip4_param;

            /* Output IPv4 packet. */
            ipcom_memset(&ip4_param, 0, sizeof(Ipnet_ip4_output_param));
            ip4_param.from  = (IP_CONST struct Ip_in_addr *)&ipsec_param.key.src.in;
            ip4_param.to    = (IP_CONST struct Ip_in_addr *)&ipsec_param.key.dst.in;
            ip4_param.proto = ipsec_param.key.proto;
            ip4_param.ttl   = param->hlim;
            pkt->ifindex    = 0; /* must clear to avoid loop-detection check */
            ret = ipnet_ip4_output(&ip4_param, pkt);
            pkt = IP_NULL;
#endif /* IPCOM_USE_INET */
            goto cleanup;
        }

        /* Reroute datagram if packet was tunneled. */
        ip_assert(ipsec_param.key.domain == IP_AF_INET6);
        if (ret == IPIPSEC_TUNNELED || param->nxt_hdr == IP_IPPROTO_ESP || param->nxt_hdr == IP_IPPROTO_AH)
            dont_cache = IP_TRUE;

        /* Update IPv6 next header (prevent caching to avoid bad param contents) */
        if (param->nxt_hdr != ipsec_param.key.proto)
            param->cache = IP_NULL;
        param->nxt_hdr = ipsec_param.key.proto;

        /* If the nexthop is explicitly forced, don't change it */
        if (param->nexthop == IP_NULL && ret == IPIPSEC_TUNNELED)
        {
            rt_next_hop = IP_NULL;

            /* Update source and destination */
            param->final_to     = param->to           = (IP_CONST struct Ip_in6_addr *)&ipsec_param.key.dst.in6;
            param->from         = param->real_from    = (IP_CONST struct Ip_in6_addr *)&ipsec_param.key.src.in6;

            if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_DONTROUTE))
                ret = ipnet_route_lookup_ip6_dont_route(IP_FALSE,
                                                        pkt->vr_index,
                                                        &ipsec_param.key.dst.in6,
                                                        0,
                                                        &rt);
            else
            {
                /* Convert to policy */
                ipnet_ip6_param2policy(param,netif,&rps,pkt);

                ret = ipnet_route_lookup_ecmp(IP_AF_INET6,
                                              pkt->vr_index,
                                              0,
                                              &ipsec_param.key.dst.in6,
                                              0,
                                              0,
                                              &rps,
                                              &rt,
                                              IP_NULL);
            }
            if (ret < 0)
            {
                IPCOM_LOG1(DEBUG, "IPSEC: no route for tunneled IPv6 packet'%s' ",
                           ipcom_inet_ntop(IP_AF_INET6, &ipsec_param.key.dst.in6, ipnet->log_buf, sizeof(ipnet->log_buf)));
                ipnet_icmp6_send_dst_unreach(pkt, ret);
                goto cleanup;
            }

            if (netif != rt->netif)
            {
                /* IPSec tunneling has changed the outgoinging interface */
                IPNET_IF_UNLOCK(netif);
                netif = rt->netif;
                IPNET_IF_LOCK(netif);
            }

            /* Update IPv6 source and destination. */
            param->has_exthdr   = IP_FALSE;
            param->cache        = IP_NULL;
        }
    }
#endif /* IPIPSEC2 */

    if (IP_IN6_IS_ADDR_MULTICAST(param->to))
    {
        /* Doesn't need any neighbor discovery entry for multicast addresses */
        IPCOM_MIB2_NETIF(ipv6IfStatsOutMcastPkts++);
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST);
        netif->omcasts++;
        IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutMcastPkts, 1);
        IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutMcastPkts, 1, netif);
        IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutMcastOctets, pkt->end - pkt->start);
        IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutMcastOctets, pkt->end - pkt->start, netif);
    }

    if (rt_next_hop == IP_NULL)
    {
        if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_GATEWAY))
            rt_next_hop = rt;
        else
        {
            /* Off link, must use a gateway */
            struct Ip_sockaddr_in6 *gw6 = (struct Ip_sockaddr_in6 *) rt->gateway;

            ip_assert(gw6->sin6_family == IP_AF_INET6);
            ret = ipnet_route_lookup_ecmp(IP_AF_INET6,
                                          IPNET_ROUTE_GET_VR(rt->head),
                                          IPNET_RTL_FLAG_LINK_LOCAL,
                                          &gw6->sin6_addr,
                                          gw6->sin6_scope_id,
                                          0,
                                          &rps,
                                          &rt_next_hop,
                                          IP_NULL);
            if (ret < 0)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 17, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                  ipnet_ip6_output, IPCOM_WV_NETD_INFO_DROPPED,
                                  IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
                IPNET_STATS(ip6_output_noroute++);
                goto cleanup;
            }
        }
    }

    nc = IPNET_IP6_NC(rt_next_hop);
    if (nc != IP_NULL && IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_LOOPBACK))
    {
        ip_assert(IP_BIT_ISSET(rt_next_hop->hdr.flags, IPNET_RTF_LLINFO));
        switch (nc->ne.state)
        {
        case IPNET_ND_UNINITIALIZED:
        case IPNET_ND_INCOMPLETE:
            /* This will restart probing if entry is running on linger time */
            ipnet_ip6_neighbor_cache_update(nc, IPNET_ND_INCOMPLETE, IP_NULL, IP_FALSE);
            dont_cache = IP_TRUE;
            if (rt != rt_next_hop
                && IP_IN6_IS_ADDR_UNSPECIFIED(rt->hdr.key)
                && rt->next != IP_NULL
                && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_PREF))
            {
                /* A default router is used that is not known to be reachable,
                   more than one default route is currently present in the
                   default router list (rt->next != IP_NULL), non of them are
                   known to be rechable (the FIB is structured so that any
                   reachable default gateway is always returned by the call to
                   ipnet_route_lookup_xxx().

                   RFC2461 chapter 6.3.6 says:
                   ...
                   The policy for selecting routers from the Default Router List is as
                   follows:

                   1) Routers that are reachable or probably reachable (i.e., in any
                      state other than INCOMPLETE) SHOULD be preferred over routers
                      whose reachability is unknown or suspect (i.e., in the
                      INCOMPLETE state, or for which no Neighbor Cache entry exists).
                      An implementation may choose to always return the same router or
                      cycle through the router list in a round-robin fashion as long
                      as it always returns a reachable or a probably reachable router
                      when one is available.

                   2) When no routers on the list are known to be reachable or
                      probably reachable, routers SHOULD be selected in a round-robin
                      fashion, so that subsequent requests for a default router do not
                      return the same router until all other routers have been
                      selected.
                   ...
                */
                Ipnet_route_entry *first_rt = rt;

                while (first_rt->prev)
                    first_rt = first_rt->prev;
                ipnet_route_rotate_gateways(first_rt);
            }

            break;
        default:
            break;
        }
    }

    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST)
        || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_MBINDING)
        || (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
            && IP_IN6_IS_ADDR_LINK_LOCAL(param->to)))
        /* Multicast can be sent to more than one outgoing if,
           mbinding is sent to more than one outgoing if by
           definition, loopedback link local addresses must know the
           original scope ID */
        dont_cache = IP_TRUE;

#ifdef IPNET_FASTPATH
    if (dont_cache == IP_FALSE
        && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED)
        && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
    {
        /* Forwarded packet, lets create a fast path for it */
        Ipnet_ip6_rt_cache *cache;
        Ipnet_pkt_ip6      *ip_hdr = (Ipnet_pkt_ip6 *)&pkt->data[pkt->start];
        Ip_u32              ver_class_flow = IP_GET_32ON16(&ip_hdr->ver_class_flow);

        /* Hop-by-hop options cannot be processed in the fast path,
         * so do not create a fast path entry for packages that
         * carries the hop-by-hop option */
#ifdef IPIPSEC2
        ipcom_memcpy(&ipsec_param.key.src.in6, &ip_hdr->src, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&ipsec_param.key.dst.in6, &ip_hdr->dst, sizeof(struct Ip_in6_addr));
        ipsec_param.key.proto  = ip_hdr->nxt;
        ipsec_param.key.domain = IP_AF_INET6;
        if (ip_hdr->nxt != IP_IPPROTO_HOPOPTS
            && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE)
            && ipipsec_3tuple_match(pkt, &ipsec_param) == IP_FALSE)

#else
        if (ip_hdr->nxt != IP_IPPROTO_HOPOPTS)
#endif /* IPIPSEC2 */
        {
            IPNET_IP6_GET_FORWARD_CACHE_ENTRY(cache, ip_hdr, ver_class_flow);
            cache->ifindex        = input_if_index;
            cache->rt_next_hop    = rt_next_hop;
            ipnet_route_cache_set_valid(&cache->rt_id);
            cache->ver_class_flow = ver_class_flow;
            cache->nxt            = ip_hdr->nxt;
            IPNET_IP6_SET_ADDR(&cache->src, &ip_hdr->src);
            IPNET_IP6_SET_ADDR(&cache->dst, &ip_hdr->dst);
        }
    }
#endif /* IPNET_FASTPATH */

    /* Check if the packet must be fragmented */
    path_mtu = (int) ((rt->metrics.rmx_mtu == 0)
                      ? netif->ipcom.mtu
                      : IP_MIN(netif->ipcom.mtu, rt->metrics.rmx_mtu));

    if (IP_LIKELY(pkt->end - pkt->start <= path_mtu)
        && IP_LIKELY(IP_BIT_ISFALSE(rt->iflags, IPNET_RTI_FLAG_FRAG_HDR)))
    {
        if (dont_cache)
            param->cache = IP_NULL;
        if (param->cache != IP_NULL)
        {
            /* No ip options, lets cache this header */
            param->cache->rt_dst      = rt;
            param->cache->rt_next_hop = rt_next_hop;
            ipcom_memcpy(&param->cache->ip_hdr, &pkt->data[pkt->start], IPNET_IP6_HDR_SIZE);
            ipnet_route_cache_set_valid(&param->cache->rt_id);
        }
    }
    else
    {
        int hdr_len = IPNET_IP6_HDR_SIZE;

        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsOutFragReqds, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsOutFragReqds, 1, netif);

        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED) || pkt->ipsec_hlen != 0)
        {
            /* Packet does not fit into the MTU, send Packet to big message */
            if (pkt->ipsec_hlen != 0)
            {
                Ipcom_pkt    *pkt_orig;

                /* Reduce MTU for IPSEC */
                path_mtu -= (int)pkt->ipsec_hlen;

                /* Recycle the original packet */
                pkt_orig = pkt->next_original;
                ip_assert(pkt_orig != IP_NULL);
                pkt->next_original = IP_NULL;
                ipcom_pkt_free(pkt);
                pkt = pkt_orig;
                pkt->start = pkt->ipstart;
            }

            ipnet_icmp6_param_init(&icmp6_param, pkt);
            icmp6_param.type = IPNET_ICMP6_PACKET_TOO_BIG;
            icmp6_param.code = 0;
            icmp6_param.data.pkt_to_big_mtu = path_mtu;
            (void) ipnet_icmp6_send(&icmp6_param);
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 18, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                              ipnet_ip6_output, IPCOM_WV_NETD_INFO_DROPPED,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(ip6_input_too_big++);
            IPCOM_MIB2_NETIF(ipv6IfStatsOutFragFails++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsOutFragFails, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsOutFragFails, 1, netif);
            goto cleanup;
        }

        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
        {
            /* The IP header must be recreated. hop-by-hop,
               destination option and routing header extensions must also
               be recreate (if they exists).
            */

            IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
            pkt->start += IPNET_IP6_HDR_SIZE;

            if (param->has_exthdr)
            {
                if (param->exthdrs.hbh != IP_NULL)
                {
                    param->nxt_hdr = ((Ip_pkt_ip6_ext_hdr *) (&pkt->data[pkt->start]))->nxt;
                    ret = (param->exthdrs.hbh->ip6h_len + 1) * 8;
                    pkt->start += ret;
                    hdr_len += ret;
                }
                if (param->exthdrs.dst != IP_NULL)
                {
                    param->nxt_hdr = ((Ip_pkt_ip6_ext_hdr *) (&pkt->data[pkt->start]))->nxt;
                    ret = (param->exthdrs.dst->ip6d_len + 1) * 8;
                    pkt->start += ret;
                    hdr_len += ret;
                }

                if (param->exthdrs.rthdr0 != IP_NULL)
                {
                    param->nxt_hdr = ((Ip_pkt_ip6_ext_hdr *) (&pkt->data[pkt->start]))->nxt;
                    ret = (param->exthdrs.rthdr0->ip6r_len + 1) * 8;
                    pkt->start += ret;
                    hdr_len += ret;
                }

                if (param->exthdrs.rthdr2 != IP_NULL)
                {
                    param->nxt_hdr = ((Ip_pkt_ip6_ext_hdr *) (&pkt->data[pkt->start]))->nxt;
                    ret = (param->exthdrs.rthdr2->ip6r_len + 1) * 8;
                    pkt->start += ret;
                    hdr_len += ret;
                }
            }
        }

        pkt = ipnet_fragment_packet(pkt, hdr_len, IP_IP6F_SIZE, path_mtu, IP_NULL);
        if (pkt == IP_NULL)
        {
            /* The original packet is free'd by ipnet_fragment_packet() */
            ret = -IP_ERRNO_ENOBUFS;
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 19, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                              ipnet_ip6_output, IPCOM_WV_NETD_NOBUFS,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(ip6_output_enobufs++);
            IPCOM_MIB2_NETIF(ipv6IfStatsOutFragFails++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsOutFragFails, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsOutFragFails, 1, netif);
            IPCOM_MIB2_NETIF(ipv6IfStatsOutDiscards++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsOutDiscards, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsOutDiscards, 1, netif);
            goto cleanup;
        }
        IPCOM_MIB2_NETIF(ipv6IfStatsOutFragOKs++);
        IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsOutFragOKs, 1);
        IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsOutFragOKs, 1, netif);
        ipnet_ip6_add_fragmentation_hdr(pkt, &param->nxt_hdr);

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

    frag_nxt_hdr = param->nxt_hdr;
    while (pkt != IP_NULL)
    {
        pkt_next_fragment  = pkt->next_fragment;
        pkt->next_fragment = IP_NULL;

        /* Add IPv6 header to all packet fragments */
        if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR))
        {
            for (;;)
            {
                int start = pkt->start;
                ret = 0;
                if (param->has_exthdr)
                {
                    param->nxt_hdr = frag_nxt_hdr;
                    ret = ipnet_ip6_add_extension_headers(param, pkt);
                }
                if (ret >= 0)
                    ret = ipnet_ip6_add_header(param, pkt);

                if (ret >= 0 && pkt->start >= netif->ipcom.link_hdr_size)
                    break;

                /* else: Could not fit all headers */
                pkt->start = start;
                ret = ipnet_increase_hdr_space(&pkt, IP_FALSE);
                if (ret < 0)
                {
                    pkt->next_fragment = pkt_next_fragment;
                    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                      1, 20, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                                      ipnet_ip6_output, IPCOM_WV_NETD_INFO_DROPPED,
                                      IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
                    IPNET_STATS(ip6_output_hdr_space++);
                    goto cleanup;
                }
            }
        }

        ++rt->use;
        if (rt != rt_next_hop)
            ++rt_next_hop->use;

        /* Clear the IP header flag to avoid problems for loopback, retransmits etc. */
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);

        /* Set ifindex for all packets */
        pkt->ifindex = netif->ipcom.ifindex;

        if (nc != IP_NULL)
        {
            if (nc->ne.state == IPNET_ND_INCOMPLETE)
            {
                ipnet_route_lock(rt_next_hop);
                ipnet_neigh_queue_until_resolved(&nc->ne.pending, pkt);
                pkt = pkt_next_fragment;
                if (ipnet_route_unlock(rt_next_hop))
                    goto cleanup;
                continue;
            }

            if (nc->ne.state == IPNET_ND_STALE && pkt_next_fragment == IP_NULL)
                ipnet_ip6_neighbor_cache_update(nc, IPNET_ND_DELAY, IP_NULL, IP_FALSE);
        }

#ifdef IPNET_USE_LOOPBACK
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST)
            && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOP_MCAST)
            && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_SIMPLEX))
        {
            Ipcom_pkt   *clone;
            Ipnet_netif *loopback;

            loopback = ipnet_loopback_get_netif(pkt->vr_index);
            if (loopback != IP_NULL)
            {
                clone = ipnet_pkt_clone(pkt, IP_TRUE);
                if (clone != IP_NULL)
                {
                    clone->ifindex = netif->ipcom.ifindex;
                    ++rt_next_hop->use;
                    (void) loopback->link_ip6_output(loopback, rt_next_hop, clone);
                }
            }
        }
        else
#endif /* IPNET_USE_LOOPBACK */
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
            && IP_IN6_IS_ADDR_LINK_LOCAL(param->to))
            /* Must store the ifindex if the interface that has the
               'to' address assigned */
            pkt->ifindex = ipnet_ip6_link_local_zone_to_ifindex(pkt->vr_index,
                                                                param->scope_id,
                                                                param->to);

#ifdef IPCOM_USE_HW_CHECKSUM_TX
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM)
            && IP_BIT_ISFALSE(netif->ipcom.drv_capabilities, IPCOM_IF_DRV_CAP_IP6_TRANSPORT_CSUM_TX))
            ipnet_finish_transport_checksum(pkt);
#endif
        if (pkt->recurs_level > IPCOM_PKT_MAX_RECURSE_LEVEL)
        {
            IPCOM_LOG1(WARNING, "Discarding IPv6 datagram on %s, too deep recurs level.", netif->ipcom.name);
            ipcom_pkt_free(pkt);
            ret = -IP_ERRNO_EHOSTUNREACH;
        }
        else
        {
            ++pkt->recurs_level;
#ifdef IPNET_USE_RFC3971
            /* Check if a digital signature needs to be added to this msg */
            if (param->nxt_hdr == IP_IPPROTO_ICMPV6
                && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
            {
                Ipnet_pkt_icmp6 *icmp_hdr;
                int rc;

                icmp_hdr = (Ipnet_pkt_icmp6 *) &pkt->data[pkt->start+IPNET_IP6_HDR_SIZE];
                switch (icmp_hdr->icmp6_type)
                {
                case IPNET_ND_NEIGHBOR_ADVERT:
                case IPNET_ND_NEIGHBOR_SOLICIT:
                case IPNET_ND_ROUTER_ADVERT:
                case IPNET_ND_ROUTER_SOLICIT:
                case IPNET_ND_REDIRECT:
                    rc = ipnet_send_sign_create(netif, rt_next_hop, pkt);
                    if (rc != IPNET_SEND_PKT_NOT_CONSUMED)
                    {
                        IPNET_IF_UNLOCK(netif);
                        return 0;
                    }
                }
            }
#endif

#ifdef IPROHC
            if (iprohc.opened &&
                (netif->ipcom.type != IP_IFT_MPLS_TUNNEL) &&
                (netif->ipcom.type != IP_IFT_TUNNEL) &&
                (pkt->ipstart == pkt->start))
            {
                /*
                 * Do ROHC on a not (MPLS and TUNNEL) interface and
                 * packet has not been processed by MPLS.
                 */
                ret = iprohc_output_hook(&netif->ipcom, pkt);
                if (ret != IPCOM_SUCCESS)
                {
                    /* ROHC failed */
                    IPCOM_LOG1(WARNING, "Discarding IPv6 datagram on %s, ROHC failed.", netif->ipcom.name);
                    ipcom_pkt_free(pkt);
                    pkt = pkt_next_fragment;
                    ret = -IP_ERRNO_EROHC;
                    goto cleanup;
                }
            }
#endif /* IPROHC */

            ret = netif->link_ip6_output(netif, rt_next_hop, pkt);
        }

        pkt = pkt_next_fragment;
        if (ret < 0)
        {
            /* The packet was released by link_ip6_output() */
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 21, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_ip6_output, IPCOM_WV_NETD_NETDOWN,
                              IPCOM_WV_IPNET_IP6_MODULE, IPCOM_WV_NETD_IP6);
            IPNET_STATS(ip6_output_link_err++);
            goto cleanup;
        }
    }

#ifdef IPIPSEC2
async:
#endif
    IPNET_IF_UNLOCK(netif);
    return 0;


 cleanup:
    if (netif)
        IPNET_IF_UNLOCK(netif);

    if (pkt != IP_NULL)
    {
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
        ipcom_pkt_free(pkt);
    }
    return ret >= 0 ? 0 : ret;
}


/*
 *===========================================================================
 *                    ipnet_ip6_get_mss
 *===========================================================================
 * Description: Returns the MSS for the connected socket 'sock'.
 * Parameters:  sock - The TCP socket to calculate MSS for.
 * Returns:     The MSS.
 *
 */
#ifdef IPTCP
IP_GLOBAL Ip_u32
ipnet_ip6_get_mss(Ipnet_socket *sock)
{
    Ipnet_route_entry *rt;
    int                ret;
    Ip_u32             mtu;
    Ip_u32             mss;

    ret = ipnet_route_lookup(IP_AF_INET6,
                             sock->vr_index,
                             IPCOM_ROUTE_TABLE_DEFAULT,
                             0,
                             &sock->ip6->daddr,
                             sock->ip6->iscope_id,
                             0,
                             &rt);

    if (ret < 0)
        mtu = ipnet_conf_ip6_min_mtu;
    else
        mtu = (rt->metrics.rmx_mtu == 0)
            ? rt->netif->ipcom.mtu
            : IP_MIN(rt->netif->ipcom.mtu, rt->metrics.rmx_mtu);

#ifdef IPIPSEC2
    {
        Ipipsec_param    ipsec_param;

        IPNET_IP6_SET_ADDR(&ipsec_param.key.src, &sock->ip6->saddr);
        IPNET_IP6_SET_ADDR(&ipsec_param.key.dst, &sock->ip6->daddr);
        ipsec_param.key.proto  = sock->proto;
        ipsec_param.key.domain = IP_AF_INET6;
        ipsec_param.key.ports.udptcp_srcdst_n[0] = ip_htons(sock->sport);
        ipsec_param.key.ports.udptcp_srcdst_n[1] = ip_htons(sock->dport);
        ipsec_param.key.srcscope_n = sock->ip6->iscope_id;
        ipsec_param.key.dstscope_n = sock->ip6->iscope_id;

        mtu -= ipipsec_output_hdrspace(&ipsec_param);
    }
#endif /* IPIPSEC2 */

    mss = mtu - sock->max_hdrspace;
    return IP_MIN((Ip_u32)sock->recv_max_bytes, mss);
}
#endif /* IPTCP */


/*
 *===========================================================================
 *                    ipnet_ip6_socket_to_netif
 *===========================================================================
 * Description: Returns the interface that has the specified address.
 * Parameters:  addr - The address the interface must have.
 * Returns:     Pointer to the interface structure or IP_NULL if no interface
 *              has the specified address.
 *
 */
#ifdef IPCOM_USE_MIB2
IP_GLOBAL Ipnet_netif *
ipnet_ip6_socket_to_netif(Ipnet_socket *sock)
{
    Ip_u32 i;
    Ipnet_netif *netif;
    struct Ipnet_ip6_addr_entry_struct *addr6_p;

    IPNET_NETIF_FOR_EACH_ON_VR(netif, sock->vr_index, i)
    {
        addr6_p = netif->inet6_addr_list;

        while(addr6_p != IP_NULL)
        {
            if(IP_IN6_ARE_ADDR_EQUAL(&addr6_p->addr, &sock->ip6->saddr))
                return netif;
            addr6_p = addr6_p->next;
        }
    }

    return IP_NULL;
}
#endif /* IPCOM_USE_MIB2 */


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */



