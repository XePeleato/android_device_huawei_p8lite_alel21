/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_policy_routing.h,v $ $Revision: 1.
 *   $Source: /home/interpeak/CVSRoot/ipnet2/include/ipnet_policy_routing.h,v $
 *   $Author: jhorteli $ $Date: 2009-02-04 10:15:57 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_POLICY_ROUTING_H
#define IPNET_POLICY_ROUTING_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* IPNET policy routing API */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_PR_BASE 20

#ifndef IPNET_PORT_SPECIFIC_PR_IOCTL
#define IPNET_SIOCGPRRULE     IP_IOWR(POLICY_RT, 0, R, struct Ipnet_policy_routing_rule)
#define IPNET_SIOCSPRRULE     IP_IOWR(POLICY_RT, 1, W, struct Ipnet_policy_routing_rule)
#define IPNET_SIOCDPRRULE     IP_IOWR(POLICY_RT, 2, W, struct Ipnet_policy_routing_rule)
#define IPNET_SIOCEPRRULE     IP_IOWR(POLICY_RT, 3, R, struct Ipnet_policy_routing_rule)
#endif /* IPNET_PORT_SPECIFIC_PR_IOCTL */

/* Rules are processed by descending priority, rules with same priority
   are processed in the order they where added (added first is processed first) */
#define IPNET_PR_PRIO_MIN   (-32768)
#define IPNET_PR_PRIO_MAX   (32767)

#define IPNET_PR_ID_NONE   0


/*
 ****************************************************************************
 * 5                 CLASSIFIER DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipnet_policy_routing_rule
 *===========================================================================
 *
 */
struct Ipnet_policy_routing_rule
{
    Ip_u32                  table;           /* The table ID of the routing table to use if this rule matches */
    Ip_bool                 last;            /* Set to IP_TRUE if no other rules should consider if the
                                                routing table with policy ID 'policy' does not give a match */
    Ip_u32                  id;              /* Unique identifier of this rule (IPNET_SIOCSPRRULE set this) */
    Ip_s16                  prio;            /* The index of this rule or one of the IPNET_PR_IDX_xxx macros */
    Ip_u16                  mask;            /* Set IPNET_PR_RULE_xxx bit for each field that must match */
    Ip_u8                   af;              /* Address family for this rule */
    Ip_u8                   ds;              /* Traffic class (IPv6) or TOS (IPv4) (== DS field for DiffServ) */
    Ip_u8                   proto;           /* Protocol in the IP header */
    Ip_u8                   saddr_prefixlen; /* Source prefixlen, set to 0 for exact match of 'saddr' */
    Ip_u8                   daddr_prefixlen; /* Destination prefixlen, set to 0 for exact match of 'daddr' */
    union Ip_in_addr_union  saddr;           /* Source address or source network */
    union Ip_in_addr_union  daddr;           /* Destination address or destination network */
    Ip_u32                  flow;            /* IPv6 flow ID in host byte order */
    Ip_u32                  ifindex;         /* Interface index in host byte order (bound if for sending, incoming if for forwarding) */
    Ip_u32                  scope;           /* The scope of the destination        */
    Ip_u32                  pkt_mask;        /* The mask to apply to the packet     */
    Ip_u32                  pkt_result;      /* The result when masking the flags   */
};

#define IPNET_PR_RULE_DS            (1 << 0)
#define IPNET_PR_RULE_PROTO         (1 << 1)
#define IPNET_PR_RULE_SADDR         (1 << 2)
#define IPNET_PR_RULE_FLOW          (1 << 3)
#define IPNET_PR_RULE_IFINDEX       (1 << 4)
#define IPNET_PR_RULE_SCOPE         (1 << 5)
#define IPNET_PR_RULE_DADDR         (1 << 7)
#define IPNET_PR_RULE_PKTFLAGS      (1 << 8)


/**** SPECIFIC ACTIONS BASED ON INFORMATION ****/

/* Do the route lookup based on the source (not destination)
   address in the matching route table */
#define IPNET_PR_RULE_RTKEYSADDR    (1 << 15)


/* Not userspace defined; merely informative for IPNET processing */
#define IPNET_PR_RULE_PKTPTR        (1 << 9)

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
