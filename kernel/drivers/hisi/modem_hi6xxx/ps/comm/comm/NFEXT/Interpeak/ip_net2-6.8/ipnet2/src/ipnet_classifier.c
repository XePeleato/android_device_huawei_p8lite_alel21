/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_classifier.c,v $ $Revision: 1.18 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_classifier.c,v $
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
/* Generic classifier for IPv4 and IPv6 packets
   This module is used to map packet content to a tag.
   The tag is an integer. */

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
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_sock.h>
#include <ipcom_syslog.h>

#include "ipnet_classifier.h"
#include "ipnet_h.h"

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

/* All fields must be in network byte order */
typedef struct Ipnet_classifier_map_struct
{
    Ipcom_list                   list;
    /* The tag packets matching these fields are mapped to */
    int                          tag;
    /* The ID of this rule to tag mapping */
    int                          id;
    /* Rule to determine which packetets that will get the tag */
    struct Ipnet_classifier_rule rule;
}
Ipnet_classifier_map;

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
 *                   ipnet_classifier_addr_match
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_classifier_addr_match(const void *addr_arg,
                            const struct Ipnet_classifier_rule *cr,
                            Ip_bool is_src)
{
    int    byte;
    int    bytes;
    int    bits;
    const Ip_u8 *addr = (const Ip_u8 *) addr_arg;
    const Ip_u8 *key;

    if (is_src)
    {
        key = (const Ip_u8 *) &cr->saddr;
        bits = cr->saddr_prefixlen;
    }
    else
    {
        key = (const Ip_u8 *) &cr->daddr;
        bits = cr->daddr_prefixlen;
    }

    bytes = (bits + 7) >> 3;

    for (byte = 0; byte < bytes; byte++, bits -= 8)
        if ((addr[byte] ^ key[byte]) >= (1 << (8 - IP_MIN(bits, 8))))
        {
            if (addr[byte] != key[byte])
                return IP_FALSE;
        }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                   ipnet_classifier_port_match
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_classifier_port_match(const Ipnet_pkt_udp *udp,
                            const struct Ipnet_classifier_rule *cr)
{
    Ip_u16 sport = ip_ntohs(udp->sport);
    Ip_u16 dport = ip_ntohs(udp->dport);

    if (IP_BIT_ISSET(cr->mask, IPNET_CLS_RULE_SPORT)
        && (sport < cr->sport_low || sport > cr->sport_high))
        return IP_FALSE;
    if (IP_BIT_ISSET(cr->mask, IPNET_CLS_RULE_DPORT)
        && (dport < cr->dport_low || dport > cr->dport_high))
        return IP_FALSE;
    return IP_TRUE;
}


/*
 *===========================================================================
 *                  ipnet_classifier_ip4_get_tag
 *===========================================================================
 * Description: Returns the tag for an IPv4 packet.
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC int
ipnet_classifier_ip4_get_tag(Ipnet_classifier *c, Ipcom_pkt *pkt)
{
    Ipnet_classifier_map *cm;
    Ipnet_pkt_ip         *ip;
    Ipnet_pkt_udp        *udp;

    ip = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    if (ip->p == IP_IPPROTO_UDP || ip->p == IP_IPPROTO_TCP)
        udp = (Ipnet_pkt_udp *) &pkt->data[pkt->ipstart + IPNET_IP4_GET_HDR_LEN(ip) * 4];
    else
        udp = IP_NULL;

    for (cm = IPCOM_LIST_FIRST(&c->rules);
         cm != IP_NULL;
         cm = IPCOM_LIST_NEXT(&cm->list))
    {
        if (cm->rule.af != 0 && cm->rule.af != IP_AF_INET)
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_DS) && ip->tos != cm->rule.ds)
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_PROTO) && ip->p != cm->rule.proto)
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_SADDR)
            && !ipnet_classifier_addr_match(ip->src, &cm->rule, IP_TRUE))
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_DADDR)
            && !ipnet_classifier_addr_match(ip->dst, &cm->rule, IP_FALSE))
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_SPORT | IPNET_CLS_RULE_DPORT)
            && udp != IP_NULL
            && !ipnet_classifier_port_match(udp, &cm->rule))
            continue;

        return cm->tag;
    }

    return IPNET_CLASSIFIER_INVALID_TAG;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                  ipnet_classifier_ip6_get_tag
 *===========================================================================
 * Description: Returns the tag for an IPv6 packet.
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_classifier_ip6_get_tag(Ipnet_classifier *c, Ipcom_pkt *pkt)
{
    Ipnet_classifier_map *cm;
    Ipnet_pkt_udp        *udp;
    Ipnet_pkt_ip6        *ip6;
    Ip_u8                 tclass;

    ip6 = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];
    tclass =  IPNET_IP6_GET_CLASS(ip6);
    if (ip6->nxt == IP_IPPROTO_UDP || ip6->nxt == IP_IPPROTO_TCP)
        udp = (Ipnet_pkt_udp *) &pkt->data[pkt->ipstart + IPNET_IP6_HDR_SIZE];
    else
        udp = IP_NULL;

    for (cm = IPCOM_LIST_FIRST(&c->rules);
         cm != IP_NULL;
         cm = IPCOM_LIST_NEXT(&cm->list))
    {
        if (cm->rule.af != 0 && cm->rule.af != IP_AF_INET6)
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_DS) && tclass != cm->rule.ds)
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_PROTO) && ip6->nxt != cm->rule.proto)
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_SADDR)
            && !ipnet_classifier_addr_match(&ip6->src, &cm->rule, IP_TRUE))
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_DADDR)
            && !ipnet_classifier_addr_match(&ip6->dst, &cm->rule, IP_FALSE))
            continue;

        if (IP_BIT_ISSET(cm->rule.mask, IPNET_CLS_RULE_SPORT | IPNET_CLS_RULE_DPORT)
            && udp != IP_NULL
            && !ipnet_classifier_port_match(udp, &cm->rule))
            continue;

        return cm->tag;
    }

    return IPNET_CLASSIFIER_INVALID_TAG;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                     ipnet_classifier_get_map
 *===========================================================================
 * Description: Returns the mapping that matches the ID.
 * Parameters:  c - A classifier.
 *              id - The ID of the mapping to return.
 * Returns:     The matching mapping or IP_NULL if not found.
 *
 */
IP_STATIC Ipnet_classifier_map *
ipnet_classifier_get_map(Ipnet_classifier *c, int id)
{
    Ipnet_classifier_map *cm;

    for (cm = IPCOM_LIST_FIRST(&c->rules);
         cm != IP_NULL;
         cm = IPCOM_LIST_NEXT(&cm->list))
        if (id == cm->id)
            return cm;
    return IP_NULL;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                   ipnet_classifier_pkt_to_tag
 *===========================================================================
 * Description: Returns the tag that matches the specified packet.
 * Parameters:  fields - Array of hashtables with exact (or net) matches.
 *              wildecards - Array of masks for tags that match any value.
 *              pkt - The packet to match to a tag, pkt->ipstart must be
 *              the offset to the IP header.
 * Returns:     The matching tag or -1 if no tag matches.
 *
 */
IP_GLOBAL int
ipnet_classifier_pkt_to_tag(Ipnet_classifier *c, Ipcom_pkt *pkt)
{
    if (pkt->ipstart < pkt->start || pkt->ipstart >= pkt->end)
        /* Not a IPv4 or IPv6 packet */
        return IPNET_CLASSIFIER_INVALID_TAG;

    switch (pkt->data[pkt->ipstart] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        return ipnet_classifier_ip4_get_tag(c, pkt);
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case 0x60:
        return ipnet_classifier_ip6_get_tag(c, pkt);
#endif /* IPCOM_USE_INET6 */

    default:
        IP_PANIC2();
        return IPNET_CLASSIFIER_INVALID_TAG;
    }
}


/*
 *===========================================================================
 *                  ipnet_classifier_set_tag
 *===========================================================================
 * Description: Sets the tag value for a specific mapping.
 * Parameters:  c - A classifier.
 *              id - ID of the mapping.
 *              tag - Tag to set on the mapping.
 * Returns:     0 = success
 *             <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_classifier_set_tag(Ipnet_classifier *c, int id, int tag)
{
    Ipnet_classifier_map *map;

    map = ipnet_classifier_get_map(c, id);
    if (map == IP_NULL)
        return -IP_ERRNO_ESRCH;
    map->tag = tag;
    return 0;
}


/*
 *===========================================================================
 *                  ipnet_classifier_invalidate_tag
 *===========================================================================
 * Description: Invalidates all entries matching the specified tag.
 * Parameters:  c - A classifier.
 *              tag - Tag to invalidate.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_classifier_invalidate_tag(Ipnet_classifier *c, int tag)
{
    Ipnet_classifier_map *cm;

    for (cm = IPCOM_LIST_FIRST(&c->rules);
         cm != IP_NULL;
         cm = IPCOM_LIST_NEXT(&cm->list))
    {
        if (cm->tag == tag)
            cm->tag = IPNET_CLASSIFIER_INVALID_TAG;
    }
}


/*
 *===========================================================================
 *                ipnet_classifier_get_smallest_unused_tag
 *===========================================================================
 * Description: Returns the smalles tag that is not in used in the specified
 *              classifier.
 * Parameters:  c - A classifier.
 * Returns:     The smallest unused tag.
 *
 */
IP_GLOBAL int
ipnet_classifier_get_smallest_unused_id(Ipnet_classifier *c)
{
    int id = 0;

    while (ipnet_classifier_get_map(c, id) != IP_NULL)
        id++;
    return id;
}


/*
 *===========================================================================
 *                   ipnet_classifier_add_mapping
 *===========================================================================
 * Description: Adds a new packet fields to tag mapping in a classifier.
 * Parameters:  c - The classifier this mapping should be inserted into.
 *              rule - The fields that must match.
 *              id - An unique identifier for this rule.
 * Returns:     0 = success.
 *             <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_classifier_add_mapping(Ipnet_classifier *c, struct Ipnet_classifier_rule *rule, int id)
{
    Ip_u8                 addr_bit_len;
    Ipnet_classifier_map *map;

    if (ipnet_classifier_get_map(c, id) != IP_NULL)
    {
        IPCOM_LOG1(ERR, "Packet classifier: ID %d already exists", id);
        return -IP_ERRNO_EEXIST;
    }

    switch (rule->af)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        addr_bit_len = sizeof(struct Ip_in_addr) * 8u;
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        addr_bit_len = sizeof(struct Ip_in6_addr) * 8u;
        break;
#endif
    default:
        if (IP_BIT_ISSET(rule->mask, IPNET_CLS_RULE_DADDR)
            || IP_BIT_ISSET(rule->mask, IPNET_CLS_RULE_SADDR))
        {
            IPCOM_LOG0(ERR, "Packet classifier: Address field used but no domain specified");
            return -IP_ERRNO_EINVAL;
        }
        addr_bit_len = 0;
        break;
    }

    map = ipcom_calloc(1, sizeof(Ipnet_classifier_map));
    if (map == IP_NULL)
    {
        IPCOM_LOG1(CRIT,
                   "Packet classifier: Failed to allocate memory (%d bytes) for a map",
                   sizeof(Ipnet_classifier_map));
        return -IP_ERRNO_ENOMEM;
    }

    ipcom_memcpy(&map->rule, rule, sizeof(map->rule));
    map->id  = id;
    map->tag = -1;

    if (map->rule.saddr_prefixlen == 0)
        map->rule.saddr_prefixlen = addr_bit_len;
    if (map->rule.daddr_prefixlen == 0)
        map->rule.daddr_prefixlen = addr_bit_len;

    if (map->rule.saddr_prefixlen > addr_bit_len
        || map->rule.daddr_prefixlen > addr_bit_len)
    {
        IPCOM_LOG1(ERR, "Packet classifier: prefixlen is too big, max is %d bits", addr_bit_len);
        ipcom_free(map);
        return -IP_ERRNO_EINVAL;
    }

    if (map->rule.sport_low > map->rule.sport_high
        || map->rule.dport_low > map->rule.dport_high)
    {
        IPCOM_LOG0(ERR, "Packet classifier: low port must be equal-to or smaller than high port");
        ipcom_free(map);
        return -IP_ERRNO_EINVAL;
    }

    ipcom_list_insert_last(&c->rules, &map->list);
    return 0;
}


/*
 *===========================================================================
 *                     ipnet_classifier_del_mapping
 *===========================================================================
 * Description: Deletes a packet to tag mapping.
 * Parameters:  c - The classifier to operate on.
 *              id - Id of the mapping to remove.
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_classifier_del_mapping(Ipnet_classifier *c, int id)
{
    Ipnet_classifier_map *cm;

    cm = ipnet_classifier_get_map(c, id);
    if (cm == IP_NULL)
        return -IP_ERRNO_ESRCH;

    ipcom_list_remove(&cm->list);
    ipcom_free(cm);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_classifier_new
 *===========================================================================
 * Description: Creating a new classifier database.
 * Parameters:
 * Returns:     An empty classifier.
 *
 */
IP_GLOBAL Ipnet_classifier *
ipnet_classifier_new(void)
{
    Ipnet_classifier *c;

    c = ipcom_calloc(1, sizeof(Ipnet_classifier));
    if (c == IP_NULL)
    {
        IPCOM_LOG1(CRIT, "Packet classifier: Out of memory when allocating %d bytes",
                   sizeof(Ipnet_classifier));
        return IP_NULL;
    }

    ipcom_list_init(&c->rules);
    return c;
}


/*
 *===========================================================================
 *                    ipnet_classifier_delete
 *===========================================================================
 * Description: Frees all resources allocated by the classifier.
 * Parameters:  c - The classifier to delete.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_classifier_delete(Ipnet_classifier *c)
{
    Ipnet_classifier_map *cm;

    for (cm = IPCOM_LIST_FIRST(&c->rules);
         cm != IP_NULL;
         cm = IPCOM_LIST_FIRST(&c->rules))
    {
        ipcom_list_remove(&cm->list);
        ipcom_free(cm);
    }

    ipcom_free(c);
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

