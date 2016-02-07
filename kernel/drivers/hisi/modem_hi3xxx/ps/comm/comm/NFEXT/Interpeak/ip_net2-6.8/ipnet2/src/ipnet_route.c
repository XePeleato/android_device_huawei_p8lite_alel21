/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_route.c,v $ $Revision: 1.314.14.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_route.c,v $
 *     $Author: jhorteli $
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

/* Functions for the fib tables
   Each entry entry that has a unique key is put in a Patricia trie.

Patricia trie example:
                               root
                                 |
                                 |
                      /--- partition bit X ----------\
                      |                              |
               bit X is not set                 bit X is set
                      |                              |
                      |              /---- partition bit (X + N) ----\
                     leaf            |                               |
                              bit (X + N) is not set          bit (X + N) is set
                                     |                               |
                                     |                               |
                              ~trie continues~                      leaf

   The internal nodes contains which bit the trie is partition at.
   The leaf nodes contains the route entries.

   IPNET:s FIB table supports having more than one entry that has the same
   key as long as mask and/or gateway is unique.

   Each leaf has this structure:

                       NULL                NULL
                         ^                   ^
                         |                   |
                         |                   |
                       prev                prev
                         |                   |
                                - narrow --->       - narrow --->
                          leaf1               leaf3               NULL
        NULL <--- widen -       <--- widen -
                         ^     |
                         |   next
                         |     |
                       prev    |
                         |     v

                          leaf2

                               |
                             next
                               |
                               |
                               v
                             NULL

   Route entries with the same mask can be traversed by following the
   'next' and 'prev' pointers. The list is sorted on increasing hop-count
   metric value.
   Equal Cost Multi-path routing is implemented by select the route entries
   with the same (and lowest) metric in a round-robin fashion.

   The route with the most generic mask (i.e. smallest prefixlen) is
   located first in the 'narrow' - 'widen' list.
   The lookup function will always select the route entry with as big
   prefixlen as possible, unless the mask is explicitly given.

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
#include <ipcom_err.h>
#include <ipcom_clib.h>
#include <ipcom_syslog.h>
#include <ipcom_cmd.h>

#include "ipnet_cmd.h"
#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_netlink.h"
#include "ipnet_route.h"
#ifdef IPCOM_USE_INET
#include "ipnet_ip4.h"
#endif
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#include "ipnet_icmp6.h"
#endif

#ifdef IPMPLS
#include "ipnet_mpls.h"
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Length of the data buffer */
#ifdef IPCOM_USE_INET6
#define IPNET_ROUTE_ENTRY_DATA_LEN sizeof(Ipnet_ip6_nc_entry)
#elif defined(IPCOM_USE_ETHERNET)
#include "ipnet_eth.h"
#define IPNET_ROUTE_ENTRY_DATA_LEN sizeof(Ipnet_arp_entry)
#else
/* No structure to go on, require the user to say how much space
   the link layers in the system needs */
#error "Must define IPNET_ROUTE_ENTRY_DATA_LEN"
#endif


/* The head of the don't list must be written/read to/from the route entry using these macros */
#define IPNET_ROUTE_DONT_ROUTE_GET_HEAD(rt)   (*(Ipnet_route_entry **) (rt)->rt_template)
#define IPNET_ROUTE_DONT_ROUTE_SET_HEAD(rt,h) ((rt)->rt_template = (Ipnet_route_entry *) &(h))

/* IPNET_AF_INET_OR_INET6 is used where it does not matter which domain is used as long
   as it is supported by the build */
#ifdef IPCOM_USE_INET
#define IPNET_AF_INET_OR_INET6 IP_AF_INET
#else
#define IPNET_AF_INET_OR_INET6 IP_AF_INET6
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* The key to lookup a route table, it is also used for looking up policy routing rules */
struct Ipnet_route_table_key
{
    /* IP_AF_xxx */
    int    domain;
    /* The routing table ID, not used when looking up policy routing rules. */
    Ip_u32 table;
    /* Virtual router index */
    Ip_u16 vr;
};


/* Head for a policy routing rule list */
typedef struct Ipnet_route_policy_rule_head_struct
{
    /* IP_AF_xxx */
    int        domain;
    /* Virtual router index */
    Ip_u16     vr;
    /* The head of the rule list */
    Ipcom_list list;
}
Ipnet_route_policy_rule_head;


/* Route table enumeration callback data */
struct Ipnet_route_get_table_ids_cb_data
{
    int     domain;
    Ip_u16  vr;
    Ip_u32 *table_ids;
    int     added;
    int     size;
};


/* Route remove callback data */
struct Ipnet_route_remove_all_cb
{
    int          domain; /* Domain to remove routes from */
    Ipnet_route_delete_for_each_data d;
};


#ifdef IPNET_USE_ROUTE_TABLE_NAMES
/* Route table name hash entry */
typedef struct Ipnet_route_name_entry_struct
{
    Ip_u16 vr;
    Ip_u32 table;
    char   name[IPCOM_RTNAMSIZ];
}  Ipnet_route_name_entry;
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */

#ifdef IPNET_USE_ROUTE_COOKIES
/* Route table cookie hash key */
typedef struct Ipnet_rtab_cookie_key_struct
{
    Ip_u16 vr;
    Ip_u32 table;
}  Ipnet_rtab_cookie_key;

/* Route table cookie hash entry */
typedef struct Ipnet_rtab_cookie_entry_struct
{
    Ipnet_rtab_cookie_key key;
    Ipnet_rt_cookie cookie;
}  Ipnet_rtab_cookie_entry;

#endif /* IPNET_USE_ROUTE_TABLE_COOKIES */


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
#if IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG || defined(IPNET_DEBUG)
IP_STATIC char *ipnet_route_addr_to_str(int domain,
                                        void *addr,
                                        char *str,
                                        Ip_size_t len);
IP_STATIC char *ipnet_route_sockaddr_to_str(struct Ip_sockaddr *sa,
                                            char *str,
                                            Ip_size_t len);
#endif

IP_PUBLIC void ipnet_route_walk_tree(Ipcom_route *rtab,
                                     Ipcom_route_walk_cb cb,
                                     void *data);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

#ifdef IPNET_USE_ROUTE_COOKIES
/*
 *===========================================================================
 *                    ipnet_route_cookie_obj_hash
 *===========================================================================
 * Description: Obj hash utility function for the vrtab cookie hash table
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_route_cookie_obj_hash(Ipnet_rtab_cookie_entry *ve)
{
    return ipcom_hash_update(&ve->key,
                             sizeof(ve->key),
                             0);
}


/*
 *===========================================================================
 *                    ipnet_route_cookie_key_hash
 *===========================================================================
 * Description: Key hash utility function for the vrtab cookie hash table
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_route_cookie_key_hash(Ipnet_rtab_cookie_key *key)
{
    return ipcom_hash_update(key,
                             sizeof(*key),
                             0);
}


/*
 *===========================================================================
 *                    ipnet_route_cookie_hash_cmp
 *===========================================================================
 * Description: Compare hash utility function for the rtab cookie hash table.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_route_cookie_hash_cmp(Ipnet_rtab_cookie_entry *ve,
                            Ipnet_rtab_cookie_key *key)
{
    return ((key->vr == ve->key.vr)
             && ((key->table == ve->key.table) || (key->table == IPCOM_ROUTE_TABLE_ANY)));
}


/*
 *===========================================================================
 *                    ipnet_route_find_cookie_cb
 *===========================================================================
 * Description: Foreach callback
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_find_cookie_cb(Ipnet_rtab_cookie_entry *hash_ve, Ipnet_rtab_cookie_entry *ve)
{
    if (hash_ve->key.vr == ve->key.vr && hash_ve->key.table == ve->key.table)
        ipcom_memcpy(&ve->cookie, &hash_ve->cookie, sizeof(Ipnet_rt_cookie));
}
#endif /* IPNET_USE_ROUTE_TABLE_COOKIES */


#ifdef IPNET_USE_ROUTE_TABLE_NAMES
/*
 *===========================================================================
 *                    ipnet_route_name_obj_hash
 *===========================================================================
 * Description: Obj hash utility function for the vrtab name hash table
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_route_name_obj_hash(Ipnet_route_name_entry *ve)
{
    return ipcom_hash_update(ve->name,
                             ipcom_strlen(ve->name),
                             0);
}


/*
 *===========================================================================
 *                    ipnet_route_name_key_hash
 *===========================================================================
 * Description: Key hash utility function for the vrtab name hash table
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_route_name_key_hash(char *rtab_name)
{
    return ipcom_hash_update(rtab_name,
                             ipcom_strlen(rtab_name),
                             0);
}


/*
 *===========================================================================
 *                    ipnet_route_name_table_init
 *===========================================================================
 * Description: Compare hash utility function for the rtab name hash table.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_route_name_hash_cmp(Ipnet_route_name_entry *ve, char *rtab_name)
{
    return ipcom_strcmp(ve->name, rtab_name) == 0;
}


/*
 *===========================================================================
 *                    ipnet_route_find_name_cb
 *===========================================================================
 * Description: Foreach callback
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_find_name_cb(Ipnet_route_name_entry *hash_ve, Ipnet_route_name_entry *ve)
{
    if (hash_ve->vr == ve->vr && hash_ve->table == ve->table)
        ipcom_strcpy(ve->name, hash_ve->name);
}
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */


/*
 *===========================================================================
 *                    ipnet_route_table_hash_key
 *===========================================================================
 * Description: Calculates a hash value for the route table search key.
 * Parameters:  key - A search key.
 * Returns:     Hash value.
 *
 */
IP_STATIC unsigned
ipnet_route_table_hash_key(struct Ipnet_route_table_key *key)
{
    return ipcom_hash_update(&key->table,
                             sizeof(key->table),
                             (key->domain << 16) + key->vr);
}


/*
 *===========================================================================
 *                    ipnet_route_table_hash_obj
 *===========================================================================
 * Description: Calculates a hash value for the route table.
 * Parameters:  rt_head - A route table head.
 * Returns:     Hash value.
 *
 */
IP_STATIC unsigned
ipnet_route_table_hash_obj(Ipcom_route *rt_head)
{
    struct Ipnet_route_table_key k;

    k.domain = IPNET_ROUTE_GET_FAMILY(rt_head);
    k.vr     = IPNET_ROUTE_GET_VR(rt_head);
    k.table  = IPNET_ROUTE_GET_TABLE(rt_head);
    return ipnet_route_table_hash_key(&k);
}


/*
 *===========================================================================
 *                    ipnet_route_table_hash_cmp
 *===========================================================================
 * Description: Checks if the key matches the route table head.
 * Parameters:  rt_head - A route table head.
 *              key - A search key.
 * Returns:     IP_TRUE if the key matches the element.
 *
 */
IP_STATIC Ip_bool
ipnet_route_table_hash_cmp(Ipcom_route *rt_head, struct Ipnet_route_table_key *key)
{
    return IPNET_ROUTE_GET_TABLE(rt_head) == key->table
        && IPNET_ROUTE_GET_FAMILY(rt_head) == key->domain
        && IPNET_ROUTE_GET_VR(rt_head) == key->vr;
}


/*
 *===========================================================================
 *                    ipnet_route_policy_hash_key
 *===========================================================================
 * Description: Calculates a hash value for the search key.
 * Parameters:  key - A search key.
 * Returns:     Hash value.
 *
 */
IP_STATIC unsigned
ipnet_route_policy_hash_key(struct Ipnet_route_table_key *key)
{
    return ((key->domain << 3) ^ (~key->vr)) + ((key->vr * 7) ^ ((-key->domain) >> 1));
}


/*
 *===========================================================================
 *                    ipnet_route_policy_hash_obj
 *===========================================================================
 * Description: Calculates a hash value for the policy rule head.
 * Parameters:  rt_head - A route table head.
 * Returns:     Hash value.
 *
 */
IP_STATIC unsigned
ipnet_route_policy_hash_obj(Ipnet_route_policy_rule_head *pr_head)
{
    struct Ipnet_route_table_key k;

    k.domain = pr_head->domain;
    k.vr     = pr_head->vr;
    return ipnet_route_policy_hash_key(&k);
}


/*
 *===========================================================================
 *                    ipnet_route_policy_hash_cmp
 *===========================================================================
 * Description: Checks if the key matches the route table head.
 * Parameters:  pr_head - A route policy table head.
 *              key - A search key.
 * Returns:     IP_TRUE if the key matches the element.
 *
 */
IP_STATIC Ip_bool
ipnet_route_policy_hash_cmp(Ipnet_route_policy_rule_head *pr_head, struct Ipnet_route_table_key *key)
{
    return pr_head->domain == key->domain && pr_head->vr == key->vr;
}


/*
 *===========================================================================
 *                 ipnet_route_create_af_link_gateway
 *===========================================================================
 * Description: Creates a gateway entry for a IPNET_RTF_LLINFO route.
 * Parameters:  dl - The gateway address.
 *              netif - The interface that going to be used.
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_create_af_link_gateway(struct Ip_sockaddr_dl *dl, Ipnet_netif *netif)
{
    IPCOM_SA_LEN_SET(dl, sizeof(struct Ip_sockaddr_dl));
    dl->sdl_family = IP_AF_LINK;
    dl->sdl_index  = (Ip_u16)netif->ipcom.ifindex;
    dl->sdl_nlen   = (Ip_u8) 0;
    dl->sdl_alen   = (Ip_u8) netif->ipcom.link_addr_size;
    dl->sdl_type   = (Ip_u8) netif->ipcom.type;
}


/*
 *===========================================================================
 *                    ipnet_route_u32_align_size
 *===========================================================================
 * Description: Align the size to 4 byte boundary
 * Parameters:  size - The size to align
 * Returns:     The aligned size
 *
 */
IP_STATIC Ip_u32
ipnet_route_u32_align_size(int size)
{
    return (size + sizeof(Ip_u32) - 1) & ~(sizeof(Ip_u32) - 1);
}


/*
 *===========================================================================
 *                    ipnet_route_u32_align
 *===========================================================================
 * Description: Add the offset and align to nearest 32-bit boundary.
 * Parameters:  ptr - The pointer to offset and align (must be 32-aligned to begin with).
 *              octet_offset - The offset to move the pointer.
 * Returns:     The aligned pointer.
 *
 */
IP_STATIC void *
ipnet_route_u32_align(void *ptr, int octet_offset)
{
    return (Ip_u32 *) ptr + (octet_offset + sizeof(Ip_u32) - 1) / sizeof(Ip_u32);
}


/*
 *===========================================================================
 *                   ipnet_route_policy_get_rule_head
 *===========================================================================
 * Description: Returns the head of the policy routing rule list.
 * Parameters:  domain - The domain the rules must be located in.
 *              vr - The virtual router the rule should apply to.
 * Returns:     The head of the policy rule list.
 *
 */
IP_STATIC Ipnet_route_policy_rule_head *
ipnet_route_policy_get_rule_head(int domain, Ip_u16 vr)
{
    struct Ipnet_route_table_key k;

    if (ipnet->policy_routing_rules == IP_NULL)
        return IP_NULL;

    k.domain = domain;
    k.vr     = vr;
    return ipcom_hash_get(ipnet->policy_routing_rules, &k);
}


/*
 *===========================================================================
 *                    ipnet_route_get_table_id
 *===========================================================================
 * Description: Returns the table ID stored in the first policy rule that
 *              matches the parameters.
 * Parameters:  domain - The routing domain (IP_AF_INET or IP_AF_INET6)
 *              vr  - The virtual router to use.
 *              rps - The selector for the domain.
 * Returns:     0 >= table ID, <0 = error code.
 *
 */
IP_STATIC int
ipnet_route_get_table_id(int                                    domain,
                         Ip_u16                                 vr,
                         IP_CONST Ipnet_route_policy_selector   *rps,
                         Ipnet_route_policy_rule                **pmatched_rule)
{
    Ipnet_route_policy_rule_head *head;
    Ipnet_route_policy_rule      *r;

    if (*pmatched_rule == IP_NULL)
    {
        head = ipnet_route_policy_get_rule_head(domain, vr);
        if (head == IP_NULL)
            return IPCOM_ROUTE_TABLE_DEFAULT;
        r = IPCOM_LIST_FIRST(&head->list);
    }
    else if ((*pmatched_rule)->rule.last)
        r = IP_NULL;
    else
        r = IPCOM_LIST_NEXT(&(*pmatched_rule)->list);

    for (; r != IP_NULL; r = IPCOM_LIST_NEXT(&r->list))
    {
        /* All rule matching works like this for each field:
           - if the field is mask in the rule -> always match
           - if the field is not masked in the rule and is
             specified as selector by the caller and the fields match -> match
           - else -> no match
           The rule is only used if all fields matches.
        */
        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_DS)
            && (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_DS)
                || r->rule.ds != rps->ds))
            continue;

        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_SCOPE))
        {
#ifdef IPCOM_USE_INET
            if (domain == IP_AF_INET)
            {
                Ip_u32  addr_type;
                if (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_DADDR))
                    continue;

                addr_type = ipnet_ip4_get_addr_type(IP_GET_32ON8(rps->daddr), vr, IP_NULL);
                if (r->rule.scope != addr_type)
                    continue;
            }
#endif
#ifdef IPCOM_USE_INET6
            if (domain == IP_AF_INET6)
            {
                if (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_SCOPE)
                    || r->rule.scope != rps->scope)
                    continue;
            }
#endif
        }

        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_PROTO)
            && (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_PROTO)
                || r->rule.proto != rps->proto))
            continue;

        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_FLOW)
            && (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_FLOW)
                || r->rule.flow != rps->flow))
            continue;

        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_IFINDEX)
            && (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_IFINDEX)
                || r->rule.ifindex != rps->ifindex))
            continue;

        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_PKTFLAGS)
            && (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_PKTFLAGS)
                || ((rps->pkt_flags & r->rule.pkt_mask) != r->rule.pkt_result)))
            continue;

        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_SADDR))
        {
            if (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_SADDR))
                continue;

            if (!ipnet_route_key_cmp(&r->rule.saddr, rps->saddr, r->rule.saddr_prefixlen))
                continue;
        }

        if (IP_BIT_ISSET(r->rule.mask, IPNET_PR_RULE_DADDR))
        {
            if (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_DADDR))
                continue;

            if (!ipnet_route_key_cmp(&r->rule.daddr, rps->daddr, r->rule.daddr_prefixlen))
                continue;
        }

        *pmatched_rule = r;
        return r->rule.table;
    }

    /* No more rules to match, return the default table */
    *pmatched_rule = IP_NULL;
    return IPCOM_ROUTE_TABLE_DEFAULT;
}


/*
 *===========================================================================
 *                    ipnet_route_set_rtab
 *===========================================================================
 * Description: Sets the routing table to represent the specified domain,
 *              virtual router and policy index.
 * Parameters:  domain - The routing domain (IP_AF_INET or IP_AF_INET6)
 *              vr - The virtual router index.
 *              table - The table ID.
 *              rt_head - The head of the routing table.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_set_rtab(int domain, Ip_u16 vr, Ip_u32 table, Ipcom_route *rt_head)
{
    struct Ipnet_route_table_key k;

    /* Convert to something sane */
    table = IPCOM_ROUTE_TABLE_GET(table);

    /* Sanity check */
    if (!IPCOM_ROUTE_TABLE_IS_DEFAULT(table)
        && ipnet_route_is_virtual_router_valid(vr) == IP_FALSE)
    {
        IPCOM_LOG1(NOTICE,
                   "cannot add new route table to virtual router %u since the router has not been created",
                   vr);
        return -IP_ERRNO_ENXIO;
    }

    IPNET_ROUTE_SET_FAMILY(rt_head, domain);
    IPNET_ROUTE_SET_VR(rt_head, vr);
    IPNET_ROUTE_SET_TABLE(rt_head, table);

    if (ipnet->rtabs == IP_NULL)
    {
        ipnet->rtabs = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_route_table_hash_obj,
                                      (Ipcom_hash_key_func) ipnet_route_table_hash_key,
                                      (Ipcom_hash_cmp_func) ipnet_route_table_hash_cmp);
        if (ipnet->rtabs == IP_NULL)
            return -IP_ERRNO_ENOMEM;
    }

    k.domain = domain;
    k.table  = table;
    k.vr     = vr;
    if (ipcom_hash_get(ipnet->rtabs, &k))
        return -IP_ERRNO_EEXIST;

    switch (ipcom_hash_add(ipnet->rtabs, rt_head))
    {
    case IPCOM_SUCCESS:
        return 0;
    default:
        return -IP_ERRNO_ENOMEM;
    }
}


/*
 *===========================================================================
 *                    ipnet_route_get
 *===========================================================================
 * Description: Get the entry that exactly match the key.
 * Parameters:  domain - The domain to use.
 *              vr - The virtual router to use.
 *              table - The route table ID.
 *              key - The search key.
 *              rtab_param - The route table used to look for the route entry
 *              rt_param - The route entry that matched.
 * Returns:     IPNET_ROUTE_PERFECT_MATCH = perfect match,
 *              IPNET_ROUTE_MASK_MATCH = mask match,
 *              <0 = error code.
 *
 */
IP_STATIC int
ipnet_route_get(int domain,
                Ip_u16 vr,
                Ip_u32 table,
                IP_CONST void *key,
                Ipcom_route **rtab_param,
                Ipnet_route_entry **rt_param)
{
    Ipnet_route_entry *rt;
    Ipcom_route       *rtab;
    Ip_bool            perfect_match;
    int                ret;

    ret = ipnet_route_get_rtab(domain, vr, table, &rtab);
    if (ret < 0)
    {
        *rtab_param = IP_NULL;
        return ret;
    }
    *rtab_param = rtab;

    rt = (Ipnet_route_entry*) ipcom_route_lookup(rtab, key, &perfect_match);
    if (rt == IP_NULL)
        return -IP_ERRNO_ENOTFOUND;

    *rt_param = rt;
    return perfect_match ? IPNET_ROUTE_PERFECT_MATCH : IPNET_ROUTE_MASK_MATCH;
}


/*
 *===========================================================================
 *                    ipnet_route_find_netif
 *===========================================================================
 * Description: Finds the network interface to use when sending packets
 *              along this route.
 * Parameters:  rt - A route.
 * Returns:     IP_TRUE if a usable network interface was found,
 *              IP_FALSE otherwise.
 *
 */
IP_STATIC Ip_bool
ipnet_route_find_netif(Ipnet_route_entry *rt)
{
    Ipnet_netif *netif;
    Ip_u32       i;

    IPNET_NETIF_FOR_EACH_ON_VR(netif, IPNET_ROUTE_GET_VR(rt->head), i)
    {
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK)
            || IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
            continue;

        if ((IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_MCAST_RO)
             && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_MULTICAST))
            || (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_BCAST_RO)
                && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_BROADCAST)))
        {
            rt->netif = netif;
            IP_BIT_CLR(rt->hdr.flags, IPNET_RTF_REJECT);
            IP_BIT_SET(rt->hdr.flags, IPNET_RTF_UP);
            return IP_TRUE;
        }
    }
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_route_replace_node
 *===========================================================================
 * Description: Replaces a route that is in the radix trie with a new one
 *              that has the same key.
 * Parameters:  [in,out] new_rt - The route to insert into the radix tree.
 *              [in] old_rt - The route that will be removed from the radix
 *              tree.
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_replace_node(Ipnet_route_entry *new_rt,
                         Ipnet_route_entry *old_rt)
{
    Ipcom_route_entry *internal = old_rt->hdr.parent;

    /* Insert the new route in the radix tree */
    new_rt->hdr.parent = internal;
    new_rt->widen      = IP_NULL;
    new_rt->prev       = IP_NULL;

    if (old_rt->narrow && old_rt->narrow != new_rt)
    {
        new_rt->narrow        = old_rt->narrow;
        new_rt->narrow->widen = new_rt;
    }
    if (old_rt->next && old_rt->next != new_rt)
    {
        new_rt->next       = old_rt->next;
        new_rt->next->prev = new_rt;
    }

    old_rt->next   = IP_NULL;
    old_rt->prev   = IP_NULL;
    old_rt->widen  = IP_NULL;
    old_rt->narrow = IP_NULL;

    if (internal->on == &old_rt->hdr)
        internal->on = &new_rt->hdr;
    else
    {
        ip_assert(internal->off == &old_rt->hdr);
        internal->off = &new_rt->hdr;
    }

    /* Remove all references to the old mask */
    while (internal)
    {
        if (old_rt->hdr.mask == internal->mask)
            internal->mask = IP_NULL;
        internal = internal->parent;
    }

    /* Initialize internal nodes with the new mask */
    ipcom_route_init_subtree_mask(&new_rt->hdr);
}


/*
 *===========================================================================
 *                       ipnet_route_unhook
 *===========================================================================
 * Description: Removes a route entry from the linked list of routes with
 *              the same key, however it does remove the entry from the radix
 *              tree if this is the last entry.
 * Parameters:  rt - The route entry to unhook.
 * Returns:     IP_TRUE if this was the last entry in the list.
 *
 */
IP_STATIC Ip_bool
ipnet_route_unhook(Ipnet_route_entry *rt)
{
    Ip_bool is_last_entry_unhooked;

    is_last_entry_unhooked = (rt->widen == IP_NULL && rt->narrow == IP_NULL
                              && rt->prev == IP_NULL && rt->next == IP_NULL);

    if (!is_last_entry_unhooked)
    {
        if (rt->widen == IP_NULL && rt->prev == IP_NULL)
        {
            /* This is the first entry in the list of entries with the
               same key */
            if (rt->next)
                ipnet_route_replace_node(rt->next, rt);
            else
            {
                ip_assert(rt->narrow != IP_NULL);
                ipnet_route_replace_node(rt->narrow, rt);
            }
        }
        else if (rt->prev)
        {
            /* Not the first entry in the list of entries with the
               same prefixlen */
            rt->prev->next = rt->next;
            if (rt->next)
                rt->next->prev = rt->prev;
        }
        else if (rt->next)
        {
            /* First entry in the list of entries with the same
               prefixlen, but the entry is not the entry with the
               least specific mask. */
            rt->next->widen  = rt->widen;
            rt->next->narrow = rt->narrow;
            rt->next->prev   = IP_NULL;

            rt->widen->narrow = rt->next;
            if (rt->narrow)
                rt->narrow->widen = rt->next;
        }
        else
        {
            /* This is the only element with this prefixlen */
            rt->widen->narrow = rt->narrow;
            if (rt->narrow)
                rt->narrow->widen = rt->widen;
        }

        rt->narrow = IP_NULL;
        rt->widen  = IP_NULL;
        rt->next   = IP_NULL;
        rt->prev   = IP_NULL;
    }

#ifdef IPNET_USE_NETLINKSOCK
    rt->netlink_pid = (Ip_pid_t)0;
#endif

    return is_last_entry_unhooked;
}

/*
 *===========================================================================
 *                    ipcom_route_walk_tree
 *===========================================================================
 * Description: Calls a callback function for each element in the route tree.
 * Parameters:  rtab - The route tree to use
 *              cb - Callback function to that will be called for each route entry.
 *              data - User data that will be passed each time the callback
 *              function is called.
 * Returns:
 *
 */
IP_PUBLIC void
ipnet_route_walk_tree(Ipcom_route *rtab,
                      Ipcom_route_walk_cb cb,
                      void *data)
{
    Ipnet_route_entry *current;
    Ipnet_route_entry *next;
    Ipnet_route_entry *next_rt1;
    Ipnet_route_entry *next_rt2;

    current = (Ipnet_route_entry *)ipcom_route_first_entry(rtab);
    while (current != IP_NULL)
    {
        next = (Ipnet_route_entry *)ipcom_route_next_entry(&current->hdr);

        /* Go through the NEXT list */
        for (next_rt1 = current->next; next_rt1 != IP_NULL; next_rt1 = next_rt2)
        {
            next_rt2 = next_rt1->next;
            if (cb(&next_rt1->hdr, data))
            {
                /* Can't be last, we've still got the list head */
                ipnet_route_unhook(next_rt1);
                ipnet_route_notify_func(rtab, &next_rt1->hdr, IPCOM_ROUTE_CODE_REMOVE);
            }
        }

        /* Go through the NARROW list */
        for (next_rt1 = current->narrow; next_rt1 != IP_NULL; next_rt1 = next_rt2)
        {
            next_rt2 = next_rt1->narrow;
            if (cb(&next_rt1->hdr, data))
            {
                /* Can't be last, we've still got the list head */
                ipnet_route_unhook(next_rt1);
                ipnet_route_notify_func(rtab, &next_rt1->hdr, IPCOM_ROUTE_CODE_REMOVE);
            }
        }

        /* Do the list head */
        if (cb(&current->hdr, data))
        {
            /* May be last */
            if (!ipnet_route_unhook(current))
                ipnet_route_notify_func(rtab, &current->hdr, IPCOM_ROUTE_CODE_REMOVE);
            else
                ipcom_route_remove(rtab,  &current->hdr);
        }
        current = next;
    }
}


/*
 *===========================================================================
 *                    ipnet_route_insert
 *===========================================================================
 * Description: Inserts 'rt' into the routing table 'rtab'.
 * Parameters:  [in] rtab - The route table this should be added to.
 *              [in,out] rt - The route entry to add.
 *              [in,out] duplicate_rt - The route entry that has the same key,
 *              but different mask, as 'rt' and/or different gateway.
 *              [in] report - Set to IP_FALSE to suppress route messages
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_insert(Ipcom_route *rtab,
                   Ipnet_route_entry *rt,
                   Ipnet_route_entry *duplicate_rt,
                   Ip_bool report)
{
    int rt_prefixlen;
    int dup_rt_prefixlen;

    rt->head       = duplicate_rt->head;
    rt->hdr.parent = duplicate_rt->hdr.parent;
    rt->hdr.on     = duplicate_rt->hdr.on;
    rt->hdr.off    = duplicate_rt->hdr.off;
    rt->hdr.flags |= duplicate_rt->hdr.flags & ~IPCOM_ROUTE_FLAG_MASK;

    rt_prefixlen = ipcom_mask_to_prefixlen(rt->hdr.mask, rt->head->bit_count);
    dup_rt_prefixlen = ipcom_mask_to_prefixlen(duplicate_rt->hdr.mask, duplicate_rt->head->bit_count);

    if (dup_rt_prefixlen > rt_prefixlen
        || (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_PREF)
            && report))
    {
        if (duplicate_rt->widen == IP_NULL)
        {
            /* The route with the smallest prefixlen must be in the radix tree */
            ipnet_route_replace_node(rt, duplicate_rt);
            /* Ignore the preferred flag to avoid infinite recursion */
            IP_BIT_SET(duplicate_rt->hdr.flags, IPNET_RTF_X_IGNORE_PREF);
            ipnet_route_insert(rtab, duplicate_rt, rt, IP_FALSE);
            IP_BIT_CLR(duplicate_rt->hdr.flags, IPNET_RTF_X_IGNORE_PREF);
        }
        else
        {
            /* The new route has a smaller prefixlen and must be placed
               in-front of the duplicate */
            rt->narrow        = duplicate_rt;
            rt->widen         = duplicate_rt->widen;
            rt->widen->narrow = rt;
            duplicate_rt->widen = rt;
        }
    }
    else
    {
        /* The new route is not inserted first in the list */
        for (;;)
        {
            Ipnet_route_entry *narrow_rt = duplicate_rt->narrow;

            if (dup_rt_prefixlen == rt_prefixlen)
            {
                /* The duplicate and this route has the same mask */
                if ((IP_BIT_ISSET(duplicate_rt->hdr.flags, IPNET_RTF_LLINFO)
                     && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO)
                     && rt->metrics.rmx_hopcount == duplicate_rt->metrics.rmx_hopcount)
                    || rt->metrics.rmx_hopcount < duplicate_rt->metrics.rmx_hopcount)
                {
                    /* 'rt' will be inserted first in the list */
                    if (duplicate_rt->widen == IP_NULL)
                    {
                        /* 'rt' will be the new node that is placed in
                           the Patricia tree */
                        ipnet_route_replace_node(rt, duplicate_rt);
                        ipnet_route_insert(rtab, duplicate_rt, rt, IP_FALSE);
                        break;
                    }
                    else
                    {
                        /* Put 'rt' as the head of the next/prev list,
                           moving 'duplicate_rt' to the 2:nd position
                           in that list. */
                        rt->next   = duplicate_rt;
                        rt->widen  = duplicate_rt->widen;
                        rt->narrow = duplicate_rt->narrow;

                        rt->widen->narrow = rt;
                        if (rt->narrow)
                            rt->narrow->widen = rt;

                        duplicate_rt->prev   = rt;
                        duplicate_rt->widen  = IP_NULL;
                        duplicate_rt->narrow = IP_NULL;
                        break;
                    }
                }
                else
                {
                    /* 'rt' will be inserted in the next/prev list,
                       but not as the new head. */
                    while (duplicate_rt->next
                           && (rt->metrics.rmx_hopcount > duplicate_rt->next->metrics.rmx_hopcount
                               || (rt->metrics.rmx_hopcount == duplicate_rt->next->metrics.rmx_hopcount
                                   && IP_BIT_ISFALSE(duplicate_rt->next->hdr.flags, IPNET_RTF_LLINFO)
                                   && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_PREF))))
                        duplicate_rt = duplicate_rt->next;

                    /* Insert 'rt' after 'duplicate_rt' */
                    if (duplicate_rt->next)
                    {
                        rt->next       = duplicate_rt->next;
                        rt->next->prev = rt;
                    }
                    rt->prev = duplicate_rt;
                    duplicate_rt->next = rt;
                    break;
                }
            }

            if (narrow_rt == IP_NULL
                || (dup_rt_prefixlen = ipcom_mask_to_prefixlen(narrow_rt->hdr.mask,
                                                               narrow_rt->head->bit_count)) > rt_prefixlen)
            {
                /* The next entry has larger prefix length, insert 'rt' before that one */
                if (duplicate_rt->narrow)
                    duplicate_rt->narrow->widen = rt;
                rt->narrow = duplicate_rt->narrow;
                rt->widen  = duplicate_rt;
                duplicate_rt->narrow = rt;
                break;
            }
            duplicate_rt = narrow_rt;
        }
    }

    if (report)
        ipnet_route_notify_func(rtab, &rt->hdr, IPCOM_ROUTE_CODE_ADD);
}


/*
 *===========================================================================
 *                   ipnet_route_get_rt_with_largest_prefixlen
 *===========================================================================
 * Description: Returns the route with the largest prefixlen that still matches
 *              the key. If more than 1 route has the same prefixlen in the
 *              list then the first is returned.
 * Parameters:  [in] rt_param - The route found in the radix tree.
 *              [in] key - The search key used in the lookup
 *              [in] flags - IPNET_RTL_FLAG_xxx flags.
 * Returns:     The entry with the largest prefixlen still matching the key.
 *
 */
IP_STATIC Ipnet_route_entry *
ipnet_route_get_rt_with_largest_prefixlen(Ipnet_route_entry *rt_param,
                                          IP_CONST void *key,
                                          int flags)
{
    Ipnet_route_entry *rt;
    Ipnet_route_entry *next_rt;
    int                prefixlen;

    prefixlen = ipcom_mask_to_prefixlen(rt_param->hdr.mask,
                                        rt_param->head->bit_count);

    for (rt = rt_param; rt->narrow != IP_NULL; rt = rt->narrow)
    {
        next_rt = rt->narrow;
        if (ipcom_route_key_cmp(rt->head->bit_count, key, next_rt->hdr.key, next_rt->hdr.mask)
            == IPCOM_ROUTE_KEY_CMP_NO_MATCH)
        {
            /* The key would no longer match if the next entry was used */
            break;
        }

        prefixlen = ipcom_mask_to_prefixlen(next_rt->hdr.mask,
                                            next_rt->head->bit_count);
    }

    /* Prefer routes that is not link local, is UP and is using a network
       interface that is UP */
    while (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO)
           || (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_UP)
               && IP_BIT_ISSET(flags, IPNET_RTL_FLAG_DOWN_OK)))
    {
        if (rt->widen == IP_NULL
            || prefixlen != ipcom_mask_to_prefixlen(rt->widen->hdr.mask,
                                                    rt->widen->head->bit_count))
        {
            break;
        }
        rt = rt->widen;
    }
    return rt;
}


/*
 *===========================================================================
 *                ipnet_route_remove_all_cb_for_each_table
 *===========================================================================
 * Description: Callback uses when all routes are to be removed from an interface.
 * Parameters:  rtab - A route table head.
 *              d - domain and network interface to delete all routers for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_remove_all_cb_for_each_table(Ipcom_route *rtab, struct Ipnet_route_remove_all_cb *d)
{
    if (d->domain == IPNET_ROUTE_GET_FAMILY(rtab))
        ipcom_route_walk_tree_backwards(rtab,
                                        (Ipcom_route_walk_cb) ipnet_route_remove_all_cb_for_each_entry,
                                        &d->d);
}


/*
 *===========================================================================
 *                     ipnet_route_are_sockaddr_equal
 *===========================================================================
 * Description: Compares two socket addresses.
 * Parameters:  [in] sa1 - A socket address.
 *              [in] sa2 - Another socket address.
 * Returns:     IP_TRUE if the addresses are equal.
 *
 */
IP_STATIC Ip_bool
ipnet_route_are_sockaddr_equal(IP_CONST struct Ip_sockaddr *sa1,
                               IP_CONST struct Ip_sockaddr *sa2,
                               Ip_bool link_agnostic)
{
    if (sa1->sa_family != sa2->sa_family)
        return IP_FALSE;
    if (sa1->sa_family == IP_AF_LINK)
    {
        if (link_agnostic)
            return IP_TRUE;
        else
        {
            struct Ip_sockaddr_dl *dl1 = (struct Ip_sockaddr_dl *) sa1;
            struct Ip_sockaddr_dl *dl2 = (struct Ip_sockaddr_dl *) sa2;

            if (dl1->sdl_alen != dl2->sdl_alen || dl1->sdl_type != dl2->sdl_type)
                return IP_FALSE;

            return ipcom_memcmp(IP_SOCKADDR_DL_LLADDR(dl1), IP_SOCKADDR_DL_LLADDR(dl2), dl1->sdl_alen) == 0;
        }
    }
#ifdef IPMPLS
    if (sa1->sa_family == IP_AF_MPLS)
        return IP_TRUE;
#endif
#ifdef IPCOM_USE_INET
    if (sa1->sa_family == IP_AF_INET)
    {
        struct Ip_sockaddr_in *in1 = (struct Ip_sockaddr_in *) sa1;
        struct Ip_sockaddr_in *in2 = (struct Ip_sockaddr_in *) sa2;
        return ipcom_memcmp(&in1->sin_addr, &in2->sin_addr, sizeof(in1->sin_addr)) == 0;
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (sa1->sa_family == IP_AF_INET6)
    {
        struct Ip_sockaddr_in6 *in1 = (struct Ip_sockaddr_in6 *) sa1;
        struct Ip_sockaddr_in6 *in2 = (struct Ip_sockaddr_in6 *) sa2;
        return in1->sin6_scope_id == in2->sin6_scope_id
            && IP_IN6_ARE_ADDR_EQUAL(&in1->sin6_addr, &in2->sin6_addr);
    }
#endif /* IPCOM_USE_INET6 */
    return IP_FALSE;
}


/*
 *===========================================================================
 *                     ipnet_route_delete_cloned_cb
 *===========================================================================
 * Description: Removes a route entry from the specified routing table.
 * Parameters:  [in] rt - A route in the tree.
 *              [in] clone_rt - The route used as templated when the routes
 *              where cloned.
 * Returns:     IP_TRUE if 'rt' should be deleted.
 *
 */
IP_STATIC Ip_bool
ipnet_route_delete_cloned_cb(Ipnet_route_entry *rt, Ipnet_route_entry *clone_rt)
{
    return (clone_rt == IP_NULL && rt->rt_template != IP_NULL)
        || (clone_rt != IP_NULL && rt->rt_template == clone_rt);
}


/*
 *===========================================================================
 *                ipnet_route_delete_non_perm_cloned_cb
 *===========================================================================
 * Description: Checks if a route entry is a non permanent cloned route.
 * Parameters:  [in] rt - A route in the tree.
 *              [in] clone_rt - The route used as templated when the routes
 *              where cloned.
 * Returns:     IP_TRUE if 'rt' should be deleted.
 *
 */
IP_STATIC Ip_bool
ipnet_route_delete_non_perm_cloned_cb(Ipnet_route_entry *rt, Ipnet_route_entry *clone_rt)
{
    if (rt->metrics.rmx_expire == IPCOM_ADDR_INFINITE)
        /* Permanent entry */
        return IP_FALSE;
    return ipnet_route_delete_cloned_cb(rt, clone_rt);
}


/*
 *===========================================================================
 *                     ipnet_route_delete_dynamic_rt_cb
 *===========================================================================
 * Description: Removes a route entry using 'llinfo_rt' as gateway
 * Parameters:  rt - A route in the tree.
 *              llinfo_rt - The route entry that the dynamically created
 *              routes cannot use anymore.
 * Returns:     IP_TRUE if 'rt' should be deleted.
 *
 */
IP_STATIC Ip_bool
ipnet_route_delete_dynamic_rt_cb(Ipnet_route_entry *rt, Ipnet_route_entry *llinfo_rt)
{
    union Ip_sockaddr_union su;

    ipnet_route_key_to_sockaddr(IPNET_ROUTE_GET_FAMILY(llinfo_rt->head),
                                llinfo_rt->hdr.key,
                                &su.sa);

    return IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_DYNAMIC)
        && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY)
        && ipnet_route_are_sockaddr_equal(&su.sa, rt->gateway, IP_FALSE);
}


/*
 *===========================================================================
 *             ipnet_route_delete_entries_with_unreach_gw_cb
 *===========================================================================
 * Description: Determines if the passed route entry is still valid.
 * Parameters:  rt - A route entry.
 * Returns:     IP_TRUE if 'rt' has a gateway that is no longer directly
 *              reachable. IP_FALSE otherwise.
 *
 */
#ifdef IPNET_USE_SCAN_GW_AT_DELETE
IP_STATIC Ip_bool
ipnet_route_delete_entries_with_unreach_gw_cb(Ipnet_route_entry *rt, void *unused)
{
    void              *gw_addr;
    Ip_u32             scope_id = 0;
    Ipnet_route_entry *res;

    IPCOM_UNUSED_ARG(unused);

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_GATEWAY))
        return IP_FALSE;

    switch (IPNET_ROUTE_GET_FAMILY(rt->head))
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        gw_addr = &((struct Ip_sockaddr_in *)rt->gateway)->sin_addr;
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        gw_addr = &((struct Ip_sockaddr_in6 *)rt->gateway)->sin6_addr;
        scope_id = ((struct Ip_sockaddr_in6 *)rt->gateway)->sin6_scope_id;
        break;
#endif
    default:
        return IP_FALSE;
    }

    if (0 > ipnet_route_raw_lookup2(IPNET_ROUTE_GET_FAMILY(rt->head),
                                    IPNET_ROUTE_GET_VR(rt->head),
                                    IPNET_ROUTE_GET_TABLE(rt->head),
                                    IPNET_RTL_FLAG_DONTCLONE,
                                    gw_addr,
                                    scope_id,
                                    IP_NULL,
                                    IP_NULL,
                                    rt->netif->ipcom.ifindex,
                                    &res))
        return IP_TRUE;

    /*
     * If 'res' is reachable through a gateway then remove this entry
     * since a gateway must be directly reachable, not reachable
     * through another gateway.
     */
    return IP_BIT_ISTRUE(res->hdr.flags, IPNET_RTF_GATEWAY);
}
#endif /* IPNET_USE_SCAN_GW_AT_DELETE */

/*
 *===========================================================================
 *                    ipnet_route_key_unpack
 *===========================================================================
 * Description: Unpacks dst and scope ID from a key.
 * Parameters:  [in] domain - The domain for the key.
 *              [in] key - The key to unpack.
 *              [out] dst - Pointer to where the destion pointer will be stored.
 *              [out] scoped_is - Pointer to where the scope ID will be stored.
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_key_unpack(int domain,
                       IP_CONST void *key,
                       IP_CONST void **dst,
                       Ip_u32 *scope_id)
{
    *scope_id = 0;

    switch (domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        *dst = key;
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        *dst = &((struct Ipnet_ipv6_key *) key)->addr;
        if (ipnet_ip6_is_scoped_addr(*dst))
            *scope_id = ((struct Ipnet_ipv6_key *) key)->scope_id;
        break;
#endif /* IPCOM_USE_INET6 */
    default:
        IP_PANIC();
    }
}


/*
 *===========================================================================
 *                      ipnet_route_in6_addr_to_key
 *===========================================================================
 * Description: Creates a lookup key from a struct Ip_in6_addr addr.
 * Parameters:  addr - IPv6 address.
 *              scope_id - the scope ID of the address.
 *              key - buffer where the lookup key should be stored.
 * Returns:     The lookup key.
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC IP_CONST union Ipnet_route_key *
ipnet_route_in6_addr_to_key(IP_CONST void *dst,
                            Ip_u32 scope_id,
                            union Ipnet_route_key *key)
{
    struct Ip_in6_addr *dst6 = (struct Ip_in6_addr *) dst;

    IPNET_IP6_SET_ADDR(&key->ipv6.addr, dst6);
    key->ipv6.scope_id = (ipnet_ip6_is_scoped_addr(dst6) ? scope_id : 0);

    return key;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_route_lookup_link_local
 *===========================================================================
 * Description: Returns the link local route that matches the specified key.
 * Parameters:  domain - The address domain to return a route for.
 *              vr - The virtual router to use.
 *              flags - IPNET_RTL_FLAG_xxx flags.
 *              key - The search key.
 *              ifindex - The interface the returned route must use or 0
 *              if any interface will do.
 *              rtp - Will hold the link local route if the call is successful
 * Returns:     IPNET_ROUTE_PERFECT_MATCH if successful
 *              <0 = error code.
 *
 */
IP_STATIC int
ipnet_route_lookup_link_local(int domain,
                              Ip_u16 vr,
                              int flags,
                              IP_CONST void *key,
                              Ip_u32 ifindex,
                              Ipnet_route_entry **rtp)
{
    Ipnet_route_entry *rt;

    rt = *rtp;
    while (rt != IP_NULL)
    {
        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO)
            || (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_GATEWAY)))
        {
            if (IP_BIT_ISFALSE(flags, IPNET_RTL_FLAG_DONTCLONE)
                && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING))
                /* Let ipnet_route_add_clone_rt() add a perfect match clone */
                break;

            *rtp = rt;
            return ipcom_memcmp(key, rt->hdr.key, rt->head->octet_count) == 0
                ? IPNET_ROUTE_PERFECT_MATCH
                : IPNET_ROUTE_MASK_MATCH;
        }
        rt = rt->next;
    }

    if (IP_BIT_ISSET(flags, IPNET_RTL_FLAG_DONTCLONE))
        return -IP_ERRNO_EHOSTUNREACH;

    return ipnet_route_add_cloned_rt(domain, vr, key, ifindex, rtp);
}


/*
 *===========================================================================
 *                    ipnet_route_timeout_cb
 *===========================================================================
 * Description: Handler for route timeouts.
 * Parameters:  rt - The route that has timed out.
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_timeout_cb(Ipnet_route_entry *rt)
{
    if (rt->timedout_cb != IP_NULL)
    {
        /* Entry has expired, call the timeout callback */
        ipnet_route_lock(rt);
        rt->timedout_cb(rt, IP_TRUE);
        if (ipnet_route_unlock(rt))
            /* Entry has been deleted */
            return;
    }

    if (ipnet_route_has_expired(rt))
    {
        /* Entry has expired */
        (void)ipnet_route_delete2(IPNET_ROUTE_GET_FAMILY(rt->head),
                                  IPNET_ROUTE_GET_VR(rt->head),
                                  IPNET_ROUTE_GET_TABLE(rt->head),
                                  rt->hdr.key,
                                  rt->hdr.mask,
                                  rt->gateway,
                                  rt->netif ? rt->netif->ipcom.ifindex : 0,
                                  0,
                                  0,
                                  IP_FALSE);
    }
    /* else: the callback extended the lifetime */
}


/*
 *===========================================================================
 *                ipnet_route_dont_route_timeout_cb
 *===========================================================================
 * Description: Handler for "don't route" route entry timeouts.
 * Parameters:  rt - The route that has timed out.
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_dont_route_timeout_cb(Ipnet_route_entry *rt)
{
    if (rt->timedout_cb != IP_NULL)
    {
        int domain = IP_AF_INET;

        if (IP_BIT_ISSET(rt->iflags, IPNET_RTI_FLAG_CLEAN))
            /* Just do the cleanups the 2:nd time */
            rt->timedout_cb(rt, IP_FALSE);
        else
        {
            /* Normal timeout operation, NDP and ARP will send the request here */
            ipnet_route_lock(rt);
            rt->timedout_cb(rt, IP_TRUE);
            if (ipnet_route_unlock(rt))
                /* Entry has been deleted */
                return;
            IP_BIT_SET(rt->iflags, IPNET_RTI_FLAG_CLEAN);
#ifdef IPCOM_USE_INET6
            if (ipnet->ip6.dont_route_list == IPNET_ROUTE_DONT_ROUTE_GET_HEAD(rt))
                domain = IP_AF_INET6;
#endif
            if (ipnet_timeout_schedule(1000 * ipnet_sysvar_netif_get_as_int(domain,
                                                                            rt->netif,
                                                                            "BaseReachableTime",
                                                                            1),
                                       (Ipnet_timeout_handler) ipnet_route_dont_route_timeout_cb,
                                       rt,
                                       IP_NULL) == 0)
                return;
        }
    }

    IPNET_ROUTE_DONT_ROUTE_GET_HEAD(rt) = rt->next;
    if (rt->prev != IP_NULL)
        rt->prev->next = rt->next;
    if (rt->next != IP_NULL)
        rt->next->prev = rt->prev;
    (void)ipnet_route_unlock(rt);
}


/*
 *===========================================================================
 *                ipnet_route_init_dont_route_entry
 *===========================================================================
 * Description: Creates and do basic initializations of a "don't route" entry.
 * Parameters:  elem_size - The size of the route entry.
 *              netif - The interface this route will use.
 *              no_ndp - IP_TRUE if this entry should not generate ARP/NDP
 * Returns:     The new entry or IP_NULL of out of memory
 *
 */
IP_STATIC Ipnet_route_entry *
ipnet_route_init_dont_route_entry(Ipnet_netif *netif, int elem_size, Ip_bool no_ndp)
{
    Ipnet_route_entry     *rt;
    struct Ip_sockaddr_dl *dl_gw;

    /* Create a new route entry */
    rt = ipcom_calloc(1, elem_size);
    if (rt == IP_NULL)
        return IP_NULL;

    if (ipnet_timeout_schedule(0,
                               (Ipnet_timeout_handler) ipnet_route_dont_route_timeout_cb,
                               rt,
                               &rt->tmo) != 0)
    {
        (void)ipnet_route_unlock(rt);
        return IP_NULL;
    }

    rt->data    = (rt + 1);
    dl_gw = (struct Ip_sockaddr_dl *) ((Ip_u8*) rt->data + IPNET_ROUTE_ENTRY_DATA_LEN);
    rt->gateway = (struct Ip_sockaddr *) dl_gw;
    rt->hdr.key = (dl_gw + 1);

    rt->hdr.flags = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_LLINFO
        | IPNET_RTF_X_DONT_ROUTE | IPNET_RTF_X_NEIGH_ACK;
    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK) || no_ndp)
        rt->hdr.flags |= IPNET_RTF_DONE;
    if (no_ndp)
        IP_BIT_SET(rt->iflags, IPNET_RTI_FLAG_CLEAN);
    ipnet_route_create_af_link_gateway(dl_gw, netif);
    rt->netif = netif;
    return rt;
}


/*
 *===========================================================================
 *                   ipnet_route_ip4_is_mcast_or_bcast
 *===========================================================================
 * Description: Determines if an IPv4 address is a multicast or broadcast.
 * Parameters:  dst - An IPv4 address
 *              ifindex - The ifindex this should be a broadcast on.
 *              rtab - The route table index to use.
 * Returns:     IP_TRUE if the address is a multicast or broadcast address
 *              IP_FALSE otherwise.
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC Ip_bool
ipnet_route_ip4_is_mcast_or_bcast(IP_CONST struct Ip_in_addr *dst, Ip_u32 ifindex, Ip_u16 vr)
{
    struct Ip_in_addr d;
    int               addr_type;

    d.s_addr = IP_GET_32ON16(dst);

    if (IP_IN_CLASSD(d.s_addr))
        return IP_TRUE;

    addr_type = ipnet_ip4_get_addr_type2(d.s_addr, ipnet_if_indextonetif(vr, ifindex));
    return addr_type == IPNET_ADDR_TYPE_BROADCAST
        || addr_type == IPNET_ADDR_TYPE_NETBROADCAST;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                   ipnet_route_get_capable_netif
 *===========================================================================
 * Description: Returns a network interface that is capable of sending
 *              multicast and/or broadcast.
 * Parameters:  vr - The virtual router the interface must be assigned to.
 *              multicast_capable - IP_TRUE if the interface must be
 *                                   multicast capable.
 *              broadcast_capable - IP_TRUE if the interface must be
 *                                  broadcast capable.
 * Returns:     An interface that has the specificed capabilities or
 *              IP_NULL if no such interface exist.
 *
 */
IP_STATIC Ipnet_netif *
ipnet_route_get_capable_netif(Ip_u16 vr,
                              Ip_bool multicast_capable,
                              Ip_bool broadcast_capable)
{
    Ipnet_netif *netif;
    unsigned     i;

    /* Use the first multicast or broadcast enabled interface */
    IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
    {
        if ((multicast_capable == IP_FALSE || IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_MULTICAST))
            && (broadcast_capable == IP_FALSE || IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_BROADCAST)))
            return netif;
    }
    return IP_NULL;
}


#if IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG || defined(IPNET_DEBUG)
/*
 *===========================================================================
 *                    ipnet_route_addr_to_str
 *===========================================================================
 * Description: Returns a string representation of an IP address.
 * Parameters:  domain - the IP domain of the address.
 *              addr - an address or IP_NULL.
 *              str - buffer to store the string representation.
 *              len - length of the of the 'str' buffer.
 * Returns:
 *
 */
IP_STATIC char *
ipnet_route_addr_to_str(int domain,
                        void *addr,
                        char *str,
                        Ip_size_t len)
{
    if (addr == IP_NULL)
        ipcom_strncpy(str, "-", len);
    else
        ipcom_inet_ntop(domain, addr, str, len);

    return str;
}


/*
 *===========================================================================
 *                    ipnet_route_sockaddr_to_str
 *===========================================================================
 * Description: Returns a string representation of a socket address.
 * Parameters:  sa - a socket address or IP_NULL.
 *              str - buffer to store the string representation.
 *              len - length of the of the 'str' buffer.
 * Returns:
 *
 */
IP_STATIC char *
ipnet_route_sockaddr_to_str(struct Ip_sockaddr *sa,
                            char *str,
                            Ip_size_t len)
{
    union Ip_sockaddr_union *su = (void *) sa;
    struct Ip_sockaddr_dl   *dl;

    ipcom_strncpy(str, "-", len);
    if (sa != IP_NULL)
    {
        switch (sa->sa_family)
        {
#ifdef IPCOM_USE_INET
        case IP_AF_INET:
            return ipnet_route_addr_to_str(IP_AF_INET,
                                           &su->sin.sin_addr,
                                           str,
                                           len);
#endif
#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            return ipnet_route_addr_to_str(IP_AF_INET6,
                                           &su->sin6.sin6_addr,
                                           str,
                                           len);
#endif
        case IP_AF_LINK:
            dl = (void *) sa;

            if (dl->sdl_alen > 0)
            {
                Ip_ssize_t x;
                Ip_ssize_t slen;
                int        byte;

                for (x = 0; x < dl->sdl_alen; x++)
                {
                    slen = len - x * 3;
                    if (slen < 0)
                        break;
                    byte = ((Ip_u8*) IP_SOCKADDR_DL_LLADDR(dl))[x];
                    ipcom_snprintf(&str[x * 3], slen, "%02x%s", byte,
                                   x + 1 == dl->sdl_alen ? "\0" : ":");
                }
            }
            else if (dl->sdl_index != 0)
                ipcom_snprintf(str, len, "link#%d", dl->sdl_index);
            break;
        default:
            break;
        }
    }
    return str;
}
#endif /* IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG || defined(IPNET_DEBUG) */


/*
 *===========================================================================
 *                    ipnet_route_to_string
 *===========================================================================
 * Description: Creates a string representation of a route entry.
 * Parameters:  rt - The route entry to stringify.
 * Returns:
 *
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_route_to_string(Ipnet_route_entry *rt, char *buf, Ip_size_t buf_len)
{
    union Ip_sockaddr_union mask;
    int                     offset;

    switch (IPNET_ROUTE_GET_FAMILY(rt->head))
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        if (rt->hdr.mask != IP_NULL)
        {
            IPCOM_SA_LEN_SET(&mask.sin, sizeof(struct Ip_sockaddr_in));
            mask.sin.sin_family = IP_AF_INET;
            ipcom_memcpy(&mask.sin.sin_addr, rt->hdr.mask, sizeof(struct Ip_in_addr));
        }
        offset = ipcom_snprintf(buf, buf_len, "IPv4 route %s",
                                ipcom_inet_ntop(IP_AF_INET, rt->hdr.key, ipnet->log_buf, sizeof(ipnet->log_buf)));
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        if (rt->hdr.mask != IP_NULL)
            ipnet_ip6_create_sockaddr(&mask.sin6, rt->hdr.mask, rt->netif->ipcom.ifindex);
        offset = ipcom_snprintf(buf, buf_len, "IPv6 route %s",
                                ipcom_inet_ntop(IP_AF_INET6, rt->hdr.key, ipnet->log_buf, sizeof(ipnet->log_buf)));
        break;
#endif /* IPCOM_USE_INET6 */
    default:
        return 0;
    }
    if (rt->hdr.mask != IP_NULL)
        offset += ipcom_snprintf(&buf[offset], buf_len - offset, "/%d", ipcom_sockaddr_to_prefixlen(&mask.sa));

    if (rt->gateway != IP_NULL)
    {
        switch (rt->gateway->sa_family)
        {
#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            (void)ipcom_snprintf(ipnet->log_buf, buf_len, "%s",
                                 ipcom_inet_ntop(IP_AF_INET6, rt->hdr.key, ipnet->log_buf, sizeof(ipnet->log_buf)));
            if (IP_IN6_IS_ADDR_LINK_LOCAL((struct Ip_in6_addr *)rt->hdr.key))
            {
                ipcom_strcat(ipnet->log_buf, "%%");
                ipcom_strcat(ipnet->log_buf, rt->netif->ipcom.name);
            }
            break;
#endif /* IPCOM_USE_INET6 */
#ifdef IPCOM_USE_INET
        case IP_AF_INET:
#endif /* IPCOM_USE_INET */
        case IP_AF_LINK:
            (void)ipnet_route_sockaddr_to_str(rt->gateway,
                                              ipnet->log_buf,
                                              sizeof(ipnet->log_buf));
            offset += ipcom_snprintf(&buf[offset], buf_len - offset, "  gw %s", ipnet->log_buf);
            break;
        default:
            break;
        }
    }

    return offset;
}
#endif /* IPNET_DEBUG */


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_route_lock
 *===========================================================================
 * Description: Adds a lock to the route to prevent it from being freed
 *              until it is unlocked.
 * Parameters:  rt - A route entry.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_lock(Ipnet_route_entry *rt)
{
    ip_assert(rt->locks != 0xffff);
    ++rt->locks;
}


/*
 *===========================================================================
 *                    ipnet_route_unlock
 *===========================================================================
 * Description: Removes a lock from the route and frees it if not more
 *              locks are on this route.
 * Parameters:  rt - A route entry.
 * Returns:     IP_TRUE if the entry has been deleted, IP_FALSE otherwise.
 *
 */
IP_GLOBAL Ip_bool
ipnet_route_unlock(Ipnet_route_entry *rt)
{
    if (rt->locks-- == 0)
    {
        ipcom_free(rt);
        return IP_TRUE;
    }
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_route_has_expired
 *===========================================================================
 * Description: Returns if this route entry has expired or not.
 * Parameters:  rt - A route entry.
 * Returns:     IP_TRUE if the entry has expired.
 *
 */
IP_GLOBAL Ip_bool
ipnet_route_has_expired(Ipnet_route_entry *rt)
{
    return (rt->metrics.rmx_expire != IPCOM_ADDR_INFINITE
	    && ((rt->tmo == IP_NULL) || (rt->tmo->pq_index == 0)));
}


/*
 *===========================================================================
 *                    ipnet_route_create_llinfo_entry
 *===========================================================================
 * Description: Clone 'rt_template' and make the new key a prefect match to
 *              'key'.
 * Parameters:  domain - The routing domain (IP_AF_INET or IP_AF_INET6)
 *              vr - The virtual router to use.
 *              key - The search key that triggered the clone.
 *              rt_template - The template for the new route or IP_NULL.
 *              netif - The network interface to use or IP_NULL if the
 *                      netif from the template should be used.
 *                      NOTE: rt_template and netif must not both be IP_NULL
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_create_llinfo_entry(int domain,
                                Ip_u16 vr,
                                IP_CONST void *key,
                                Ipnet_route_entry *rt_template,
                                Ipnet_netif *netif)
{
    struct Ipnet_route_add_param param;
    struct Ipnet_rt_metrics      metrics;
    union Ipnet_link_addr_union {
        struct Ip_sockaddr_dl    dl;
    } link_addr;
    int                          ret;

    if (rt_template == IP_NULL)
        ipcom_memset(&metrics, 0, sizeof(metrics));
    else
    {
        if (IP_BIT_ISSET(rt_template->iflags, IPNET_RTI_FLAG_CLEAN))
            /* The template are beeing deleted, no new clones are allowed */
            return -IP_ERRNO_EINVAL;

        if (ipnet->cloned_route_count
            >= (unsigned) ipcom_sysvar_get_as_int0("ipnet.route.MaxCloneCount", 1000))
        {
            /* Too many cloned routes, remove all cloned routes. The
               cloned routes will be recreated when needed */
            ipnet->cloned_route_count = 0;
            ipcom_route_walk_tree(rt_template->head,
                                  (Ipcom_route_walk_cb) ipnet_route_delete_non_perm_cloned_cb,
                                  IP_NULL);
        }

        ip_assert(IP_BIT_ISSET(rt_template->hdr.flags, IPNET_RTF_CLONING));
        ++rt_template->use;
        netif = rt_template->netif;
        metrics = rt_template->metrics;
    }
    metrics.rmx_expire   = 0;
    metrics.rmx_hopcount = 10;

    /* Allocate a bigger buffer for 'link_addr' if this assert trigger */
    ip_assert(sizeof(link_addr) - ip_offsetof(union Ipnet_link_addr_union, dl.sdl_data)
              >= netif->ipcom.link_addr_size); /*lint !e545 */
    ipcom_memset(&link_addr, 0, sizeof(struct Ip_sockaddr_dl));
    ipnet_route_create_af_link_gateway(&link_addr.dl, netif);

    ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
    param.domain       = domain;
    param.vr           = vr;
    param.table        = IPCOM_ROUTE_TABLE_DEFAULT;
    param.key          = key;
    param.netif        = netif;
    param.metrics      = &metrics;
    param.flags        = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_LLINFO;
    param.gateway      = (struct Ip_sockaddr *) &link_addr.dl;
    param.rt_template  = rt_template;
    param.no_ref_count = IP_TRUE;
    ret = ipnet_route_add(&param);

    if (ret == -IP_ERRNO_EEXIST)
        /* EEXIST can occur when a route is cloned from a template with policy ID != 0,
           this is perfectly OK */
        return 0;

    if (ret >= 0 && rt_template != IP_NULL)
        ++ipnet->cloned_route_count;

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_route_get_rtab
 *===========================================================================
 * Description: Verifies that 'domain' and 'vr' is sane and
 *              returns the pointer to the route table and/or the next timeout.
 * Parameters:  domain - The routing domain (IP_AF_INET or IP_AF_INET6)
 *              vr - The virtual router to use.
 *              table - The ID of the route table.
 *              rtab - Will hold the route table.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_get_rtab(int domain, Ip_u16 vr, Ip_u32 table, Ipcom_route **rtab)
{
    struct Ipnet_route_table_key k;

    if (ipnet->rtabs == IP_NULL)
        return -IP_ERRNO_ESRCH;

    k.domain = domain;
    k.table  = IPCOM_ROUTE_TABLE_GET(table);
    k.vr     = vr;
    *rtab = ipcom_hash_get(ipnet->rtabs, &k);
    if (*rtab == IP_NULL)
        /* The the route table does not exist */
        return -IP_ERRNO_ENXIO;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_route_apply_mask
 *===========================================================================
 * Description: Applies the mask to the 'net' variable.
 * Parameters:  net - The network.
 *              mask - The mask to apply.
 *              bitcount - The length of the address in bits.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_apply_mask(void *net, IP_CONST void *mask, int bitcount)
{
    int     i;
    Ip_u16  bit_n;
    Ip_u16 *net16;
    Ip_u16 *mask16;

    net16 = (Ip_u16*) net;
    mask16 = (Ip_u16*) mask;
    for (i = 0; i < bitcount; i++)
    {
        bit_n = (Ip_u16) ip_htons(0x8000 >> (i % 16));
        if (IP_BIT_ISFALSE(mask16[i >> 4], bit_n))
            IP_BIT_CLR(net16[i >> 4], bit_n);
    }
}


/*
 *===========================================================================
 *                    ipnet_route_cache_invalidate
 *===========================================================================
 * Description: Invalidate all route caches.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_cache_invalidate(void)
{
    ipnet->route_cache_id++;
    if (ipnet->route_cache_id == 0)
        ipnet->route_cache_id++;
}


/*
 *===========================================================================
 *                    ipnet_route_notify_func
 *===========================================================================
 * Description: Callback that is used each time the status of the route
 *              table changes.
 * Parameters:  rtab - The route table that has changed.
 *              entry - The entry in the route table that was affected.
 *              code - Code that describes the event.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_notify_func(Ipcom_route *rtab, Ipcom_route_entry *entry, int code)
{
    Ipnet_route_entry *rt = (Ipnet_route_entry *) entry;

    IPCOM_UNUSED_ARG(rtab);

    if (rt->rt_template == IP_NULL || code != IPCOM_ROUTE_CODE_ADD)
        ipnet_route_cache_invalidate();

    if (code == IPCOM_ROUTE_CODE_REMOVE)
    {
#if IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG
        int domain = IPNET_ROUTE_GET_FAMILY(rtab);


#ifdef IPCOM_USE_INET
        if (domain == IP_AF_INET)
        {
            char                   dst_str[IP_INET_ADDRSTRLEN];
            char                   mask_str[IP_INET_ADDRSTRLEN];
            struct Ip_sockaddr_in *in_gw = (struct Ip_sockaddr_in *) rt->gateway;

            IPCOM_LOG8(DEBUG,
                       "IPv4: deleting route %s mask=%s gw=%s netif=%s flags=%s (0x%lx) vr=%u table=%u",
                       ipnet_route_addr_to_str(IP_AF_INET, rt->hdr.key, dst_str, sizeof(dst_str)),
                       ipnet_route_addr_to_str(IP_AF_INET, rt->hdr.mask, mask_str, sizeof(mask_str)),
                       ipnet_route_sockaddr_to_str(rt->gateway, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       rt->netif ? rt->netif->ipcom.name : "-",
                       ipnet_cmd_rt_flags_to_str_short(rt->hdr.flags, IP_TRUE),
                       (Ip_u32)rt->hdr.flags,
                       IPNET_ROUTE_GET_VR(rtab),
                       IPNET_ROUTE_GET_TABLE(rtab));
            (void)dst_str;
            (void)mask_str;
            (void)in_gw;
        }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if (domain == IP_AF_INET6)
        {
            char                    dst_str[IP_INET6_ADDRSTRLEN];
            char                    mask_str[IP_INET6_ADDRSTRLEN];
            struct Ip_sockaddr_in6 *in_gw = (struct Ip_sockaddr_in6 *) rt->gateway;

            IPCOM_LOG8(DEBUG,
                       "IPv6: deleting route %s mask=%s gw=%s netif=%s flags=%s (0x%lx) vr=%u table=%u",
                       ipnet_route_addr_to_str(IP_AF_INET6, rt->hdr.key, dst_str, sizeof(dst_str)),
                       ipnet_route_addr_to_str(IP_AF_INET6, rt->hdr.mask, mask_str, sizeof(mask_str)),
                       ipnet_route_sockaddr_to_str(rt->gateway, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       rt->netif ? rt->netif->ipcom.name : "-",
                       ipnet_cmd_rt_flags_to_str_short(rt->hdr.flags, IP_TRUE),
                       (Ip_u32)rt->hdr.flags,
                       IPNET_ROUTE_GET_VR(rtab),
                       IPNET_ROUTE_GET_TABLE(rtab));
            (void)dst_str;
            (void)mask_str;
            (void)in_gw;
        }
#endif /* IPCOM_USE_INET6 */
#endif /* IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG */
#ifdef IPMPLS
        if (rt->gateway && rt->gateway->sa_family == IP_AF_MPLS)
            ipnet_mpls_shortcut_route_clear(rt);
#endif /* IPMPLS */
    }

    if (code == IPCOM_ROUTE_CODE_ADD)
    {
#ifdef IPCOM_USE_MIB2_NEW
        rt->msec_changed = ipnet->msec_now;
#endif

        if (IP_BIT_ISFALSE(entry->flags, IPNET_RTF_X_SILENT))
        {
            IPNET_ROUTESOCK(ipnet_routesock_rt_add(rt));
            IPNET_NETLINKSOCK(ipnet_rtnetlink_route_add(rt));
        }
        IP_BIT_CLR(entry->flags, IPNET_RTF_X_SILENT);
    }
    else if (code == IPCOM_ROUTE_CODE_REMOVE || code == IPCOM_ROUTE_CODE_DUPLICATE)
    {
        ipnet_timeout_cancel(rt->tmo);
        if (rt->timedout_cb != IP_NULL)
            rt->timedout_cb(rt, IP_FALSE);
        if (IP_BIT_ISFALSE(entry->flags, IPNET_RTF_X_SILENT))
        {
            IPNET_ROUTESOCK(ipnet_routesock_rt_delete(rt));
            IPNET_NETLINKSOCK(ipnet_rtnetlink_route_del(rt));
        }
        rt->pid = ~0;
        (void)ipnet_route_unlock(rt);
    }
}


/*
 *===========================================================================
 *                    ipnet_route_add_cloned_rt
 *===========================================================================
 * Description: Adds a cloned route that will match the key and the ifindex.
 *              NOTE: Cloning will only take place in the route table with
 *                    policy index == 0.
 * Parameters:  domain - The address domain to return a route for.
 *              vr - The virtual router table to use.
 *              flags - IPNET_RTL_FLAG_xxx flags.
 *              key - The search key.
 *              ifindex - 0 if the stack should select a interface or the
 *                        index of the interface to use.
 *              rtp - Will hold the link local route if the call is successful
 * Returns:     IPNET_ROUTE_PERFECT_MATCH if successful
 *              <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_add_cloned_rt(int domain,
                          Ip_u16 vr,
                          IP_CONST void *key,
                          int ifindex,
                          Ipnet_route_entry **rtp)
{
    union {
#ifdef IPCOM_USE_INET
        struct Ip_in_addr  in4;
#endif
#ifdef IPCOM_USE_INET6
        struct Ip_in6_addr in6;
#endif
    } net;
    Ipnet_netif       *netif = IP_NULL;
    Ipnet_route_entry *template_rt;
    Ipnet_route_entry *rt;
    IP_CONST void     *dst = IP_NULL;
    Ip_u32             scope_id;
    int                ret;
    Ip_u32             i;

    /* Find the network route matching this key */
    ipnet_route_key_unpack(domain, key, &dst, &scope_id);
    ipcom_memset(&net, 0, sizeof(net));

    IPNET_NETIF_FOR_EACH_OR_SPECIFIC_ON_VR(netif, vr, ifindex, i)
    {
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_NOARP | IP_IFF_POINTOPOINT | IP_IFF_LOOPBACK))
            continue;

#ifdef IPCOM_USE_INET
        if (domain == IP_AF_INET)
        {
            Ipnet_ip4_addr_entry *addr;
            IP_CONST int          bit_size = sizeof(net.in4) * 8;

            for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
            {
                if (ipcom_route_key_cmp(bit_size, dst, &addr->ipaddr_n, &addr->netmask_n)
                    != IPCOM_ROUTE_KEY_CMP_NO_MATCH)
                {
                    ipcom_memcpy(&net.in4, &addr->ipaddr_n, sizeof(net.in4));
                    ipnet_route_apply_mask(&net.in4, &addr->netmask_n, bit_size);
                    goto found;
                }
            }

        }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if (domain == IP_AF_INET6)
        {
            struct Ip_in6_addr    mask;
            Ipnet_ip6_addr_entry *addr;
            IP_CONST int          bit_size = sizeof(net.in6) * 8;

            for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
            {
                if (addr->prefixlen == 0)
                    continue;
                ipcom_memset(&mask, 0, sizeof(mask));
                ipnet_route_create_mask(&mask, addr->prefixlen);
                if (ipcom_route_key_cmp(bit_size, dst, &addr->addr, &mask)
                    != IPCOM_ROUTE_KEY_CMP_NO_MATCH)
                {
                    IPNET_IP6_SET_ADDR(&net.in6, &addr->addr);
                    ipnet_route_apply_mask(&net.in6, &mask, bit_size);
                    goto found;
                }
            }
        }
#endif /* IPCOM_USE_INET6 */
    }

    return -IP_ERRNO_EHOSTUNREACH;

found:
    ret = ipnet_route_raw_lookup2(domain,
                                  vr,
                                  IPCOM_ROUTE_TABLE_DEFAULT,
                                  0,
                                  &net,
                                  scope_id,
                                  IP_NULL,
                                  IP_NULL,
                                  netif->ipcom.ifindex,
                                  &template_rt);
    if (ret != IPNET_ROUTE_PERFECT_MATCH)
        return -IP_ERRNO_EHOSTUNREACH;

    /* Try to find a network route that can be used for cloning */
    while (IP_BIT_ISFALSE(template_rt->hdr.flags, IPNET_RTF_CLONING))
    {
        rt = template_rt;
        do
        {
            /* First walk down the list of entries using the same key and mask */
            template_rt = template_rt->next;
            if (template_rt == IP_NULL)
            {
                template_rt = rt;
                break;
            }
        } while (IP_BIT_ISFALSE(template_rt->hdr.flags, IPNET_RTF_CLONING));

        if (IP_BIT_ISFALSE(template_rt->hdr.flags, IPNET_RTF_CLONING))
        {
            /* Move to entries using a less specific mask */
            template_rt = template_rt->widen;
            if (template_rt == IP_NULL)
                return -IP_ERRNO_EHOSTUNREACH;
        }
    }

    ret = ipnet_route_create_llinfo_entry(domain, vr, key, template_rt, IP_NULL);
    if (ret < 0)
        return ret;

    ret = ipnet_route_raw_lookup2(domain,
                                  vr,
                                  IPCOM_ROUTE_TABLE_DEFAULT,
                                  0,
                                  dst,
                                  scope_id,
                                  IP_NULL,
                                  IP_NULL,
                                  ifindex,
                                  &rt);
    if (ret < 0)
        return ret;

    while (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO))
    {
        rt = rt->next;
        if (rt == IP_NULL)
            return -IP_ERRNO_EHOSTUNREACH;
    }
    *rtp = rt;
    return IPNET_ROUTE_PERFECT_MATCH;
}


/*
 *===========================================================================
 *                    ipnet_route_add
 *===========================================================================
 * Description: Adds a route entry to the specified routing tabel.
 * Parameters:  [in] param - Paramters needed to add the new route.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_add(IP_CONST struct Ipnet_route_add_param *param)
{
#ifdef IPCOM_USE_INET6
    Ip_bool            in6_scoped_addr = IP_FALSE;
#endif
    Ipcom_route       *rtab = IP_NULL;
    Ip_bool            create_mask;
    Ip_size_t          entry_size;
    Ipnet_route_entry *leaf;
    Ipnet_route_entry *rt;
    Ipnet_route_entry *duplicate_leaf = IP_NULL;
    void              *ptr;  /* Points the where the next octet should be written */
    int                ret;
    int                flags;
    Ipnet_netif       *netif;
    union Ipnet_route_key  keybuf;
    IP_CONST union Ipnet_route_key *key;

    ip_assert(param != IP_NULL);
    ip_assert(param->key != IP_NULL);

    ret = ipnet_route_get_rtab(param->domain, param->vr, param->table, &rtab);
    if (ret < 0)
    {
        IPCOM_LOG4(DEBUG, "Failed to add route on domain=%d, VR=%u, table=%u: %s",
                   param->domain,
                   param->vr,
                   param->table,
                   ipcom_strerror(-ret));
        return ret;
    }

    if (param->netmask == IP_NULL)
        key = param->key;
    else
    {
        ipcom_memcpy(&keybuf, param->key, rtab->octet_count);
        ipnet_route_apply_mask(&keybuf, param->netmask, rtab->bit_count);
        key = &keybuf;
    }
    flags = param->flags;
    netif = param->netif;

#ifdef IPCOM_USE_INET
#if IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG
    if (param->domain == IP_AF_INET)
    {
        char                   dst_str[IP_INET_ADDRSTRLEN];
        char                   mask_str[IP_INET_ADDRSTRLEN];
        char                   gw_str[IP_INET_ADDRSTRLEN];
        struct Ip_sockaddr_in *in_gw = (struct Ip_sockaddr_in *) param->gateway;

        ipcom_snprintf(ipnet->log_buf,
                       sizeof(ipnet->log_buf),
                       "IPv4: adding route %s mask=%s gw=%s netif=%s flags=%s (0x%x) vr=%d table=%u",
                       ipcom_inet_ntop(IP_AF_INET, key, dst_str, sizeof(dst_str)),
                       param->netmask ? ipcom_inet_ntop(IP_AF_INET, param->netmask, mask_str, sizeof(mask_str)) : "-",
                       param->gateway ? (param->gateway->sa_family == IP_AF_INET ?
                                         ipcom_inet_ntop(IP_AF_INET, &in_gw->sin_addr, gw_str, sizeof(gw_str)) :
                                         "link") : "-",
                       netif ? netif->ipcom.name : "-",
                       ipnet_cmd_rt_flags_to_str_short(flags, IP_TRUE),
                       (unsigned int)flags,
                       (int)param->vr,
                       (unsigned int)param->table);
    }
#endif /* IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG */
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (param->domain == IP_AF_INET6)
    {
        ipnet_ip6_extract_scoped_addr(param->gateway, netif ? netif->ipcom.ifindex : 0);

#if IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG
        {
            char dst_str[IP_INET6_ADDRSTRLEN];
            char mask_str[IP_INET6_ADDRSTRLEN];
            char gw_str[IP_INET6_ADDRSTRLEN];
            char scope_str[16];
            struct Ip_sockaddr_in6 *in6_gw = (struct Ip_sockaddr_in6 *)param->gateway;

            if (in6_gw && in6_gw->sin6_family == IP_AF_INET6 && in6_gw->sin6_scope_id)
                ipcom_sprintf(scope_str, "%%%ld", in6_gw->sin6_scope_id);
            else
                *scope_str = '\0';

            ipcom_snprintf(ipnet->log_buf,
                           sizeof(ipnet->log_buf),
                           "IPv6: adding route %s mask=%s gw=%s%s netif=%s flags=%s (0x%x) vr=%d table=%u",
                           ipcom_inet_ntop(IP_AF_INET6, key, dst_str, sizeof(dst_str)),
                           param->netmask ? ipcom_inet_ntop(IP_AF_INET6, param->netmask, mask_str, sizeof(mask_str)) : "-",
                           param->gateway ? (param->gateway->sa_family == IP_AF_INET6 ?
                                             ipcom_inet_ntop(IP_AF_INET6, &in6_gw->sin6_addr, gw_str, sizeof(gw_str)) :
                                             "link") : "-",
                           scope_str,
                           netif ? netif->ipcom.name : "-",
                           ipnet_cmd_rt_flags_to_str_short(flags, IP_TRUE),
                           (unsigned int)flags,
                           (int)param->vr,
                           (unsigned int)param->table);
        }
#endif /* IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_DEBUG */
    }
#endif /* IPCOM_USE_INET6 */

    ret = ipnet_route_get(param->domain, param->vr, param->table, key, &rtab, &rt);
    if (ret < 0 && ret != -IP_ERRNO_ENOTFOUND)
    {
        IPCOM_LOG3(DEBUG, "%s --- FAIL (%s : %d)", ipnet->log_buf, ipcom_strerror(-ret), ret);
        return ret;
    }

    if (netif && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_NOARP))
        /* Do not use cloning if ARP and NDP is disabled */
        IP_BIT_CLR(flags, IPNET_RTF_CLONING);

    if (ipcom_mask_to_prefixlen(param->netmask, rtab->bit_count) == rtab->bit_count)
        create_mask = IP_FALSE;
    else
        create_mask = IP_TRUE;

    if (ret == IPNET_ROUTE_PERFECT_MATCH)
    {
        IP_CONST void  *dst = IP_NULL;
        Ip_u32 scope_id;

        /* An entry with a matching key already exists,
           check if mask, interface and gateway also match */
        duplicate_leaf = rt;
        ipnet_route_key_unpack(param->domain, key, &dst, &scope_id);
        ret = ipnet_route_raw_lookup2(param->domain,
                                      param->vr,
                                      param->table,
                                      IPNET_RTL_FLAG_DONTCLONE,
                                      dst,
                                      scope_id,
                                      create_mask ? param->netmask : IP_NULL,
                                      param->gateway,
                                      netif ? netif->ipcom.ifindex : 0,
                                      &rt);

        if (ret == IPNET_ROUTE_PERFECT_MATCH)
        {
            if (param->no_ref_count)
            {
                ret = -IP_ERRNO_EEXIST;
                IPCOM_LOG3(DEBUG, "%s --- FAIL (%s : %d)", ipnet->log_buf, ipcom_strerror(-ret), ret);
                return ret;
            }
            /* else: just increase the reference count */
            rt->ref_count++;
            IPCOM_LOG2(DEBUG, "%s (DUP, ref = %d)", ipnet->log_buf, rt->ref_count);
            return 0;
        }
        /* else: all fields did NOT match */
    }

#ifdef IPCOM_USE_INET6
    if (param->domain == IP_AF_INET6
        && ipnet_ip6_is_scoped_addr(&key->ipv6.addr))
    {
        in6_scoped_addr = IP_TRUE;
    }
#endif /* IPCOM_USE_INET6 */

    /* Calculate the size of the entry */
    entry_size = ipnet_route_u32_align_size(rtab->octet_count + sizeof(Ipnet_route_entry));
    if (param->gateway)
        entry_size += ipnet_route_u32_align_size(IPCOM_SA_LEN_GET(param->gateway));
    else if (netif && IP_BIT_ISSET(flags, IPNET_RTF_CLONING))
        entry_size += ipnet_route_u32_align_size(sizeof(struct Ip_sockaddr_dl));
    if (create_mask)
        entry_size += ipnet_route_u32_align_size(rtab->octet_count);
    if (IP_BIT_ISSET(flags, IPNET_RTF_LLINFO))
        entry_size += ipnet_route_u32_align_size(IPNET_ROUTE_ENTRY_DATA_LEN);
    if (param->rtinfo)
        entry_size += ipnet_route_u32_align_size(param->rtinfo->rt_info_size);

    /* Allocate memory for the radix tree leaf */
    leaf = ipcom_calloc(1, entry_size);
    if (leaf == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    /* Initialize the entry */
    leaf->head      = rtab;
    leaf->ref_count = 1;
    leaf->hdr.flags = flags;
    leaf->hdr.key   = (leaf + 1);
    ptr = ipnet_route_u32_align(leaf->hdr.key, rtab->octet_count);
    ipcom_memcpy(leaf->hdr.key, key, rtab->octet_count);

    if (param->gateway == IP_NULL)
    {
        if (IP_BIT_ISSET(flags, IPNET_RTF_GATEWAY))
        {
            /* Neither llinfo nor gateway flag can be set */
            IPCOM_LOG1(DEBUG, "%s  --- FAIL (bad flags)", ipnet->log_buf);
            ret = -IP_ERRNO_EINVAL;
            goto errout;
        }

        if (IP_BIT_ISFALSE(flags, IPNET_RTF_CLONING))
            leaf->gateway = IP_NULL;
        else if(netif)
        {
            struct Ip_sockaddr_dl  *dl;

            leaf->gateway = (struct Ip_sockaddr *) ptr;
            dl = (struct Ip_sockaddr_dl *)ptr;
            ptr = ipnet_route_u32_align(ptr, sizeof(struct Ip_sockaddr_dl));

            IPCOM_SA_LEN_SET(dl, sizeof(struct Ip_sockaddr_dl));
            dl->sdl_family = IP_AF_LINK;
            dl->sdl_index  = (Ip_u16)netif->ipcom.ifindex;
            dl->sdl_type   = (Ip_u8)netif->ipcom.type;
        }
    }
    else
    {
        Ipnet_route_entry *gw = IP_NULL;
        Ip_bool            find_outgoing_netif = (netif == IP_NULL);

        leaf->gateway = (struct Ip_sockaddr *) ptr;
        ptr = ipnet_route_u32_align(ptr, IPCOM_SA_LEN_GET(param->gateway));
        ipcom_memcpy(leaf->gateway, param->gateway, IPCOM_SA_LEN_GET(param->gateway));

        /* Loopback special case -> always reachable. */
        if (netif && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK))
            goto build_mask;

        switch (leaf->gateway->sa_family)
        {
#ifdef IPCOM_USE_INET
        case IP_AF_INET:
            if (find_outgoing_netif)
            {
                Ipnet_route_policy_selector         rps_grp;

                /* Only got the group field */
                rps_grp.fields      = IPNET_PR_RULE_PKTFLAGS | IPNET_PR_RULE_DADDR;
                rps_grp.pkt_flags   = 0;
                rps_grp.daddr       = key;

                /* Do a policy based lookup */
                ret = ipnet_route_lookup_ecmp(IP_AF_INET,
                                              param->vr,
                                              IPNET_RTL_FLAG_DONTCLONE,
                                              &((struct Ip_sockaddr_in *) leaf->gateway)->sin_addr,
                                              0,
                                              0,
                                              &rps_grp,
                                              &gw,
                                              IP_NULL);
            }
            break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            if (find_outgoing_netif)
            {
                /* Verify that the gateway is on-link */
                struct Ip_sockaddr_in6              *in6_gw = (struct Ip_sockaddr_in6 *) leaf->gateway;

                Ipnet_route_policy_selector         rps_grp;

                /* Only got the group field */
                rps_grp.fields      = IPNET_PR_RULE_PKTFLAGS | IPNET_PR_RULE_DADDR;
                rps_grp.pkt_flags   = 0;
                rps_grp.daddr       = key;

                /* Do a policy based lookup */
                ret = ipnet_route_lookup_ecmp(IP_AF_INET6,
                                              param->vr,
                                              IPNET_RTL_FLAG_DONTCLONE,
                                              &in6_gw->sin6_addr,
                                              in6_gw->sin6_scope_id,
                                              0,
                                              &rps_grp,
                                              &gw,
                                              IP_NULL);
            }
            break;
#endif /* IPCOM_USE_INET6 */
#ifdef IPMPLS
        case IP_AF_MPLS:
            /* Prepare MPLS pseudo-wire route */
            ret = ipnet_mpls_shortcut_route_setup(param->vr, leaf, &netif);

            if (ret < 0)
            {
                ret = -IP_ERRNO_ENXIO;
                goto errout;
            }
            find_outgoing_netif = IP_FALSE;
            break;
#endif /* IPMPLS */
        default:
            find_outgoing_netif = IP_FALSE;
            break;
        }

        if (find_outgoing_netif)
        {
            /* Figure out the outgoing interface from the gateway route */
            if (ret < 0)
            {
                IPCOM_LOG1(DEBUG, "%s  --- FAIL (unreach route)", ipnet->log_buf);
                ret = -IP_ERRNO_ENETUNREACH;
                goto errout;
            }

            ip_assert(gw != IP_NULL);
            ip_assert(gw->netif != IP_NULL);

            if (netif == IP_NULL)
            {
                if (IP_BIT_ISFALSE(gw->hdr.flags, IPNET_RTF_GATEWAY)
                    || (ret == IPNET_ROUTE_PERFECT_MATCH && IP_BIT_ISSET(gw->hdr.flags, IPNET_RTF_HOST))
                    || (ret == IPNET_ROUTE_MASK_MATCH && IP_BIT_ISSET(gw->hdr.flags, IPNET_RTF_CLONING)))
                    netif = gw->netif;
            }
        }
    }

 build_mask:
    if (!create_mask)
    {
        if (IP_BIT_ISFALSE(flags, IPNET_RTF_HOST))
        {
            /* Must be a host route since no mask is used */
            IPCOM_LOG1(DEBUG, "%s  --- FAIL (missing HOST flag)", ipnet->log_buf);
            ret = -IP_ERRNO_EINVAL;
            goto errout;
        }
        leaf->hdr.mask = IP_NULL;
        IP_BIT_CLR(leaf->hdr.flags, IPNET_RTF_MASK);
    }
    else
    {
        if (IP_BIT_ISTRUE(flags, IPNET_RTF_HOST)
            && ipcom_mask_to_prefixlen(param->netmask, rtab->bit_count) != rtab->bit_count)
        {
            /* Cannot be a host route since the mask isn't all 1:s */
            IPCOM_LOG1(DEBUG, "%s  --- FAIL (bad HOST flag)", ipnet->log_buf);
            ret = -IP_ERRNO_EINVAL;
            goto errout;
        }
        IP_BIT_SET(leaf->hdr.flags, IPNET_RTF_MASK);
        leaf->hdr.mask = ptr;
        ptr = ipnet_route_u32_align(ptr, rtab->octet_count);

        {
            const Ip_u8 Ip_all_ones[16] = {0xff, 0xff, 0xff, 0xff,
                                           0xff, 0xff, 0xff, 0xff,
                                           0xff, 0xff, 0xff, 0xff,
                                           0xff, 0xff, 0xff, 0xff
            };

            ipcom_memcpy(leaf->hdr.mask, param->netmask ? param->netmask : Ip_all_ones, rtab->octet_count);
        }
    }

    if (param->rtinfo)
    {
        leaf->rt_func_info = ptr;
        ptr = ipnet_route_u32_align(ptr, param->rtinfo->rt_info_size);
        ipcom_memcpy(leaf->rt_func_info, param->rtinfo, param->rtinfo->rt_info_size);
    }

    if (IP_BIT_ISSET(leaf->hdr.flags, IPNET_RTF_LLINFO))
        leaf->data  = ptr;


#ifdef IPCOM_USE_INET6
    if (param->domain == IP_AF_INET6 && !in6_scoped_addr)
    {
        struct Ipnet_ipv6_key *ipv6_key;

        /* Scope ID must be 0 for non-scoped addresses (or networks) */
        ipv6_key = (struct Ipnet_ipv6_key *) leaf->hdr.key;
        ipv6_key->scope_id = 0;
    }
#endif /* IPCOM_USE_INET6 */

    ipnet_route_set_lifetime(leaf, IPCOM_ADDR_INFINITE);
    if (IP_BIT_ISFALSE(flags, IPNET_RTF_X_MCAST_RO | IPNET_RTF_X_BCAST_RO))
    {
        if (IP_BIT_ISSET(flags, IPNET_RTF_REJECT | IPNET_RTF_BLACKHOLE)
            && IP_BIT_ISFALSE(flags, IPNET_RTF_GATEWAY)
            && netif == IP_NULL)
        {
            /* Allow rejcet/blackhole routes to be specified without gw or netif */
            leaf->netif = ipnet_loopback_get_netif(param->vr);
        }
        else
            leaf->netif = netif;

        /* Check interface */
        if (leaf->netif == IP_NULL)
        {
            IPCOM_LOG1(DEBUG, "%s  --- FAIL (no interface specifed)", ipnet->log_buf);
            ret = -IP_ERRNO_EINVAL;
            goto errout;
        }
    }

    leaf->pid         = param->pid;
    leaf->seq         = param->seq;
    leaf->rt_template = param->rt_template;

    /* Inherit FUNC info */
    if (leaf->rt_template != IP_NULL && leaf->rt_func_info == IP_NULL)
        leaf->rt_func_info = leaf->rt_template->rt_func_info;

#ifdef IPNET_USE_ROUTE_COOKIES
    if (param->cookie)
        ipcom_memcpy(&leaf->cookie, param->cookie, sizeof(Ipnet_rt_cookie));
#endif

    ret = 0;
    if (duplicate_leaf != IP_NULL)
        ipnet_route_insert(rtab, leaf, duplicate_leaf, IP_TRUE);
    else
    {
        if (ipcom_route_add(rtab, &leaf->hdr) != IPCOM_SUCCESS)
            ret = -IP_ERRNO_ENOMEM;
    }

    if (IP_BIT_ISSET(leaf->hdr.flags, IPNET_RTF_LLINFO))
    {
        switch (param->domain)
        {
#ifdef IPCOM_USE_INET
        case IP_AF_INET:
            if (netif->inet4_neigh_init)
                netif->inet4_neigh_init(leaf);
            break;
#endif
#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            ipnet_icmp6_neighbor_cache_init(leaf);
            break;
#endif
        }
    }

    if (ret == 0)
    {
        if (param->metrics != IP_NULL)
        {
            /* Setup route lifetime */
            ipcom_memcpy(&leaf->metrics, param->metrics, sizeof(struct Ipnet_rt_metrics));
            ipnet_route_set_lifetime(leaf, IP_GET_32ON16(&param->metrics->rmx_expire));
        }

        IPCOM_LOG1(DEBUG, "%s", ipnet->log_buf);
        return 0;
    }

    IPCOM_LOG3(DEBUG, "%s  --- FAIL (%s : %d)", ipnet->log_buf, ipcom_strerror(-ret), ret);

 errout:
    (void)ipnet_route_unlock(leaf);
    return ret;
}


/*
 *===========================================================================
 *                  ipnet_route_for_each_data_init
 *===========================================================================
 * Description: Initializes the route-delete-for-each-data structure.
 * Parameters:  d - The structure to initialize
 *              netif - The interface the to-be-deleted routes must point to,
 *                      IP_NULL means any network interface.
 *              gateway - The gateway the to-be-deleted routes must point to,
 *                      IP_NULL means any gateway.
 *              has_not_flags - IPNET_RTF_xxx flags that the to-be-deleted
 *                              routes must not have set.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_for_each_data_init(Ipnet_route_delete_for_each_data *d,
                               struct Ipnet_netif_struct *netif,
                               IP_CONST struct Ip_sockaddr *gateway,
                               Ip_u32 has_not_flags)
{
    d->netif         = netif;
    d->gateway       = gateway;
    d->has_not_flags = has_not_flags;
}


/*
 *===========================================================================
 *                  ipnet_route_remove_all_cb_for_each_entry
 *===========================================================================
 * Description: Callback used when all routes are to be removed from a specified
 *              interface.
 * Parameters:  [in] rt - The current route in the walk.
 *              [in] netif - The interface the route should be using to be removed.
 * Returns:     IP_TRUE - remove this route
 *              IP_FALSE - do not remove this route.
 *
 */
IP_GLOBAL Ip_bool
ipnet_route_remove_all_cb_for_each_entry(Ipnet_route_entry *rt,
                                         Ipnet_route_delete_for_each_data *d)
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

            if (rt->netif != d->netif)
                continue;
            if (d->gateway != IP_NULL
                && (rt->gateway == IP_NULL
                    || !ipnet_route_are_sockaddr_equal(d->gateway, rt->gateway, IP_FALSE)))
                continue;
            if (IP_BIT_ISSET(rt->hdr.flags, d->has_not_flags))
                continue;

            if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_MCAST_RO | IPNET_RTF_X_BCAST_RO))
            {
                rt->netif = IP_NULL;
                IP_BIT_CLR(rt->hdr.flags, IPNET_RTF_UP);
                continue;
            }

            /**/
            if (IP_BIT_ISSET(rt->iflags, IPNET_RTI_FLAG_ORPHAN))
            {
                if (IP_BIT_ISFALSE(rt->netif->ipcom.flags, IP_IFF_LOOPBACK))
                {
                    Ipnet_netif *n = ipnet_loopback_get_netif(rt->netif->vr_index);
                    if (n)
                    {
                        rt->netif = n;
                        continue;
                    }
                }
            }

            if (rt->narrow == IP_NULL && rt->widen == IP_NULL
                && rt->next == IP_NULL && rt->prev == IP_NULL &&
                IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_CLONING))
                /* No entries using the same key, let the IPCOM code remove this route */
                return IP_TRUE;

            (void)ipnet_route_delete2(IPNET_ROUTE_GET_FAMILY(rt->head),
                                      IPNET_ROUTE_GET_VR(rt->head),
                                      IPNET_ROUTE_GET_TABLE(rt->head),
                                      rt->hdr.key,
                                      rt->hdr.mask,
                                      rt->gateway,
                                      rt->netif->ipcom.ifindex,
                                      0,
                                      0,
                                      IP_FALSE);
        }
    }
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_route_delete
 *===========================================================================
 * Description: Removes a route entry from the specified routing table.
 * Parameters:  [in] domain - The route domain (IP_AF_INET or IP_AF_INET6).
 *              [in] vr - The virtual router to use (0 = default).
 *              [in] key - The key for this route, can be a host or a network.
 *              The IPv6 key also include (32-bit) scope ID.
 * Returns:     0 = perfect match, 1 = mask match, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_delete(int domain, Ip_u16 vr, IP_CONST void *key)
{
    return ipnet_route_delete2(domain, vr, IPCOM_ROUTE_TABLE_DEFAULT, key,
                               IP_NULL, IP_NULL, 0, 0, 0, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_route_delete2
 *===========================================================================
 * Description: Removes a route entry from the specified routing table.
 * Parameters:  [in] domain - The route domain (IP_AF_INET or IP_AF_INET6).
 *              [in] vr - The virtual router to use (0 = default).
 *              [in] table - The route table ID to delete from.
 *              [in] key - The key for this route, can be a host or a network.
 *              The IPv6 key also include (32-bit) scope ID.
 *              [in] mask - The mask for this route or IP_NULL.
 *              [in] gw - The gateway the mask must have or IP_NULL.
 *              [in] netif - The interface index for the route to remove or 0
 *              for any interface.
 *              [in] pid - The process ID of the deleter.
 *              [in] seq - The sequence number specified by the deleter.
 *              [in] use_ref_count - IP_TRUE if the reference counter should
 *              be used.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_delete2(int domain,
                    Ip_u16 vr,
                    int table,
                    IP_CONST void *key,
                    IP_CONST void *mask,
                    struct Ip_sockaddr *gw,
                    Ip_u32 ifindex,
                    Ip_pid_t pid,
                    int seq,
                    Ip_bool use_ref_count)
{
    int                ret;
    Ipcom_route       *rt_head;
    Ipnet_route_entry *del_rt;
    Ip_bool            is_last_entry_remove;
    IP_CONST void     *dst = IP_NULL;
    Ip_u32             scope_id;

#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
        ipnet_ip6_extract_scoped_addr(gw, ifindex);
#endif /* IPCOM_USE_INET6 */

    ipnet_route_key_unpack(domain, key, &dst, &scope_id);
    ret = ipnet_route_raw_lookup2(domain,
                                  vr,
                                  table,
                                  IPNET_RTL_FLAG_DONTCLONE | IPNET_RTL_FLAG_DOWN_OK,
                                  dst,
                                  scope_id,
                                  mask,
                                  gw,
                                  ifindex,
                                  &del_rt);
    if (ret != IPNET_ROUTE_PERFECT_MATCH)
    {
        /* Try again with link local flag since the link local route can be
           shadowed by a host route */
        ret = ipnet_route_raw_lookup2(domain,
                                      vr,
                                      table,
                                      IPNET_RTL_FLAG_DONTCLONE | IPNET_RTL_FLAG_DOWN_OK | IPNET_RTL_FLAG_LINK_LOCAL,
                                      dst,
                                      scope_id,
                                      mask,
                                      gw,
                                      ifindex,
                                      &del_rt);

        if (ret != IPNET_ROUTE_PERFECT_MATCH)
        {
            return -IP_ERRNO_ESRCH;
        }
    }

    if (use_ref_count && --del_rt->ref_count)
        return 0;

    rt_head = del_rt->head;
    is_last_entry_remove = ipnet_route_unhook(del_rt);

    del_rt->pid = pid;
    del_rt->seq = seq;

    if (IP_BIT_ISSET(del_rt->hdr.flags, IPNET_RTF_CLONING))
    {
        /* Set this bit to prevent creation of more clones */
        IP_BIT_SET(del_rt->iflags, IPNET_RTI_FLAG_CLEAN);
        if (IP_UNLIKELY(IPNET_ROUTE_GET_TABLE(rt_head) != IPCOM_ROUTE_TABLE_DEFAULT))
        {
            Ipcom_route *rtab;
            ret = ipnet_route_get_rtab(IPNET_ROUTE_GET_FAMILY(rt_head),
                                       IPNET_ROUTE_GET_VR(rt_head),
                                       IPCOM_ROUTE_TABLE_DEFAULT,
                                       &rtab);
            if (ret < 0)
                IP_PANIC();
            else
            {
                ipnet_route_walk_tree(rtab,
                                      (Ipcom_route_walk_cb)ipnet_route_delete_cloned_cb,
                                      del_rt);
            }
        }
        else
        {
            ipnet_route_walk_tree(rt_head,
                                  (Ipcom_route_walk_cb)ipnet_route_delete_cloned_cb,
                                  del_rt);
        }
    }

    if (IP_BIT_ISSET(del_rt->hdr.flags, IPNET_RTF_LLINFO))
    {
        /* Remove all dynamically created routes that using this route entry as gateway */
        ipcom_route_walk_tree(rt_head,
                              (Ipcom_route_walk_cb) ipnet_route_delete_dynamic_rt_cb,
                              del_rt);
    }

    if (!is_last_entry_remove)
    {
        /* This function will also free the memory for the route */
        ipnet_route_notify_func(rt_head, &del_rt->hdr, IPCOM_ROUTE_CODE_REMOVE);
        return 0;
    }

    ret = ipcom_route_remove(rt_head, &del_rt->hdr);
    /* Should not fail since the entry was received from ipcom_route_lookup() */
    ip_assert(ret == IPCOM_SUCCESS);

    if (ret != IPCOM_SUCCESS)
        return -IP_ERRNO_EINVAL;

#ifdef IPNET_USE_SCAN_GW_AT_DELETE
    /*
     * Go through the FIB and remove any entry that has a gateway that
     * isn't directly reachable anymore.
     */
    ipcom_route_walk_tree(rt_head,
                          (Ipcom_route_walk_cb) ipnet_route_delete_entries_with_unreach_gw_cb,
                          IP_NULL);
#endif /* IPNET_USE_SCAN_GW_AT_DELETE */

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_route_lookup
 *===========================================================================
 * Description: Returns the route matching the key, returns only routes that
 *              are UP and do not have the reject flag set.
 * Parameters:  domain - The route domain (IP_AF_INET or IP_AF_INET6).
 *              vr - The virtual router to use (0 = default).
 *              table - The route table (0 = default)
 *              flags - IPNET_RTL_FLAG_xxx flags.
 *              key - The destination for this route, can be a host or a
 *              network (the dst address + the scope ID is the key for IPv6).
 *              scope_id - The scope ID in host byte order (used only for IPv6).
 *              ifindex - The interface the returned route must use or 0
 *              if any interface will do.
 *              re - Will contain the pointer to the route entry if the function
 *              is successful.
 * Returns:     IPNET_ROUTE_PERFECT_MATCH = perfect match,
 *              IPNET_ROUTE_MASK_MATCH = mask match,
 *              <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_lookup(int domain,
                   Ip_u16 vr,
                   Ip_u32 table,
                   int flags,
                   IP_CONST void *dst,
                   Ip_u32 scope_id,
                   Ip_u32 ifindex,
                   Ipnet_route_entry **re)
{
    int                ret;
    Ipnet_route_entry *rt;

    ret = ipnet_route_raw_lookup(domain, vr, table, flags, dst, scope_id, ifindex, re);
    if (ret < 0)
    {
        *re = IP_NULL;
        return ret;
    }
    rt = *re;

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_UP))
    {
        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_HOST))
            return -IP_ERRNO_EHOSTUNREACH;
        return -IP_ERRNO_ENETUNREACH;
    }

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_REJECT))
        /* There is no really good error code for this... */
        return -IP_ERRNO_EACCES;

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING)
        && IP_BIT_ISFALSE(flags, IPNET_RTL_FLAG_DONTCLONE))
        return -IP_ERRNO_EADDRNOTAVAIL;

    return ret;
}


/*
 *===========================================================================
 *                   ipnet_route_lookup_ip4_dont_route
 *===========================================================================
 * Description: Does a lookup, but bypasses normal routing. The host
 *              beeing looked upped must be link local to this node.
 * Parameters:  neighbor_update - IP_TRUE if this lookup is done just to
 *              update a neighbor entry, no new route entry will be created
 *              in this case and no loopback route will be returned.
 *              vr - The virtual router to use (0 = default).
 *              key - The destination for this route.
 *              ifindex - The interface the returned route must use or 0
 *              if any interface will do.
 *              re - Will contain the pointer to the route entry if the function
 *              is successful.
 * Returns:      0 = entry found
 *              <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET
IP_GLOBAL int
ipnet_route_lookup_ip4_dont_route(Ip_bool neighbor_update,
                                  Ip_u16 vr,
                                  IP_CONST struct Ip_in_addr *dst,
                                  Ip_u32 ifindex,
                                  Ipnet_route_entry **re)
{
    Ipnet_ip4_addr_entry *addr;
    Ipnet_route_entry    *rt;
    Ip_u32                i;
    int                   best_prefixlen = 0;
    int                   addr_type;
    Ipnet_netif          *best_netif = IP_NULL;
    Ip_u32                d;
    Ip_bool               is_multicast;
    Ip_bool               is_broadcast;
    Ipnet_netif          *netif;

    d = IP_GET_32ON16(dst);
    netif = ipnet_if_indextonetif(vr, ifindex);
    addr_type = ipnet_ip4_get_addr_type(d,
                                        vr,
                                        netif);
    is_multicast = (IP_IN_CLASSD(d) || addr_type == IPNET_ADDR_TYPE_MULTICAST);
    is_broadcast = (addr_type == IPNET_ADDR_TYPE_BROADCAST
                    || addr_type == IPNET_ADDR_TYPE_NETBROADCAST);

    /* Check if the destination is reachable at all, this has to be done before
       looking for existing router entries to detect if the whole network
       has been deleted */
    if (neighbor_update == IP_FALSE && addr_type == IPNET_ADDR_TYPE_UNICAST)
        /* Packet beeing sent to one of the local addresses */
        best_netif = ipnet_loopback_get_netif(vr);
    else if (is_multicast || is_broadcast)
    {
        if (ifindex)
            best_netif = netif;
        else
            best_netif = ipnet_route_get_capable_netif(vr, is_multicast, is_broadcast);
    }
    else
    {
        IPNET_NETIF_FOR_EACH_OR_SPECIFIC_ON_VR(netif, vr, ifindex, i)
        {
            if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
                continue;
            for (addr = netif->inet4_addr_list;
                 addr != IP_NULL && addr->type == IPNET_ADDR_TYPE_UNICAST;
                 addr = addr->next)
            {
                if (ipcom_route_key_cmp(32, dst, &addr->ipaddr_n, &addr->netmask_n)
                    != IPCOM_ROUTE_KEY_CMP_NO_MATCH)
                {
                    int this_prefixlen = ipcom_mask_to_prefixlen(&addr->netmask_n, 32);

                    if (best_prefixlen < this_prefixlen)
                    {
                        best_prefixlen = this_prefixlen;
                        best_netif = netif;
                    }
                }
            }
        }
    }

    if (best_netif == IP_NULL)
        return -IP_ERRNO_EHOSTUNREACH;

    /* See if the entry already exists */
    for (rt = ipnet->ip4.dont_route_list;
         rt != IP_NULL;
         rt = rt->next)
    {
        if (((struct Ip_in_addr *) rt->hdr.key)->s_addr == IP_GET_32ON16(dst)
            && vr == rt->netif->vr_index
            && (ifindex == 0 || ifindex == rt->netif->ipcom.ifindex))
        {
            *re = rt;
            return 0;
        }
    }

    if (neighbor_update)
        return -IP_ERRNO_ESRCH;

    rt = ipnet_route_init_dont_route_entry(best_netif,
                                           sizeof(Ipnet_route_entry)
                                           + sizeof(struct Ip_in_addr)
                                           + sizeof(struct Ip_sockaddr_dl)
                                           + IPNET_ROUTE_ENTRY_DATA_LEN,
                                           is_multicast | is_broadcast);
    if (rt == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    ipcom_memcpy(rt->hdr.key, dst, sizeof(*dst));
    if (rt->netif->inet4_neigh_init)
        rt->netif->inet4_neigh_init(rt);

    if (ipnet->ip4.dont_route_list != IP_NULL)
    {
        rt->next = ipnet->ip4.dont_route_list;
        rt->next->prev = rt;

    }
    ipnet->ip4.dont_route_list = rt;
    IPNET_ROUTE_DONT_ROUTE_SET_HEAD(rt, ipnet->ip4.dont_route_list);

    *re = rt;
    return 0;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                   ipnet_route_lookup_ip6_dont_route
 *===========================================================================
 * Description: Does a lookup, but bypasses normal routing. The host
 *              beeing looked upped must be link local to this node.
 * Parameters:  neighbor_update - IP_TRUE if this lookup is done just to
 *              update a neighbor entry, no new route entry will be created
 *              in this case and no loopback route will be returned.
 *              vr - The virtual router to use (0 = default).
 *              key - The destination for this route.
 *              scope_id - The scope ID in host byte order.
 *              ifindex - The interface the returned route must use or 0
 *              if any interface will do.
 *              re - Will contain the pointer to the route entry if the function
 *              is successful.
 * Returns:      0 = entry found
 *              <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET6
IP_GLOBAL int
ipnet_route_lookup_ip6_dont_route(Ip_bool neighbor_update,
                                  Ip_u16 vr,
                                  IP_CONST struct Ip_in6_addr *dst,
                                  Ip_u32 ifindex,
                                  Ipnet_route_entry **re)
{
    struct Ip_in6_addr    mask;
    Ipnet_ip6_addr_entry *addr;
    Ipnet_route_entry    *rt;
    unsigned              i;
    int                   addr_type;
    unsigned              best_prefixlen = 0;
    Ipnet_netif          *best_netif = IP_NULL;
    Ip_bool               is_multicast;
    Ipnet_netif          *netif;

    netif = ipnet_if_indextonetif(vr, ifindex);
    addr_type = ipnet_ip6_get_addr_type(dst,
                                        vr,
                                        netif);
    is_multicast = (IP_IN6_IS_ADDR_MULTICAST(dst) || addr_type == IPNET_ADDR_TYPE_MULTICAST);
    /* Check if the destination is reachable at all, this has to be done before
       looking for existing router entries to detect if the whole network
       has been deleted */
    if (neighbor_update == IP_FALSE
        && (addr_type == IPNET_ADDR_TYPE_UNICAST
            || addr_type == IPNET_ADDR_TYPE_ANYCAST))
        best_netif = ipnet_loopback_get_netif(vr);
    else if (is_multicast)
    {
        if (ifindex)
            best_netif = netif;
        else
            best_netif = ipnet_route_get_capable_netif(vr, is_multicast, IP_FALSE);
    }
    else
    {
        IPNET_NETIF_FOR_EACH_OR_SPECIFIC_ON_VR(netif, vr, ifindex, i)
        {
            if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
                continue;

            for (addr = netif->inet6_addr_list;
                 addr != IP_NULL && addr->type == IPNET_ADDR_TYPE_UNICAST;
                 addr = addr->next)
            {
                if (addr->prefixlen == 0)
                    continue;
                ipcom_memset(&mask, 0, sizeof(mask));
                ipnet_route_create_mask(&mask, addr->prefixlen);
                if (ipcom_route_key_cmp(128, dst, &addr->addr, &mask)
                    != IPCOM_ROUTE_KEY_CMP_NO_MATCH)
                {
                    if (best_prefixlen < addr->prefixlen)
                    {
                        best_prefixlen = addr->prefixlen;
                        best_netif = netif;
                    }
                }
            }
        }
    }

    if (best_netif == IP_NULL)
        return -IP_ERRNO_EHOSTUNREACH;

    /* See if the entry already exists */
    for (rt = ipnet->ip6.dont_route_list;
         rt != IP_NULL;
         rt = rt->next)
    {
        if (IP_IN6_ARE_ADDR_EQUAL(dst, rt->hdr.key)
            && vr == rt->netif->vr_index
            && (ifindex == 0 || ifindex == rt->netif->ipcom.ifindex))
        {
            *re = rt;
            return 0;
        }
    }
    if (neighbor_update)
        return -IP_ERRNO_ESRCH;

    rt = ipnet_route_init_dont_route_entry(best_netif,
                                           sizeof(Ipnet_route_entry)
                                           + sizeof(struct Ip_in6_addr)
                                           + sizeof(struct Ip_sockaddr_dl)
                                           + IPNET_ROUTE_ENTRY_DATA_LEN,
                                           is_multicast);
    if (rt == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    ipcom_memcpy(rt->hdr.key, dst, sizeof(*dst));
    ipnet_icmp6_neighbor_cache_init(rt);
    ipnet_ip6_neighbor_cache_update(IPNET_IP6_NC(rt),
                                    (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_DONE)
                                     ? IPNET_ND_REACHABLE
                                     : IPNET_ND_INCOMPLETE),
                                    IP_NULL, 
                                    IP_TRUE);

    if (ipnet->ip6.dont_route_list != IP_NULL)
    {
        rt->next = ipnet->ip6.dont_route_list;
        rt->next->prev = rt;

    }
    ipnet->ip6.dont_route_list = rt;
    IPNET_ROUTE_DONT_ROUTE_SET_HEAD(rt, ipnet->ip6.dont_route_list);

    *re = rt;
    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                        ipnet_route_lookup_ecmp
 *===========================================================================
 * Description: Returns the route matching the key. It will select the same
 *              route for the same micro flow if there are more than one
 *              matching route entry.
 * Parameters:  domain - The route domain (IP_AF_INET or IP_AF_INET6).
 *              vr - The virtual router index.
 *              flags - IPNET_RTL_FLAG_xxx flags.
 *              dst - The destination for this route, can be a host or a
 *                    network (the dst address + the scope ID is the key
 *                    for IPv6).
 *              scope_id - The scope ID in host byte order (used only for
 *                         IPv6).
 *              ifindex - The interface the returned route must use or 0
 *                        if any interface will do.
 *              rps - Extra route lookup selectors.
 *              re - Will contain the pointer to the route entry if the
 *                   function is successful.
 *              dont_cache - Will be set to IP_TRUE if the returned route
 *                           entry cannot be put into the fast forward cache.
 * Returns:     IPNET_ROUTE_PERFECT_MATCH = perfect match,
 *              IPNET_ROUTE_MASK_MATCH = mask match,
 *              <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_lookup_ecmp(int domain, Ip_u16 vr, int flags,
                        IP_CONST void *dst, Ip_u32 scope_id, Ip_u32 ifindex,
                        IP_CONST Ipnet_route_policy_selector *rps,
                        Ipnet_route_entry **re, Ip_bool *dont_cache)
{
    Ipnet_route_policy_rule *pr = IP_NULL;
    Ip_bool            last_rtab = IP_TRUE;
    int                ret;
    int                table = IPCOM_ROUTE_TABLE_DEFAULT;
    Ipnet_route_entry *rt;
    unsigned           equal_count = 1;
    unsigned           h;
#ifdef IPNET_USE_ECMP_HIGHEST_RANDOM_WEIGHT
    int                i;
    Ipnet_route_entry *hrw_rt = IP_NULL;
    unsigned           hrw_max = 0;
    unsigned           hrw_this;
#endif

    *re = IP_NULL;
    do
    {
        if (ipnet->policy_routing_rules != IP_NULL
            && rps != IP_NULL
            && IP_BIT_ISFALSE(flags, IPNET_RTL_FLAG_LINK_LOCAL))
        {
            table = ipnet_route_get_table_id(domain, vr, rps, &pr);
            if (table < 0)
                return table;
            last_rtab = (pr == IP_NULL);
        }

        /* */
        if (pr == IP_NULL || IP_BIT_ISFALSE(pr->rule.mask, IPNET_PR_RULE_RTKEYSADDR))
            /* Do the normal routing lookup */
            ret = ipnet_route_raw_lookup(domain, vr, table, flags, dst, scope_id, ifindex, &rt);
        else
        {
            /* Do a lookup using the reverse key in this particular table. This is the easiest
             * way to make the FA work properly. Dunno if anyone else is interested though
             * Basically this forces traffic to route depending solely on source address */
            if (IP_BIT_ISFALSE(rps->fields, IPNET_PR_RULE_SADDR))
                return -IP_ERRNO_ENETUNREACH;
            ret = ipnet_route_raw_lookup(domain, vr, table, flags, rps->saddr, scope_id, ifindex, &rt);
        }

        /* */
        if (rps != IP_NULL && ret >= 0)
        {
            if (IP_BIT_ISSET(rps->fields, IPNET_PR_RULE_PKTPTR))
            {
                while (rt != IP_NULL)
                {
                    if (IP_UNLIKELY(rt->rt_func_info != IP_NULL && rt->rt_func_info->rt_compare != IP_NULL))
                    {
                        if ((*rt->rt_func_info->rt_compare) (rt, rps->pkt) == IP_FALSE)
                        {
                            rt = rt->next;
                            continue;
                        }
                    }

                    break;
                }

                if (rt == IP_NULL)
                {
                    ret = -IP_ERRNO_ENETUNREACH;
                    continue;
                }
            }

            if (rt)
            {
                /* Bookkeep in SKIP routes; UGLY, but I need the stats */
                if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_SKIP))
                {
                    ret = -IP_ERRNO_ENETUNREACH;
                    /**/
                    if (rps != IP_NULL && IP_BIT_ISSET(rps->fields, IPNET_PR_RULE_PKTPTR) && rt->rt_func_info)
                    {
                        (*rt->rt_func_info->rt_info) (&rt, rps->pkt, IP_NULL, dont_cache);

                        /* Dont cache this route */
                        if (dont_cache)
                            *dont_cache = IP_TRUE;
                    }
                }
            }
        }

    }  while (ret < 0 && !last_rtab);

    if (ret < 0)
        return ret;

    if (rt == IP_NULL)
        return -IP_ERRNO_ENETUNREACH;

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_PREF))
    {
        while (rt->next != IP_NULL
               && rt->metrics.rmx_hopcount == rt->next->metrics.rmx_hopcount)
        {
            rt = rt->next;
            equal_count++;
        }

        if (equal_count > 1)
        {
            IP_CONST union Ipnet_route_key *key;
#ifdef IPCOM_USE_INET6
            union Ipnet_route_key           key_buf;
#endif

            h = 0;
            if (rps != IP_NULL)
            {
                if (IP_BIT_ISSET(rps->fields, IPNET_PR_RULE_DS))
                    h = rps->ds << 24;
                if (IP_BIT_ISSET(rps->fields, IPNET_PR_RULE_PROTO))
                    h = rps->proto << 16;
                if (IP_BIT_ISSET(rps->fields, IPNET_PR_RULE_FLOW))
                    h ^= rps->flow;
                if (IP_BIT_ISSET(rps->fields, IPNET_PR_RULE_SADDR) && rps->saddr != IP_NULL)
                {
                    key = rps->saddr;
#ifdef IPCOM_USE_INET6
                    if (domain == IP_AF_INET6)
                        key = ipnet_route_in6_addr_to_key(rps->saddr, scope_id, &key_buf);
#endif /* IPCOM_USE_INET6 */
                    h = ipcom_hash_update(key, rt->head->octet_count, h);
                }
            }

            key = dst;
#ifdef IPCOM_USE_INET6
            if (domain == IP_AF_INET6)
                key = ipnet_route_in6_addr_to_key(dst, scope_id, &key_buf);
#endif /* IPCOM_USE_INET6 */
            h = ipcom_hash_update(key, rt->head->octet_count, h);

#ifdef IPNET_USE_ECMP_HIGHEST_RANDOM_WEIGHT
            for (i = 0; i < equal_count; i++)
            {
                hrw_this = ipcom_hash_update(rt->gateway, IPCOM_SA_LEN_GET(rt->gateway), h);
                if (hrw_this >= hrw_max)
                {
                    hrw_max = hrw_this;
                    hrw_rt = rt;
                }
                rt = rt->prev;
            }
            rt = hrw_rt;
#else /* Modulo-N hash */
            h %= equal_count;
            while (h--)
                rt = rt->prev;
#endif
        }
    }

    *re = rt;
    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_UP))
    {
        if (IP_BIT_ISSET(rt->hdr.flags,IPNET_RTF_HOST))
            return -IP_ERRNO_EHOSTUNREACH;
        return -IP_ERRNO_ENETUNREACH;
    }

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_REJECT))
    {
        /**/
        if (rps != IP_NULL && IP_BIT_ISSET(rps->fields, IPNET_PR_RULE_PKTPTR) && rt->rt_func_info)
        {
            /* Do statistics or bookkeeping */
            (*rt->rt_func_info->rt_info) (&rt, rps->pkt, IP_NULL, dont_cache);
        }

        /* There is no really good error code for this... */
        return -IP_ERRNO_EACCES;
    }

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING)
        && IP_BIT_ISFALSE(flags, IPNET_RTL_FLAG_DONTCLONE))
        return -IP_ERRNO_EADDRNOTAVAIL;

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_route_raw_lookup
 *===========================================================================
 * Description: Returns the route matching the key.
 * Parameters:  domain - The route domain (IP_AF_INET or IP_AF_INET6).
 *              vr - The route table to use (0 = default).
 *              table - The table ID to use (0 = default)
 *              flags - IPNET_RTL_FLAG_xxx flags.
 *              dst - The destination for this route, can be a host or a
 *              network.
 *              scope_id - The scope ID in host byte order (used only for IPv6).
 *              ifindex - The interface the returned route must use or 0
 *              if any interface will do.
 *              rtp - Will contain the pointer to the route entry if the
 *              function call is successful.
 * Returns:     IPNET_ROUTE_PERFECT_MATCH = perfect match,
 *              IPNET_ROUTE_MASK_MATCH = mask match,
 *              <0 = error code.
 *
 */
/*lint -efunc(413,ipnet_route_raw_lookup)  Likely use of null pointer */
IP_GLOBAL int
ipnet_route_raw_lookup(int domain,
                       Ip_u16 vr,
                       int table,
                       int flags,
                       IP_CONST void *dst,
                       Ip_u32 scope_id,
                       Ip_u32 ifindex,
                       Ipnet_route_entry **rtp)
{
    union Ipnet_route_key key_buf;
    Ipcom_route          *rtab;
    Ipnet_route_entry    *rt;
    int                   ret;
    Ip_bool               always_mask_match = IP_FALSE;
    IP_CONST union Ipnet_route_key *key;

    key = dst;

#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        struct Ip_in6_addr *dst6 = (struct Ip_in6_addr *) dst;

        if (IP_IN6_IS_ADDR_V4MAPPED(dst6))
        {
#ifdef IPCOM_USE_INET
            /* Copy the IPv4 part of the address and do an IPv4 lookup */
            ipcom_memcpy(&key_buf.ipv4,
                         &dst6->in6.addr16[6],
                         sizeof(key_buf.ipv4));
            key = &key_buf;
            domain = IP_AF_INET;
#else
            return -IP_ERRNO_ENETUNREACH;
#endif /* IPCOM_USE_INET */
        }
        else
            key = ipnet_route_in6_addr_to_key(dst6, scope_id, &key_buf);
    }
#endif /* IPCOM_USE_INET6 */

 retry:
    ret = ipnet_route_get(domain, vr, table, key, &rtab, &rt);

    if (ret >= 0 && rt->narrow)
        rt = ipnet_route_get_rt_with_largest_prefixlen(rt, key, flags);

    if (ret >= 0 && ifindex != 0)
    {
        Ipnet_route_entry *original_rt = rt;
        do
        {
            /* Try to find a route that uses the specified network interface */
            if (rt->netif == IP_NULL || rt->netif->ipcom.ifindex == ifindex)
                break;
            else
            {
                if (rt->next != IP_NULL)
                    rt = rt->next;
                else
                {
                    rt = rt->rt_template;
                    while (rt != IP_NULL && rt->prev != IP_NULL)
                        rt = rt->prev;
                    ret = IPNET_ROUTE_MASK_MATCH;
                }
            }
        } while (rt != IP_NULL);

        if (rt == IP_NULL)
            switch (IPNET_ROUTE_GET_FAMILY(rtab))
            {
#ifdef IPCOM_USE_INET
            case IP_AF_INET:
                if (ipnet_route_ip4_is_mcast_or_bcast(dst, ifindex, vr))
                    rt = original_rt;
                break;
#endif
#ifdef IPCOM_USE_INET6
            case IP_AF_INET6:
                if (IP_IN6_IS_ADDR_MULTICAST(dst))
                    rt = original_rt;
                break;
#endif
            default:
                IP_PANIC2();
                break;
            }

        if (rt == IP_NULL)
        {
            Ipcom_route_entry *r = (Ipcom_route_entry *) original_rt;
            /* Backtrack to find a route that matches the interface */
            while (r->parent != IP_NULL)
            {
                if (r->parent->on == r && r->parent->mask != IP_NULL)
                {
                    ipcom_memcpy(&key_buf, key, rtab->octet_count);
                    ipnet_route_apply_mask(&key_buf, r->parent->mask, rtab->bit_count);
                    always_mask_match = IP_TRUE;
                    key = &key_buf;
                    goto retry;
                }
                r = r->parent;
            }
        }

        if (rt == IP_NULL)
            return -IP_ERRNO_ENETUNREACH;
    }

    if ((ret == IPNET_ROUTE_MASK_MATCH || always_mask_match)
        && IP_BIT_ISFALSE(flags, IPNET_RTL_FLAG_DONTRECURSE | IPNET_RTL_FLAG_DONTCLONE))
    {
        Ipnet_route_entry *trt = rt;

        /* Try to create a perfect match route by cloning */
        for (trt = rt;
             trt != IP_NULL;
             trt = (IP_BIT_ISSET(flags, IPNET_RTL_FLAG_LINK_LOCAL) ? trt->next : IP_NULL))
            if (IP_BIT_ISSET(trt->hdr.flags, IPNET_RTF_CLONING))
            {
                ret = ipnet_route_create_llinfo_entry(domain, vr, key, trt, IP_NULL);
                if (ret >= 0)
                    ret = ipnet_route_raw_lookup(domain,
                                                 vr,
                                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                                 IPNET_RTL_FLAG_DONTRECURSE,
                                                 key,
                                                 scope_id,
                                                 ifindex,
                                                 &rt);
                break;
            }
    }

    if (ret < 0)
    {
        if (rtab != IP_NULL)
        {
            IPNET_ROUTESOCK(ipnet_routesock_rt_miss(rtab, key));
            IPNET_NETLINKSOCK(ipnet_rtnetlink_route_miss(rtab, key));
        }
        return -IP_ERRNO_ENETUNREACH;
    }

    if (rt->netif == IP_NULL)
    {
        ip_assert(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_MCAST_RO | IPNET_RTF_X_BCAST_RO));
        if (ipnet_route_find_netif(rt) == IP_FALSE)
            return -IP_ERRNO_ENETUNREACH;
    }

    if (IP_BIT_ISSET(flags, IPNET_RTL_FLAG_LINK_LOCAL)
        && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO)
        && (ret != IPNET_ROUTE_PERFECT_MATCH || IP_BIT_ISFALSE(rt->netif->ipcom.flags, IP_IFF_LOOPBACK)))
    {
        /* Try to find a matching link local route */
        ret = ipnet_route_lookup_link_local(domain, vr, flags, key, ifindex, &rt);
    }

    *rtp = rt;
    return ret;
}


/*
 *===========================================================================
 *                       ipnet_route_lookup2
 *===========================================================================
 * Description: Looks up a route based on key, mask and interface index.
 * Parameters:  domain - The domain.
 *              vr - The virtual route table to use.
 *              table - The route table ID to use.
 *              flags - IPNET_RTL_FLAG_xxx flags.
 *              dst - The destination address.
 *              scope_id - The destination adress scope ID.
 *              mask - The mask the route must match or IP_NULL.
 *              gw - The gateway the mask must have or IP_NULL.
 *              ifindex - The ifindex the route must match or 0.
 *              prt - The mathing route if the function is successful
 * Returns:     IPNET_ROUTE_PERFECT_MATCH = perfect match,
 *              IPNET_ROUTE_MASK_MATCH = mask match,
 *              <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_raw_lookup2(int domain,
                        Ip_u16 vr,
                        int table,
                        int flags,
                        IP_CONST void *dst,
                        Ip_u32 scope_id,
                        IP_CONST void *mask,
                        struct Ip_sockaddr *gw,
                        Ip_u32 ifindex,
                        Ipnet_route_entry **prt)
{
    Ipnet_route_entry *rt;
    Ipnet_route_entry *rt2;
    int                ret;
    Ip_bool            narrow_search;

    ret = ipnet_route_raw_lookup(domain,
                                 vr,
                                 table,
                                 flags,
                                 dst,
                                 scope_id,
                                 0,
                                 &rt);
    if (ret < 0)
        return ret;

    if (ipcom_mask_to_prefixlen(mask, rt->head->bit_count) == rt->head->bit_count)
        /* mask with all bits set to 1 is represented as mask == IP_NULL */
        mask = IP_NULL;

    if (gw != IP_NULL || mask != IP_NULL || ifindex != 0)
        narrow_search = IP_TRUE;
    else
        narrow_search = IP_FALSE;

    *prt = rt;
    if (narrow_search)
    {
        for (rt2 = *prt; rt2 != IP_NULL; rt2 = rt2->widen)
        {
            for (rt = rt2; rt != IP_NULL; rt = rt->next)
            {
                if (ifindex != 0 && ifindex != rt->netif->ipcom.ifindex)
                    /* The interface does not match */
                    continue;

                if (mask != IP_NULL
                    && (rt->hdr.mask == IP_NULL
                        || ipcom_memcmp(mask, rt->hdr.mask, rt->head->octet_count) != 0))
                    /* The specified mask does not match */
                    continue;

                if (mask == IP_NULL && rt->hdr.mask != IP_NULL)
                    /* To be able to add xx.xx.xx.0 as a host route */
                    continue;

                if (gw != IP_NULL)
                {
                    if (rt->gateway == IP_NULL ||
                        ipnet_route_are_sockaddr_equal(rt->gateway, gw, rt->rt_func_info == IP_NULL) == IP_FALSE)
                        continue;

                }

                *prt = rt;
                return ret;
            }
        }
        ret = -IP_ERRNO_ESRCH;
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_route_set_lifetime
 *===========================================================================
 * Description: Sets the lifetime on a route entry.
 * Parameters:  rt - The route entry for which the lifetime should be set.
 *              lifetime_sec - The number of seconds this route entry should
 *              exist.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_set_lifetime(Ipnet_route_entry *rt, Ip_u32 lifetime_sec)
{
    Ip_u32 old_rmx_expire;

#ifdef IPNET_DEBUG
    {
        static Ip_bool init = IP_FALSE;
        if (!init)
        {
            init = IP_TRUE;
            ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_route_timeout_cb,
                                    (Ipnet_timeout_to_string_f) ipnet_route_to_string);
        }
    }
#endif

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE))
        /* Timeouts for don't route entries are handled separatly */
        return;

    if (lifetime_sec != IPCOM_ADDR_INFINITE
        && rt->tmo != IP_NULL
        && rt->tmo->pq_index != 0
        && rt->metrics.rmx_expire == IPNET_SECONDS_ABS + lifetime_sec)
        /* The lifetime has not changed and a timer is already running */
        return;

    if (rt->metrics.rmx_expire != 0)
        old_rmx_expire = rt->metrics.rmx_expire;
    else
        old_rmx_expire = IPCOM_ADDR_INFINITE;

    ipnet_timeout_cancel(rt->tmo);

    if (lifetime_sec == IPCOM_ADDR_INFINITE)
        rt->metrics.rmx_expire = IPCOM_ADDR_INFINITE;
    else
    {
        rt->metrics.rmx_expire = IPNET_SECONDS_ABS + lifetime_sec;
        while (rt->metrics.rmx_expire == IPCOM_ADDR_INFINITE || rt->metrics.rmx_expire == 0)
            /* Both IPCOM_ADDR_INFINITE and 0 is used for infinite lifetime */
            rt->metrics.rmx_expire--;
        (void) ipnet_timeout_schedule(lifetime_sec * 1000,
                                      (Ipnet_timeout_handler) ipnet_route_timeout_cb,
                                      rt,
                                      &rt->tmo);
    }

    if (old_rmx_expire != rt->metrics.rmx_expire && lifetime_sec != 0)
    {
        IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
    }
    /* lifetime_sec == 0 is ignored since a lifetime of zero is only
       used as a way to defer an operation to "as soon as possible,
       but not now" */
}


/*
 *===========================================================================
 *                    ipnet_route_first_entry
 *===========================================================================
 * Description: Return the first entry in the route tree.
 * Parameters:  domain - The route domain (IP_AF_INET or IP_AF_INET6).
 *              vr - The route table to use (0 = default).
 *              table - The table ID of the route table.
 * Returns:     The first entry or IP_NULL if the tree is empty.
 *
 */
IP_GLOBAL Ipnet_route_entry *
ipnet_route_first_entry(int domain, Ip_u16 vr, Ip_u32 table)
{
    Ipcom_route_entry *rt;
    Ipcom_route       *rtab;

    if (ipnet_route_get_rtab(domain, vr, table, &rtab) < 0)
        return IP_NULL;

    rt = rtab->top;
    while (rt->off != IP_NULL)
        rt = rt->off;
    if (IP_BIT_ISSET(rt->flags, IPCOM_ROUTE_FLAG_INIT))
        rt = (Ipcom_route_entry *) ipnet_route_next_entry((Ipnet_route_entry *) rt);

    return (Ipnet_route_entry *) rt;
}


/*
 *===========================================================================
 *                    ipnet_route_next_entry
 *===========================================================================
 * Description: Return the next entry in the tree.
 * Parameters:  rt - Current entry.
 * Returns:     The next entry or IP_NULL if the previous one was the last.
 *
 */
IP_GLOBAL Ipnet_route_entry *
ipnet_route_next_entry(Ipnet_route_entry *rt)
{
    Ipcom_route_entry *current;

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPCOM_ROUTE_FLAG_INIT))
    {
        if (rt->next)
            return rt->next;

        while (rt->prev)
            rt = rt->prev;

        if (rt->narrow)
            return rt->narrow;

        while (rt->widen)
            rt = rt->widen;
    }

    current = (Ipcom_route_entry *) rt;
    while (current->parent->on == current)
    {
        current = current->parent;
        if (current->parent == IP_NULL)
            return IP_NULL;
    }
    current = current->parent;
    current = current->on;
    while (current->off != IP_NULL)
        current = current->off;
    if (IP_BIT_ISSET(current->flags, IPCOM_ROUTE_FLAG_INIT))
        return IP_NULL;
    return (Ipnet_route_entry *) current;
}


/*
 *===========================================================================
 *                 ipnet_route_get_table_ids_cb
 *===========================================================================
 * Description: Callback that will be called for each routing table.
 * Parameters:  rtab - A routing table head.
 *              d - Callback data.
 * Returns:
 *
 */
IP_STATIC void
ipnet_route_get_table_ids_cb(Ipcom_route *rtab,
                             struct Ipnet_route_get_table_ids_cb_data *d)
{
    if (IPNET_ROUTE_GET_FAMILY(rtab) != d->domain
        || IPNET_ROUTE_GET_VR(rtab) != d->vr)
        return;

    if (d->table_ids != IP_NULL)
    {
        if (d->size == d->added)
        {
            IPCOM_LOG0(WARNING, "ipnet_route_get_table_ids_cb: Too small buffer");
            d->table_ids = IP_NULL;
            return;
        }
        d->table_ids[d->added] = IPNET_ROUTE_GET_TABLE(rtab);
    }
    d->added++;
}


/*
 *===========================================================================
 *                    ipnet_route_get_table_ids
 *===========================================================================
 * Description: Returns an array of all policy indexes that exists on the
 *              specified virtual router.
 * Parameters:  vr - Index of a virtual router.
 *              table_ids - The array where the table IDs should be stored,
 *                         can be IP_NULL if the number of policies should be
 *                         returned in 'num_tables'.
 *              num_tables - The number of elements that can fit into the
 *                          'table_ids' buffer if that pointer is != IP_NULL
 *                           Will contain the number of entries copied.
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_route_get_table_ids(Ip_u16 vr, Ip_u32 *table_ids, unsigned *num_tables)
{
    struct Ipnet_route_get_table_ids_cb_data cb;

    cb.domain    = IPNET_AF_INET_OR_INET6;
    cb.vr        = vr;
    cb.table_ids = table_ids;
    cb.added     = 0;
    cb.size      = *num_tables;

    /* The route table with policy index 0 always exists and is not placed
       in the hash table */
    ipcom_hash_for_each(ipnet->rtabs,
                        (Ipcom_hash_foreach_cb_func) ipnet_route_get_table_ids_cb,
                        &cb);
    *num_tables = cb.added;
    return table_ids != IP_NULL && cb.table_ids == IP_NULL ? -IP_ERRNO_ENOBUFS : 0;
}


/*
 *===========================================================================
 *                    ipnet_route_key_to_sockaddr
 *===========================================================================
 * Description: Create a socket address from the search key.
 * Parameters:  domain - The domain the key is used in (IP_AF_INET or
 *              IP_AF_INET6).
 *              key - The key to convert.
 *              sa - Pointer to the socket address to create.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_key_to_sockaddr(int domain, IP_CONST void *key, struct Ip_sockaddr *sa)
{
#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        ipcom_memset(sa, 0, sizeof(struct Ip_sockaddr_in));
        IPCOM_SA_LEN_SET(sa, sizeof(struct Ip_sockaddr_in));
        ipcom_memcpy(&((struct Ip_sockaddr_in *) sa)->sin_addr, key,
                     sizeof(struct Ip_in_addr));
    }
    else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        struct Ipnet_ipv6_key *ipv6_key = (struct Ipnet_ipv6_key *) key;

        ipcom_memset(sa, 0, sizeof(struct Ip_sockaddr_in6)); /*lint -e419 */
        IPCOM_SA_LEN_SET(sa, sizeof(struct Ip_sockaddr_in6));
        ipcom_memcpy(&((struct Ip_sockaddr_in6 *) sa)->sin6_addr, &ipv6_key->addr,
                     sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&((struct Ip_sockaddr_in6 *) sa)->sin6_scope_id, &ipv6_key->scope_id,
                     sizeof(Ip_u32));
    }
    else
#endif /* IPCOM_USE_INET6 */
    {
        IP_PANIC();
    }

    sa->sa_family = (Ip_sa_family_t)domain;
}


/*
 *===========================================================================
 *                      ipnet_route_has_changed
 *===========================================================================
 * Description: Update the internal structures since the route parameters
 *              has been changed.
 * Parameters:  [in] - The route that has been modified.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_has_changed(Ipnet_route_entry *rt)
{
    Ipnet_route_entry *dup_rt = rt;

#ifdef IPCOM_USE_MIB2_NEW
    rt->msec_changed = ipnet->msec_now;
#endif

    while (dup_rt->prev)
        dup_rt = dup_rt->prev;

    while (dup_rt->widen)
        dup_rt = dup_rt->widen;

    if (dup_rt == rt)
    {
        /* No route before this one, use the route that will be first
           after ipnet_route_unhook(), if any */
        if (dup_rt->next)
            dup_rt = dup_rt->next;
        else if (dup_rt->narrow)
            dup_rt = dup_rt->narrow;
    }

    if (dup_rt != rt)
    {
        /* Unhook & re-insert to make sure that the entry located at the
           correct slot in the list */
        (void)ipnet_route_unhook(rt);
        ipnet_route_insert(rt->head, rt, dup_rt, IP_FALSE);
    }
}


/*
 *===========================================================================
 *                      ipnet_route_rotate_gateways
 *===========================================================================
 * Description: Rotates a list of gateways that have the same lookup key
 *              and same metric. The passed in gateway will become the last
 *              gateway after this call.
 * Parameters:  rt - first gateway in the list of gateways to be rotated.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_rotate_gateways(Ipnet_route_entry *rt)
{
    Ipnet_route_entry *new_first_rt = rt->next;

    if (new_first_rt)
    {
        ipnet_route_unhook(rt);
        /* The insert call inserts "rt" at the end of the list */
        ipnet_route_insert(rt->head, rt, new_first_rt, IP_FALSE);
    }
}


/*
 *===========================================================================
 *                    ipnet_route_remove_all
 *===========================================================================
 * Description: Removes all routes on the specified domain that uses the
 *              specified interface and optionally a specific gateway.
 * Parameters:  domain - The domain to remove routes from.
 *              netif - The interface for which all routes should be removed.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_route_remove_all(int domain, Ipnet_netif *netif)
{
    struct Ipnet_route_remove_all_cb d;

    d.domain = domain;
    ipnet_route_for_each_data_init(&d.d, netif, IP_NULL, 0);
    ipcom_hash_for_each(ipnet->rtabs,
                        (Ipcom_hash_foreach_cb_func) ipnet_route_remove_all_cb_for_each_table,
                        &d);
}


/*
 *===========================================================================
 *                    ipnet_route_sockaddr_to_key
 *===========================================================================
 * Description: Convert a socket address to a search key.
 * Parameters:  domain - The domain the key is used in (IP_AF_INET or
 *              IP_AF_INET6).
 *              sa - Pointer to the socket address.
 *              key - The key buffer.
 * Returns:
 *
 */
#ifdef IPNET_USE_ROUTESOCK
IP_GLOBAL void
ipnet_route_sockaddr_to_key(int domain, IP_CONST struct Ip_sockaddr *sa, void *key)
{
#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        ipcom_memcpy(key, &((struct Ip_sockaddr_in *) sa)->sin_addr,
            sizeof(struct Ip_in_addr));
    }
    else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        struct Ipnet_ipv6_key *ipv6_key = (struct Ipnet_ipv6_key *) key;
        struct Ip_sockaddr_in6 *sa6 = (struct Ip_sockaddr_in6 *) sa;

        IPNET_IP6_SET_ADDR(&ipv6_key->addr, &sa6->sin6_addr);
        ipv6_key->scope_id = IP_GET_32ON16(&sa6->sin6_scope_id);
    }
    else
#endif /* IPCOM_USE_INET6 */
        IP_PANIC();
}
#endif /* IPNET_USE_ROUTESOCK */


/*
 *===========================================================================
 *                    ipnet_route_add_table
 *===========================================================================
 * Description: Adds the IPv4 and IPv6 route table for the specified
 *              virtual router and with the specified ID.
 * Parameters:  vr - The virtual router index.
 *              table - The table ID.
 *              pvr - The selected VR
 *              ptable - The selected table ID.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_route_add_table(Ip_u16 vr, Ip_u32 table, Ip_u16 *pvr, Ip_u32 *ptable)
{
    int          ret;
    Ipcom_route *rtab;
    int          domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6,
#endif
    };

    /* Sanity; don't allow the unspecified table to survive */
    table = IPCOM_ROUTE_TABLE_GET(table);
    if (vr == IPCOM_VR_ANY)
    {
        /* Create a new VR */
        table = IPCOM_ROUTE_TABLE_DEFAULT;
        vr = 0;
        while (ipnet_route_get_rtab(domains[0], vr, table, &rtab) == 0)
            vr++;
    }
    else if (IPCOM_ROUTE_TABLE_IS_ANY(table))
    {
        /* Create a new table */
        table = 1;
        while (ipnet_route_get_rtab(domains[0], vr, table, &rtab) == 0)
            table++;
    }
    else if (ipnet_route_get_rtab(domains[0], vr, table, &rtab) == 0)
        return -IP_ERRNO_EEXIST;

#ifdef IPCOM_USE_INET
    ret = ipnet_ip4_add_route_table(vr, table);
    if (ret < 0)
        return ret;
#endif
#ifdef IPCOM_USE_INET6
    ret = ipnet_ip6_add_route_table(vr, table);
    if (ret < 0)
    {
        (void)ipnet_route_delete_table(vr, table);
        return ret;
    }
#endif

    if (IPCOM_ROUTE_TABLE_IS_DEFAULT(table))
    {
        /* Attach loopback interface */
#ifdef IP_PORT_LKM
        ret = ipcom_create_vr_loopback(vr);
#ifdef IPNET_USE_TUNNEL
        ipcom_create_base_tunnels(vr);
#endif
#else
#ifdef IPNET_USE_LOOPBACK
        ret = ipnet_loopback_attach(vr, IPNET_IF_LOOPBACK_NAME);
#endif /* IPNET_USE_LOOPBACK */
#endif /* IP_PORT_LKM */
        if (ret < 0)
            return ret;

#ifdef IPMPLS
        /* Notify MPLS module about new virtual router has been created */
        ipnet_mpls_route_table_notify(vr, IP_TRUE);
#endif
    }

#ifdef IPCOM_USE_INET
    ret = ipnet_ip4_configure_route_table(vr, table);
    if (ret < 0)
    {
        (void)ipnet_route_delete_table(vr, table);
        return ret;
    }
#endif
#ifdef IPCOM_USE_INET6
    ret = ipnet_ip6_configure_route_table(vr, table);
    if (ret < 0)
    {
        (void)ipnet_route_delete_table(vr, table);
        return ret;
    }
#endif

    IPNET_NETLINKSOCK(ipnet_netlinksock_vrtab_add(vr, table));
    if (pvr != IP_NULL)
        *pvr = vr;
    if (ptable != IP_NULL)
        *ptable = table;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_route_delete_table
 *===========================================================================
 * Description: Removes the IPv4 and IPv6 route table for the specified
 *              virtual router and with the specified ID.
 * Parameters:  vr - The virtual router index.
 *              table - The table ID.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_route_delete_table(Ip_u16 vr, Ip_u32 table)
{
    Ipcom_route *rtab;
    int          err;
    int          domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6,
#endif
    };
    Ip_size_t    d;
#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    char         rtname[IPCOM_RTNAMSIZ];
#endif
    Ipnet_netif *netif;
    Ip_u32       i;


    /* Convert to something sane; don't allow the unspecified route to survive */
    table = IPCOM_ROUTE_TABLE_GET(table);

#ifdef IP_PORT_LKM
    /*
     * Check for interface name collisions.
     * It is not allowed to have multiple interface
     * with the same name within the same VR
     */
    if (IPCOM_ROUTE_TABLE_IS_DEFAULT(table))
    {
        IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
        {
            if ((ipnet_if_nametonetif(0, netif->ipcom.name) != IP_NULL)
                && (ipcom_netif_is_vr_bound(&netif->ipcom) == IP_FALSE))
                return -IP_ERRNO_EPERM;
        }
    }
#endif /* IP_PORT_LKM */

    for (d = 0; d < sizeof(domains) / sizeof(domains[0]); d++)
    {
        err = ipnet_route_get_rtab(domains[d], vr, table, &rtab);
        if (err < 0)
            return err;
        (void)ipcom_hash_remove(ipnet->rtabs, rtab);
        ipcom_route_free_table(rtab);
    }

    IPNET_NETLINKSOCK(ipnet_netlinksock_vrtab_del(vr, table));

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    ipnet_route_delete_name(ipnet_route_get_name(vr, table, rtname));
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */

    if (IPCOM_ROUTE_TABLE_IS_DEFAULT(table))
    {
#ifdef IPMPLS
        /* Notify the MPLS module about the deleted virtual router */
        ipnet_mpls_route_table_notify(vr, IP_FALSE);
#endif /* IPMPLS */

#ifdef IPNET_USE_LOOPBACK
        {
            Ipnet_netif *loif = ipnet_loopback_get_netif(vr);

            if (loif != IP_NULL)
            {
                (void)ipnet_if_link_ioctl(loif, IP_SIOCXCLOSE, IP_NULL);
                (void)ipcom_if_detach(&loif->ipcom);
                (void)ipcom_if_free(&loif->ipcom);
            }
        }
#endif /* IPNET_USE_LOOPBACK */
#ifdef IP_PORT_LKM
        /* Remove virtual loopbacks */
        (void)ipcom_destroy_vr_loopback(vr);
#ifdef IPNET_USE_TUNNEL
        /* Destroy the base tunnel interfaces */
        ipcom_destroy_base_tunnels(vr);
#endif
#endif /* IP_PORT_LKM */

        /* Reassign all interfaces on this virtual router to virtual router 0 */
        IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
        {
            ipnet_ioctl_move_if_rt_to_vr(netif, 0);
        }
    }

    /* Remove all rules that points to the removed table */
    for (d = 0; d < sizeof(domains) / sizeof(domains[0]); d++)
    {
        Ipnet_route_policy_rule_head *pr_head;

        pr_head = ipnet_route_policy_get_rule_head(domains[d], vr);
        if (pr_head != IP_NULL)
        {
            Ipnet_route_policy_rule *pr;
            Ipnet_route_policy_rule *next_pr;

            pr = IPCOM_LIST_FIRST(&pr_head->list);
            while (pr != IP_NULL)
            {
                next_pr = IPCOM_LIST_NEXT(&pr->list);
                if (IPCOM_ROUTE_TABLE_IS_EQUAL(pr->rule.table, table))
                {
                    ipcom_list_remove(&pr->list);
                    ipcom_free(pr);
                }
                pr = next_pr;
            }
        }
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_route_delete_vr
 *===========================================================================
 * Description: Deletes a virtual router.
 * Parameters:  vr - The virtual router index.
 *              table - The table ID.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_route_delete_vr(Ip_u16 vr)
{
    unsigned  t;
    unsigned  num_tables = 1;
    Ip_u32   *table_ids;
    Ip_u32    table_id;
    int       ret = 0;
    int       domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6,
#endif
    };

    if (vr == 0)
        return -IP_ERRNO_EACCES;

    if (ipnet_route_is_virtual_router_valid(vr) == IP_FALSE)
        return -IP_ERRNO_ENXIO;

#ifdef IP_PORT_LKM
    {
        Ipnet_netif *netif;
        Ip_u32       i;

        /*
         * A VR is removable only if all interfaces mapped to
         * it are DOWN.
         */
        IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
        {
            if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
                return -IP_ERRNO_EACCES;
        }
    }
#endif

    table_ids = &table_id;
    (void)ipnet_route_get_table_ids(vr, table_ids, &num_tables);
    if (num_tables != 1)
    {
        /* More than one table in this VR, need to dynamically
           allocate a buffer that can hold all FIBs. */
        table_ids = ipcom_calloc(num_tables, sizeof(*table_ids));
        if (table_ids == IP_NULL)
            return -IP_ERRNO_ENOMEM;

        (void) ipnet_route_get_table_ids(vr, table_ids, &num_tables);
    }

    for (t = 0; t < num_tables; t++)
    {
        ret = ipnet_route_delete_table(vr, table_ids[t]);
        ip_assert(ret == 0);
    }

    /* Remove all policy routing rules for this virtual router */
    for (t = 0; t < sizeof(domains) / sizeof(domains[0]); t++)
    {
        Ipnet_route_policy_rule_head *pr_head;

        pr_head = ipnet_route_policy_get_rule_head(domains[t], vr);
        if (pr_head != IP_NULL)
        {
            Ipnet_route_policy_rule *pr;

            for (pr = IPCOM_LIST_FIRST(&pr_head->list);
                 pr != IP_NULL;
                 pr = IPCOM_LIST_FIRST(&pr_head->list))
            {
                ipcom_list_remove(&pr->list);
                ipcom_free(pr);
            }
            (void)ipcom_hash_remove(ipnet->policy_routing_rules, pr_head);
        }
    }

    IPNET_NETLINKSOCK(ipnet_netlinksock_vrtab_del(vr, IPCOM_ROUTE_TABLE_ANY));
    if (table_ids != &table_id)
        ipcom_free(table_ids);
    return ret;
}


/*
 *===========================================================================
 *                  ipnet_route_is_virtual_router_valid
 *===========================================================================
 * Description: Retruns if the specified virtual router index is currently
 *              valid.
 * Parameters:  vr - A virtual router index.
 * Returns:     IP_TRUE if the virtual router exists.
 *
 */
IP_GLOBAL Ip_bool
ipnet_route_is_virtual_router_valid(Ip_u16 vr)
{
    struct Ipnet_route_table_key k;

    k.domain = IPNET_AF_INET_OR_INET6;
    k.table  = IPCOM_ROUTE_TABLE_DEFAULT;
    k.vr     = vr;
    return ipcom_hash_get(ipnet->rtabs, &k) != IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_route_policy_ioctl
 *===========================================================================
 * Description: Handles policy routing IO controls.
 * Parameters:  request - The IO control request number.
 *              rule - The rule to add/delete/get.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_policy_ioctl(Ip_pid_t pid, Ip_u16 vr, Ip_u32 request, struct Ipnet_policy_routing_rule *rule)
{
    Ipnet_route_policy_rule_head *pr_head;
    Ipnet_route_policy_rule      *pr;
    Ipnet_route_policy_rule      *new_pr;
    Ip_bool                       add_last = IP_FALSE;

    /* Unused when built without netlink support */
    IPCOM_UNUSED_ARG(pid);

    switch (rule->af)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
#endif
        break;
    default:
        return -IP_ERRNO_EPFNOSUPPORT;
    }

    if (ipnet->policy_routing_rules == IP_NULL)
    {
        ipnet->policy_routing_rules = ipcom_hash_new((Ipcom_hash_obj_func)ipnet_route_policy_hash_obj,
                                                     (Ipcom_hash_key_func)ipnet_route_policy_hash_key,
                                                     (Ipcom_hash_cmp_func)ipnet_route_policy_hash_cmp);
        if (ipnet->policy_routing_rules == IP_NULL)
            return -IP_ERRNO_ENOMEM;
    }

    pr_head = ipnet_route_policy_get_rule_head(rule->af, vr);
    if (pr_head == IP_NULL)
    {
        if (request != IPNET_SIOCSPRRULE)
            return -IP_ERRNO_ESRCH;

        pr_head = ipcom_malloc(sizeof(*pr_head));
        if (pr_head == IP_NULL)
        {
            IP_PANIC2();
            return -IP_ERRNO_ENOMEM;
        }
        pr_head->domain = rule->af;
        pr_head->vr     = vr;
        ipcom_list_init(&pr_head->list);
        if (ipcom_hash_add(ipnet->policy_routing_rules, pr_head) != IPCOM_SUCCESS)
        {
            ipcom_free(pr_head);
            IP_PANIC2();
            return -IP_ERRNO_ENOMEM;
        }
    }

    pr = IPCOM_LIST_FIRST(&pr_head->list);
    /* Use identifier if specified */
    if (rule->id == IPNET_PR_ID_NONE)
    {
        switch (request)
        {
        case IPNET_SIOCEPRRULE:
            if (pr == IP_NULL)
                return -IP_ERRNO_ENXIO;
            break;
        case IPNET_SIOCSPRRULE:
            /* Find correct place in list */
            while (pr != IP_NULL && rule->prio <= pr->rule.prio)
            {
                pr = IPCOM_LIST_NEXT(&pr->list);
                add_last = IP_TRUE;
            }
            break;
        default:
            IPCOM_LOG0(NOTICE,
                       "Rule ID must be specified for policy routing ioctl != add/enum");
            return -IP_ERRNO_EINVAL;
        }
    }
    else
    {
        /* Locate the correct entry by ID */
        while (pr != IP_NULL && pr->rule.id != rule->id)
            pr = IPCOM_LIST_NEXT(&pr->list);

        /* A specific identifier was specified, but we could not locate it */
        if (pr == IP_NULL)
            return -IP_ERRNO_ENXIO;

        if (request == IPNET_SIOCEPRRULE)
            /* We are enumerating entries, return the next one */
            if (IP_NULL == (pr = IPCOM_LIST_NEXT(&pr->list)))
                /* Enumeration done */
                return -IP_ERRNO_ESRCH;
    }

    switch (request)
    {
    case IPNET_SIOCGPRRULE:
    case IPNET_SIOCEPRRULE:
        ipcom_memcpy(rule, &pr->rule, sizeof(*rule));
        break;

    case IPNET_SIOCSPRRULE:
        new_pr = ipcom_malloc(sizeof(*new_pr));
        if (new_pr == IP_NULL)
            return -IP_ERRNO_ENOMEM;

        /* Convert to something usable; don't allow the UNSPEC to remain */
        rule->table = IPCOM_ROUTE_TABLE_GET(rule->table);
        ipcom_memcpy(&new_pr->rule, rule, sizeof(*rule));

        if (new_pr->rule.af == IP_AF_INET
            && new_pr->rule.saddr_prefixlen > 32)
            new_pr->rule.saddr_prefixlen = 32;
        else if (new_pr->rule.af == IP_AF_INET6
                 && new_pr->rule.saddr_prefixlen > 128)
            new_pr->rule.saddr_prefixlen = 128;

        /* Allocate a unique ID */
        /* NOTE!! We lack wraparound protection here !! */
        if (++ipnet->last_policy == 0)
            /* Use first valid */
            ipnet->last_policy = 1;

        /* Store and return */
        new_pr->rule.id = rule->id = ipnet->last_policy;

        /* Allocate */
        if (pr != IP_NULL)
            ipcom_list_insert_before(&pr->list, &new_pr->list);
        else if (add_last)
            ipcom_list_insert_last(&pr_head->list, &new_pr->list);
        else
            ipcom_list_insert_first(&pr_head->list, &new_pr->list);
        ipnet_route_cache_invalidate();
        IPNET_NETLINKSOCK(ipnet_rtnetlink_rule_event(&new_pr->rule, vr, pid, IP_RTM_NEWRULE));
        break;

    case IPNET_SIOCDPRRULE:
        ipcom_list_remove(&pr->list);
        IPNET_NETLINKSOCK(ipnet_rtnetlink_rule_event(&pr->rule, vr, pid, IP_RTM_DELRULE));
        ipcom_free(pr);
        ipnet_route_cache_invalidate();
        break;

    default:
        return -IP_ERRNO_EINVAL;
    }

    return 0;
}


#ifdef IPNET_USE_ROUTE_TABLE_NAMES
/*
 *===========================================================================
 *                    ipnet_route_vr_and_table_from_name
 *===========================================================================
 * Description: Finds the virtual router index and table ID from a router
 *              table name.
 * Parameters:  rtab_name - A route table name.
 *              pvr - Will contain the virtual router index.
 *              ptable - Will contain the route table ID.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_route_vr_and_table_from_name(char *rtab_name, Ip_u16 *pvr, Ip_u32 *ptable)
{
    Ipnet_route_name_entry *ve;

    if (rtab_name == IP_NULL || rtab_name[0] == '\0')
        return -IP_ERRNO_EINVAL;

    if (ipnet->rtab_names == IP_NULL)
        return -IP_ERRNO_ESRCH;

    ve = ipcom_hash_get(ipnet->rtab_names, rtab_name);
    if (ve == IP_NULL)
        return -IP_ERRNO_ESRCH;

    *pvr = ve->vr;
    *ptable = ve->table;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_route_get_name
 *===========================================================================
 * Description: Returns the name of the specified route table.
 * Parameters:  vr - A virtual router index.
 *              index - A route table id
 * Returns:     != IP_NULL = success
 *
 */
IP_GLOBAL char *
ipnet_route_get_name(Ip_u16 vr, Ip_u32 table, char *rtname)
{
    Ipnet_route_name_entry ve;

    if (ipnet->rtab_names == IP_NULL)
        return IP_NULL;

    ve.vr      = vr;
    ve.table   = table;
    ve.name[0] = '\0';

    ipcom_hash_for_each(ipnet->rtab_names,
                        (Ipcom_hash_foreach_cb_func)ipnet_route_find_name_cb,
                        &ve);

    ipcom_strcpy(rtname, ve.name);
    return ve.name[0] == '\0' ? IP_NULL : rtname;
}


/*
 *===========================================================================
 *                    ipnet_route_set_name
 *===========================================================================
 * Description: Names a route table at a specific virtual router.
 * Parameters:  vr - The virtual router index.
 *              table - The route table ID.
 *              rtab_name - The name for the route table.
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_route_set_name(Ip_u16 vr, Ip_u32 table, char *rtab_name)
{
    Ipnet_route_name_entry *ve = IP_NULL;
    char                    rtname[IPCOM_RTNAMSIZ];

    if (rtab_name == IP_NULL || *rtab_name == '\0')
        /* No name to set */
        return -IP_ERRNO_EINVAL;

    if (ipnet->rtab_names == IP_NULL)
    {
        ipnet->rtab_names = ipcom_hash_new((Ipcom_hash_obj_func)ipnet_route_name_obj_hash,
                                           (Ipcom_hash_key_func)ipnet_route_name_key_hash,
                                           (Ipcom_hash_cmp_func)ipnet_route_name_hash_cmp);
        if (ipnet->rtab_names == IP_NULL)
        {
            IP_PANIC();
            return -IP_ERRNO_ENOMEM;
        }
    }

    if (ipnet_route_get_name(vr, table, rtname) != IP_NULL)
        return -IP_ERRNO_EEXIST;

    ve = ipcom_malloc(sizeof(*ve));
    if (ve == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    /* Setup new hash entry and add */
    ipcom_strncpy(ve->name, rtab_name, IPCOM_RTNAMSIZ);
    ve->vr    = vr;
    ve->table = table;

    switch (ipcom_hash_add(ipnet->rtab_names, ve))
    {
    case IPCOM_SUCCESS:
        /* Notify new name */
        IPNET_NETLINKSOCK(ipnet_netlinksock_vrtab_add(vr, table));
        return 0;
    default:
        ipcom_free(ve);
        return -IP_ERRNO_ENOMEM;
    }
}

/*
 *===========================================================================
 *                    ipnet_route_delete_name
 *===========================================================================
 * Description:
 * Parameters:  rtab_name - A name of a route table.
 * Returns:     -
 *
 */
IP_GLOBAL void
ipnet_route_delete_name(char *rtab_name)
{
    Ipnet_route_name_entry *ve;

    if (rtab_name == IP_NULL)
        return;

    ve = ipcom_hash_get(ipnet->rtab_names, rtab_name);

    if (ve != IP_NULL)
    {
        (void)ipcom_hash_remove(ipnet->rtab_names, ve);
        ipcom_free(ve);
    }
}

#endif /* IPNET_USE_ROUTE_TABLE_NAMES */


#ifdef IPNET_USE_ROUTE_COOKIES
/*
 *===========================================================================
 *                    ipnet_route_get_cookie
 *===========================================================================
 * Description: Returns the cookie of the specified route table.
 * Parameters:  vr - A virtual router index.
 *              index - A route table id
 * Returns:     != IP_NULL = success
 *
 */
IP_GLOBAL int
ipnet_route_get_cookie(Ip_u16 vr, Ip_u32 table, Ipnet_rt_cookie *cookie)
{
    int                     ret = -1;
    Ipnet_rt_cookie         zcookie;
    Ipnet_rtab_cookie_entry ve;

    if ((ipnet->rtab_cookies == IP_NULL) || (cookie == IP_NULL))
        goto out;

    ve.key.vr    = vr;
    ve.key.table = table;
    ipcom_memset(&ve.cookie,0, sizeof(Ipnet_rt_cookie));
    ipcom_memset(&zcookie,0, sizeof(Ipnet_rt_cookie));

    ipcom_hash_for_each(ipnet->rtab_cookies,
                        (Ipcom_hash_foreach_cb_func)ipnet_route_find_cookie_cb,
                        &ve);

    if (ipcom_memcmp(&zcookie, &ve.cookie, sizeof(Ipnet_rt_cookie) != 0))
    {
        /* Cookie found */
        ipcom_memcpy(cookie, &ve.cookie, sizeof(Ipnet_rt_cookie));
        ret = 0;
    }

 out:

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_route_set_cookie
 *===========================================================================
 * Description: Cookies a route table at a specific virtual router.
 * Parameters:  vr - The virtual router index.
 *              table - The route table ID.
 *              rtab_cookie - The cookie for the route table.
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_route_set_cookie(Ip_u16 vr, Ip_u32 table, Ipnet_rt_cookie *rtab_cookie)
{
    Ipnet_rtab_cookie_entry *ve = IP_NULL;

    if (rtab_cookie == IP_NULL)
        /* No cookie to set */
        return -IP_ERRNO_EINVAL;

    if (ipnet->rtab_cookies == IP_NULL)
    {
        ipnet->rtab_cookies = ipcom_hash_new((Ipcom_hash_obj_func)ipnet_route_cookie_obj_hash,
                                           (Ipcom_hash_key_func)ipnet_route_cookie_key_hash,
                                           (Ipcom_hash_cmp_func)ipnet_route_cookie_hash_cmp);
        if (ipnet->rtab_cookies == IP_NULL)
        {
            IP_PANIC();
            return -IP_ERRNO_ENOMEM;
        }
    }

    /* Clear any existing cookie for this table */
    ipnet_route_delete_cookie(vr, table);

    ve = ipcom_malloc(sizeof(*ve));
    if (ve == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    /* Setup new hash entry and add */
    ve->key.vr    = vr;
    ve->key.table = table;
    ipcom_memcpy(&ve->cookie, rtab_cookie, sizeof(Ipnet_rt_cookie));

    if (ipcom_hash_add(ipnet->rtab_cookies, ve) == IPCOM_SUCCESS)
        return 0;
    else
    {
        ipcom_free(ve);
        return -IP_ERRNO_ENOMEM;
    }
}


/*
 *===========================================================================
 *                    ipnet_route_delete_cookie
 *===========================================================================
 * Description:
 * Parameters:  rtab_name - A name of a route table.
 * Returns:     -
 *
 */
IP_GLOBAL void
ipnet_route_delete_cookie(Ip_u16 vr, Ip_u32 table)
{
    Ipnet_rtab_cookie_entry *ve;
    Ipnet_rtab_cookie_key   key;

    key.vr    = vr;
    key.table = table;

    if (ipnet->rtab_cookies == IP_NULL)
        return;

    do
    {
        ve = ipcom_hash_get(ipnet->rtab_cookies, &key);

        if (ve != IP_NULL)
        {
            (void)ipcom_hash_remove(ipnet->rtab_cookies, ve);
            ipcom_free(ve);
        }
    } while (ve);
}
#endif /* IPNET_USE_ROUTE_COOKIES */


/*
 *===========================================================================
 *                   ipnet_route_policy_get_rule_chain
 *===========================================================================
 * Description: Returns the first rule in a policy chain
 * Parameters:  domain - The domain the rules must be located in.
 *              vr - The virtual router the rule should apply to.
 * Returns:     The head of the policy rule list.
 *
 */
IP_GLOBAL Ipnet_route_policy_rule *
ipnet_route_policy_get_rule_chain(int domain, Ip_u16 vr)
{
    Ipnet_route_policy_rule_head    *head = ipnet_route_policy_get_rule_head(domain, vr);
    if (head)
        return IPCOM_LIST_FIRST(&head->list);
    return IP_NULL;
}

/*
 *===========================================================================
 *                    ipnet_route_key_cmp
 *===========================================================================
 * Description: Compare two keys with the provided length.
 * Parameters:  key1    - First key
 *              key2    - Second key.
 *              key_len - Length of keys to match
 * Returns:     IP_TRUE if equal.
 *
 */
IP_GLOBAL Ip_bool
ipnet_route_key_cmp(IP_CONST void   *key1,
                    IP_CONST void   *key2,
                    Ip_u8           key_len)
{
    int             bytes;
    Ip_u8           x;

    bytes = key_len >> 3;

    /* Match the complete bytes */
    if (0 != ipcom_memcmp(key1, key2, bytes))
        return IP_FALSE;

    /* Match the odd bits */
    if (key_len & 0x7)
    {
        x = ((Ip_u8 *)key1)[bytes] ^ ((Ip_u8 *)key2)[bytes];
        if (x > (1 << (8 - (key_len & 0x7))))
            return IP_FALSE;
    }

    return IP_TRUE;
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_route_create_mask
 *===========================================================================
 * Description: Creates a network mask with the 'netbits' leftmost bits set.
 * Parameters:  [in] mask - Pointer to a buffer where the mask should be stored.
 *              [in] netbits - The number of set bits.
 * Returns:
 *
 */
IP_PUBLIC void
ipnet_route_create_mask(void *mask, int netbits)
{
    int i;

    for (i = 0; i < netbits; i++)
    {
        IP_BIT_SET(((Ip_u16*) mask)[i / 16], ip_htons(0x8000 >> (i % 16)));
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

