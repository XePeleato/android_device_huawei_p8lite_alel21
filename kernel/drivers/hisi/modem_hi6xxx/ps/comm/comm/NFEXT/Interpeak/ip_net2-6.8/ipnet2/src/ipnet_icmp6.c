/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_icmp6.c,v $ $Revision: 1.191.16.3 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_icmp6.c,v $
 *   $Author: kenneth $
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

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPSCTP
#include <ipsctp.h>
#endif

#ifdef IPNET_USE_RFC4941
#ifdef IPMCRYPTO
#include <ip/openssl/md5.h>
#else
#include <openssl/md5.h>
#endif
#endif

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_icmp6.h"
#include "ipnet_ip6.h"
#include "ipnet_route.h"
#include "ipnet_loopback.h"
#ifdef IPNET_USE_RFC3971
#include "ipnet_send.h"
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Seconds an IP address is invalid when NDP fails. */
#define IPNET_NDP_SEC_INVAL    25

/* Size of the hop-by-hop option that is added in all MLD messages */
#define IPNET_ICMP6_HBH_SIZE   8

#define IPNET_ICMP6_PREFERRED_HOPCOUNT 0
#define IPNET_ICMP6_DEFAULT_HOPCOUNT   64

/* Returns the free space at the end of an ICMPv6 packet
   NOTE: is only valid inside ipnet_icmp6_send()
 */
#define IPNET_ICMP6_PKT_PAYLOAD_SPACE(pkt) \
    ((int) ipnet_conf_ip6_min_mtu - (pkt->end - pkt->start) - IPNET_IP6_HDR_SIZE)


/* The interface is running on a link with at least one MLDv1 host/router */
#define IPNET_MLDV1_MODE   1
/* MLDv2 can be used on this link */
#define IPNET_MLDV2_MODE   2


#define IPNET_ICMP6_TWO_HOUR  (2 * 60 * 60)

#ifdef IPNET_USE_RFC4941
/* RFC4941, chapter 5. Defined Constants */
#define IPNET_RFC4941_TEMP_VALID_LIFETIME     (7 * 24 * 60 * 60) /* 1 week */
#define IPNET_RFC4941_TEMP_PREFERRED_LIFETIME (1 * 24 * 60 * 60) /* 1 day */
#define IPNET_RFC4941_REGEN_ADVANCE           5
#define IPNET_RFC4941_TEMP_IDGEN_RETRIES      3
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* MLDv2 report callback */
struct Ipnet_mldv2_report_for_each_data
{
    Ipnet_ip6_addr_entry          *addr_entry;
    Ipnet_pkt_mldv2_group_record  *group_record;
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
ipnet_icmp6_input_mld_query(Ipnet_netif *netif, int hlim, Ipcom_pkt *pkt);

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_STATIC void
ipnet_mldv2_create_membership_report(Ipcom_pkt *pkt, Ipnet_ip6_addr_entry *addr_entry);
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

#ifdef IPNET_USE_RFC4941
IP_STATIC Ip_bool
ipnet_icmp6_tmp_idgen_retries(Ipnet_netif *netif,
                              IP_CONST struct Ip_in6_addr *prefix,
                              Ip_u8 prefixlen,
                              Ip_bool dad_failed);
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
 *                         ipnet_icmp6_get_netif_from_addr
 *===========================================================================
 * Description: Returns the interface that has the specified address.
 * Parameters:  vr - The virtual router the the address must be assigned to.
 *              ipaddr - An IPv6 address.
 * Returns:     A network interface or IP_NULL if none of the interfaces
 *              has the specified address.
 */
IP_STATIC Ipnet_netif *
ipnet_icmp6_get_netif_from_addr(Ip_u16 vr, Ipnet_ip6_output_param *param)
{
    Ipnet_ip6_addr_entry        *a;
    Ipnet_route_entry           *r;
    Ipnet_route_policy_selector rps;
    Ip_bool                     dc;

    a = ipnet_ip6_get_addr_entry(param->to, vr, IP_NULL);
    if (a != IP_NULL)
        return a->netif;

    ipnet_ip6_param2policy(param, IP_NULL, &rps, IP_NULL);
    if (ipnet_route_lookup_ecmp(IP_AF_INET6,
                                vr,
                                0,
                                param->to,
                                param->scope_id,
                                IP_NULL,
                                &rps,
                                &r,
                                &dc) >= 0)
        return r->netif;

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_default_router_delete
 *===========================================================================
 * Description: Deletes a entry from the default router list and invalidates
 *              all entries in the destination cache that used the deleted
 *              router.
 * Parameters:  netif - The link from which the router should be removed.
 *              addr - The link local IPv6 address of the router.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_default_router_delete(Ipnet_netif *netif, IP_CONST struct Ip_in6_addr *addr)
{
    Ipcom_route                     *rtab;
    Ipnet_route_delete_for_each_data d;
    struct Ip_sockaddr_in6           gateway;

    ipnet_ip6_create_sockaddr(&gateway, addr, netif->ipcom.ifindex);
    ipnet_route_for_each_data_init(&d,
                                   netif,
                                   (struct Ip_sockaddr *) &gateway,
                                   IPNET_RTF_STATIC);

    if (ipnet_route_get_rtab(IP_AF_INET6, netif->vr_index, IPCOM_ROUTE_TABLE_DEFAULT, &rtab) >= 0)
        ipcom_route_walk_tree(rtab,
                              (Ipcom_route_walk_cb) ipnet_route_remove_all_cb_for_each_entry,
                              &d);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_default_router_add
 *===========================================================================
 * Description: Adds an entry in the default router list.
 * Parameters:  netif - The link this router is on.
 *              addr - The link local IPv6 address of the router.
 *              ra - The ICMPv6 router advertisment message.
 *              preferred - IP_TRUE if this is a preferred default router.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_default_router_add(Ipnet_netif *netif,
                               IP_CONST struct Ip_in6_addr *addr,
                               Ipnet_nd_router_advert *ra,
                               Ip_bool preferred)
{
    struct Ipnet_route_add_param param;
    struct Ip_sockaddr_in6       gw;
    struct Ipnet_rt_metrics      metrics;

    ip_assert(addr != IP_NULL);
    ip_assert(ra != IP_NULL);
    ip_assert(netif != IP_NULL);

    if (ra->ipnet_nd_ra_router_lifetime == 0)
    {
        ipnet_icmp6_default_router_delete(netif, addr);
        return;
    }

    if (ipnet_icmp6_default_router_change(netif,
                                          addr,
                                          IP_TRUE,
                                          ip_ntohs(ra->ipnet_nd_ra_router_lifetime),
                                          IP_TRUE,
                                          preferred))
        return;

    /* This is a new default router */
    ipnet_ip6_create_sockaddr(&gw, addr, netif->ipcom.ifindex);
    ipcom_memset(&metrics, 0, sizeof(metrics));
    metrics.rmx_expire   = ip_ntohs(ra->ipnet_nd_ra_router_lifetime);
    metrics.rmx_hopcount = (preferred
			    ? IPNET_ICMP6_PREFERRED_HOPCOUNT
			    : (IPNET_ICMP6_DEFAULT_HOPCOUNT + IPNET_ICMP6_PREFERRED_HOPCOUNT) / 2);

    ipcom_memset(&param, 0, sizeof(param));
    param.domain     = IP_AF_INET6;
    param.vr         = netif->vr_index;
    param.table      = IPCOM_ROUTE_TABLE_DEFAULT;
    param.netif      = netif;
    param.flags      = IPNET_RTF_UP | IPNET_RTF_DONE | IPNET_RTF_GATEWAY;
    param.key        = &ip_in6addr_any;
    param.netmask    = &ip_in6addr_any;
    param.gateway    = (struct Ip_sockaddr *) &gw;
    param.metrics    = &metrics;

    (void) ipnet_route_add(&param);
}


#ifdef IPNET_USE_RFC4941
/*
 *===========================================================================
 *             ipnet_icmp6_use_privacy_extension_to_autoconf
 *===========================================================================
 * Description: Checks if privacy extension to autoconfiguration is enabled
 *              for the specified prefix and interface.
 * Parameters:  netif -
 *              prefix -
 *              prefix_len -
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_icmp6_use_privacy_extension_to_autoconf(Ipnet_netif *netif,
                                              IP_CONST struct Ip_in6_addr *prefix,
                                              Ip_u8 prefix_len)
{
    char str_prefix[IP_INET6_ADDRSTRLEN];
    char *sysvar_name = ipnet->log_buf;
    int pe_enabled;

    /* RFC 4941, chapter 3.6
       ...
       Additionally, sites might wish to selectively enable or disable
       the use of temporary addresses for some prefixes.  For example,
       a site might wish to disable temporary address generation for
       "Unique local" [ULA] prefixes while still generating temporary
       addresses for all other global prefixes.  Another site might
       wish to enable temporary address generation only for the
       prefixes 2001::/16 and 2002::/16, while disabling it for all
       other prefixes.  To support this behavior, implementations
       SHOULD provide a way to enable and disable generation of
       temporary addresses for specific prefix subranges.  This per-
       prefix setting SHOULD override the global settings on the node
       with respect to the specified prefix subranges.  Note that the
       pre-prefix setting can be applied at any granularity, and not
       necessarily on a per-subnet basis.
    */
    ipcom_sprintf(sysvar_name,
                  "%s/%d.inet6.EnablePrivacyExtensions",
                  ipcom_inet_ntop(IP_AF_INET6,
                                  prefix,
                                  str_prefix,
                                  sizeof(str_prefix)),
                  prefix_len);
    if (ipcom_sysvar_get_descriptive_int0(sysvar_name,
                                          0,
                                          &pe_enabled,
                                          ipnet_bool_map) >= 0)
        return pe_enabled != 0;

    /* RFC 4941, chapter 3.6

       Devices implementing this specification MUST provide a way for
       the end user to explicitly enable or disable the use of
       temporary addresses.  In addition, a site might wish to disable
       the use of temporary addresses in order to simplify network
       debugging and operations.  Consequently, implementations SHOULD
       provide a way for trusted system administrators to enable or
       disable the use of temporary addresses.
    */
    return (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6,
                                             netif,
                                             "EnablePrivacyExtensions",
                                             IP_FALSE,
                                             ipnet_bool_map) != IP_FALSE);

}


/*
 *===========================================================================
 *                  ipnet_icmp6_calc_tmp_if_id
 *===========================================================================
 * Description: Calculates a new temporary interface ID.
 * Parameters:  netif - Any attached interface, is only used to provide
 *                      possibly unique data (MAC address).
 * Returns:     0 = success, <0 = error code.
 */
IP_STATIC void
ipnet_icmp6_calc_tmp_if_id(Ipnet_netif *netif)
{
    MD5_CTX ctx;
    Ip_u8   digest[MD5_DIGEST_LENGTH];

    MD5_Init(&ctx);

    if (IP_BIT_ISFALSE(ipnet_flags, IPNET_FLAG_IPV6_RFC4941_INIT))
    {
        unsigned i;

        /* RFC 4941
           3.2.2. In The Absence of Stable Storage

           In the absence of stable storage, no history value will be
           available across system restarts to generate a
           pseudo-random sequence of interface identifiers.
           Consequently, the initial history value used above SHOULD
           be generated at random.  A number of techniques might be
           appropriate.  Consult [RANDOM] for suggestions on good
           sources for obtaining random numbers.  Note that even
           though machines may not have stable storage for storing a
           history value, they will in many cases have configuration
           information that differs from one machine to another (e.g.,
           user identity, security keys, serial numbers, etc.).  One
           approach to generating a random initial history value in
           such cases is to use the configuration information to
           generate some data bits (which may remain constant for the
           life of the machine, but will vary from one machine to
           another), append some random data, and compute the MD5
           digest as before.
        */

        /* Include node unique information as seed */
        if (netif != IP_NULL)
            MD5_Update(&ctx, netif->ipcom.link_addr, netif->ipcom.link_addr_size);

        /* Initialize the history value to a pseudo-random value */
        for (i = 0; i < sizeof(ipnet->ip6.history_value); i++)
            ipnet->ip6.history_value[i] = (Ip_u8) ipcom_random();

        IP_BIT_SET(ipnet_flags, IPNET_FLAG_IPV6_RFC4941_INIT);
    }

    MD5_Update(&ctx, ipnet->ip6.randomized_if_id, sizeof(ipnet->ip6.randomized_if_id));
    MD5_Update(&ctx, ipnet->ip6.history_value, sizeof(ipnet->ip6.history_value));
    MD5_Final(digest, &ctx);
    ipcom_memcpy(ipnet->ip6.randomized_if_id,
                 digest,
                 sizeof(ipnet->ip6.randomized_if_id));
    ipcom_memcpy(ipnet->ip6.history_value,
                 &digest[sizeof(ipnet->ip6.randomized_if_id)],
                 sizeof(ipnet->ip6.history_value));
    /* Clear the universal/local bit to indicating local
       significance only  */
    ipnet->ip6.randomized_if_id[0] &= ~0x02;

    /* RFC 4941, chapter 3.5. Regeneration of Randomized Interface Identifiers
       ...
       Nodes following this specification SHOULD generate new
       temporary addresses on a periodic basis.  This can be achieved
       automatically by generating a new randomized interface
       identifier at least once every (TEMP_PREFERRED_LIFETIME -
       REGEN_ADVANCE - DESYNC_FACTOR) time units.
       ...

       This is implemented by regenerating the randomized host ID each
       time the valid time of a address expires, which happens every
       (TEMP_PREFERRED_LIFETIME - REGEN_ADVANCE - DESYNC_FACTOR)
       seconds.
       It is also regenerated if the node is attached to a new link.
     */
}


/*
 *===========================================================================
 *                  ipnet_icmp6_add_tmp_autonomous_addr
 *===========================================================================
 * Description: Adds an auto address configuration address.
 * Parameters:  netif - The interface to add the address to.
 *              NOTE: the "netif" pointer can be invalid when this function
 *                    returns.
 *              prefix - A address prefix.
 *              prefixlen - Number of bits that defines the network ID.
 *              preferred - The number of seconds this address is preferred.
 *              valid - The number of seconds this address is valid.
 * Returns:
 */
IP_STATIC void
ipnet_icmp6_add_tmp_autonomous_addr(Ipnet_netif *netif,
                                    IP_CONST struct Ip_in6_addr *prefix,
                                    Ip_u8 prefixlen,
                                    Ip_u32 preferred,
                                    Ip_u32 valid)
{
    struct Ip_in6_addr    new_addr;
    struct Ip_in6_addr    mask;
    Ipnet_ip6_addr_entry *addr;
    Ip_u32                regen_advance;
    Ip_u32                tmp_preferred;
    Ip_bool               calc_new_if_id;

    if (!ipnet_icmp6_tmp_idgen_retries(netif, prefix, prefixlen, IP_FALSE))
        /* Not allowed to regenerate the address anymore */
        return;

    if (!ipnet_icmp6_use_privacy_extension_to_autoconf(netif, prefix, prefixlen))
        return;

    regen_advance = (Ip_u32) ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                           netif,
                                                           "RegenAdvance",
                                                           IPNET_RFC4941_REGEN_ADVANCE);
    tmp_preferred = (Ip_u32) ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                           netif,
                                                           "TempPreferredLifetime",
                                                           IPNET_RFC4941_TEMP_PREFERRED_LIFETIME);
    /* RFC 4941, chapter 3.3 Its Preferred Lifetime is the lower of
       the Preferred Lifetime of the public address or
       TEMP_PREFERRED_LIFETIME - DESYNC_FACTOR.

       DESYNC_FACTOR is not subtracted here since RFC 4941, chapter 5 states:
       ...
       DESYNC_FACTOR -- A random value within the range 0 -
       MAX_DESYNC_FACTOR.  It is computed once at system start (rather
       than each time it is used) and must never be greater than
       (TEMP_VALID_LIFETIME - REGEN_ADVANCE).
       ...
    */
    preferred = IP_MIN(preferred, tmp_preferred);

    if (preferred <= regen_advance)
        /* RFC 4941, chapter 3.4 Expiration of Temporary Addresses
           ...
           When a temporary address becomes deprecated, a new one MUST
           be generated.  This is done by repeating the actions
           described in Section 3.3, starting at step 3).  Note that,
           except for the transient period when a temporary address is
           being regenerated, in normal operation at most one
           temporary address per prefix should be in a non-deprecated
           state at any given time on a given interface.  Note that if
           a temporary address becomes deprecated as result of
           processing a Prefix Information Option with a zero
           Preferred Lifetime, then a new temporary address MUST NOT
           be generated.
           ...
        */
        return;

    calc_new_if_id = IP_BIT_ISFALSE(ipnet_flags, IPNET_FLAG_IPV6_RFC4941_INIT);

    ipcom_memset(&mask, 0, sizeof(mask));
    ipnet_route_create_mask(&mask, prefixlen);

    for (addr = netif->inet6_addr_list;
         addr != IP_NULL && addr->type == IPNET_ADDR_TYPE_UNICAST;
         addr = addr->next)
    {
        if (IP_BIT_ISFALSE(addr->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY)
            || IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_NEXT_TMP_DONE))
            continue;
        if (ipcom_route_key_cmp(128, &addr->addr, prefix, &mask)
            == IPCOM_ROUTE_KEY_CMP_NO_MATCH)
            continue;
        if ((int) (addr->preferred_sec - IPNET_SECONDS_ABS)
            <= ipnet_sysvar_netif_get_as_int(IP_AF_INET6, netif, "RegenAdvance", 30))
        {
            /* This temporary address is about to become non-preferred */
            calc_new_if_id = IP_TRUE;
            break;
        }
        /* Valid temporary already present for this prefix, do not generate a new one */
        return;
    }

    if (calc_new_if_id)
        ipnet_icmp6_calc_tmp_if_id(netif);

    ipcom_memset(&new_addr, 0, sizeof(new_addr));
    ipcom_memcpy(&new_addr, prefix, sizeof(new_addr) / 2);
    ipcom_memcpy(&new_addr.in6.addr32[2], ipnet->ip6.randomized_if_id, sizeof(new_addr) / 2);

    /* RFC 4941, chapter 3.5. Regeneration of Randomized Interface Identifiers

       Nodes following this specification SHOULD generate new
       temporary addresses on a periodic basis.  This can be achieved
       automatically by generating a new randomized interface
       identifier at least once every (TEMP_PREFERRED_LIFETIME -
       REGEN_ADVANCE - DESYNC_FACTOR) time units.  As described above,
       generating a new temporary address REGEN_ADVANCE time units
       before a temporary address becomes deprecated produces
       addresses with a preferred lifetime no larger than
       TEMP_PREFERRED_LIFETIME.  The value DESYNC_FACTOR is a random
       value (different for each client) that ensures that clients
       don't synchronize with each other and generate new addresses at
       exactly the same time.  When the preferred lifetime expires, a
       new temporary address MUST be generated using the new
       randomized interface identifier.
    */
    preferred -= regen_advance;
    if (preferred > ipnet->ip6.desync_factor)
        /* Remove the DESYNC_FACTOR if it is not larger than
           TEMP_PREFERRED_LIFETIME - REGEN_ADVANCED */
        preferred -= ipnet->ip6.desync_factor;

    /* RFC 4941, chapter 3.3
       Its Valid Lifetime is the lower of the Valid Lifetime of the
       public address or TEMP_VALID_LIFETIME.
    */
    valid = IP_MIN(valid, (Ip_u32) ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                                 netif,
                                                                 "TempValidLifetime",
                                                                 IPNET_RFC4941_TEMP_VALID_LIFETIME));

    /* Do not add the network route that corresponds to this
       address/prefix since that is handled separately by a call to
       ipnet_icmp6_prefix_add() */
    if (ipnet_ip6_add_addr(netif,
                           &new_addr,
                           IPNET_IP6_ADDR_FLAG_AUTONOMOUS
                           | IPNET_IP6_ADDR_FLAG_DONT_ADD_NETWORK_RT
                           | IPNET_IP6_ADDR_FLAG_TEMPORARY
                           | IPNET_IP6_ADDR_FLAG_TENTATIVE,
                           preferred,
                           valid,
                           prefixlen) == 0)
        if (addr != IP_NULL && addr->type == IPNET_ADDR_TYPE_UNICAST)
            IP_BIT_SET(addr->flags, IPNET_IP6_ADDR_FLAG_NEXT_TMP_DONE);
}


/*
 *===========================================================================
 *                     ipnet_icmp6_tmp_idgen_retries
 *===========================================================================
 * Description: Checks if there another temporary address can be generated.
 * Parameters:  prefix - prefix to test
 *              prefixlen - length of the prefix mask
 *              dad_failed - IP_TRUE if called in response to DAD failure
 * Returns:     IP_TRUE - can retry
 *              IP_FALSE - cannot retry
 *
 */
IP_STATIC Ip_bool
ipnet_icmp6_tmp_idgen_retries(Ipnet_netif *netif,
                              IP_CONST struct Ip_in6_addr *prefix,
                              Ip_u8 prefixlen,
                              Ip_bool dad_failed)
{
    Ipnet_route_entry *rt;

    rt = ipnet_icmp6_prefix_get(netif,
                                prefix,
                                prefixlen);
    if (rt)
    {
        int temp_idgen_retries;
        temp_idgen_retries = ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                           netif,
                                                           "TempIdgenRetries",
                                                           IPNET_RFC4941_TEMP_IDGEN_RETRIES);
        if (dad_failed)
            ++rt->temp_idgen_retries;

        if (rt->temp_idgen_retries >= temp_idgen_retries)
        {
            if (dad_failed)
                IPCOM_LOG4(ERR,
                           "IPv6: Failed to create a temporary address after %d tries on %s for prefix %s/%d",
                           temp_idgen_retries,
                           netif->ipcom.name,
                           ipcom_inet_ntop(IP_AF_INET6, rt->hdr.key, ipnet->log_buf, sizeof(ipnet->log_buf)),
                           prefixlen);
            return IP_FALSE;
        }
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_tmp_addr_dad_failed
 *===========================================================================
 * Description: Called when duplicate address detection fails for a temporary
 *              address
 * Parameters:  addr6 - address for which DAD failed
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_tmp_addr_dad_failed(Ipnet_ip6_addr_entry *addr6)
{
    Ipnet_netif *netif = addr6->netif;

    /* RFC4941, chapter 3.3
       ...
       7.  The node MUST perform duplicate address
       detection (DAD) on the generated temporary
       address.  If DAD indicates the address is
       already in use, the node MUST generate a new
       randomized interface identifier as described
       in Section 3.2 above, and repeat the
       previous steps as appropriate up to
       TEMP_IDGEN_RETRIES times.  If after
       TEMP_IDGEN_RETRIES consecutive attempts no
       non-unique address was generated, the node
       MUST log a system error and MUST NOT attempt
       to generate temporary addresses for that
       interface.  Note that DAD MUST be performed
       on every unicast address generated from this
       randomized interface identifier.
       ...
    */

    if (ipnet_icmp6_tmp_idgen_retries(netif,
                                      &addr6->addr,
                                      addr6->prefixlen,
                                      IP_TRUE))
    {
        ipnet_icmp6_calc_tmp_if_id(netif);
        ipnet_icmp6_add_tmp_autonomous_addr(netif,
                                            &addr6->addr,
                                            addr6->prefixlen,
                                            addr6->preferred_sec - addr6->added_sec,
                                            addr6->expires_sec - addr6->added_sec);
    }
}
#endif /* IPNET_USE_RFC4941 */


/*
 *===========================================================================
 *                    ipnet_icmp6_check_tentative_addresses
 *===========================================================================
 * Description: Check if this message was sent to any of the current tentative
 *              addresses on this node.
 * Parameters:  netif - The interface on which the packet was received.
 *              addr - The address for which a Neighbor Solication or
 *              Neighbor Advertisment message has been received.
 *              is_sol - IP_TRUE if this function is done for a neighbor
 *              solicitation.
 *              src - The sender of the neighbor solicitation, should be
 *              IP_NULL if 'is_sol' is IP_FALSE.
 * Returns:     IPNET_IP6_STOP_PROCESSING = No more processing should be done on this message
 *              IPNET_IP6_CONTINUE_PROCESSING = Continue to process the message.
 */
IP_STATIC int
ipnet_icmp6_check_tentative_addresses(Ipnet_netif *netif,
                                      Ipcom_pkt *pkt,
                                      IP_CONST struct Ip_in6_addr* addr,
                                      Ip_bool is_sol,
                                      IP_CONST struct Ip_in6_addr* src)
{
    struct Ip_in6_addr     eui_64_addr;
    Ipnet_ip6_addr_entry  *addr6;
    Ipnet_ip6_addr_entry **paddr6;
    int                    start;
    int                    ret;
    char                   buf[IP_INET6_ADDRSTRLEN];

    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
        || IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
        /* Sent by this node on loopback */
        return IPNET_IP6_STOP_PROCESSING;

    for (paddr6 = &netif->inet6_addr_list; *paddr6 != IP_NULL; paddr6 = &(*paddr6)->next)
    {
        addr6 = *paddr6;
        if (IP_IN6_ARE_ADDR_EQUAL(addr, &addr6->addr))
        {
            if (is_sol
                && IP_BIT_ISSET(addr6->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE)
                && !IP_IN6_IS_ADDR_UNSPECIFIED(src))
                /* Must ignore address resolutions for this address when in tentative mode */
                return IPNET_IP6_STOP_PROCESSING;

            if (netif->link_get_frame_info != IP_NULL)
            {
                start = pkt->start;
                pkt->start = pkt->ipstart - netif->ipcom.link_hdr_size;
                ret = netif->link_get_frame_info(netif, pkt, IP_NULL, IP_NULL, (Ip_u8 *) buf, IP_NULL);
                pkt->start = start;
                if (ret < 0)
                    /* Failed to get the link source address from the frame */
                    return ret;

                if (0 < netif->ipcom.link_addr_size
                    && ipcom_memcmp(netif->ipcom.link_addr, buf, netif->ipcom.link_addr_size) == 0)
                    /* Sent by this node */
                    return IPNET_IP6_STOP_PROCESSING;
            }

            if (IP_BIT_ISFALSE(addr6->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
                return IPNET_IP6_CONTINUE_PROCESSING;

            /* Duplicate Address Detection has failed since the message
               was not sent by this node. Remove the tentive address since
               the node cannot use it */
            IPCOM_LOG1(WARNING,
                       "IPv6: DAD failed for tentative address %s",
                       ipcom_inet_ntop(IP_AF_INET6, addr, buf, sizeof(buf)));

#ifdef IPNET_USE_RFC4941
            if (IP_BIT_ISSET(addr6->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY))
                ipnet_icmp6_tmp_addr_dad_failed(addr6);
#endif /* IPNET_USE_RFC4941 */

            (void) ipcom_inet_pton(IP_AF_INET6, "FE80::", &eui_64_addr);
            if (netif->link_ip6_create_addr(netif, &eui_64_addr, &eui_64_addr) == 0
                && IP_IN6_ARE_ADDR_EQUAL(&addr6->addr, &eui_64_addr))
            {
                /* RFC4862, chapter 5.4.5.
                   ...
                   If the address is a link-local address formed from
                   an interface identifier based on the hardware
                   address, which is supposed to be uniquely assigned
                   (e.g., EUI-64 for an Ethernet interface), IP
                   operation on the interface SHOULD be disabled.
                   ...
                */
                IPCOM_LOG2(WARNING,
                           "IPv6: DAD failed for non-temporary link local address %s on (%s)",
                           ipcom_inet_ntop(IP_AF_INET6, addr, buf, sizeof(buf)),
                           netif->ipcom.name);
                IP_BIT_SET(netif->flags2, IPNET_IFF2_IPV6_DISABLED);
            }

            (void)ipnet_ip6_remove_addr(netif, &addr6->addr);

            return IPNET_IP6_STOP_PROCESSING;
        }
    }
    return IPNET_IP6_CONTINUE_PROCESSING;
}



/*
 *===========================================================================
 *                  ipnet_icmp6_add_autonomous_addr
 *===========================================================================
 * Description: Adds an auto address configuration address.
 * Parameters:  netif - The interface to add the address to.
 *              NOTE: 'netif' may have been freed when this function returns
 *              since the code lock can be released/locked before this
 *              functions returns.
 *              opt - The prefix option sent from a router.
 *              preferred - The number of seconds this address is preferred.
 *              valid - The number of seconds this address is valid.
 * Returns:     0 = success, <0 = error code.
 */
IP_STATIC void
ipnet_icmp6_add_autonomous_addr(Ipnet_netif *netif,
                                Ipnet_pkt_nd_opt_prefix_info *opt,
                                Ip_u32 preferred,
                                Ip_u32 valid)
{
    struct Ip_in6_addr new_addr;
    Ip_u32             addr_flags;
    int                ret;

    if (ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                      netif,
                                      "AddAutonomousAddr", IP_TRUE) == IP_FALSE)
        /* Autonomous address disabled on this interface */
        return;

#ifdef IPNET_USE_RFC4941
    ipnet_icmp6_add_tmp_autonomous_addr(netif,
                                        &opt->nd_opt_pi_prefix,
                                        opt->nd_opt_pi_prefix_len,
                                        preferred,
                                        valid);
#endif /* IPNET_USE_RFC4941 */

    ret = netif->link_ip6_create_addr(netif, &opt->nd_opt_pi_prefix, &new_addr);
    if (ret < 0)
    {
        IPCOM_LOG2(WARNING,
                   "IPv6: failed to create an automatic address for interface %s (%s)",
                   netif->ipcom.name, ipcom_strerror(-ret));
        return;
    }

    if (valid == 0)
    {
        Ipnet_ip6_addr_entry *addr_entry;
        /* Address delete request */
        addr_entry = ipnet_ip6_get_addr_entry(&new_addr,
                                              netif->vr_index,
                                              netif);
        if (addr_entry != IP_NULL
            && IP_BIT_ISSET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS))
            (void)ipnet_ip6_remove_addr(netif, &new_addr);
        return;
    }

    /* Do not add the network route that corresponds to this address/prefix
       since that is handled separately by a call to ipnet_icmp6_prefix_add() */
    addr_flags = IPNET_IP6_ADDR_FLAG_AUTONOMOUS | IPNET_IP6_ADDR_FLAG_DONT_ADD_NETWORK_RT;

    if (ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                      netif,
                                      "DupAddrDetectTransmits", 0) > 0)
    {
        IP_BIT_SET(addr_flags, IPNET_IP6_ADDR_FLAG_TENTATIVE);
    }

    (void) ipnet_ip6_add_addr(netif,
                              &new_addr,
                              addr_flags,
                              preferred,
                              valid,
                              opt->nd_opt_pi_prefix_len);
}


/*
 *===========================================================================
 *                       ipnet_icmp6_prefix_get_ex
 *===========================================================================
 * Description: Returns a prefix entry.
 * Parameters:  netif - interface the prefix must be valid for
 *              prefix - the prefix
 *              prefix_len - the prefix length
 *              gw_addr - gateway to use in order to reach the network
 *                        defined by prefix/prefix_len or IP_NULL if the
 *                        prefix is link-local to this node
 * Returns:     The prefix or IP_NULL if no matching prefix exists.
 *
 */
IP_STATIC Ipnet_route_entry *
ipnet_icmp6_prefix_get_ex(Ipnet_netif *netif,
                          IP_CONST struct Ip_in6_addr *prefix,
                          Ip_u8 prefix_len,
                          IP_CONST struct Ip_in6_addr *gw_addr,
                          struct Ipnet_ipv6_key  *pkey,
                          struct Ipnet_ipv6_key  *pmask,
                          struct Ip_sockaddr_in6 *pgw)
{
    struct Ipnet_ipv6_key  key;
    struct Ipnet_ipv6_key  mask;
    struct Ip_sockaddr_in6 gw;
    Ipnet_route_entry     *rt;

    IPNET_IP6_SET_ADDR(&key.addr, prefix);
    ipcom_memset(&mask, 0, sizeof(mask));
    ipnet_route_create_mask(&mask.addr, prefix_len);
    ipnet_route_apply_mask(&key.addr, &mask.addr, 128);

    key.scope_id = IPNET_IP6_DEFAULT_ZONE;
    if (ipnet_ip6_is_scoped_addr(&key.addr))
    {
        mask.scope_id = ~mask.scope_id;
        key.scope_id = ipnet_ip6_get_zone(netif->ipcom.ifindex,
                                          &key.addr);
    }

    if (gw_addr)
    {
        ipnet_ip6_create_sockaddr(&gw, gw_addr, netif->ipcom.ifindex);
        if (pgw)
            *pgw = gw;
    }

    if (pkey)
        *pkey = key;

    if (pmask)
        *pmask = mask;

    if (ipnet_route_raw_lookup2(IP_AF_INET6,
                                netif->vr_index,
                                IPCOM_ROUTE_TABLE_DEFAULT,
                                0,
                                &key.addr,
                                key.scope_id,
                                &mask,
                                gw_addr ? (struct Ip_sockaddr *) &gw : IP_NULL,
                                netif->ipcom.ifindex,
                                &rt) == IPNET_ROUTE_PERFECT_MATCH)
        return rt;
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_prefix_add
 *===========================================================================
 * Description: Adds a new prefix entry.
 * Parameters:  netif - The network interface on which this prefix was announced.
 *              router_addr - The address of the router that sent this prefix.
 *              opt - The prefix option.
 *              lifetime_sec - The number of seconds this prefix is valid.
 * Returns:     0 = success,
 *              1 = stop processing this prefix
 *              <0 = error code.
 *
 */
IP_STATIC int
ipnet_icmp6_prefix_add(Ipnet_netif *netif,
                       IP_CONST struct Ip_in6_addr *router_addr,
                       Ipnet_pkt_nd_opt_prefix_info *opt,
                       Ip_u32 *lifetime_sec)
{
    struct Ipnet_route_add_param param;
    struct Ipnet_rt_metrics      metrics;
    Ipnet_route_entry           *rt;
    Ip_bool                      on_link;
    struct Ipnet_ipv6_key        key;
    struct Ipnet_ipv6_key        mask;
    struct Ip_sockaddr_in6       gw;

    on_link = IP_BIT_ISTRUE(opt->nd_opt_pi_flags_reserved, IPNET_ND_OPT_PI_FLAG_ONLINK);

    rt = ipnet_icmp6_prefix_get_ex(netif,
                                   &opt->nd_opt_pi_prefix,
                                   opt->nd_opt_pi_prefix_len,
                                   on_link ? IP_NULL : router_addr,
                                   &key,
                                   &mask,
                                   &gw);
    if (rt != IP_NULL)
    {
        /* RFC2462, chapter 5.5.3
           ...
           1) If the received Lifetime is greater than 2 hours or greater
              than StoredLifetime, update the stored Lifetime of the
              corresponding address.

           2) If the StoredLifetime is less than or equal to 2 hours and the
              received Lifetime is less than or equal to StoredLifetime,
              ignore the prefix, unless the Router Advertisement from which
              this Prefix Information option was obtained has been
              authenticated (e.g., via IPSec [RFC2402]). If the Router
              Advertisment was authenticated, the StoredLifetime should be
              set to the Lifetime in the received option.

           3) Otherwise, reset the stored Lifetime in the corresponding
              address to two hours.
              ...
         */
        if (IPCOM_IS_LT(*lifetime_sec, IPNET_ICMP6_TWO_HOUR)
            && IPCOM_IS_LT(*lifetime_sec, rt->metrics.rmx_expire - IPNET_SECONDS_ABS)
            /* TODO: && !authenticated */ )
        {
            if (IPCOM_IS_LT(rt->metrics.rmx_expire - IPNET_SECONDS_ABS, IPNET_ICMP6_TWO_HOUR))
                return IPNET_IP6_STOP_PROCESSING;
            *lifetime_sec = IPNET_ICMP6_TWO_HOUR;
        }

        /* The lifetime should be updated */
        IP_BIT_SET(rt->hdr.flags, IPNET_RTF_MODIFIED);
        ipnet_route_set_lifetime(rt, *lifetime_sec);
        IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
        return IPNET_IP6_CONTINUE_PROCESSING;
    }

    if (lifetime_sec == 0)
        /* Delete request and the prefix is already removed */
        return IPNET_IP6_CONTINUE_PROCESSING;

    ipcom_memset(&metrics, 0, sizeof(struct Ipnet_rt_metrics));
    metrics.rmx_expire = *lifetime_sec;

    ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
    param.key        = &key;
    param.netmask    = &mask;
    param.flags      = IPNET_RTF_UP | IPNET_RTF_DONE | IPNET_RTF_X_AUTO;
    param.domain     = IP_AF_INET6;
    param.vr         = netif->vr_index;
    param.table      = IPCOM_ROUTE_TABLE_DEFAULT;
    param.netif      = netif;
    param.metrics    = &metrics;

    if (!on_link)
    {
        /* This does not mean that the prefix is off-link, but it could be so
           packet sent to this prefix has to go through the router */
        param.gateway = (struct Ip_sockaddr *) &gw;
        IP_BIT_SET(param.flags, IPNET_RTF_GATEWAY);
        /* Use a higher cost that 0 so that user added routes will override */
        metrics.rmx_hopcount = IPNET_ICMP6_DEFAULT_HOPCOUNT;
    }
    else if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_NOARP | IP_IFF_POINTOPOINT))
        IP_BIT_SET(param.flags, IPNET_RTF_CLONING);

    return ipnet_route_add(&param);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_dst_create_rt
 *===========================================================================
 * Description: Creates a host route to the specified destination.
 * Parameters:  netif - The interface to use when sending packets to the destination.
 *              dst - The destination address.
 *              gw - The gateway to use or IP_NULL if the stack should select
 *              a gateway.
 * Returns:     The route entry or IP_NULL if it could not be created.
 *
 */
IP_STATIC Ipnet_route_entry *
ipnet_icmp6_dst_create_rt(Ipnet_netif *netif,
                          struct Ip_in6_addr *dst,
                          struct Ip_in6_addr *gw)
{
    Ipnet_route_entry           *rt;
    int                          ret;
    Ip_u32                       lifetime;
    struct Ipnet_route_add_param param;
    struct Ipnet_rt_metrics      metrics;
    struct Ipnet_ipv6_key        key;
    struct Ip_sockaddr_in6       gateway;
    Ip_bool                      is_redirect = (gw != IP_NULL);

    ip_assert(dst != IP_NULL);

    IPNET_IP6_SET_ADDR(&key.addr, dst);  /*lint !e668 Possibly passing a null pointer */
    if (ipnet_ip6_is_scoped_addr(&key.addr))
        key.scope_id = ipnet_ip6_get_zone(netif->ipcom.ifindex,
                                          &key.addr);
    else
        key.scope_id = IPNET_IP6_DEFAULT_ZONE;

    lifetime = ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                             netif,
                                             "DstCacheToLive",
                                             600);

    ret = ipnet_route_raw_lookup(IP_AF_INET6,
                                 netif->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 0,
                                 dst,
                                 key.scope_id,
                                 netif->ipcom.ifindex,
                                 &rt);

    if (ret < 0
        || (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_GATEWAY)
            && is_redirect))
        /* No route to the gateway or the host is directly reachable */
        return IP_NULL;

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
        ipcom_memcpy(&gateway, rt->gateway, sizeof(gateway));

    if (ret == IPNET_ROUTE_PERFECT_MATCH)
    {
        if (!is_redirect || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_DYNAMIC))
            /* Not added in response to a redirect, keep this entry */
            return rt;
        /* The route already exists, remove it and add it using the new parameters */
        (void) ipnet_route_delete2(IP_AF_INET6, netif->vr_index, IPCOM_ROUTE_TABLE_DEFAULT,
                                   rt->hdr.key, rt->hdr.mask, rt->gateway,
                                   rt->netif->ipcom.ifindex, 0, 0, IP_FALSE);
    }

    ipcom_memset(&metrics, 0, sizeof(struct Ipnet_rt_metrics));
    metrics.rmx_expire = lifetime;

    ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
    param.domain  = IP_AF_INET6;
    param.key     = &key;
    param.gateway = IP_IN6_IS_ADDR_MULTICAST(dst) ? IP_NULL : (struct Ip_sockaddr *) &gateway;
    param.vr      = netif->vr_index;
    param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
    param.netif   = netif;
    param.metrics = &metrics;
    param.flags   = IPNET_RTF_UP | IPNET_RTF_DYNAMIC | IPNET_RTF_DONE | IPNET_RTF_HOST;

    if (is_redirect)
    {
        if (!IP_IN6_ARE_ADDR_EQUAL(dst, gw))
        {
            ipnet_ip6_create_sockaddr(&gateway, gw, netif->ipcom.ifindex);
            IP_BIT_SET(param.flags, IPNET_RTF_GATEWAY);
        }
    }
    else if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
        IP_BIT_SET(param.flags, IPNET_RTF_GATEWAY);

    if (IP_BIT_ISSET(param.flags, IPNET_RTF_GATEWAY)
        || IP_IN6_IS_ADDR_MULTICAST(dst))
        ret = ipnet_route_add(&param);
    else
        ret = ipnet_route_create_llinfo_entry(param.domain,
                                              param.vr,
                                              param.key,
                                              IP_NULL,
                                              param.netif);
    if (ret < 0)
        return IP_NULL;

    ret = ipnet_route_raw_lookup(IP_AF_INET6,
                                 netif->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 0,
                                 dst,
                                 key.scope_id,
                                 netif->ipcom.ifindex,
                                 &rt);
    if (ret < 0)
        return IP_NULL;

    return rt;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_validate_options_len
 *===========================================================================
 * Description: Checks that all included options has a valid length field
 * Parameters:  [in] first_opt - Points to the first option in the packet.
 *              [in] tot_opt_len - Length in byte of ICMPv6 header + data.
 * Returns:     IP_FALSE if some option was invalid, the whole IP packet must
 *              be discared.
 *
 */
IP_GLOBAL Ip_bool
ipnet_icmp6_validate_options_len(Ipnet_pkt_nd_opt_hdr *first_opt, int tot_opt_len)
{
    int                   opt_len;
    Ipnet_pkt_nd_opt_hdr *opt = first_opt;

    while (tot_opt_len > 0)
    {
        opt_len = opt->nd_opt_len;
        tot_opt_len -= opt_len << 3;
        if (opt_len == 0 || tot_opt_len < 0)
            return IP_FALSE;
        opt = (Ipnet_pkt_nd_opt_hdr *) ((Ip_u16*) opt + (opt_len << 2));
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_get_option
 *===========================================================================
 * Description: Returns a specific option.
 *              NOTE: it is unsafe to call this function without validating
 *              the options with a call to ipnet_icmp6_validate_options_len()
 * Parameters:  [in] first_opt - Points to the first option in the packet.
 *              [in] tot_opt_len - Length in byte of ICMPv6 header + data.
 *              [in] type - The option type to find.
 *              [in] nr - The number of the option (if the same option can occure
 *              multiple times).
 * Returns:     The option or IP_NULL if it cannot be found.
 *
 */
IP_GLOBAL Ipnet_pkt_nd_opt_hdr *
ipnet_icmp6_get_option(Ipnet_pkt_nd_opt_hdr *first_opt,
                       int tot_opt_len,
                       Ip_u16 type,
                       int nr)
{
    int                   opt_len;
    Ipnet_pkt_nd_opt_hdr *opt = first_opt;

    while (tot_opt_len > 0)
    {
        opt_len = opt->nd_opt_len;
        if (opt->nd_opt_type == type && nr-- == 0)
            return opt;
        tot_opt_len -= opt_len << 3;
        opt = (Ipnet_pkt_nd_opt_hdr *) ((Ip_u16*) opt + (opt_len << 2));
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                ipnet_icmp6_process_flags_expansion_opt
 *===========================================================================
 * Description: Processes the Flags Expansion Option.
 * Parameters:  netif - interface the option was received on
 *              opt_ptr - pointer to beginning of option.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_process_flags_expansion_opt(Ipnet_netif *netif,
                                        void *opt_ptr)
{
    Ipnet_pkt_nd_opt_flags_expansion *opt = opt_ptr;

    if (IPNET_ND_OPT_FE_BIT_ISSET(opt, IPNET_ND_OPT_FE_PRIV_0))
        IPCOM_LOG1(INFO, "Private Experimentation flag #0 is set in RA received on %s",
                   netif->ipcom.name);

    if (IPNET_ND_OPT_FE_BIT_ISSET(opt, IPNET_ND_OPT_FE_PRIV_1))
        IPCOM_LOG1(INFO, "Private Experimentation Flag #1 is set in RA received on %s",
                   netif->ipcom.name);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_echo_request
 *===========================================================================
 * Description: Processes echo request message.
 * Parameters:  src - The address of the echo request sender.
 *              dst - The address this echo request was directed to, can
 *              be unicast .
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_echo_request(Ipnet_netif *netif,
                               IP_CONST struct Ip_in6_addr *src,
                               IP_CONST struct Ip_in6_addr *dst,
                               Ipcom_pkt *pkt)
{
    Ipnet_icmp6_param icmp6_param;

    if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6, netif, "IcmpIgnoreEchoRequest", 0, ipnet_bool_map))
    {
        IPCOM_LOG2(DEBUG, "ignore echo6 request from %s on %s",
                   ipcom_inet_ntop(IP_AF_INET6, src, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   netif->ipcom.name);
        ipcom_pkt_free(pkt);
        return 0;
    }

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 1, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                      ipnet_icmp6_input_echo_request, IPCOM_WV_NETD_INFO_RECEIVE,
                      IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);

    icmp6_param.vr         = pkt->vr_index;
    icmp6_param.ifindex    = pkt->ifindex;
    icmp6_param.recv_pkt   = pkt;
    icmp6_param.to         = src;
    icmp6_param.type       = IPNET_ICMP6_ECHO_REPLY;
    icmp6_param.code       = 0;
    icmp6_param.rate_limit = IP_TRUE;

    if (ipnet_ip6_get_addr_type(dst,
                                pkt->vr_index,
                                ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex))
        != IPNET_ADDR_TYPE_UNICAST)
    {
        icmp6_param.from = ipnet_ip6_get_src_addr(pkt->vr_index,
                                                  dst,
                                                  pkt->ifindex,
                                                  IP_NULL,
                                                  IP_NULL,
                                                  netif,
                                                  0);
    }
    else
        icmp6_param.from = dst;

    return ipnet_icmp6_send(&icmp6_param);
}


/*
 *===========================================================================
 *                   ipnet_icmp6_input_notify_affected_sockets
 *===========================================================================
 * Description: Checks if the ICMP message affect any sockets and notify
 *              it in that case.
 * Parameters:  pkt - Packet containing a ICMPv6 message, pkt->start is the
 *                    offset to the ICMPv6 header. The packet can be modified
 *                    and will freed.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_input_notify_affected_sockets(Ipcom_pkt *pkt)
{
    Ipnet_process_ext_param failed_pkt_ext_param;
    Ipnet_socket           *sock;
    Ipnet_pkt_icmp6        *icmp6;
    Ipnet_pkt_ip6 *failed_ip6_hdr = IP_NULL;
    int                     proto;
    Ip_u16                  sport = 0;
    Ip_u16                  dport = 0;
    int                     so_errno;

    icmp6 = (Ipnet_pkt_icmp6*) &pkt->data[pkt->start];

    /* Move to start of the IPv6 header of the packet that caused the
       destination unreachable error to be sent */
    pkt->start += IPNET_ICMPV6_HDR_SIZE;
    if (pkt->end - pkt->start > IPNET_IP6_HDR_SIZE)
    {
        /* Packet might be truncated, recalculate the payload length
           so the ipnet_ip6_process_exthdr() does not reject the packet */
        failed_ip6_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];
        failed_ip6_hdr->plen = (Ip_u16) ip_htons(pkt->end - pkt->start - IPNET_IP6_HDR_SIZE);
    }

    failed_pkt_ext_param.ppkt           = &pkt;
    failed_pkt_ext_param.route          = IP_FALSE;
    failed_pkt_ext_param.error          = IP_TRUE;
    failed_pkt_ext_param.ip6_dst_entry  = IP_NULL;
    proto = ipnet_ip6_process_exthdr(&failed_pkt_ext_param);

    if (proto <= 0)
    {
        /* Failed to process the extension headers */
        if (pkt != IP_NULL)
            ipcom_pkt_free(pkt);
        return;
    }
#ifdef IPSCTP
    if (proto == IP_IPPROTO_SCTP && pkt->end - pkt->start >= IPSCTP_SIZE_SCTP_HDR)
    {
        ipsctp_icmp6_cb(pkt, icmp6, failed_ip6_hdr);
        return;
    }
#endif

    if ((proto == IP_IPPROTO_UDP || proto == IP_IPPROTO_TCP)
        && pkt->end - pkt->start >= IPNET_UDP_HDR_SIZE)
    {
        /* TCP ports is located at the same offset as UDP ports */
        Ipnet_pkt_udp *failing_udp = (Ipnet_pkt_udp*) &pkt->data[pkt->start];

        sport = ip_ntohs(failing_udp->sport);
        dport = ip_ntohs(failing_udp->dport);
    }

    /* Find the affected socket */
    sock = ipnet_sock_ip6_lookup(pkt->vr_index,
                                 proto,
                                 &failed_pkt_ext_param.ip6_hdr->src, sport,
                                 &failed_pkt_ext_param.ip6_hdr->dst, dport);

    if (sock != IP_NULL
        && IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED  | IPNET_SOCKET_FLAG_CONNECTING))
    {
#ifdef IPTCP
        if (sock->tcb != IP_NULL)
        {
            if (pkt->end - pkt->start >= IPTCP_TCP_HDR_SIZE)
                iptcp_icmp6_report(sock, icmp6->icmp6_type, icmp6->icmp6_code, &pkt->data[pkt->start]);
        }
        else
#endif
        if (IPNET_ICMP6_DST_UNREACH == icmp6->icmp6_type)
        {
            switch (icmp6->icmp6_code)
            {
            case IPNET_ICMP6_DST_UNREACH_NOROUTE:
                so_errno = IP_ERRNO_ENETUNREACH;
            break;
            case IPNET_ICMP6_DST_UNREACH_BEYONDSCOPE:
            case IPNET_ICMP6_DST_UNREACH_ADDR:
                so_errno = IP_ERRNO_EHOSTUNREACH;
                break;
            case IPNET_ICMP6_DST_UNREACH_ADMIN:
            case IPNET_ICMP6_DST_UNREACH_SRC_ADDR_FAILED:
            case IPNET_ICMP6_DST_UNREACH_REJECT_DST:
            case IPNET_ICMP6_DST_UNREACH_NOPORT:
                so_errno = IP_ERRNO_ECONNREFUSED;
                break;
            default:
                /* invalid/unsupported code, ignore the packet */
                so_errno = 0;
                break;
            }

            if (so_errno)
                /* Wake up any process sleeping on the socket */
                ipnet_sock_data_avail(sock, so_errno, IP_SHUT_RDWR);
        }
    }
    ipcom_pkt_free(pkt);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_dst_unreachable
 *===========================================================================
 * Description: Processes destination unreachable message.
 * Parameters:  netif - The network interface that received the message
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_dst_unreachable(Ipcom_pkt *pkt)
{
    ipnet_icmp6_input_notify_affected_sockets(pkt);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_packet_too_big
 *===========================================================================
 * Description: Processes packet too big message (used by path MTU discovery).
 * Parameters:  netif - The network interface that received the message
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_packet_too_big(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_icmp6    *icmp6;
    Ipnet_pkt_ip6      *exceeding_ip6;
    Ipnet_route_entry  *rt;

    icmp6 = (Ipnet_pkt_icmp6*) &pkt->data[pkt->start];

    /* Locate the start of the IPv6 header of the exceeding packet */
    exceeding_ip6 = (Ipnet_pkt_ip6*) &pkt->data[pkt->start + IPNET_ICMPV6_HDR_SIZE];

    rt = ipnet_icmp6_dst_create_rt(netif, &exceeding_ip6->dst, IP_NULL);
    if (rt != IP_NULL)
    {
        Ip_u32 new_path_mtu;
        Ip_u32 current_path_mtu;

        current_path_mtu = (rt->metrics.rmx_mtu ? rt->metrics.rmx_mtu : netif->ipcom.mtu);
        new_path_mtu = IP_GET_NTOHL(&icmp6->ipnet_icmp6_mtu);
        if (new_path_mtu < ipnet_conf_ip6_min_mtu)
        {
            new_path_mtu = ipnet_conf_ip6_min_mtu;
            IP_BIT_SET(rt->iflags, IPNET_RTI_FLAG_FRAG_HDR);
        }

        if (current_path_mtu > new_path_mtu)
        {
            rt->metrics.rmx_mtu = new_path_mtu;
            IP_BIT_SET(rt->hdr.flags, IPNET_RTF_MODIFIED);
            IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));
            IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
        }
    }

    ipnet_icmp6_input_notify_affected_sockets(pkt);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_router_sol
 *===========================================================================
 * Description: Extracts link layer addresses from router solicitation message.
 * Parameters:  netif - The network interface that received the message
 *              src - The address (link-local or unspecified) of the host
 *              that sent this message.
 *              hlim - The hoplimit value from the IPv6 header.
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_router_sol(Ipnet_netif *netif,
                             IP_CONST struct Ip_in6_addr *src,
                             int hlim,
                             Ipcom_pkt *pkt)
{
    Ipnet_pkt_nd_opt_hdr *opt;
    Ipnet_ip6_nc_entry   *neighbor;
    int                   rs_payload_len;
    Ipnet_pkt_icmp6      *rs = (Ipnet_pkt_icmp6*) &pkt->data[pkt->start];

    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
        || IP_BIT_ISFALSE(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT))
        /* Ignore router advertisments from loopback and
           links where this host acts as a router */
        goto done;

    if (hlim != 255
        || rs->icmp6_code != 0
        || (IP_IN6_IS_ADDR_LINK_LOCAL(src) == IP_FALSE
            && IP_IN6_IS_ADDR_UNSPECIFIED(src) == IP_FALSE))
        goto invalid_packet;

    if (IP_IN6_IS_ADDR_UNSPECIFIED(src))
        /* Cannot contain link layer information about the sender */
        goto done;

    neighbor = ipnet_ip6_neighbor_cache_find(netif, src, IP_TRUE);
    if (neighbor == IP_NULL)
        goto done;

    /* Try to get the link address of the sender */
    rs_payload_len = pkt->end - pkt->start - sizeof(Ipnet_pkt_icmp6);
    if (!ipnet_icmp6_validate_options_len((Ipnet_pkt_nd_opt_hdr*) (rs + 1),
                                          rs_payload_len))
    {
        /* Faulty option found */
        goto invalid_packet;
    }

    /* RFC 2461, chapter 6.2.6
       ...
       Whether or not a Source Link-Layer Address option
       is provided, if a Neighbor Cache entry for the solicitation's sender
       exists (or is created) the entry's IsRouter flag MUST be set to
       FALSE.
       ...
    */
    neighbor->is_router = IP_FALSE;

    opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (rs + 1),
                                 rs_payload_len,
                                 IPNET_ND_OPT_SOURCE_LINKADDR,
                                 0);
    if (opt)
    {
        struct Ip_sockaddr_dl *gw_dl;

        /* The requestor included its link layer address */
        if (opt->nd_opt_len != ((9 + netif->ipcom.link_addr_size) >> 3))
            /* Wrong length on the link address */
            goto invalid_packet;


        gw_dl = (struct Ip_sockaddr_dl*) neighbor->ne.rt->gateway;
        if (ipcom_memcmp(opt->nd_opt_data,
                         IP_SOCKADDR_DL_LLADDR(gw_dl),
                         gw_dl->sdl_alen) != 0)
            ipnet_ip6_neighbor_cache_update(neighbor, IPNET_ND_STALE, opt->nd_opt_data, IP_FALSE);
    }

done:
    ipcom_pkt_free(pkt);
    return 0;

invalid_packet:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_router_adv
 *===========================================================================
 * Description: Processes router advertisment message.
 * Parameters:  netif - The network interface that received the message
 *              src - The address (link-local) of the router that sent this message.
 *              hlim - The hoplimit value from the IPv6 header.
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_router_adv(Ipnet_netif *netif,
                             IP_CONST struct Ip_in6_addr *src,
                             int hlim,
                             Ipcom_pkt *pkt)
{
    Ipnet_nd_router_advert       *ra;
    Ipnet_pkt_nd_opt_hdr         *opt;
    Ipnet_pkt_nd_opt_prefix_info *prefix_opt;
    Ipnet_ip6_nc_entry           *neighbor;
    Ip_u32                        valid_lifetime;
    Ip_u32                        preferred_lifetime;
    Ip_u32                        tmp_u32;
    int                           ra_payload_len;
    int                           i;
    Ip_u32                        link_mtu = netif->ipcom.mtu;
    Ip_bool                       preferred_router = IP_FALSE;

    IPCOM_LOG2(DEBUG,
               "IPv6: Got RA from %s on %s",
               ipcom_inet_ntop(IP_AF_INET6, src, ipnet->log_buf, sizeof(ipnet->log_buf)),
               netif->ipcom.name);

    if (IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_ADV_ACCEPT))
    {
        /* Ignore router advertisments from other routers when this
           node is acting as a router on this link.
        */
        goto done;
    }

    /* RFC 2641, chapter 6.1.2

       A node MUST silently discard any received Router Advertisement
       messages that do not satisfy all of the following validity checks:

       1. IP Source Address is a link-local address.  Routers must use
          their link-local address as the source for Router Advertisement
          and Redirect messages so that hosts can uniquely identify
          routers.

      2. The IP Hop Limit field has a value of 255, i.e., the packet
         could not possibly have been forwarded by a router.

      3. If the message includes an IP Authentication Header, the message
         authenticates correctly.

      4. ICMP Checksum is valid.

      5. ICMP Code is 0.

      6. ICMP length (derived from the IP length) is 16 or more octets.

      7. All included options have a length that is greater than zero.
      ...
    */

    /* 3. and 4. has already been verified */

    /* Check 1. and 2. */
    if (!IP_IN6_IS_ADDR_LINK_LOCAL(src) || hlim != 255)
        goto invalid_packet;

    ra = (Ipnet_nd_router_advert *) &pkt->data[pkt->start];
    ra_payload_len = pkt->end - pkt->start - sizeof(Ipnet_nd_router_advert);

    /* Check 5. and 6. */
    if (pkt->end - pkt->start < (int) sizeof(Ipnet_nd_router_advert)
        || ra->nd_ra_hdr.icmp6_code != 0)
        goto invalid_packet;

    /* Check 7. */
    if (!ipnet_icmp6_validate_options_len((Ipnet_pkt_nd_opt_hdr*) (ra + 1), ra_payload_len))
        /* Faulty option found */
        goto invalid_packet;

    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
    {
        /*
          Router adv sent by local node is partly processed, link
          metrics must be processed.
        */
        netif = ipnet_if_indextonetif(IPCOM_VR_ANY, pkt->ifindex);
        if (netif == IP_NULL)
            goto done;
        goto link_metric;
    }

    /* Check for Flags Expansion Option */
    opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (ra + 1),
                                 ra_payload_len,
                                 IPNET_ND_OPT_FLAGS_EXPANSION,
                                 0);
    if (opt != IP_NULL)
        ipnet_icmp6_process_flags_expansion_opt(netif, opt);

    /* Try to get the link address of the sender */
    opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (ra + 1),
                                 ra_payload_len,
                                 IPNET_ND_OPT_SOURCE_LINKADDR,
                                 0);

    /* Create a new entry only if there is a link address to initialize it with */
    neighbor = ipnet_ip6_neighbor_cache_find(netif, src, opt != IP_NULL);
    if (neighbor != IP_NULL)
    {
        if (opt != IP_NULL)
        {
            struct Ip_sockaddr_dl *gw_dl;

            /* The router included its link layer address */
            if (opt->nd_opt_len != ((9 + netif->ipcom.link_addr_size) >> 3))
                /* Wrong length on the link address */
                goto invalid_packet;

            /* */
            gw_dl = (struct Ip_sockaddr_dl*) neighbor->ne.rt->gateway;
            if (ipcom_memcmp(opt->nd_opt_data,
                             IP_SOCKADDR_DL_LLADDR(gw_dl),
                             gw_dl->sdl_alen) != 0)
                /* Update the dynamic entry */
                ipnet_ip6_neighbor_cache_update(neighbor, IPNET_ND_STALE, opt->nd_opt_data, IP_FALSE);
        }

        neighbor->is_router = IP_TRUE;
        if (neighbor->ne.state != IPNET_ND_INCOMPLETE)
            preferred_router = IP_TRUE;
    }

    ipnet_icmp6_default_router_add(netif,
                                   src,
                                   ra,
                                   preferred_router);

    /* Check for any address prefix(es) in the message */
    for (i = 0; ; i++)
    {
        int retval;
        union {
            Ipnet_pkt_nd_opt_prefix_info *prefix_info;
            Ipnet_pkt_nd_opt_hdr         *opt;

        } opt_u;

        opt_u.opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (ra + 1),
                                           ra_payload_len,
                                           IPNET_ND_OPT_PREFIX_INFORMATION,
                                           i);
        prefix_opt = opt_u.prefix_info;
        if (prefix_opt == IP_NULL)
            /* No more address prefix options in the message */
            break;

        if (prefix_opt->nd_opt_pi_prefix_len > 128)
            /* Invalid prefixlen */
            continue;

        if (IP_IN6_IS_ADDR_LINK_LOCAL(&prefix_opt->nd_opt_pi_prefix))
            /* RFC2462 chapter 5.5.3.
               b) If the prefix is the link-local prefix, silently ignore the
                  Prefix Information option. */
            continue;

        if (IP_BIT_ISSET(prefix_opt->nd_opt_pi_flags_reserved, IPNET_ND_OPT_PI_FLAG_AUTO)
            && prefix_opt->nd_opt_pi_prefix_len + netif->inet6_if_id_bits < 128)
            /* RFC2462 chapter 5.5.3.
               If the sum of the prefix length and interface identifier length
               does not equal 128 bits, the Prefix Information option MUST be
               ignored.  An implementation MAY wish to log a system management
               error in this case. It is the responsibility of the system
               administrator to insure that the lengths of prefixes contained in
               Router Advertisements are consistent with the length of interface
               identifiers for that link type. Note that interface identifiers
               will typically be 64-bits long and based on EUI-64 identifiers as
               described in [ADDR-ARCH].

               Note that RFC4862 supersedes RFC2462. RFC4862 allows
               for prefixes where the sum of the link-local prefix and
               the host-ID is equal to or larger than 128 bits.
            */
            continue;

        valid_lifetime = IP_GET_NTOHL(&prefix_opt->nd_opt_pi_valid_time);
        valid_lifetime = IP_MIN(0x7fffffff / 1000, valid_lifetime);
        preferred_lifetime = IP_GET_NTOHL(&prefix_opt->nd_opt_pi_preferred_time);
        preferred_lifetime = IP_MIN(0x7fffffff / 1000, preferred_lifetime);
        if (valid_lifetime < preferred_lifetime)
            /* Not a valid configuration */
            continue;

        retval = ipnet_icmp6_prefix_add(netif,
                                        src,
                                        prefix_opt,
                                        &valid_lifetime);
        if (retval == IPNET_IP6_STOP_PROCESSING)
            /* Ignore this prefix */
            continue;
        if (retval != IPNET_IP6_CONTINUE_PROCESSING)
            goto out_of_mem;

        if (IP_BIT_ISSET(prefix_opt->nd_opt_pi_flags_reserved, IPNET_ND_OPT_PI_FLAG_AUTO)
            && prefix_opt->nd_opt_pi_prefix_len + netif->inet6_if_id_bits == 128)
            /* This prefix should be used for auto address configuration */
            ipnet_icmp6_add_autonomous_addr(netif, prefix_opt, preferred_lifetime, valid_lifetime);
        /* else:
           The 2:nd comparison checks for RFC4862, chapter 5.3. Note
           that the case where the sum of the link-local prefix and N
           is smaller than 128 bit is considered invalid and such
           messages has been rejected above in "RFC2462 chapter 5.5.3..."
           ...
           If the sum of the link-local prefix length and N is larger than 128,
           autoconfiguration fails and manual configuration is required.  The
           length of the interface identifier is defined in a separate link-
           type-specific document, which should also be consistent with the
           address architecture [RFC4291] (see Section 2).
           ...
        */
    }

 link_metric:
    /* Figure out the link MTU */
    opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (ra + 1),
                                 ra_payload_len,
                                 IPNET_ND_OPT_MTU,
                                 0);
    if (opt != IP_NULL)
    {
        /* Link MTU specified, use it */
        Ipnet_pkt_nd_opt_mtu *mtu_opt = (Ipnet_pkt_nd_opt_mtu *) opt;
        link_mtu = IP_MIN(IP_GET_NTOHL(&mtu_opt->nd_opt_mtu_mtu), link_mtu);
    }
    if (link_mtu < ipnet_conf_ip6_min_mtu)
        link_mtu = ipnet_conf_ip6_min_mtu;

    netif->ipcom.mtu = link_mtu;

    /* RFC2461, chapter 6.3.4
       ...
       Some Router Advertisement fields (e.g., Cur Hop Limit, Reachable Time
       and Retrans Timer) may contain a value denoting unspecified.  In such
       cases, the parameter should be ignored and the host should continue
       using whatever value it is already using.  In particular, a host MUST
       NOT interpret the unspecified value as meaning change back to the
       default value that was in use before the first Router Advertisement
       was received.  This rule prevents hosts from continually changing an
       internal variable when one router advertises a specific value, but
       other routers advertise the unspecified value.
       ...
    */
    tmp_u32 = IP_GET_NTOHL(&ra->nd_ra_reachable);
    if (tmp_u32 != 0)
        netif->inet6_nd_reachable_time  = tmp_u32;

    tmp_u32 = IP_GET_NTOHL(&ra->nd_ra_retransmit);
    if (tmp_u32 != 0)
        netif->inet6_nd_retransmit_time = tmp_u32;

    if (ra->ipnet_nd_ra_curhoplimit != 0)
        netif->inet6_nd_curhoplimit = ra->ipnet_nd_ra_curhoplimit;

    /* Disable sending RS since on this link since at least one RA has been receieved.
       Only disable if we already have a valid unicast address */
    if (ipnet_netif_has_valid_inet6_unicast_addr(netif))
    {
        ipnet_timeout_cancel(netif->inet6_rs_tmo);
        netif->inet6_rs_retrans_count = IPNET_INET6_RS_RETRANS_COUNT_DISABLED;
    }

 done:
    ipcom_pkt_free(pkt);
    return 0;

    /* Invalid ICMP6 type */
 invalid_packet:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;

 out_of_mem:
    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                      1, 2, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_RECV,
                      ipnet_icmp6_input_router_adv, IPCOM_WV_NETD_NOBUFS,
                      IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);
    IPNET_STATS(icmp6_input_nomem++);
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_ENOBUFS;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_neighbor_sol
 *===========================================================================
 * Description: Processes neighbor solicitation message.
 * Parameters:  netif - The network interface that received the message
 *              src - The address (link-local) of the host that which to know
 *              our link address or the unspecified address if DAD is performed.
 *              dst - One of the addresses of the local node.
 *              hlim - The hoplimit value from the IPv6 header.
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_neighbor_sol(Ipnet_netif *netif,
                               IP_CONST struct Ip_in6_addr *src,
                               IP_CONST struct Ip_in6_addr *dst,
                               int hlim,
                               Ipcom_pkt *pkt)
{
    Ipnet_pkt_nd_neighbor_solicit *sol;
    Ipnet_icmp6_param              icmp6_param;
    Ipnet_pkt_nd_opt_hdr          *opt;
    IP_CONST struct Ip_in6_addr   *reply_addr = src;
    struct Ip_in6_addr             all_nodes_addr;
    int                            sol_payload_len;
    int                            ret = 0;
    Ipnet_ip6_addr_entry          *addr_entry;

    /* RFC 2461
       7.1.1.  Validation of Neighbor Solicitations

       A node MUST silently discard any received Neighbor Solicitation
       messages that do not satisfy all of the following validity checks:

       - The IP Hop Limit field has a value of 255, i.e., the packet
       could not possibly have been forwarded by a router.

       - If the message includes an IP Authentication Header, the message
       authenticates correctly.

       - ICMP Checksum is valid.

       - ICMP Code is 0.

       - ICMP length (derived from the IP length) is 24 or more octets.

       - Target Address is not a multicast address.

       - All included options have a length that is greater than zero.

       - If the IP source address is the unspecified address, the IP
       destination address is a solicited-node multicast address.

       - If the IP source address is the unspecified address, there is no
       source link-layer address option in the message.
    */

    if (hlim != 255
        || pkt->end - pkt->start < (int) sizeof(*sol)
        || (IP_IN6_IS_ADDR_UNSPECIFIED(src) && !IPNET_IP6_IS_ADDR_SOLICITED(dst)))
        goto invalid_packet;

    sol = (Ipnet_pkt_nd_neighbor_solicit*) &pkt->data[pkt->start];
    sol_payload_len = pkt->end - pkt->start - sizeof(Ipnet_pkt_nd_neighbor_solicit);

    if (IPNET_IP6_IS_ADDR_ALL_NODES(&sol->nd_ns_target)
        || sol->nd_ns_hdr.icmp6_code != 0)
        goto invalid_packet;

    if (IP_IN6_IS_ADDR_UNSPECIFIED(src))
    {
        IPNET_IP6_SET_ALL_NODES_ADDR(&all_nodes_addr);
        reply_addr = &all_nodes_addr;
    }

    /* Check if this message affect this node, the target address must be a unicast address */
    addr_entry = ipnet_ip6_get_addr_entry(&sol->nd_ns_target,
                                          pkt->vr_index,
                                          IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK) ? IP_NULL : netif);
    if (addr_entry == IP_NULL
        || (addr_entry->type != IPNET_ADDR_TYPE_UNICAST
            && addr_entry->type != IPNET_ADDR_TYPE_ANYCAST))
        /* Message was not to this node */
        goto done;

    if (!ipnet_icmp6_validate_options_len((Ipnet_pkt_nd_opt_hdr*) (sol + 1),
                                          sol_payload_len))
    {
        /* Faulty option found */
        goto invalid_packet;
    }

    opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (sol + 1),
                                 sol_payload_len,
                                 IPNET_ND_OPT_SOURCE_LINKADDR,
                                 0);

    if ((opt && IP_IN6_IS_ADDR_UNSPECIFIED(src))
        || (opt == IP_NULL
            && !IP_IN6_IS_ADDR_UNSPECIFIED(src)
            && IPNET_IP6_IS_ADDR_SOLICITED(dst)
            && netif->ipcom.link_addr_size > 0))
    {
        /* RFC2461: The link-layer address for the sender MUST NOT be
           included when the source IP address is the unspecified
           address.  Otherwise, on link layers that have addresses
           this option MUST be included in multicast solicitations and
           SHOULD be included in unicast solicitations. */
        goto invalid_packet;
    }

    if (ipnet_icmp6_check_tentative_addresses(netif, pkt, &sol->nd_ns_target, IP_TRUE, src)
        != IPNET_IP6_CONTINUE_PROCESSING)
        /* Duplicate address detection failed or this was sent by this node */
        goto done;

    if (opt)
    {
        /* The peer included its link address, update the ND cache */
        Ipnet_ip6_nc_entry *neighbor;

        if (opt->nd_opt_len != ((9 + netif->ipcom.link_addr_size) >> 3))
            /* Wrong length on the link address */
            goto invalid_packet;

        neighbor = ipnet_ip6_neighbor_cache_find(netif, src, IP_TRUE);
        if (neighbor != IP_NULL)
        {
            Ipnet_route_entry     *rt = neighbor->ne.rt;
            struct Ip_sockaddr_dl *gw_dl = (struct Ip_sockaddr_dl*) rt->gateway;
            Ip_bool                nd_update = IP_FALSE;

            /* Either route is down, or the LLADDR has changed */
            if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_UP)
                || ipcom_memcmp(opt->nd_opt_data,
                                IP_SOCKADDR_DL_LLADDR(gw_dl),
                                gw_dl->sdl_alen) != 0)
                nd_update = IP_TRUE;

            if ((neighbor->ne.state != IPNET_ND_REACHABLE
                 && neighbor->ne.state != IPNET_ND_PROBE
                 && neighbor->ne.state != IPNET_ND_DELAY)
                || nd_update)
                ipnet_ip6_neighbor_cache_update(neighbor, IPNET_ND_STALE, opt->nd_opt_data, IP_FALSE);
        }
    }
    /*
       else:
       ...
       RFC 4861, chapter 7.2
       It is possible that a host may receive a solicitation, a router
       advertisement, or a Redirect message without a link-layer address
       option included.  These messages MUST NOT create or update neighbor
       cache entries, except with respect to the IsRouter flag as specified
       in Sections 6.3.4 and 7.2.5.
       ...
     */

    /* The message is processed, create a response */
    ipcom_memset(&icmp6_param, 0, sizeof(icmp6_param));
    icmp6_param.type     = IPNET_ND_NEIGHBOR_ADVERT;
    icmp6_param.vr       = netif->vr_index;
    icmp6_param.ifindex  = netif->ipcom.ifindex;
    icmp6_param.to       = reply_addr;
    icmp6_param.data.neighbor_adv.target_addr = &sol->nd_ns_target;
    icmp6_param.data.neighbor_adv.is_router   = (Ip_u8) IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT);
    icmp6_param.data.neighbor_adv.solicated   = (Ip_u8) !IP_IN6_IS_ADDR_MULTICAST(reply_addr);
    icmp6_param.data.neighbor_adv.override    = (addr_entry->type == IPNET_ADDR_TYPE_UNICAST);

    ret = ipnet_icmp6_send(&icmp6_param);

 done:
    ipcom_pkt_free(pkt);
    return ret;

 invalid_packet:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_neighbor_adv
 *===========================================================================
 * Description: Processes neighbor advertisment message.
 * Parameters:  netif - The network interface that received the message
 *              dst - One of the addresses of the local node.
 *              hlim - The hoplimit value from the IPv6 header.
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_neighbor_adv(Ipnet_netif *netif,
                               IP_CONST struct Ip_in6_addr *dst,
                               int hlim,
                               Ipcom_pkt *pkt)
{
    Ipnet_pkt_nd_neighbor_advert *adv;
    Ipnet_pkt_nd_opt_hdr         *opt;
    Ipnet_ip6_nc_entry           *neighbor = IP_NULL;
    Ipnet_route_entry            *rt;
    struct Ip_sockaddr_dl        *gw_dl;
    int                           adv_payload_len;
    Ip_bool                       update_is_router = IP_TRUE;
    Ip_bool                       override;
    Ip_bool                       different_ll_addr;

    /* RFC 2461
       7.1.2.  Validation of Neighbor Advertisements

       A node MUST silently discard any received Neighbor Advertisement
       messages that do not satisfy all of the following validity checks:

       1. The IP Hop Limit field has a value of 255, i.e., the packet
       could not possibly have been forwarded by a router.

       2. If the message includes an IP Authentication Header, the message
       authenticates correctly.

       3. ICMP Checksum is valid.

       4. ICMP Code is 0.

       5. ICMP length (derived from the IP length) is 24 or more octets.

       6. Target Address is not a multicast address.

       7. If the IP Destination Address is a multicast address the
       Solicited flag is zero.

       8. All included options have a length that is greater than zero.
    */

    /* Bullets 2, 3 has already been verified */

    /* Check 1. */
    if (hlim != 255)
        goto invalid_packet;

    /* Check 5. */
    if (pkt->end - pkt->start < (int) sizeof(Ipnet_pkt_nd_neighbor_advert))
        goto invalid_packet;

    adv = (Ipnet_pkt_nd_neighbor_advert *) &pkt->data[pkt->start];
    adv_payload_len = pkt->end - pkt->start - sizeof(Ipnet_pkt_nd_neighbor_advert);
    override = IPNET_ND_IS_OVERRIDE(adv);

    /* Check 4. */
    if (adv->nd_na_hdr.icmp6_code != 0)
        /* Invalid code */
        goto invalid_packet;

    /* Check 6. */
    if (IP_IN6_IS_ADDR_MULTICAST(&adv->nd_na_target))
        goto invalid_packet;

    /* Check 7. */
    if (IP_IN6_IS_ADDR_MULTICAST(dst) && IPNET_ND_IS_SOLICATED(adv))
        goto invalid_packet;

    /* Check 8. */
    if (!ipnet_icmp6_validate_options_len((Ipnet_pkt_nd_opt_hdr*) (adv + 1),
                                          adv_payload_len))
    {
        /* Faulty option found */
        goto invalid_packet;
    }

    opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (adv + 1),
                                 adv_payload_len,
                                 IPNET_ND_OPT_TARGET_LINKADDR,
                                 0);

    if (opt != IP_NULL && opt->nd_opt_len != ((9 + netif->ipcom.link_addr_size) >> 3))
        /* Wrong length on the link address */
        goto invalid_packet;

    if (IP_IN6_IS_ADDR_MULTICAST(dst) && opt == IP_NULL)
        /*
         * The specification does not say anything about this being
         * invalid on the receiver side. However, sending a RA without
         * LLA is not OK, so can only end up here if the sender is not
         * following the specification.
         */
        goto invalid_packet;

    if (ipnet_icmp6_check_tentative_addresses(netif, pkt, &adv->nd_na_target, IP_FALSE, IP_NULL)
        != IPNET_IP6_CONTINUE_PROCESSING)
        /* Duplicate address detection failed or this was sent by this node */
        goto done;

    if (ipnet_route_lookup_ip6_dont_route(IP_TRUE,
                                          netif->vr_index,
                                          &adv->nd_na_target,
                                          netif->ipcom.ifindex,
                                          &rt) == 0)
    {
        /* Update the don't route entry for this host */
        ipnet_ip6_neighbor_cache_update(IPNET_IP6_NC(rt),
                                        IPNET_ND_REACHABLE,
                                        opt ? opt->nd_opt_data : IP_NULL,
                                        IP_FALSE);
    }

    neighbor = ipnet_ip6_neighbor_cache_find(netif, &adv->nd_na_target, IP_FALSE);
    if (neighbor == IP_NULL)
        /* Does not look like this node asked for this information */
        goto done;

#ifdef IPCOM_STORE_PHYSICAL_IF
    neighbor->ne.physical_ifindex = pkt->physical_ifindex;
#endif

    if (neighbor->ne.state == IPNET_ND_INCOMPLETE)
    {
        if (opt != IP_NULL || netif->ipcom.link_addr_size == 0)
        {
            ipnet_ip6_neighbor_cache_update(neighbor,
                                            (IPNET_ND_IS_SOLICATED(adv) ? IPNET_ND_REACHABLE : IPNET_ND_STALE),
                                            opt ? opt->nd_opt_data : IP_NULL,
                                            IP_FALSE);
        }
        /* else: RFC 2461 chapter 7.2.5.  Receipt of Neighbor Advertisements
         * Message must be discareded if the link layer has addresses
         * and no Target Link-Layer address option is included
         */
        goto done;
    }

    gw_dl = (struct Ip_sockaddr_dl*) neighbor->ne.rt->gateway;

    if (opt == IP_NULL)
        different_ll_addr = IP_FALSE;
    else
        different_ll_addr = ipcom_memcmp(opt->nd_opt_data,
                                         IP_SOCKADDR_DL_LLADDR(gw_dl),
                                         gw_dl->sdl_alen) != 0;

    if (!override && different_ll_addr)
    {
        /* RFC 2461, chapter 7.2.5
           ...
           If the Override flag is clear and the supplied link-layer address
           differs from that in the cache, then one of two actions takes place: if the
           state of the entry is REACHABLE, set it to STALE, but do not update the entry
           in any other way; otherwise, the received advertisement should be ignored and
           MUST NOT update the cache.
           ...
        */
        update_is_router = IP_FALSE;
        if (neighbor->ne.state == IPNET_ND_REACHABLE)
            ipnet_ip6_neighbor_cache_update(neighbor, IPNET_ND_STALE, IP_NULL, IP_FALSE);
    }
    else if (override || !different_ll_addr || opt == IP_NULL)
    {
        /* RFC 2461, chapter 7.2.5
           ...
           If the Override flag is set, both the Override
           flag is clear and the supplied link-layer address is the same as that
           in the cache, or no Target Link-layer address option was supplied,
           the received advertisement MUST update the Neighbor Cache entry as
           follows:

           - The link-layer address in the Target Link-Layer Address option
           MUST be inserted in the cache (if one is supplied and is different
           than the already recorded address).

           - If the Solicited flag is set, the state of the entry MUST be set
           to REACHABLE.  If the Solicited flag is zero and the link-layer
           address was updated with a different address the state MUST be set
           to STALE.  Otherwise, the entry's state remains unchanged
           ...
         */
        if (IPNET_ND_IS_SOLICATED(adv))
            ipnet_ip6_neighbor_cache_update(neighbor,
                                            IPNET_ND_REACHABLE,
                                            opt ? opt->nd_opt_data : IP_NULL, IP_FALSE);
        else if (different_ll_addr)
            ipnet_ip6_neighbor_cache_update(neighbor,
                                            IPNET_ND_STALE,
                                            opt->nd_opt_data,
                                            IP_FALSE);
    }

 done:
    if (neighbor && update_is_router)
    {
        if (neighbor->is_router && !IPNET_ND_IS_ROUTER(adv))
            /* The sending host has stopped forwarding packets */
            ipnet_icmp6_default_router_delete(netif, &adv->nd_na_target);

        neighbor->is_router = IPNET_ND_IS_ROUTER(adv) ? IP_TRUE : IP_FALSE;
    }
    ipcom_pkt_free(pkt);
    return 0;

 invalid_packet:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input_redirect
 *===========================================================================
 * Description: Processes redirects from a router.
 * Parameters:  netif - The network interface that received the message
 *              src - The address of the sender.
 *              hlim - The hoplimit value from the IPv6 header.
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_redirect(Ipnet_netif *netif,
                           IP_CONST struct Ip_in6_addr *src,
                           int hlim,
                           Ipcom_pkt *pkt)
{
    Ipnet_pkt_nd_redirect *rct;
    Ipnet_pkt_nd_opt_hdr  *opt;
    Ipnet_ip6_nc_entry    *neighbor;
    int                    rct_payload_len;
    Ipnet_route_entry     *rt   = IP_NULL;

    if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6,
                                         netif,
                                         "IcmpRedirectReceive",
                                         1,
                                         ipnet_bool_map) == 0)
        /* Redirect processing disabled */
        goto done;

    /* RFC 2461, chapter 8.1
       A host MUST silently discard any received Redirect message that does
       not satisfy all of the following validity checks:

       1. IP Source Address is a link-local address.  Routers must use
          their link-local address as the source for Router Advertisement
          and Redirect messages so that hosts can uniquely identify
          routers.

       2. The IP Hop Limit field has a value of 255, i.e., the packet
          could not possibly have been forwarded by a router.

       3. If the message includes an IP Authentication Header, the message
          authenticates correctly.

       4. ICMP Checksum is valid.

       5. ICMP Code is 0.

       6. ICMP length (derived from the IP length) is 40 or more octets.

       7. The IP source address of the Redirect is the same as the current
          first-hop router for the specified ICMP Destination Address.

       8. The ICMP Destination Address field in the redirect message does
          not contain a multicast address.

       9. The ICMP Target Address is either a link-local address (when
          redirected to a router) or the same as the ICMP Destination
          Address (when redirected to the on-link destination).

       10. All included options have a length that is greater than zero.
    */

    /* Bullet 3, 4 has already been verified */

    /* Check 1. */
    if (!IP_IN6_IS_ADDR_LINK_LOCAL(src))
        goto invalid_packet;

    /* Check 2. and 6. */
    if (hlim != 255 || pkt->end - pkt->start < 40)
        goto invalid_packet;

    rct = (Ipnet_pkt_nd_redirect *) &pkt->data[pkt->start];
    rct_payload_len = pkt->end - pkt->start - sizeof(Ipnet_pkt_nd_redirect);

    /* Check 5. */
    if (rct->ipnet_nd_rd_code != 0)
        goto invalid_packet;

    /* Check 7. */
    {
        Ipnet_route_entry *rt2;

        /* Find the destination route(s) based on the target and scope */
        if (ipnet_route_raw_lookup(IP_AF_INET6,
                                   netif->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   0,
                                   &rct->nd_rd_dst,
                                   netif->ipcom.ifindex,
                                   0,
                                   &rt2) < 0)
            goto invalid_packet;

        /* Is it possible, by our routing table, to have used the redirecting
         * gateway?
         *
         * We need to verify this more extensively, merely looking at the first
         * route can yield a different result than ecmp, which would render
         * all redirections from a specific router/target combination invalid
         * even though they should be acted upon.
         */
        for (; rt2 != IP_NULL && rt == IP_NULL; rt2 = rt2->widen)
        {
            for (rt = rt2; rt != IP_NULL; rt = rt->next)
            {
                struct Ip_sockaddr_in6 *in6 = (struct Ip_sockaddr_in6 *)rt->gateway;

                /* Does family and gateway match? We do not need to verify interface and scope
                 * since it has already been done by the route_raw_lookup
                 */
                if (in6 != IP_NULL && in6->sin6_family == IP_AF_INET6 && IP_IN6_ARE_ADDR_EQUAL(&in6->sin6_addr, src))
                    break;
            }
        }

        /* Did we locate a route? If not, there's no possibility for this node to
         * to route to the target using the redirecting gateway, using the default
         * routing table.
         */
        if (rt == IP_NULL)
            goto invalid_packet;
    }

    /* Check 8. */
    if (IP_IN6_IS_ADDR_MULTICAST(&rct->nd_rd_dst))
        goto invalid_packet;

    /* Check 9. */
    if (!IP_IN6_IS_ADDR_LINK_LOCAL(&rct->nd_rd_target)
        && !IP_IN6_ARE_ADDR_EQUAL(&rct->nd_rd_dst, &rct->nd_rd_target))
        goto invalid_packet;

    /* Check 10. */
    if (!ipnet_icmp6_validate_options_len((Ipnet_pkt_nd_opt_hdr*) (rct + 1),
                                          rct_payload_len))
        goto invalid_packet;

    rt = ipnet_icmp6_dst_create_rt(netif, &rct->nd_rd_dst, &rct->nd_rd_target);
    if (rt == IP_NULL)
        /*  Failed to create redirect entry */
        goto out_of_mem;

    opt = ipnet_icmp6_get_option((Ipnet_pkt_nd_opt_hdr*) (rct + 1),
                                 rct_payload_len,
                                 IPNET_ND_OPT_TARGET_LINKADDR,
                                 0);

    neighbor = ipnet_ip6_neighbor_cache_find(netif, &rct->nd_rd_target, opt != IP_NULL);
    if (neighbor != IP_NULL)
    {
        /* RFC 2641, chapter 8.3
           ...
           If the Target and Destination Addresses are the same, the host MUST
           treat the Target as on-link.  If the Target Address is not the same
           as the Destination Address, the host MUST set IsRouter to TRUE for
           the target.  If the Target and Destination Addresses are the same,
           however, one cannot reliably determine whether the Target Address is
           a router.  Consequently, newly created Neighbor Cache entries should
           set the IsRouter flag to FALSE, while existing cache entries should
           leave the flag unchanged.  If the Target is a router, subsequent
           Neighbor Advertisement or Router Advertisement messages will update
           IsRouter accordingly.
           ...
        */
        if (!IP_IN6_ARE_ADDR_EQUAL(&rct->nd_rd_dst, &rct->nd_rd_target))
            neighbor->is_router = IP_TRUE;

        /* RFC 2641, chapter 8.3
           ...
           If the redirect contains a Target Link-Layer Address option the host
           either creates or updates the Neighbor Cache entry for the target.
           In both cases the cached link-layer address is copied from the Target
           Link-Layer Address option.  If a Neighbor Cache entry is created for
           the target its reachability state MUST be set to STALE as specified
           in Section 7.3.3.  If a cache entry already existed and it is updated
           with a different link-layer address, its reachability state MUST also
           be set to STALE.  If the link-layer address is the same as that
           already in the cache, the cache entry's state remains unchanged.
           ...
        */
        if (opt != IP_NULL)
        {
            struct Ip_sockaddr_dl *gw_dl = (struct Ip_sockaddr_dl*) neighbor->ne.rt->gateway;

            if (ipcom_memcmp(opt->nd_opt_data,
                             IP_SOCKADDR_DL_LLADDR(gw_dl),
                             gw_dl->sdl_alen) != 0)
                /* The link layer address of the new next hop was included and it does not
                   match the link layer already cached */
                ipnet_ip6_neighbor_cache_update(neighbor, IPNET_ND_STALE, opt->nd_opt_data, IP_FALSE);
        }
        else if (neighbor->ne.state == IPNET_ND_UNINITIALIZED)
            ipnet_ip6_neighbor_cache_update(neighbor, IPNET_ND_INCOMPLETE, IP_NULL, IP_FALSE);
    }

    IPNET_ROUTESOCK(ipnet_routesock_rt_redirect(rt->head,
                                                netif->ipcom.ifindex,
                                                &rct->nd_rd_dst,
                                                &rct->nd_rd_target));
    IPNET_NETLINKSOCK(ipnet_rtnetlink_route_redirect(rt->head,
                                                     netif->ipcom.ifindex,
                                                     &rct->nd_rd_dst,
                                                     &rct->nd_rd_target));

 done:
    ipcom_pkt_free(pkt);
    return 0;

 invalid_packet:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;

 out_of_mem:
    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                      1, 3, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_RECV,
                      ipnet_icmp6_input_neighbor_sol, IPCOM_WV_NETD_NOBUFS,
                      IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);
    IPNET_STATS(icmp6_input_nomem++);
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_ENOBUFS;
}


/*
 *===========================================================================
 *                    ipnet_mld_should_send_message
 *===========================================================================
 * Description: Determines if a MLD message should be sent for the
 *              specified multicast address.
 * Parameters:
 * Returns:     >=0 = success , <0 = error code.
 *
 */
IP_STATIC Ip_bool
ipnet_mld_should_send_message(IP_CONST Ipnet_ip6_addr_entry *addr_entry)
{
    struct Ip_in6_addr           addr;
    IP_CONST struct Ip_in6_addr *mlt_addr = &addr_entry->addr;

    ip_assert(IP_IN6_IS_ADDR_MULTICAST(mlt_addr));

    if (IP_BIT_ISSET(addr_entry->netif->ipcom.flags, IP_IFF_LOOPBACK))
        /* No need for MLD messages on the loopback device */
        return IP_FALSE;

    /* RFC 3810, chapter 6
       ...
       The link-scope all-nodes multicast address, (FF02::1), is
       handled as a special case.  On all nodes -- that is all hosts
       and routers, including multicast routers -- listening to
       packets destined to the all-nodes multicast address, from all
       sources, is permanently enabled on all interfaces on which
       multicast listening is supported.  No MLD messages are ever
       sent regarding neither the link-scope all-nodes multicast
       address, nor any multicast address of scope 0 (reserved) or 1
       (node-local).
       ...
    */

    if (IPNET_IP6_MC_SCOPE(mlt_addr) <= IPNET_IP6_SCOPE_NODE_LOCAL)
        return IP_FALSE;

    IPNET_IP6_SET_ALL_NODES_ADDR(&addr);
    if (IP_IN6_ARE_ADDR_EQUAL(mlt_addr, &addr))
        return IP_FALSE;

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_rate_reseed
 *===========================================================================
 * Description: Reseeds the rate limit counter for ICMPv6 packets.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_rate_reseed(void *unused)
{
    IPCOM_UNUSED_ARG(unused);
    ipnet->ip6.icmp_send_limit = (Ip_u32)
        ipcom_sysvar_get_as_int0("ipnet.inet6.IcmpRatelimitBucketsize", 10);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_nd_probe
 *===========================================================================
 * Description: Probes to verify or finds the physical address for the
 *              neighbor entry.
 * Parameters:  neigh - neighbor entry to probe for
 *              unicast_probe - IP_TRUE = send unicast probe
 *                              IP_FALSE = send multicast probe
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_nd_probe(Ipnet_neigh_entry *neigh, Ip_bool unicast_probe)
{
    Ipnet_icmp6_param   icmp6_param;
    struct Ip_in6_addr  dst;
    struct Ip_in6_addr *nc_addr;
    Ipnet_netif        *netif = neigh->rt->netif;

    ip_assert(neigh != IP_NULL);
    ip_assert(neigh->state == IPNET_ND_PROBE
              || neigh->state == IPNET_ND_INCOMPLETE);
    ip_assert(neigh->remaining_ucast_probes + neigh->remaining_mcast_probes >= 0);

    nc_addr = &((struct Ipnet_ipv6_key *) (neigh)->rt->hdr.key)->addr;

    ipcom_memset(&icmp6_param, 0, sizeof(icmp6_param));
    icmp6_param.type     = IPNET_ND_NEIGHBOR_SOLICIT;
    icmp6_param.ifindex  = netif->ipcom.ifindex;
    icmp6_param.vr       = netif->vr_index;
    icmp6_param.data.neigbor_sol.target_addr = nc_addr;

    if (unicast_probe)
        icmp6_param.to = nc_addr;
    else
    {
        IPNET_IP6_SET_SOLICITED_ADDR(&dst, nc_addr);
        icmp6_param.to = &dst;
    }

    if (neigh->pending.pkt_list != IP_NULL)
    {
        /* RFC 2461, chapter 7.2.2
           ...
           If the source address of the packet prompting the solicitation is the
           same as one of the addresses assigned to the outgoing interface, that
           address SHOULD be placed in the IP Source Address of the outgoing
           solicitation.
           ...
        */

        Ipnet_pkt_ip6 *ip6_hdr;
        Ipcom_pkt     *pkt = neigh->pending.pkt_list;

        ip6_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];
        if (ipnet_ip6_get_addr_type(&ip6_hdr->src, netif->vr_index, netif)
            == IPNET_ADDR_TYPE_UNICAST)
            icmp6_param.from = &ip6_hdr->src;
    }

    (void) ipnet_icmp6_send(&icmp6_param);
}


/*
 *===========================================================================
 *                  ipnet_icmp6_get_nd_retransmit_duration
 *===========================================================================
 * Description: Returns the duration, in seconds, that should be used for
 *              retransmissions of neighbor solicitations.
 * Parameters:  netif - The interface the NS is being set at.
 * Returns:     Duration in seconds.
 *
 */
IP_STATIC Ip_u32
ipnet_icmp6_get_nd_retransmit_duration(Ipnet_netif *netif)
{
    if (netif->inet6_nd_retransmit_time)
        return netif->inet6_nd_retransmit_time / 1000;

    return (Ip_u32) ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                  netif,
                                                  "BaseRetransmitTime", 1);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_neighbor_cache_timeout
 *===========================================================================
 * Description: Handles resends of ND messages and removes entries that
 *              has been unused for a long time.
 * Parameters:  rt - The route entry that contains neighbor cache information.
 *              is_timeout - IP_TRUE if this call is done as a result of a timeout event
 *                           IP_FALSE if the call is done as a cleanup event only.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_neighbor_cache_timeout(Ipnet_route_entry *rt, Ip_bool is_timeout)
{
    Ipnet_ip6_nc_entry *nc;

    ip_assert(rt != IP_NULL);

    nc = IPNET_IP6_NC(rt);
    ip_assert(nc != IP_NULL);

    ipnet_neigh_timeout(IP_AF_INET6, &nc->ne, is_timeout);

    if (!ipnet_route_has_expired(rt))
        return;

    if (nc->is_router)
    {
        /* Change this default router to "not preferred" since this
	   entry will be deleted. */
        (void)ipnet_icmp6_default_router_change(rt->netif,
                                                rt->hdr.key,
                                                IP_FALSE,
                                                0,
                                                IP_TRUE,
                                                IP_FALSE);
    }

#ifdef IPNET_USE_RFC3971
    if (nc->pending_sign_create)
    {
        ipcom_pkt_free(nc->pending_sign_create);
        nc->pending_sign_create = IP_NULL;
    }
    if (nc->pending_sign_verify)
    {
        ipcom_pkt_free(nc->pending_sign_verify);
        nc->pending_sign_verify = IP_NULL;
    }
    if (nc->rt_next_hop)
    {
        ipnet_route_unlock(nc->rt_next_hop);
        nc->rt_next_hop = IP_NULL;
    }
#endif

    /* The entry has expired and will be removed */
}


/*
 *===========================================================================
 *                    ipnet_icmp6_send_addr_unreachable
 *===========================================================================
 * Description: Sends a ICMP host unreachable to the originator of the
 *              specified packet.
 * Parameters:  pkt - Packet that could not be sent.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_send_addr_unreachable(Ipcom_pkt *pkt)
{
    Ipnet_icmp6_param p;
    Ipnet_pkt_ip6    *ip = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];

    if (pkt->ipstart < pkt->start
        || pkt->ipstart + IPNET_IP6_HDR_SIZE > pkt->end
        || IPNET_IP6_GET_VER(ip) != 6)
        /* Cannot be a valid IPv6 packet */
        return;

    ipnet_icmp6_param_init(&p, pkt);
    p.type = IPNET_ICMP6_DST_UNREACH;
    p.code = IPNET_ICMP6_DST_UNREACH_ADDR;;
    (void) ipnet_icmp6_send(&p);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_schedule_addr_unreachable
 *===========================================================================
 * Description: Schedules a ICMP host unreachable to the originator of the
 *              specified packet.
 * Parameters:  pkt - Packet that could not be sent.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_schedule_addr_unreachable(Ipcom_pkt *pkt)
{
    ipnet_schedule_pkt_report(pkt, (Ipnet_softirq_handler) ipnet_icmp6_send_addr_unreachable);
}


#ifdef IPNET_DEBUG
/*
 *===========================================================================
 *                    ipnet_icmp6_input_log
 *===========================================================================
 * Description: Logs which ICMPv6 message that was received.
 * Parameters:  netif - interface the message was recieved at.
 *              src - address of the message sender
 *              dst - address of the message recipient
 *              icmp6_type - ICMPv6 message type
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_input_log(Ipnet_netif *netif,
                      IP_CONST struct Ip_in6_addr *src,
                      IP_CONST struct Ip_in6_addr *dst,
                      Ip_u8 icmp6_type)
{
    char *str_msg_type;
    char  buf[8];

    switch (icmp6_type)
    {
    case IPNET_ICMP6_DST_UNREACH:
        str_msg_type = "DESTINATION UNREACHABLE";
        break;
    case IPNET_ICMP6_PACKET_TOO_BIG:
        str_msg_type = "PACKET TOO BIG";
        break;
    case IPNET_ICMP6_TIME_EXCEEDED:
        str_msg_type = "TIME EXCEEDED";
        break;
    case IPNET_ICMP6_PARAM_PROB:
        str_msg_type = "PARAMETER PROBLEM";
        break;
    case IPNET_ICMP6_ECHO_REQUEST:
        str_msg_type = "ECHO REQUEST";
        break;
    case IPNET_ICMP6_ECHO_REPLY:
        str_msg_type = "ECHO REPLY";
        break;
    case IPNET_MLD_LISTENER_QUERY:
        str_msg_type = "LISTENER QUERY";
        break;
    case IPNET_MLD_V1_LISTENER_REPORT:
        str_msg_type = "V1 LISTENER QUERY";
        break;
    case IPNET_MLD_V2_LISTENER_REPORT:
        str_msg_type = "V2 LISTENER QUERY";
        break;
    case IPNET_MLD_LISTENER_REDUCTION :
        str_msg_type = "V2 LISTENER REDUCTION";
        break;
    case IPNET_ND_ROUTER_SOLICIT :
        str_msg_type = "ROUTER SOLICITATION";
        break;
    case IPNET_ND_ROUTER_ADVERT :
        str_msg_type = "ROUTER ADVERTISEMENT";
        break;
    case IPNET_ND_NEIGHBOR_SOLICIT :
        str_msg_type = "NEIGHBOR SOLICITATION";
        break;
    case IPNET_ND_NEIGHBOR_ADVERT :
        str_msg_type = "NEIGHBOR ADVERTISEMENT";
        break;
    case IPNET_ND_REDIRECT :
        str_msg_type = "REDIRECT";
        break;
    default :
        str_msg_type = buf;
        ipcom_snprintf(buf, sizeof(buf), "%d", icmp6_type);
        break;
    }

    IPCOM_LOG4(DEBUG,
               "IPv6: Got %s on %s from %s to %s",
               str_msg_type,
               netif->ipcom.name,
               ipcom_inet_ntop(IP_AF_INET6,
                               src,
                               ipnet->log_buf,
                               sizeof(ipnet->log_buf)),
               ipcom_inet_ntop(IP_AF_INET6,
                               dst,
                               ipnet->log_buf + IP_INET6_ADDRSTRLEN,
                               sizeof(ipnet->log_buf) - IP_INET6_ADDRSTRLEN));
}


/*
 *===========================================================================
 *                 ipnet_icmp6_send_router_sol_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_icmp6_send_router_sol_to_string(Ipnet_netif *netif, char *buf, int buf_len)
{
    return ipcom_snprintf(buf,
                          buf_len,
                          "ICMPv6 router solicitation on %s(%u)",
                          netif->ipcom.name,
                          (unsigned)netif->ipcom.ifindex);
}
#endif /* IPNET_DEBUG */


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                       ipnet_icmp6_prefix_get
 *===========================================================================
 * Description: Returns a prefix entry.
 * Parameters:  netif - interface the prefix must be valid for
 *              prefix - the prefix
 *              prefix_len - the prefix length
 * Returns:     The prefix or IP_NULL if no matching prefix exists.
 *
 */
IP_GLOBAL Ipnet_route_entry *
ipnet_icmp6_prefix_get(Ipnet_netif *netif,
                       IP_CONST struct Ip_in6_addr *prefix,
                       Ip_u8 prefix_len)
{
    return ipnet_icmp6_prefix_get_ex(netif,
                                     prefix,
                                     prefix_len,
                                     IP_NULL,
                                     IP_NULL,
                                     IP_NULL,
                                     IP_NULL);
}

/*
 *===========================================================================
 *                    ipnet_icmp6_option_present
 *===========================================================================
 * Description: Checks if an option is present in an icmp6 msg.
 * Parameters:  pkt - icmp6 msg
 *              opt - the icmp6 option to look for
 * Returns:     IP_TRUE if the option is present, IP_FALSE otherwise
 *
 */
IP_GLOBAL Ip_bool
ipnet_icmp6_option_present(Ipcom_pkt *pkt, Ip_u8 opt)
{
    Ipnet_pkt_nd_opt_hdr*          opt_hdr;
    int tot_opt_len;

    opt_hdr = ipnet_icmp6_find_opts(pkt, &tot_opt_len);
    if (!opt_hdr)
        return IP_FALSE;

    if (ipnet_icmp6_get_option(opt_hdr, tot_opt_len, opt, 0))
        return IP_TRUE;

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_icmp6_find_opts
 *===========================================================================
 * Description: Finds the option section in an icmp6 msg.
 * Parameters:
 * Returns: Pointer to the first option and the total length of the option
 *          section.
 *
 */
IP_GLOBAL Ipnet_pkt_nd_opt_hdr*
ipnet_icmp6_find_opts(Ipcom_pkt *pkt, int *tot_opt_len)
{
    Ipnet_pkt_nd_opt_hdr*          opt;
    Ipnet_pkt_icmp6               *icmp6;
    Ipnet_pkt_nd_neighbor_solicit *neigh_sol;
    Ipnet_pkt_nd_neighbor_advert  *neigh_adv;
    Ipnet_nd_router_advert        *adv;
    Ipnet_pkt_nd_redirect         *redir;

    opt = IP_NULL;
    *tot_opt_len = 0;

    icmp6 = (Ipnet_pkt_icmp6*) &pkt->data[pkt->start];
    switch (icmp6->icmp6_type)
    {
    case IPNET_ND_ROUTER_SOLICIT:
        opt = (Ipnet_pkt_nd_opt_hdr*)&icmp6[1];
        *tot_opt_len = pkt->end-pkt->start - sizeof(Ipnet_pkt_icmp6);
        break;

    case IPNET_ND_ROUTER_ADVERT:
        adv = (Ipnet_nd_router_advert*)icmp6;
        opt = (Ipnet_pkt_nd_opt_hdr*)&adv[1];
        *tot_opt_len = pkt->end-pkt->start - sizeof(Ipnet_nd_router_advert);
        break;

    case IPNET_ND_NEIGHBOR_SOLICIT:
        neigh_sol = (Ipnet_pkt_nd_neighbor_solicit*)icmp6;
        opt = (Ipnet_pkt_nd_opt_hdr*)&neigh_sol[1];
        *tot_opt_len = pkt->end-pkt->start - sizeof(Ipnet_pkt_nd_neighbor_solicit);
        break;

    case IPNET_ND_NEIGHBOR_ADVERT:
        neigh_adv = (Ipnet_pkt_nd_neighbor_advert*)icmp6;
        opt = (Ipnet_pkt_nd_opt_hdr*)&neigh_adv[1];
        *tot_opt_len = pkt->end-pkt->start - sizeof(Ipnet_pkt_nd_neighbor_advert);
        break;

    case IPNET_ND_REDIRECT:
        redir = (Ipnet_pkt_nd_redirect*)icmp6;
        opt = (Ipnet_pkt_nd_opt_hdr*)&redir[1];
        *tot_opt_len = pkt->end-pkt->start - sizeof(Ipnet_pkt_nd_redirect);
        break;

    case IPNET_ICMP6_ECHO_REQUEST:

    default:
#if 0
        IP_PANIC(); /* Not all icmp msg types implemented yet */
#endif
        break;
    }

    if (*tot_opt_len == 0)
        opt=IP_NULL;

    return opt;
}

/*
 *===========================================================================
 *                     ipnet_icmp6_param_init
 *===========================================================================
 * Description: Initializes the ICMPv6 paramter structure to create
 *              ICMP messages on failing packets.
 * Parameters:  [out] icmp6_param - Initialized ICMP param struct
 *              [in] pkt - The packet that caused the error to be sent.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_icmp6_param_init(Ipnet_icmp6_param *icmp6_param, Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip6 *ip6_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];

    ipcom_memset(icmp6_param, 0, sizeof(*icmp6_param));
    icmp6_param->vr         = pkt->vr_index;
    icmp6_param->ifindex    = pkt->ifindex;
    icmp6_param->recv_pkt   = pkt;
    icmp6_param->to         = &ip6_hdr->src;
    icmp6_param->rate_limit = IP_TRUE;
    if (ipnet_ip6_get_addr_type(&ip6_hdr->dst, pkt->vr_index, IP_NULL) == IPNET_ADDR_TYPE_UNICAST)
        icmp6_param->from = &ip6_hdr->dst;
    else
        icmp6_param->from = ipnet_ip6_get_src_addr(pkt->vr_index,
                                                   icmp6_param->to,
                                                   pkt->ifindex,
                                                   IP_NULL,
                                                   IP_NULL,
                                                   ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex),
                                                   0);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_input
 *===========================================================================
 * Description: Processes the ICMPv6 message and send a reply if neccessary.
 *              The processing of ICMPv6 messages is described in RFC 2461
 * Parameters:  netif - The network interface that received the message
 *              src - The source address of the packet.
 *              dst - The destination address for the packet (is a local address).
 *              hlim - The hoplimit value from the IPv6 header.
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_GLOBAL int
ipnet_icmp6_input(Ipnet_netif *netif,
                  IP_CONST struct Ip_in6_addr *src,
                  IP_CONST struct Ip_in6_addr *dst,
                  int hlim,
                  Ipcom_pkt *pkt)
{
    Ipnet_pkt_icmp6      *icmp6;
    int icmp6_len;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 4, IPCOM_WV_NETDEVENT_START,
                       ipnet_icmp6_input, IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);
    IPNET_STATS(icmp6_input++);
    IPCOM_MIB2_NETIF(ipv6IfIcmpInMsgs++);
    IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpStatsInMsgs, 1);

    icmp6 = (Ipnet_pkt_icmp6*) &pkt->data[pkt->start];
    icmp6_len = pkt->end - pkt->start;

    if (ipnet_ip6_checksum(src,
                           dst,
                           IP_IPPROTO_ICMPV6,
                           (Ip_u16*) icmp6,
                           (Ip_u16) icmp6_len,
                           pkt))

    {
        /* Checksum did not match */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 5, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_icmp6_input, IPCOM_WV_NETD_BADSUM,
                          IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);
        IPNET_STATS(icmp6_input_badchksum++);
        goto invalid_packet;
    }

#ifdef IPNET_USE_RFC3971
    if (ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_SEND_SIG_VERIFIED) == IP_NULL)
    {
        if (ipnet_send_sign_verify(netif, src, hlim, pkt) == IPNET_SEND_PKT_CONSUMED)
            return 0;
    }
#endif
#ifdef IPCOM_USE_MIB2
    /* Update MIB-2 stats */
    IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpMsgStatsInPkts[icmp6->icmp6_type], 1);
    switch (icmp6->icmp6_type)
    {
    case IPNET_ICMP6_DST_UNREACH :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInDestUnreachs++);
        if (icmp6->icmp6_code == IPNET_ICMP6_DST_UNREACH_ADMIN
            || icmp6->icmp6_code == IPNET_ICMP6_DST_UNREACH_SRC_ADDR_FAILED
            || icmp6->icmp6_code == IPNET_ICMP6_DST_UNREACH_REJECT_DST)
            IPCOM_MIB2_NETIF(ipv6IfIcmpInAdminProhibs++);
        break;
    case IPNET_ICMP6_PACKET_TOO_BIG :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInPktTooBigs++);
        break;
    case IPNET_ICMP6_TIME_EXCEEDED :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInTimeExcds++);
        break;
    case IPNET_ICMP6_PARAM_PROB :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInParmProblems++);
        break;
    case IPNET_ICMP6_ECHO_REQUEST :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInEchos++);
        break;
    case IPNET_ICMP6_ECHO_REPLY :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInEchoReplies++);
        break;
    case IPNET_MLD_LISTENER_QUERY :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInGroupMembQueries++);
        break;
    case IPNET_MLD_V1_LISTENER_REPORT :
    case IPNET_MLD_V2_LISTENER_REPORT :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInGroupMembResponses++);
        break;
    case IPNET_MLD_LISTENER_REDUCTION :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInGroupMembReductions++);
        break;
    case IPNET_ND_ROUTER_SOLICIT :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInRouterSolicits++);
        break;
    case IPNET_ND_ROUTER_ADVERT :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInRouterAdvertisements++);
        break;
    case IPNET_ND_NEIGHBOR_SOLICIT :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInNeighborSolicits++);
        break;
    case IPNET_ND_NEIGHBOR_ADVERT :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInNeighborAdvertisements++);
        break;
    case IPNET_ND_REDIRECT :
        IPCOM_MIB2_NETIF(ipv6IfIcmpInRedirects++);
        break;
    default :
        break;
    }
#endif /* IPCOM_USE_MIB2 */

    IPNET_DEBUG_LINE(ipnet_icmp6_input_log(netif, src, dst, icmp6->icmp6_type));

    if (icmp6->icmp6_type == IPNET_ICMP6_ECHO_REPLY)
    {
        (void) ipnet_raw_input(pkt,
                               IP_TRUE,
                               IP_IPPROTO_ICMPV6, src, 0, dst, 0,
                               (Ipnet_sock_lookup_f) ipnet_sock_ip6_lookup);
        /* The packet has been sent to all matching raw sockets */
        return 0;
    }

    if (IP_BIT_ISFALSE(pkt->flags, IPNET_IP6_PKT_SENT_TO_SOCK))
        /* Give a copy of this packet to all matching raw sockets */
        (void) ipnet_raw_input(pkt,
                               IP_FALSE,
                               IP_IPPROTO_ICMPV6, src, 0, dst, 0,
                               (Ipnet_sock_lookup_f) ipnet_sock_ip6_lookup);

    switch (icmp6->icmp6_type)
    {
    case IPNET_ICMP6_ECHO_REQUEST:
        return ipnet_icmp6_input_echo_request(netif, src, dst, pkt);

    case IPNET_ICMP6_DST_UNREACH:
        return ipnet_icmp6_input_dst_unreachable(pkt);

    case IPNET_ICMP6_PACKET_TOO_BIG:
        return ipnet_icmp6_input_packet_too_big(netif, pkt);

    case IPNET_ND_ROUTER_SOLICIT:
        return ipnet_icmp6_input_router_sol(netif, src, hlim, pkt);

    case IPNET_ND_ROUTER_ADVERT:
        return ipnet_icmp6_input_router_adv(netif, src, hlim, pkt);

    case IPNET_ND_NEIGHBOR_SOLICIT:
        return ipnet_icmp6_input_neighbor_sol(netif, src, dst, hlim, pkt);

    case IPNET_ND_NEIGHBOR_ADVERT:
        return ipnet_icmp6_input_neighbor_adv(netif, dst, hlim, pkt);

    case IPNET_ND_REDIRECT:
        return ipnet_icmp6_input_redirect(netif, src, hlim, pkt);

    case IPNET_MLD_LISTENER_QUERY:
        return ipnet_icmp6_input_mld_query(netif, hlim, pkt);

    default:
        /* Invalid or unsupported type, RFC 4443, chapter 2.4 says:
         ...
         (a) If an ICMPv6 error message of unknown type is received at its
         destination, it MUST be passed to the upper-layer process that
         originated the packet that caused the error, where this can be
         identified (see Section 2.4, (d)).
         ...
         All messages is gived to mathcing raw sockets via the
         ipnet_raw_input() above unless the packet
        */
        break;
    }


    /* Invalid ICMP6 type */
invalid_packet:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;

}


/*
 *===========================================================================
 *                    ipnet_icmp6_send_dad
 *===========================================================================
 * Description: Initiates duplicate address detection by sending a neighbor
 *              solicitation for the specified address.
 * Parameters:  [in] netif - The interface the address will be assigned to.
 *              [in] addr_entry - The address entry to test.
 * Returns:     0 = success, <0 = error code
 */
IP_GLOBAL int
ipnet_icmp6_send_dad(Ipnet_ip6_addr_entry *addr_entry)
{
    Ipnet_icmp6_param  icmp6_param;
    struct Ip_in6_addr tentative_sol_addr;
    Ipnet_netif       *netif = addr_entry->netif;

    if (ipnet_ip6_addr_set_timeout(addr_entry,
                                   ipnet_icmp6_get_nd_retransmit_duration(netif),
                                   IP_FALSE))
        return -IP_ERRNO_ENOMEM;

    /* The address is assigned to the interface if it passes
       dupicate address detection */
    IPNET_IP6_SET_SOLICITED_ADDR(&tentative_sol_addr, &addr_entry->addr);

    /* This address is tentative */
    IP_BIT_SET(addr_entry->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE);

    ipcom_memset(&icmp6_param, 0, sizeof(icmp6_param));
    icmp6_param.vr       = netif->vr_index;
    icmp6_param.ifindex  = netif->ipcom.ifindex;
    icmp6_param.type     = IPNET_ND_NEIGHBOR_SOLICIT;
    icmp6_param.from     = &ip_in6addr_any;
    icmp6_param.to       = &tentative_sol_addr;
    icmp6_param.data.neigbor_sol.target_addr = &addr_entry->addr;
    return ipnet_icmp6_send(&icmp6_param);
}


/*
 *===========================================================================
 *                     ipnet_icmp6_send_dst_unreach
 *===========================================================================
 * Description: Send a destination unreachable since the routing of the
 *              specified pkt failed.
 * Parameters:  failing_pkt - The packet that caused the error to be sent.
 *              error_code - IP_ERRNO_Exxx error code.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_icmp6_send_dst_unreach(Ipcom_pkt *failing_pkt, int error_code)
{
    Ipnet_icmp6_param icmp6_param;

    if (IP_BIT_ISSET(failing_pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
    {
        /* No route to host, send back a Host Unreachable error */
        ipnet_icmp6_param_init(&icmp6_param, failing_pkt);
        icmp6_param.type = IPNET_ICMP6_DST_UNREACH;
        switch (error_code)
        {
        case IP_ERRNO_EHOSTUNREACH:
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_ADDR;
            break;
        case IP_ERRNO_ENETUNREACH:
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_NOROUTE;
            break;
        case IP_ERRNO_EPERM:
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_ADMIN;
            break;
        case IP_ERRNO_EACCES:
#ifdef IPNET_USE_RFC4443
            /* RFC 4443, chapter 3.1
               Codes 5 and 6 are more informative subsets of code 1. */
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_REJECT_DST;
#else
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_ADMIN;
#endif
            break;
        default:
            IP_PANIC2();
            icmp6_param.code = IPNET_ICMP6_DST_UNREACH_NOROUTE;
            break;
        }
        (void) ipnet_icmp6_send(&icmp6_param);
    }
    else
    {
        if (error_code == -IP_ERRNO_ENETUNREACH)
        {
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsOutNoRoutes, 1);
        }
    }

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                      1, 6, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                      ipnet_icmp6_send_dst_unreach, IPCOM_WV_NETD_NOROUTE,
                      IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);
    IPNET_STATS(ip6_output_noroute++);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_send_router_sol_msg
 *===========================================================================
 * Description: Sends a router solicitation message to quickly get information
 *              about routers on the link.
 * Parameters:  netif - The interface to which the message should be sent.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_icmp6_send_router_sol_msg(Ipnet_netif *netif)
{
    struct Ip_in6_addr dst;
    Ipnet_icmp6_param  icmp6_param;

    if (netif->inet6_rs_tmo != IP_NULL)
        /* RS message is already pending */
        return;

    if (netif->inet6_rs_retrans_count == IPNET_INET6_RS_RETRANS_COUNT_NEED_INIT)
        netif->inet6_rs_retrans_count = (Ip_u8)
            ipnet_sysvar_netif_get_as_int(IP_AF_INET6, netif, "RtSolicitationCount", 3);

    if (netif->inet6_rs_retrans_count == IPNET_INET6_RS_RETRANS_COUNT_DISABLED)
        return;

    IPNET_IP6_SET_ALL_ROUTERS_ADDR(&dst);
    ipcom_memset(&icmp6_param, 0, sizeof(icmp6_param));
    icmp6_param.type     = IPNET_ND_ROUTER_SOLICIT;
    icmp6_param.vr       = netif->vr_index;
    icmp6_param.ifindex  = netif->ipcom.ifindex;
    icmp6_param.to       = &dst;
    icmp6_param.from     = ipnet_ip6_get_src_addr(netif->vr_index,
                                                  &dst,
                                                  netif->ipcom.ifindex,
                                                  IP_NULL,
                                                  IP_NULL,
                                                  netif,
                                                  0);

    if (icmp6_param.from == IP_NULL)
        /* This interface does not have a link local address for some reason,
           it is allowed to use the ANY address though */
        icmp6_param.from = &ip_in6addr_any;

    if (--netif->inet6_rs_retrans_count > 0)
    {
        Ip_u32 rs_min_msec;
        Ip_u32 rs_max_msec;

        rs_min_msec = ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                    netif,
                                                    "RtSolicitationMinInterval",
                                                    4000);
        rs_max_msec = ipnet_sysvar_netif_get_as_int(IP_AF_INET6,
                                                    netif,
                                                    "RtSolicitationMaxInterval",
                                                    4500);

        (void)ipnet_timeout_schedule(rs_min_msec + ipcom_rand() % (rs_max_msec - rs_min_msec),
                                     (Ipnet_timeout_handler) ipnet_icmp6_send_router_sol_msg,
                                     netif,
                                     &netif->inet6_rs_tmo);
    }

    (void) ipnet_icmp6_send(&icmp6_param);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_send
 *===========================================================================
 * Description: Creates and send a ICMPv6 message.
 * Parameters:  icmp6_param - The parameters needed to create the ICMPv6 message.
 * Returns:     0 = success, <0 error code.
 *
 */
IP_GLOBAL int
ipnet_icmp6_send(Ipnet_icmp6_param *icmp6_param)
{
    struct Ipnet_ipv6_key          to;
    struct Ip_in6_addr             from;
    Ipnet_ip6_output_param         param;
    Ip_u32                         hbh_router_alert[2];
    Ipnet_pkt_nd_neighbor_advert  *adv;
    Ipnet_pkt_nd_neighbor_solicit *sol;
    Ipnet_pkt_nd_redirect         *redirect;
    Ipnet_pkt_mld_hdr             *mld;
    Ipcom_pkt                     *pkt;
    Ipcom_pkt                     *failing_pkt;
    Ipnet_pkt_icmp6               *icmp6;
    Ipnet_pkt_nd_opt_hdr          *opt;
    Ipnet_netif                   *netif;
    Ipnet_route_policy_selector    rps;
    Ip_u8                          link_layer_addr_len;
    Ipnet_netif                   *recv_netif = IP_NULL;

#ifndef __LCC__
    ip_assert(
              icmp6_param->type == IPNET_ICMP6_DST_UNREACH
              || icmp6_param->type == IPNET_ICMP6_PACKET_TOO_BIG
              || icmp6_param->type == IPNET_ICMP6_TIME_EXCEEDED
              || icmp6_param->type == IPNET_ICMP6_PARAM_PROB
              || icmp6_param->type == IPNET_ICMP6_ECHO_REPLY
              || icmp6_param->type == IPNET_MLD_V1_LISTENER_REPORT
              || icmp6_param->type == IPNET_MLD_V2_LISTENER_REPORT
              || icmp6_param->type == IPNET_MLD_LISTENER_REDUCTION
              || icmp6_param->type == IPNET_ND_ROUTER_SOLICIT
              || icmp6_param->type == IPNET_ND_NEIGHBOR_SOLICIT
              || icmp6_param->type == IPNET_ND_NEIGHBOR_ADVERT
              || icmp6_param->type == IPNET_ND_REDIRECT
              );
#endif
    ip_assert(icmp6_param->to != IP_NULL);

    if (ipnet_route_is_virtual_router_valid(icmp6_param->vr) == IP_FALSE)
    {
        /* The virtual router has been deleted */
        IPCOM_LOG4(NOTICE,
                   "IPv6: send ICMPv6 type:%d code:%d to %s dropped since VR#%u has been removed",
                   icmp6_param->type,
                   icmp6_param->code,
                   ipcom_inet_ntop(IP_AF_INET6, icmp6_param->to, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   icmp6_param->vr);
        goto abort_send;
    }

    failing_pkt = icmp6_param->recv_pkt;
    if (failing_pkt != IP_NULL)
    {
        Ipnet_pkt_ip6 *ip6hdr;

        ip6hdr = (Ipnet_pkt_ip6 *) &failing_pkt->data[failing_pkt->ipstart];
        icmp6 = (Ipnet_pkt_icmp6 *) (ip6hdr + 1);
        if (icmp6_param->type < IPNET_ICMP6_INFOMSG_MASK)
        {
            Ip_bool is_multicast = IP_IN6_IS_ADDR_MULTICAST(&ip6hdr->dst);

            if (IP_BIT_ISSET(failing_pkt->flags, IPCOM_PKT_FLAG_LINK_MULTICAST | IPCOM_PKT_FLAG_MULTICAST))
                is_multicast = IP_TRUE;

            if ((ip6hdr->nxt == IP_IPPROTO_ICMPV6
                 && (icmp6->icmp6_type < IPNET_ICMP6_INFOMSG_MASK
                     || icmp6->icmp6_type == IPNET_ND_REDIRECT))
                || (is_multicast
                    && icmp6_param->type != IPNET_ICMP6_PACKET_TOO_BIG
                    && icmp6_param->type != IPNET_ICMP6_PARAM_PROB)
                || IP_IN6_IS_ADDR_UNSPECIFIED(&ip6hdr->src)
                || IP_IN6_IS_ADDR_MULTICAST(&ip6hdr->src))
                /* RFC 4443
                   (e) An ICMPv6 error message MUST NOT be originated as a result of
                       receiving the following:

                   (e.1) An ICMPv6 error message.

                   (e.2) An ICMPv6 redirect message [IPv6-DISC].

                   (e.3) A packet destined to an IPv6 multicast address.  (There are
                         two exceptions to this rule: (1) the Packet Too Big Message
                         (Section 3.2) to allow Path MTU discovery to work for IPv6
                         multicast, and (2) the Parameter Problem Message, Code 2
                         (Section 3.4) reporting an unrecognized IPv6 option (see
                         Section 4.2 of [IPv6]) that has the Option Type highest-
                         order two bits set to 10).

                   (e.4) A packet sent as a link-layer multicast (the exceptions
                         from e.3 apply to this case, too).

                   (e.5) A packet sent as a link-layer broadcast (the exceptions
                         from e.3 apply to this case, too).

                   (e.6) A packet whose source address does not uniquely identify a
                         single node -- e.g., the IPv6 Unspecified Address, an IPv6
                         multicast address, or an address known by the ICMP message
                         originator to be an IPv6 anycast address.
                */
                return 0;
        }
        recv_netif = ipnet_if_indextonetif(failing_pkt->vr_index,
                                           failing_pkt->ifindex);
    }

    switch (icmp6_param->type)
    {
    case IPNET_ICMP6_TIME_EXCEEDED:
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6, recv_netif, "IcmpSendTimeExceeded", 1, ipnet_bool_map) == 0)
            /* Silently ignore */
            return 0;
        break;

    case IPNET_ICMP6_DST_UNREACH:
        if (icmp6_param->code == IPNET_ICMP6_DST_UNREACH_NOROUTE)
        {
#ifdef IPCOM_USE_MIB2
            /* Make sure that netif is initialized for the MIB variable update below */
            netif = recv_netif;
#endif
            IPCOM_MIB2_NETIF(ipv6IfStatsInNoRoutes++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, ipSystemStatsInNoRoutes, 1);
            IPCOM_MIB2_PERIF_U32_ADD(v6, ipIfStatsInNoRoutes, 1, recv_netif);
        }
        if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6, recv_netif, "IcmpSendDestinationUnreachable", 1, ipnet_bool_map) == 0)
            /* Silently ignore */
            return 0;
        break;

    default:
        break;
    }

    /* RFC 4443, chapter 2.4
     ...
     (f) Finally, in order to limit the bandwidth and forwarding costs
         incurred by originating ICMPv6 error messages, an IPv6 node MUST
         limit the rate of ICMPv6 error messages it originates.  This
         situation may occur when a source sending a stream of erroneous
         packets fails to heed the resulting ICMPv6 error messages.

         Rate-limiting of forwarded ICMP messages is out of scope of this
         specification.

         A recommended method for implementing the rate-limiting function
         is a token bucket, limiting the average rate of transmission to
         N, where N can be either packets/second or a fraction of the
         attached link's bandwidth, but allowing up to B error messages to
         be transmitted in a burst, as long as the long-term average is
         not exceeded.
     ...
    */
    if (icmp6_param->rate_limit)
    {
        if (ipnet->ip6.icmp_send_limit_tmo == IP_NULL)
        {
            int ratelimit_interval;

            if (ipnet->ip6.icmp_send_limit == 0)
                /* First time called since boot, initialize the ICMP message count */
                ipnet_icmp6_rate_reseed(IP_NULL);

            ratelimit_interval = ipcom_sysvar_get_as_int0("ipnet.inet6.IcmpRatelimitInterval", 100);
            if (ratelimit_interval)
                (void) ipnet_timeout_schedule(ratelimit_interval,
                                              (Ipnet_timeout_handler) ipnet_icmp6_rate_reseed,
                                              IP_NULL,
                                              &ipnet->ip6.icmp_send_limit_tmo);
            else
                /* Disable ratelimit */
                ipnet->ip6.icmp_send_limit = ~0u;
        }

        if (ipnet->ip6.icmp_send_limit == 0)
        {
            IPCOM_LOG3(DEBUG2,
                       "IPv6: send ICMPv6 type:%d code:%d to %s dropped due to rate limit policy",
                       icmp6_param->type,
                       icmp6_param->code,
                       ipcom_inet_ntop(IP_AF_INET6, icmp6_param->to, ipnet->log_buf, sizeof(ipnet->log_buf)));
            goto abort_send;
        }
        --ipnet->ip6.icmp_send_limit;
    }

    netif = ipnet_if_indextonetif(icmp6_param->vr, icmp6_param->ifindex);

    ipcom_memset(&param, 0, sizeof(param));
    IPNET_IP6_SET_ADDR(&to.addr, icmp6_param->to);
    param.to       = &to.addr;
    param.final_to = &to.addr;
    param.nxt_hdr  = IP_IPPROTO_ICMPV6;

    if (ipnet_is_loopback(icmp6_param->vr, icmp6_param->ifindex)
        || ipnet_ip6_is_scoped_addr(param.to))
        param.scope_id = ipnet_ip6_get_zone(icmp6_param->ifindex, param.to);
    else
    {
        netif = ipnet_icmp6_get_netif_from_addr(icmp6_param->vr, &param);
        if (netif != IP_NULL)
            param.scope_id = ipnet_ip6_get_zone(netif->ipcom.ifindex, param.to);
    }

    if (IP_IN6_IS_ADDR_UNSPECIFIED(param.to))
    {
        if (icmp6_param->type != IPNET_ND_NEIGHBOR_ADVERT)
            /* Only answer requests with the unspecified address as source that
               are part of duplicate address detection */
            return 0;
        IPNET_IP6_SET_ALL_NODES_ADDR(param.to);
    }

    if (icmp6_param->type == IPNET_MLD_V1_LISTENER_REPORT
        || icmp6_param->type == IPNET_MLD_V2_LISTENER_REPORT
        || icmp6_param->type == IPNET_MLD_LISTENER_REDUCTION)
    {
        /* RFC 2710 chapter "3.  Message Format"
           ...
           All MLD messages described in this document are sent with
           a link-local IPv6 Source Address, an IPv6 Hop Limit of 1,
           and an IPv6 Router Alert option in a Hop-by-Hop Options header.
           ...
        */

        if (netif == IP_NULL)
            goto abort_send;


        hbh_router_alert[0] = 0;
        hbh_router_alert[1] = ip_htonl((IP_IP6OPT_ROUTER_ALERT << 24) | (2 << 16) | IP_IP6_ALERT_MLD);
        param.has_exthdr        = IP_TRUE;
        param.exthdrs.hbh       = (Ip_pkt_ip6_hbh *) hbh_router_alert;
        param.exthdrs.total_len = sizeof(hbh_router_alert);

        param.from = ipnet_ip6_get_src_addr(icmp6_param->vr,
                                            param.to,
                                            ipnet_ip6_get_zone(icmp6_param->ifindex, param.to),
                                            IP_NULL,
                                            IP_NULL,
                                            netif,
                                            IPNET_IPV6_REQUIRE_SRC_LINKLOCAL);
        if (param.from == IP_NULL)
            param.from = &ip_in6addr_any;
    }
    else if (icmp6_param->from == IP_NULL || IP_IN6_IS_ADDR_MULTICAST(icmp6_param->from))
    {
        /* RFC 2461 chapter "4.5.  Redirect Message Format"
           ...
           Source Address
           MUST be the link-local address assigned to the
           interface from which this message is sent.
           ...
        */

        if (netif == IP_NULL)
            goto abort_send;

        ipnet_ip6_param2policy(&param, netif, &rps, IP_NULL);
        param.from = ipnet_ip6_get_src_addr(icmp6_param->vr,
                                            param.to,
                                            ipnet_ip6_get_zone(icmp6_param->ifindex, param.to),
                                            IP_NULL,
                                            &rps,
                                            netif,
                                            (icmp6_param->type == IPNET_ND_REDIRECT
                                             ? IPNET_IPV6_REQUIRE_SRC_LINKLOCAL
                                             : 0));
        if (param.from == IP_NULL)
            return -IP_ERRNO_EHOSTUNREACH;
    }
    else
    {
        param.from = &from;
        IPNET_IP6_SET_ADDR(&from, icmp6_param->from);

        if (netif == IP_NULL)
        {
            /* Do resolve again with the source address set */
            netif = ipnet_icmp6_get_netif_from_addr(icmp6_param->vr, &param);
            if (netif != IP_NULL)
                param.scope_id = ipnet_ip6_get_zone(netif->ipcom.ifindex, param.to);
            else
                goto abort_send;
        }
    }

    if (icmp6_param->type == IPNET_ICMP6_ECHO_REPLY)
    {
#ifdef IP_PORT_LKM
        /* Clone the received packet */
        pkt = ipnet_pkt_clone(icmp6_param->recv_pkt, IP_TRUE);
        ipcom_pkt_free(icmp6_param->recv_pkt);
        icmp6_param->recv_pkt = IP_NULL;

        if (pkt == IP_NULL)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                              1, 7, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                              ipnet_icmp6_send, IPCOM_WV_NETD_NOBUFS,
                              IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);
            IPNET_STATS(icmp6_send_nomem++);
            IPCOM_MIB2_NETIF(ipv6IfIcmpOutErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpStatsOutErrors, 1);
            return -IP_ERRNO_ENOMEM;
        }
#else
        /* Reuse the received packet */
        pkt = icmp6_param->recv_pkt;
#endif
        pkt->ifindex = 0;
    }
    else
    {
        pkt = ipcom_pkt_malloc(ipnet_conf_ip6_min_mtu,
                               IP_FLAG_FC_STACKCONTEXT);
        if (pkt == IP_NULL)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                              1, 8, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                              ipnet_icmp6_send, IPCOM_WV_NETD_NOBUFS,
                              IPCOM_WV_IPNET_ICMP6_MODULE, IPCOM_WV_NETD_ICMP6);
            IPNET_STATS(icmp6_send_nomem++);
            IPCOM_MIB2_NETIF(ipv6IfIcmpOutErrors++);
            IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpStatsOutErrors, 1);
            return -IP_ERRNO_ENOMEM;
        }
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING);
        /* Set pkt->start to the beginning of ICMPv6 data */
        pkt->start = IPNET_IP6_HDR_SIZE
            + ipcom_conf_max_link_hdr_size
            + (param.exthdrs.hbh ? IPNET_ICMP6_HBH_SIZE : 0);

        /* Make room for the ICMPv6 header */
        pkt->end = pkt->start + IPNET_ICMPV6_HDR_SIZE;
    }

    /* Set the route table */
    pkt->vr_index = icmp6_param->vr;

    icmp6 = (Ipnet_pkt_icmp6 *)&pkt->data[pkt->start];

    /* Initialize the data field */
    switch (icmp6_param->type)
    {
    case IPNET_ICMP6_PACKET_TOO_BIG:
        IP_SET_HTONL(&icmp6->ipnet_icmp6_mtu, icmp6_param->data.pkt_to_big_mtu);
        break;

    case IPNET_ICMP6_PARAM_PROB:
        IP_SET_HTONL(&icmp6->ipnet_icmp6_pptr, icmp6_param->data.param_prob_pointer);
        break;

    case IPNET_ICMP6_ECHO_REPLY:
        /* Contains id and sequence number for echo request/reply messages */
        break;

    default:
        icmp6->ipnet_icmp6_data16[0] = 0;
        icmp6->ipnet_icmp6_data16[1] = 0;
        break;
    }

    /* Set the hop limit */
    switch (icmp6_param->type)
    {
    case IPNET_MLD_V1_LISTENER_REPORT:
    case IPNET_MLD_V2_LISTENER_REPORT:
    case IPNET_MLD_LISTENER_REDUCTION:
        /* Hop limit must be 1 since this message must never be forwarded */
        param.hlim = 1;
        break;
    case IPNET_ND_ROUTER_SOLICIT:
    case IPNET_ND_NEIGHBOR_SOLICIT:
    case IPNET_ND_NEIGHBOR_ADVERT:
    case IPNET_ND_REDIRECT:
        /* Must be 255, RFC 2461 */
        param.hlim = 255;
        break;
    case IPNET_ICMP6_ECHO_REPLY:
    default:
        param.hlim = IPNET_IP6_OUTPUT_PARAM_USE_DEFAULT_HLIM;
        break;
    }

    /* Add message specific parameters */
    switch (icmp6_param->type)
    {
    case IPNET_ICMP6_DST_UNREACH:
    case IPNET_ICMP6_PACKET_TOO_BIG:
    case IPNET_ICMP6_TIME_EXCEEDED:
    case IPNET_ICMP6_PARAM_PROB:
        /* Add as much as possible of the failing packet */
        if (IPNET_ICMP6_PKT_PAYLOAD_SPACE(pkt) < failing_pkt->end - failing_pkt->ipstart)
            /* Truncate the failing packet */
            failing_pkt->end = failing_pkt->ipstart + IPNET_ICMP6_PKT_PAYLOAD_SPACE(pkt);

        ipcom_memcpy(&pkt->data[pkt->end],
                     &failing_pkt->data[failing_pkt->ipstart],
                     failing_pkt->end - failing_pkt->ipstart);
        pkt->end += failing_pkt->end - failing_pkt->ipstart;
        break;

    case IPNET_ND_NEIGHBOR_SOLICIT:
        /* Add the destination address in the neighbor solicitation message */
        sol = (Ipnet_pkt_nd_neighbor_solicit *) icmp6;
        IPNET_IP6_SET_ADDR(&sol->nd_ns_target, icmp6_param->data.neigbor_sol.target_addr);
        pkt->end += sizeof(struct Ip_in6_addr);
        break;

    case IPNET_ND_NEIGHBOR_ADVERT:
        adv = (Ipnet_pkt_nd_neighbor_advert *) icmp6;
        if (icmp6_param->data.neighbor_adv.is_router)
            IPNET_ND_SET_IS_ROUTER(adv);
        if (icmp6_param->data.neighbor_adv.solicated)
            IPNET_ND_SET_SOLICATED_FLAG(adv);
        if (icmp6_param->data.neighbor_adv.override)
            IPNET_ND_SET_OVERRIDE_FLAG(adv);
        IPNET_IP6_SET_ADDR(&adv->nd_na_target, icmp6_param->data.neighbor_adv.target_addr);
        pkt->end += sizeof(struct Ip_in6_addr);
        break;

    case IPNET_ND_REDIRECT:
        redirect = (Ipnet_pkt_nd_redirect *) icmp6;

        IPNET_IP6_SET_ADDR(&redirect->nd_rd_target, icmp6_param->data.redirect.target_addr);
        IPNET_IP6_SET_ADDR(&redirect->nd_rd_dst, icmp6_param->data.redirect.dst_addr);
        pkt->end += sizeof(struct Ip_in6_addr) * 2;

        if (IP_BIT_ISSET(icmp6_param->data.redirect.dst_rt->hdr.flags, IPNET_RTF_DONE)
            && IP_BIT_ISSET(icmp6_param->data.redirect.dst_rt->hdr.flags, IPNET_RTF_LLINFO))
        {
            /* Add target link-layer address */
            struct Ip_sockaddr_dl *link_addr = (struct Ip_sockaddr_dl *)
                icmp6_param->data.redirect.dst_rt->gateway;

            opt = (Ipnet_pkt_nd_opt_hdr*) &pkt->data[pkt->end];
            link_layer_addr_len = (Ip_u8) ((sizeof(opt->nd_opt_type)
                                            + sizeof(opt->nd_opt_len)
                                            + link_addr->sdl_alen + 7) / 8);
            pkt->end += link_layer_addr_len << 3;
            opt->nd_opt_type = IPNET_ND_OPT_TARGET_LINKADDR;
            opt->nd_opt_len  = link_layer_addr_len;
            ipcom_memcpy(opt->nd_opt_data,
                         IP_SOCKADDR_DL_LLADDR(link_addr),
                         link_addr->sdl_alen);
        }

        if (icmp6_param->recv_pkt)
        {
            /* Add as much as possible of the redirected packet */
            Ipnet_pkt_nd_opt_rh *opt_rh;
            Ipcom_pkt           *r_pkt = icmp6_param->recv_pkt;
            int                  rh_opt_len;

            if (pkt->end - pkt->start + r_pkt->end - r_pkt->ipstart > ipnet_conf_ip6_min_mtu)
                r_pkt->end = ipnet_conf_ip6_min_mtu - pkt->end + pkt->start + r_pkt->ipstart;
            rh_opt_len = (sizeof(Ipnet_pkt_nd_opt_rh) + r_pkt->end - r_pkt->ipstart + 7) / 8;

            opt_rh = (Ipnet_pkt_nd_opt_rh*) &pkt->data[pkt->end];
            ipcom_memset(opt_rh, 0, rh_opt_len);
            opt_rh->nd_opt_rh_type = IPNET_ND_OPT_REDIRECTED_HEADER;
            opt_rh->nd_opt_rh_len  = (Ip_u8) rh_opt_len;
            ipcom_memcpy(opt_rh + 1, &r_pkt->data[r_pkt->start], r_pkt->end - r_pkt->ipstart);
        }

        break;

    case IPNET_MLD_V1_LISTENER_REPORT:
    case IPNET_MLD_LISTENER_REDUCTION:
        mld = (Ipnet_pkt_mld_hdr *) icmp6;

        IPNET_IP6_SET_ADDR(&mld->mcast_addr, &icmp6_param->data.mld.addr_entry->addr);
        param.netif = icmp6_param->data.mld.addr_entry->netif;
        pkt->end += sizeof(struct Ip_in6_addr);
        break;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    case IPNET_MLD_V2_LISTENER_REPORT:
        ipnet_mldv2_create_membership_report(pkt, icmp6_param->data.mld.addr_entry);
        break;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    default:
        break;
    }

    /* ICMPv6 options */
    if (!IP_IN6_IS_ADDR_UNSPECIFIED(param.from)
        && (icmp6_param->type == IPNET_ND_ROUTER_SOLICIT
            || icmp6_param->type == IPNET_ND_NEIGHBOR_SOLICIT
            || icmp6_param->type == IPNET_ND_NEIGHBOR_ADVERT))
    {
        /* Add Source Link Address Option */
        opt = (Ipnet_pkt_nd_opt_hdr*) &pkt->data[pkt->end];

        /* Length in 32-bits word */
        link_layer_addr_len = (Ip_u8) ((2 + netif->ipcom.link_addr_size + 7) / 8);
        pkt->end += link_layer_addr_len << 3;

        if (icmp6_param->type == IPNET_ND_NEIGHBOR_ADVERT)
            opt->nd_opt_type = IPNET_ND_OPT_TARGET_LINKADDR;
        else
            opt->nd_opt_type = IPNET_ND_OPT_SOURCE_LINKADDR;

        opt->nd_opt_len = link_layer_addr_len;
        ipcom_memcpy(opt->nd_opt_data, netif->ipcom.link_addr, netif->ipcom.link_addr_size);
    }

    ip_assert(pkt->end <= pkt->maxlen);

    /* RFC 4443
       2.3.  Message Checksum Calculation

       The checksum is the 16-bit one's complement of the one's complement
       sum of the entire ICMPv6 message, starting with the ICMPv6 message
       type field, and prepended with a "pseudo-header" of IPv6 header
       fields, as specified in [IPv6, Section 8.1].  The Next Header value
       used in the pseudo-header is 58.  (The inclusion of a pseudo-header
       in the ICMPv6 checksum is a change from IPv4; see [IPv6] for the
       rationale for this change.)

       For computing the checksum, the checksum field is first set to zero.
     */
    icmp6->icmp6_type  = icmp6_param->type;
    icmp6->icmp6_code  = icmp6_param->code;
    icmp6->icmp6_cksum = 0;
    param.chksum_ptr = &icmp6->icmp6_cksum;
    pkt->chk = ipcom_in_checksum_update(icmp6, (Ip_size_t) (pkt->end - pkt->start));
    /* The pseudo header checksum is calculated in ipnet_ip6_output() */

    /* Update MIB-2 stats */
    IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpStatsOutMsgs, 1);
    IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpMsgStatsOutPkts[icmp6->icmp6_type], 1);
    switch (icmp6->icmp6_type)
    {
    case IPNET_ICMP6_DST_UNREACH:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutDestUnreachs++);
        if (icmp6->icmp6_code == IPNET_ICMP6_DST_UNREACH_ADMIN)
            IPCOM_MIB2_NETIF(ipv6IfIcmpOutAdminProhibs++);
        break;
    case IPNET_ICMP6_PACKET_TOO_BIG:
        IPCOM_MIB2_NETIF(ipv6IfStatsInTooBigErrors++);
        break;
    case IPNET_ICMP6_TIME_EXCEEDED:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutTimeExcds++);
        break;
    case IPNET_ICMP6_PARAM_PROB:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutParmProblems++);
        break;
    case IPNET_MLD_LISTENER_QUERY:
    case IPNET_ICMP6_ECHO_REQUEST:
        /* Handled separately */
        break;
    case IPNET_ICMP6_ECHO_REPLY:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutEchoReplies++);
        break;
    case IPNET_MLD_V1_LISTENER_REPORT:
    case IPNET_MLD_V2_LISTENER_REPORT:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutGroupMembResponses++);
        break;
    case IPNET_MLD_LISTENER_REDUCTION:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutGroupMembReductions++);
        break;
    case IPNET_ND_ROUTER_SOLICIT:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutRouterSolicits++);
        break;
    case IPNET_ND_ROUTER_ADVERT:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutRouterAdvertisements++);
        break;
    case IPNET_ND_NEIGHBOR_SOLICIT:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutNeighborSolicits++);
        break;
    case IPNET_ND_NEIGHBOR_ADVERT:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutNeighborAdvertisements++);
        break;
    case IPNET_ND_REDIRECT:
        IPCOM_MIB2_NETIF(ipv6IfIcmpOutRedirects++);
        break;
    default :
        break;
    }

    return ipnet_ip6_output(&param, pkt);

 abort_send:
    if (icmp6_param->type == IPNET_ICMP6_ECHO_REPLY)
    {
        ipcom_pkt_free(icmp6_param->recv_pkt);
        icmp6_param->recv_pkt = IP_NULL;
    }
    return 0;
}


#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
/*
 *===========================================================================
 *                  ipnet_mldv2_add_source_to_group_record
 *===========================================================================
 * Description: Possibly adds one source address to a group record.
 * Parameters:  source_addr - The source address that might be added.
 *              d - Callback data.
 * Returns:
 *
 */
IP_STATIC void
ipnet_mldv2_add_source_to_group_record(struct Ip_in6_addr *source_addr,
                                       struct Ipnet_mldv2_report_for_each_data *d)
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
    IPNET_IP6_SET_ADDR(&d->group_record->source_addr[d->group_record->number_of_sources], source_addr);
    d->group_record->number_of_sources++;
}


/*
 *===========================================================================
 *                  ipnet_mldv2_create_membership_report
 *===========================================================================
 * Description: Creates a MLDv2 membership report.
 * Parameters:  pkt - The packet where the report should be stored.
 *              addr_entry - The multicast address entry a report should
 *                           be created for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_mldv2_create_membership_report(Ipcom_pkt *pkt, Ipnet_ip6_addr_entry *addr_entry)
{
    struct Ipnet_mldv2_report_for_each_data d;
    Ipnet_pkt_mldv2_report *report;
    Ipcom_set              *set;

    report = (Ipnet_pkt_mldv2_report *) &pkt->data[pkt->start];
    report->number_of_group_records = ip_htons(1);

    d.pkt          = pkt;
    d.addr_entry   = addr_entry;
    d.set          = IP_NULL;
    d.group_record = &report->group_records[0];
    d.group_record->aux_data_len      = 0;
    d.group_record->number_of_sources = 0;
    IPNET_IP6_SET_ADDR(&d.group_record->multicast_addr, &addr_entry->addr);
    pkt->end = pkt->start + IPNET_MLDV2_REPORT_MIN_SIZE;

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
                       (Ipcom_set_foreach_cb_func) ipnet_mldv2_add_source_to_group_record,
                       &d);
    d.group_record->number_of_sources = ip_htons(d.group_record->number_of_sources);
}


/*
 *===========================================================================
 *                  ipnet_mldv2_build_if_filter
 *===========================================================================
 * Description: Creates an interface from all the sockte filters.
 * Parameters:  netif - The inteface the multicast address is/was assigned to
 *              group - An multicast address.
 *              source - An unicast address.
 *              filter_mode -
 *              is_add - IP_TRUE if the source was added, IP_FALSE otherwise.
 * Returns:
 *
 */
IP_STATIC void
ipnet_mldv2_build_if_filter(int *sockfd, Ipnet_ip6_addr_entry *addr)
{
    Ipnet_ip6_sock_mcast_data *mcast_data;
    Ipnet_socket              *sock = ipnet->sockets[*sockfd];

    if (sock == IP_NULL)
    {
        /* The socket should always be removed from the set if closed */
        IP_PANIC2();
        return;
    }

    mcast_data = *ipnet_sock_ip6_get_mcast_data(sock,
                                                addr->netif,
                                                &addr->addr);
    if (mcast_data == IP_NULL)
    {
        /* The socket must not be in the socket set if is has not joined a group */
        IP_PANIC2();
        return;
    }

    ipnet_mcast_build_if_filter(&addr->mcast, mcast_data->sources);
}


/*
 *===========================================================================
 *                  ipnet_mldv2_report_change
 *===========================================================================
 * Description: Creates a report about filter change on a multicast address.
 * Parameters:  netif - The inteface the multicast address is/was assigned to
 *              group - An multicast address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_mldv2_report_change(Ipnet_netif *netif, struct Ip_in6_addr *group)
{
    Ipnet_ip6_addr_entry *addr_entry;

    addr_entry = ipnet_ip6_get_addr_entry(group, netif->vr_index, netif);
    if (addr_entry == IP_NULL)
        /* No longer member of this multicast address on this interface */
        return;

    if (ipnet_mcast_build_source_change_report(&addr_entry->mcast,
                                               (Ipcom_set_foreach_cb_func) ipnet_mldv2_build_if_filter,
                                               addr_entry))
    {
        addr_entry->mcast.filter_change_resend_count = (Ip_u8) netif->mld_robustness_variable;
        ipnet_mld_report_filter_change(addr_entry);
    }
}


/*
 *===========================================================================
 *                    ipnet_mld_host_compatibility_mode
 *===========================================================================
 * Description: Returns the MLD mode the host is running at on the specified
 *              interface.
 * Parameters:  netif - The interface that joined the multicast group.
 * Returns:     IPNET_MLD_V[1|2}_MODE
 *
 */
IP_STATIC int
ipnet_mld_host_compatibility_mode(Ipnet_netif *netif)
{
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    int compat_version = IPNET_MLDV2_MODE;
#else
    int compat_version = IPNET_MLDV1_MODE;
#endif
    int maximum_mld_version =
        ipnet_sysvar_netif_get_as_int(IP_AF_INET,
                                      netif,
                                      "MldVersion",
                                      IPNET_MLDV2_MODE);

    if (netif->mldv1_querier_present_tmo != IP_NULL)
        compat_version = IPNET_MLDV1_MODE;

    return IP_MIN(compat_version, maximum_mld_version);
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */


/*
 *===========================================================================
 *                    ipnet_mld_report
 *===========================================================================
 * Description: Sends a host membership report for the multicast address.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 *              report_type - One of the IPNET_MCAST_REPORT_xxx constants.
 * Returns:
 *
 */
IP_STATIC void
ipnet_mld_report(Ipnet_ip6_addr_entry *addr_entry, Ip_u8 report_type)
{
    Ipnet_icmp6_param  param;
    int                mode;
    struct Ip_in6_addr mldv2_routers;
    Ipnet_netif       *netif = addr_entry->netif;

    if (ipnet_mld_should_send_message(addr_entry) == IP_FALSE)
        return;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    mode = ipnet_mld_host_compatibility_mode(netif);
#else
    mode = IPNET_MLDV1_MODE;
#endif

    ipcom_memset(&param, 0, sizeof(param));
    param.to = &addr_entry->addr;

    if (mode == IPNET_MLDV1_MODE)
        param.type = IPNET_MLD_V1_LISTENER_REPORT;
    else
    {
        param.type = IPNET_MLD_V2_LISTENER_REPORT;
        param.to   = &mldv2_routers;
        (void)ipcom_inet_pton(IP_AF_INET6, "ff02::16", &mldv2_routers);
    }

    addr_entry->mcast.report_type = report_type;

    param.vr                  = netif->vr_index;
    param.ifindex             = netif->ipcom.ifindex;
    param.data.mld.addr_entry = addr_entry;

    (void)ipnet_icmp6_send(&param);

    ipnet_mcast_report_finish(&addr_entry->mcast,
                              (Ipnet_timeout_handler) ipnet_mld_report_filter_change,
                              addr_entry);
}


/*
 *===========================================================================
 *                    ipnet_mld_report_general_query
 *===========================================================================
 * Description: Sends a host membership report for the multicast address
 *              as a response to a general query.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_mld_report_general_query(Ipnet_ip6_addr_entry *addr_entry)
{
    ipnet_mld_report(addr_entry, IPNET_MCAST_REPORT_GENERAL_QUERY);
}


/*
 *===========================================================================
 *                    ipnet_mld_report_specific_query
 *===========================================================================
 * Description: Sends a host membership report for the multicast address
 *              as a response to a specific query.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_mld_report_specific_query(Ipnet_ip6_addr_entry *addr_entry)
{
    ipnet_mld_report(addr_entry, IPNET_MCAST_REPORT_SPECIFIC_QUERY);
}


/*
 *===========================================================================
 *                    ipnet_mld_report_filter_change
 *===========================================================================
 * Description: Sends a host membership report for the multicast address
 *              as a response to a filter change at the socket level.
 * Parameters:  addr_entry - The address entry for which the report should be
 *                           sent for.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_mld_report_filter_change(Ipnet_ip6_addr_entry *addr_entry)
{
    if (addr_entry->mcast.filter_change_resend_count
        == addr_entry->netif->mld_robustness_variable)
    {
        ipnet_timeout_cancel(addr_entry->mcast.filter_change_tmo);
        --addr_entry->mcast.filter_change_resend_count;
    }
    ipnet_mld_report(addr_entry, IPNET_MCAST_REPORT_FILTER_CHANGE);
}


/*
 *===========================================================================
 *                    ipnet_mld_report_all
 *===========================================================================
 * Description: Sends a multicast listener report message for all multicast
 *              addresses on the specified link.
 * Parameters:  netif - The interface to report multicast group memberships
 *              for.
 *              max_delay_msec - A response will be sent within
 *                               [0..max_delay_msec] milliseconds.
 * Returns:
 *
 */
IP_STATIC void
ipnet_mld_report_all(Ipnet_netif *netif, Ip_u32 max_delay_msec)
{
    Ipnet_ip6_addr_entry *addr;

    for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
        if (IP_IN6_IS_ADDR_MULTICAST(&addr->addr))
            ipnet_mcast_schedule_membership_report(&addr->mcast,
                                                   IP_FALSE,
                                                   max_delay_msec,
                                                   (Ipnet_timeout_handler)
                                                   ipnet_mld_report_general_query,
                                                   addr);
}


/*
 *===========================================================================
 *                    ipnet_mld_done
 *===========================================================================
 * Description: Sends a multicast listener done message.
 * Parameters:  maddr - The address being removed from the interface.
 * Returns:     >=0 = success , <0 = error code.
 *
 */
IP_GLOBAL void
ipnet_mld_done(Ipnet_ip6_addr_entry *maddr)
{
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    Ipnet_icmp6_param  param;
    struct Ip_in6_addr all_routers_addr;
    Ipnet_netif       *netif = maddr->netif;
#endif

    if (ipnet_mld_should_send_message(maddr) == IP_FALSE)
        return;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    if (ipnet_mld_host_compatibility_mode(netif) == IPNET_MLDV1_MODE)
    {
        ipcom_memset(&all_routers_addr, 0, sizeof(all_routers_addr));
        all_routers_addr.in6.addr16[0] = ip_htons(0xFF02);
        all_routers_addr.in6.addr16[7] = ip_htons(0x0002);

        ipcom_memset(&param, 0, sizeof(param));
        param.type                = IPNET_MLD_LISTENER_REDUCTION;
        param.to                  = &all_routers_addr;
        param.vr                  = netif->vr_index;
        param.ifindex             = netif->ipcom.ifindex;
        param.data.mld.addr_entry = maddr;
        (void) ipnet_icmp6_send(&param);
    }
#endif

    /* MLDv2 */
    ipnet_mcast_clear(&maddr->mcast);
    ipnet_mld_report(maddr, IPNET_MCAST_REPORT_FILTER_CHANGE);
}


/*
 *===========================================================================
 *                    ipnet_mldv2_time_to_msec
 *===========================================================================
 * Description: Convert the time value used by MLDv2 to milliseconds.
 * Parameters:  t - The time field.
 * Returns:     The time in milliseconds.
 *
 */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_STATIC Ip_u32
ipnet_mldv2_time_to_msec(Ip_u16 t)
{
    t = ip_ntohs(t);
    if (t < 32768)
        return t;

    /*       0 1 2 3 4 5 6 7
             +-+-+-+-+-+-+-+-+
             |1| exp | mant  |
             +-+-+-+-+-+-+-+-+
             Time = (mant | 0x10) << (exp + 3) */
    return ((t & 0xfff) | 0x1000) << (((t & 0x7000) >> 12) + 3);
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */


/*
 *===========================================================================
 *                    ipnet_icmp6_input_mld_query
 *===========================================================================
 * Description: Processes MLD query requests by sending a MLD report to the
 *              requestor.
 * Parameters:  netif - The network interface that received the message
 *              hlim - The hoplimit value from the IPv6 header.
 *              pkt - The received ICMP6 message, pkt->start is the offset to
 *              the ICMP6 header.
 * Returns:     0 = success , <0 = error code
 *
 */
IP_STATIC int
ipnet_icmp6_input_mld_query(Ipnet_netif *netif, int hlim, Ipcom_pkt *pkt)
{
    Ipnet_pkt_mld_hdr    *mld;
    Ipnet_ip6_addr_entry *addr_entry = IP_NULL;
    unsigned              msg_len = (unsigned) (pkt->end - pkt->start);
    Ip_u32                max_delay_msec;
    Ip_bool               is_specific_query = IP_FALSE;

    mld = (Ipnet_pkt_mld_hdr*) &pkt->data[pkt->start];

    if (hlim != 1
        || (msg_len != IPNET_MLDV1_QUERY_SIZE
            && (msg_len < IPNET_MLDV2_MIN_QUERY_SIZE
                || (msg_len != IPNET_MLDV2_MIN_QUERY_SIZE
                    + ip_ntohs(mld->number_of_sources) * sizeof(struct Ip_in6_addr)))))
        goto invalid_packet;

    if (IP_IN6_IS_ADDR_UNSPECIFIED(&mld->mcast_addr) == IP_FALSE)
    {
        if (IP_IN6_IS_ADDR_MULTICAST(&mld->mcast_addr)== IP_FALSE)
            /* Invalid group */
            goto invalid_packet;
        addr_entry = ipnet_ip6_get_addr_entry(&mld->mcast_addr, pkt->vr_index, netif);
    }

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    if (msg_len == IPNET_MLDV1_QUERY_SIZE)
    {
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

        max_delay_msec = ip_ntohs(mld->ipnet_mld_maxdelay);
        netif->mld_query_interval = IPNET_MCAST_DEFAULT_QUERY_INTERVAL;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        if (netif->mldv1_querier_present_tmo == IP_NULL)
            IPCOM_LOG1(INFO, "IPv4: enter MLDv1 compatibility mode on interface %s",
                       netif->ipcom.name);
        else
            ipnet_timeout_cancel(netif->mldv1_querier_present_tmo);

        (void) ipnet_timeout_schedule(1000 * netif->mld_robustness_variable * netif->mld_query_interval
                                      + max_delay_msec,
                                      IP_NULL,
                                      IP_NULL,
                                      &netif->mldv1_querier_present_tmo);
    }
    else
    {
        Ipcom_set *sources = IP_NULL;

        /* MLDv2 */
        if (addr_entry != IP_NULL)
        {
            if (addr_entry->mcast.specific_query_sources != IP_NULL)
                /*  Source specific query already pending, append sources to this one */
                sources = addr_entry->mcast.specific_query_sources;
            else
            {
                sources = ipcom_set_new(sizeof(struct Ip_in6_addr));
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

            if (mld->number_of_sources == 0
                || sources->user == IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE)
            {
            out_of_mem:
                sources->user = IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE;
                ipcom_set_remove_all(sources);
            }
            else
            {
                sources->user = IPNET_MCAST_RECORD_TYPE_IS_INCLUDE;
                for (i = 0; i < ip_ntohs(mld->number_of_sources); i++)
                    if (ipcom_set_add(sources, &mld->source_addr[i]) == IPCOM_ERR_NO_MEMORY)
                        /* Send report for everything on this group */
                        goto out_of_mem;
            }
        }

        max_delay_msec = ipnet_mldv2_time_to_msec(mld->ipnet_mld_maxdelay);
        netif->mld_query_interval = ipnet_mcast_time_to_msec(mld->qqic);
        if (netif->mld_query_interval == 0)
            netif->mld_query_interval = IPNET_MCAST_DEFAULT_QUERY_INTERVAL;
    }
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    if (max_delay_msec == 0)
        /* RFC3810 does not directly say that this is invalid,
           but it would result in "ack-implosion" that is specifically mention
           as something not allowed */
        goto invalid_packet;

    if (IP_IN6_IS_ADDR_UNSPECIFIED(&mld->mcast_addr))
        ipnet_mld_report_all(netif, max_delay_msec);
    else if (addr_entry != IP_NULL)
        ipnet_mcast_schedule_membership_report(&addr_entry->mcast,
                                               is_specific_query,
                                               max_delay_msec,
                                               (Ipnet_timeout_handler)
                                               (is_specific_query
                                                ? ipnet_mld_report_specific_query
                                                : ipnet_mld_report_general_query),
                                               addr_entry);

    ipcom_pkt_free(pkt);
    return 0;

invalid_packet:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                ipnet_icmp6_neighbor_send_pending_pkt
 *===========================================================================
 * Description: Sends a packet that has been waiting for NDP response.
 * Parameters:  neigh - neighbor entry that has received its link layer addr.
 *              pkt - An IPv6 packet.
 * Returns:
 *
 */
IP_STATIC void
ipnet_icmp6_neighbor_send_pending_pkt(Ipnet_neigh_entry *neigh,
                                      Ipcom_pkt *pkt)
{
    Ipnet_route_entry     *rt = neigh->rt;
    Ipnet_netif           *netif = rt->netif;

    (void)netif->link_ip6_output(netif, rt, pkt);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_neighbor_cache_init
 *===========================================================================
 * Description: Initialize the route entry to be used in NDP.
 * Parameters:  rt - The route table entry to initialize as a neighbor entry.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_icmp6_neighbor_cache_init(Ipnet_route_entry *rt)
{
    ipnet_neigh_init(rt,
                     ipnet_icmp6_neighbor_cache_timeout,
                     ipnet_icmp6_nd_probe,
                     ipnet_icmp6_schedule_addr_unreachable,
                     ipnet_icmp6_neighbor_send_pending_pkt);
}


/*
 *===========================================================================
 *                    ipnet_icmp6_default_router_change
 *===========================================================================
 * Description: Updates an entry in the default router list.
 * Parameters:  netif - The link this router is on.
 *              addr - The link local IPv6 address of the router.
 *              ra - The ICMPv6 router advertisment message.
 * Returns:     IP_TRUE if the entry was successfully updated.
 *
 */
IP_GLOBAL Ip_bool
ipnet_icmp6_default_router_change(Ipnet_netif *netif,
                                  IP_CONST struct Ip_in6_addr *gw_addr,
                                  Ip_bool adjust_lifetime,
                                  Ip_u32 lifetime_sec,
                                  Ip_bool adjust_preference,
                                  Ip_bool preferred)
{
    int                     ret;
    Ipnet_route_entry      *rt;
    struct Ip_sockaddr_in6  gw;

    ipnet_ip6_create_sockaddr(&gw, gw_addr, netif->ipcom.ifindex);
    ret = ipnet_route_raw_lookup2(IP_AF_INET6,
                                  netif->vr_index,
                                  IPCOM_ROUTE_TABLE_DEFAULT,
                                  0,
                                  &ip_in6addr_any,
                                  0,
                                  &ip_in6addr_any,
                                  (struct Ip_sockaddr *) &gw,
                                  netif->ipcom.ifindex,
                                  &rt);

    if (ret != IPNET_ROUTE_PERFECT_MATCH)
        return IP_FALSE;

    if (adjust_preference)
    {
        Ip_u32 new_hopcount = (preferred
                               ? IPNET_ICMP6_PREFERRED_HOPCOUNT
                               : IPNET_ICMP6_DEFAULT_HOPCOUNT);
        if (rt->metrics.rmx_hopcount != new_hopcount)
        {
            rt->metrics.rmx_hopcount = new_hopcount;
            ipnet_route_has_changed(rt);
            if (!adjust_lifetime)
            {
                IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));
                IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
            }
        }
    }

    if (adjust_lifetime)
        ipnet_route_set_lifetime(rt, lifetime_sec);

    return IP_TRUE;
}

#ifdef IPNET_DEBUG
/*
 *===========================================================================
 *                     ipnet_icmp6_init
 *===========================================================================
 * Description: Initializes the ICMPv6 module.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_icmp6_init(void)
{
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_icmp6_send_router_sol_msg,
                            (Ipnet_timeout_to_string_f) ipnet_icmp6_send_router_sol_to_string);

}
#endif /* IPNET_DEBUG */

#endif /* IPCOM_USE_INET6 */


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

