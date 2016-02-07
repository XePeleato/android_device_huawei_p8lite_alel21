/*
 ******************************************************************************
 *                     INTERPEAK INTERNAL API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_h.h,v $ $Revision: 1.13 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_h.h,v $
 *   $Author: ulf $ $Date: 2009-06-08 11:06:02 $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright Interpeak AB 2000-2002 <www.interpeak.se>. All rights reserved.
 *     Design and implementation by FirstName LastName <email@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_NAT_H_H
#define IPNET_NAT_H_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Internal header file for the NAT.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipnet_nat.h"

#include <ipcom_list.h>
#include <ipcom_hash.h>
#include <ipcom_pqueue.h>
#include <ipcom_inet.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      IP_IP_NAT
 *===========================================================================
 *  NAT socket option. Define here for backward compatibility if not already
 *  defined.
 */
#ifndef IP_IP_NAT
#define IP_IP_NAT               50  /* Network address translation control */
#endif


/*
 *===========================================================================
 *                      IPNET_NAT_MAX_RULES
 *===========================================================================
 *  Defines the maximum number of rules that can be added to the NAT.
 */
#define IPNET_NAT_MAX_RULES     100


/*
 *===========================================================================
 *                      IPNET_NAT_MAX_PROXIES
 *===========================================================================
 *  Defines the maximum number of proxies that can be added to the NAT.
 */
#define IPNET_NAT_MAX_PROXIES    10


/*
 *===========================================================================
 *                      IPNET_NAT_MAX_RULE_ARGS
 *===========================================================================
 *  Defines the maximum number of arguments in a NAT rule string.
 */
#define IPNET_NAT_MAX_RULE_ARGS    16


/*
 *===========================================================================
 *                      IPNET_NAT_PROXY_LABEL_LEN
 *===========================================================================
 *  Defines the maximum length of a proxy label.
 *
 */
#define IPNET_NAT_PROXY_LABEL_LEN  32


/*
 *===========================================================================
 *                      IPNET_NAT_CTRL_XXX
 *===========================================================================
 *  Defines the available NAT control commands used to pass data to and from
 *  the NAT kernel using the IP_IP_NAT socket option.
 */
#define IPNET_NAT_CTRL_ENABLE           0
#define IPNET_NAT_CTRL_DISABLE          1

#define IPNET_NAT_CTRL_ADD_RULE         2
#define IPNET_NAT_CTRL_DEL_RULE         3
#define IPNET_NAT_CTRL_FLUSH_RULES      4
#define IPNET_NAT_CTRL_GET_RULE         5

#define IPNET_NAT_CTRL_GET_INFO         6
#define IPNET_NAT_CTRL_CLEAR_STATS      7

#define IPNET_NAT_CTRL_FLUSH_MAPPINGS   8
#define IPNET_NAT_CTRL_GET_MAPPING      9

#define IPNET_NAT_CTRL_GET_PROXY        12


/*
 *===========================================================================
 *                         Session states
 *===========================================================================
 */
#define IPNET_NAT_MAPPING_CLO        0
#define IPNET_NAT_MAPPING_SYN        1
#define IPNET_NAT_MAPPING_EST        2
#define IPNET_NAT_MAPPING_FIN        3


/*
 *===========================================================================
 *                         TCP flags
 *===========================================================================
 */
#define IPNET_NAT_TCPFLAG_URGENT   0x0020
#define IPNET_NAT_TCPFLAG_ACK      0x0010
#define IPNET_NAT_TCPFLAG_PUSH     0x0008
#define IPNET_NAT_TCPFLAG_RESET    0x0004
#define IPNET_NAT_TCPFLAG_SYN      0x0002
#define IPNET_NAT_TCPFLAG_FIN      0x0001


/*
 *===========================================================================
 *                         ICMP destination unreachable codes
 *===========================================================================
 */
#define IPNET_NAT_ICMP4_CODE_DST_UNREACH_NET        0
#define IPNET_NAT_ICMP4_CODE_DST_UNREACH_HOST       1
#define IPNET_NAT_ICMP4_CODE_DST_UNREACH_PROTO      2
#define IPNET_NAT_ICMP4_CODE_DST_UNREACH_PORT       3
#define IPNET_NAT_ICMP4_CODE_DST_NEEDFRAG           4
#define IPNET_NAT_ICMP4_CODE_DST_SRCFAIL            5
#define IPNET_NAT_ICMP4_CODE_DST_UNKNOWN_NET        6
#define IPNET_NAT_ICMP4_CODE_DST_UNKNOWN_HOST       7
#define IPNET_NAT_ICMP4_CODE_DST_ISOLATED           8
#define IPNET_NAT_ICMP4_CODE_DST_PROHIBITED_NET     9
#define IPNET_NAT_ICMP4_CODE_DST_PROHIBITED_HOST    10
#define IPNET_NAT_ICMP4_CODE_DST_UNREACH_TOS_NET    11
#define IPNET_NAT_ICMP4_CODE_DST_UNREACH_TOS_HOST   12


/*
 *===========================================================================
 *                         ICMP query messages common length
 *===========================================================================
 */
#define IPNET_NAT_ICMP_QUERY_LENGTH             8


/*
 *===========================================================================
 *                         NAT return codes
 *===========================================================================
 */
#define IPNET_NAT_PT_TRANSLATED     2   /* Applied NAT-PT */
#define IPNET_NAT_TRANSLATED        1   /* Applied NAT */
#define IPNET_NAT_NOMATCH           0   /* No matching rule */
#define IPNET_NAT_INVALID          -1   /* Cannot NAT packet */
#define IPNET_NAT_DROP             -2   /* Drop packet */


/*
 *===========================================================================
 *                         ICMP request type
 *===========================================================================
 */
#define IPNET_NAT_IS_ICMP_REQUEST(type)  (type == IPCOM_ICMP_ECHO_REQUEST || \
                                          type == IPCOM_ICMP_TSTAMP_REQUEST || \
                                          type == IPCOM_ICMP_INFO_REQUEST || \
                                          type == IPCOM_ICMP_MASK_REQUEST)

/*
 *===========================================================================
 *                         ICMP reply type
 *===========================================================================
 */
#define IPNET_NAT_IS_ICMP_REPLY(type)    (type == IPCOM_ICMP_ECHO_REPLY || \
                                          type == IPCOM_ICMP_TSTAMP_REPLY || \
                                          type == IPCOM_ICMP_INFO_REPLY || \
                                          type == IPCOM_ICMP_MASK_REPLY)

/*
 *===========================================================================
 *                         ICMP error type
 *===========================================================================
 */
#define IPNET_NAT_IS_ICMP_ERROR(type)    (type == IPCOM_ICMP_SOURCEQUENCH || \
                                          type == IPCOM_ICMP_UNREACH || \
                                          type == IPCOM_ICMP_PARAMPROB || \
                                          type == IPCOM_ICMP_TIMEXCEED || \
                                          type == IPCOM_ICMP_REDIRECT)

/*
 *===========================================================================
 *                         Link header size
 *===========================================================================
 * Assume Ethernet and VLAN (IPNET will increase header space if required)
 */
#define IPNET_NAT_LINK_HDR_SIZE (14 + 4)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_nat_pseudo_hdr
 *===========================================================================
 * Pseudo header for IPv4 used in UDP/TCP checksum calculations.
 *
 */
#include <ipcom_align16.h>
typedef IP_PACKED1 struct Ipnet_nat_pseudo_hdr_struct
{
    Ip_u32 src_n;
    Ip_u32 dst_n;
    Ip_u8  mbz;
    Ip_u8  protocol;
    Ip_u16 length_n;
}
IP_PACKED2 Ipnet_nat_pseudo_hdr;
#include <ipcom_align16.h>


/*
 *===========================================================================
 *                         Ipnet_nat_stats
 *===========================================================================
 * NAT statistics.
 */
typedef struct Ipnet_nat_stats_struct
{
    Ip_u32  translated_in;
    Ip_u32  translated_out;
    Ip_u32  invalid_in;
    Ip_u32  invalid_out;
    Ip_u32  dropped_in;
    Ip_u32  dropped_out;
    Ip_u32  no_rule_match_in;
    Ip_u32  no_rule_match_out;
    Ip_u32  mappings_added;
    Ip_u32  mappings_expired;
    Ip_u32  mapping_failures;
}
Ipnet_nat_stats;


/*
 *===========================================================================
 *                        Ipnet_nat_data_inner
 *===========================================================================
 *
 */
struct Ipnet_nat_data_inner
{
    Ip_s32 open;                    /* Opened or closed */
    Ip_u16 port_lo;                 /* Start port for automatically NATed ports */
    Ip_u16 port_hi;                 /* End port for automatically NATed ports */
    Ip_u32 second;                  /* Current nat time */
    Ipnet_nat_stats stats;          /* Statistics */
    Ip_s32 timeout_icmp;            /* icmp timeout */
    Ip_s32 timeout_udp;             /* udp timeout */
    Ip_s32 timeout_tcp;             /* tcp timeout */
    Ip_s32 timeout_other;           /* tcp other */
    Ip_s32 max_mappings;            /* max number of mappings */
};


/*
 *===========================================================================
 *                         Ipnet_nat_data
 *===========================================================================
 * NAT data.
 */
typedef struct Ipnet_nat_data_struct
{
    Ipcom_list head_rule;           /* Rule list */
    Ipcom_list head_proxy;          /* Proxy list */
    Ipcom_list head_mapping;        /* Mapping list */
    Ipcom_hash *hash_out_port;      /* Hash table for outgoing packets with port protocols */
    Ipcom_hash *hash_out_noport;    /* Hash table for outgoing packets with noport protocols */
    Ipcom_hash *hash_in_port;       /* Hash table for incoming packets with port protocols */
    Ipcom_hash *hash_in_noport;     /* Hash table for incoming packets with noport protocols */
    Ip_u32 *ports;                  /* Pointer to port array for automatically NATed ports */
    struct Ip_in_addr *origfrom;    /* original param->from */
    struct Ip_in_addr newsource;    /* new source address */
    struct Ipnet_nat_data_inner inner;
}
Ipnet_nat_data;


/*
 *===========================================================================
 *                         Ipnet_nat_tuple
 *===========================================================================
 * NAT packet tuple.
 */
typedef struct Ipnet_nat_tuple_struct
{
    Ip_u8   protocol;
    Ip_u8   icmp_type;
    Ip_u16  icmp_id_n;
    Ip_u16  src_port_n;
    Ip_u16  dst_port_n;
    Ip_u32  src_n;
    Ip_u32  dst_n;
    Ip_u32  fragid;
    Ip_u16  fragoff;
    Ip_u16  tcp_flags;
    Ip_u8   fragmf;
    Ip_u8   tos;
    Ip_u8   ttl;
#ifdef IPNET_USE_NAT_PT
    struct Ip_in6_addr  pt_src;
    struct Ip_in6_addr  pt_dst;
#endif
}
Ipnet_nat_tuple;


/*
 *===========================================================================
 *                         Ipnet_nat_rule_inner
 *===========================================================================
 *
 */
struct Ipnet_nat_rule_inner
{
    /* Actions */
    Ip_u8      map;         /* Do not add any entries before the map member */
    Ip_u8      map_block;
    Ip_u8      rdr;
    Ip_u8      pt;

    Ip_u8      pt_block;
    Ip_u8      proxy_nonapt;
    Ip_u8      unused[2];

    /* Interface */
    char       ifname[IP_IFNAMSIZ];

    /* Source address/mask */
    Ip_u32     src_n;
    Ip_u32     src_mask_n;
#ifdef IPNET_USE_NAT_PT
    struct Ip_in6_addr src_n_pt;
    struct Ip_in6_addr src_mask_n_pt;
#endif

    /* Destination address/mask */
    Ip_u32     dst_n;
    Ip_u32     dst_mask_n;

    /* NAT address/mask */
    Ip_u32     nat_addr_n;
    Ip_u32     nat_mask_n;
#ifdef IPNET_USE_NAT_PT
    struct Ip_in6_addr nat_addr_n_pt;
    struct Ip_in6_addr nat_mask_n_pt;
#endif

    /* portmap and icmpidmap keyword */
    Ip_u8      icmpidmap;
    Ip_u8      portmap;
    Ip_u8      portmap_tcpudp;
    Ip_u8      portmap_protocol;
    Ip_u16     port_lo;
    Ip_u16     port_hi;

    /* proxy keyword */
    Ip_u8      proxy;
    Ip_u8      proxy_protocol;
    Ip_u16     proxy_trigger;
    char       proxy_label[IPNET_NAT_PROXY_LABEL_LEN];

    /* Redirection keyword */
    Ip_u16     rdr_old_port_n;
    Ip_u16     rdr_new_port_n;
    Ip_u8      rdr_tcpudp;
    Ip_u8      rdr_protocol;
};


/*
 *===========================================================================
 *                         Ipnet_nat_rule
 *===========================================================================
 *  NAT rule
 */
typedef struct Ipnet_nat_rule_struct
{
    Ipcom_list list_rule;   /* Keep list entry first */
    Ip_u32     *ports;
    Ipnet_nat_proxy_func proxy_func;
    void      *proxy_cookie;
    struct Ipnet_nat_rule_inner inner;
}
Ipnet_nat_rule;


/*
 *===========================================================================
 *                        Ipnet_nat_mapping_inner
 *===========================================================================
 *
 */
struct Ipnet_nat_mapping_inner
{

    Ip_u32               tmo_abs_sec;    /* Absolute timeout in seconds */
    Ip_u32               tmo_int_sec;    /* Timeout interval seconds */
    Ipnet_nat_tuple      tuple;          /* Packet tuple */
    Ip_u32               nat_addr_n;     /* New address */
    Ip_u16               nat_port_n;     /* New port */
    Ip_u8                inner_state;    /* State inner */
    Ip_u8                outer_state;    /* State outer */
    Ip_u32               seq_start;      /* Initial sequence number */
    Ip_s32               curr_delta;     /* Current delta sequence number */
    Ip_s32               prev_delta;     /* Previous delta sequence number */
    Ip_u8                seq_initiated;  /* Sequence number initiated */
    Ip_u8                seq_incoming;   /* Incoming sequence number */
    Ip_u8                pt;             /* Set for pt mappings when listed */
    Ip_u8                rdr;            /* Mapping created by redirect rule */
    Ip_u32               autoport;       /* Set when automatic port is allocated */
#ifdef IPNET_USE_NAT_PT
    struct Ip_in6_addr   nat_addr_n_pt;  /* New address */
#endif
};


/*
 *===========================================================================
 *                         Ipnet_nat_mapping
 *===========================================================================
 *  NAT mapping.
 */
typedef struct Ipnet_nat_mapping_struct
{
    Ipcom_list           list;           /* Keep list entry first */
    Ipnet_timeout       *tmo;            /* Timeout handler */
    Ipnet_nat_rule      *rule;           /* Reference to the rule */
    Ipnet_nat_proxy_func proxy_func;     /* Proxy function */
    void                *proxy_cookie;   /* Proxy cookie */
    struct Ipnet_nat_mapping_inner inner;
}
Ipnet_nat_mapping;


/*
 *===========================================================================
 *                         Ipnet_nat_hash_key
 *===========================================================================
 * NAT hash key.
 */
typedef struct Ipnet_nat_hash_key_struct
{
    Ip_u8   protocol;
    Ip_u16  src_port_n;
    Ip_u16  dst_port_n;
    Ip_u32  src_n;
    Ip_u32  dst_n;
    Ip_u32  nat_addr_n;
    Ip_u16  nat_port_n;
}
Ipnet_nat_hash_key;


/*
 *===========================================================================
 *                        Ipnet_nat_proxy_inner
 *===========================================================================
 *
 */
struct Ipnet_nat_proxy_inner
{
    char  label[IPNET_NAT_PROXY_LABEL_LEN];
    Ip_u8 protocol;
};


/*
 *===========================================================================
 *                         Ipnet_nat_proxy
 *===========================================================================
 * NAT proxy.
 */
typedef struct Ipnet_nat_proxy_struct
{
    Ipcom_list list_proxy;  /* Keep list entry first */
    Ipnet_nat_proxy_func  func;
    void                 *cookie;
    struct Ipnet_nat_proxy_inner inner;
}
Ipnet_nat_proxy;


/*
 *===========================================================================
 *                         Ipnet_nat_ctrl
 *===========================================================================
 * NAT control structure for the IP_IP_NAT socket option.
 */
typedef struct Ipnet_nat_ctrl_struct
{
    Ip_s32 cmd;     /* Control command */
    Ip_s32 seqno;   /* Optional sequence number */
    union
    {
        struct Ipnet_nat_data_inner    info;
        struct Ipnet_nat_rule_inner    rule_info;
        struct Ipnet_nat_mapping_inner mapping_info;
        struct Ipnet_nat_proxy_inner   proxy_info;
    }
    type;   /* Optional type */
}
Ipnet_nat_ctrl;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void   ipnet_nat_translate_tcpudp_port(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping);
IP_GLOBAL void   ipnet_nat_checksum_update(unsigned char *chksum, unsigned char *optr, int olen, unsigned char *nptr, int nlen);
IP_GLOBAL void   ipnet_nat_update_sequence_number(Ip_bool incoming, Ipnet_nat_mapping *nat_mapping, Ip_u32 seq_num, Ip_s32 delta);
IP_GLOBAL void   ipnet_nat_translate_sequence_number(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping);
IP_GLOBAL void   ipnet_nat_translate_ack_number(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping);
IP_GLOBAL void   ipnet_nat_update_mapping(Ip_bool incoming, Ipnet_nat_mapping *nat_mapping, Ipnet_nat_tuple *nat_tuple);

IP_GLOBAL Ip_s32 ipnet_nat_match_rule(Ip_bool incoming, Ipnet_nat_rule *nat_rule, Ip_u32 ifindex, Ipnet_nat_tuple *nat_tuple);
IP_GLOBAL Ip_s32 ipnet_nat_proxy(Ip_bool incoming, Ipcom_pkt **pktp, Ipnet_nat_tuple *nat_tuple, Ipnet_nat_mapping *nat_mapping,
                                 int hlen, Ip_bool *mod, Ip_s32 *delta);
IP_GLOBAL Ip_s32 ipnet_nat_protocol_number(const char *name);
IP_GLOBAL Ip_s32 ipnet_nat_output_hook(Ipcom_pkt **pktp, Ip_bool no_rules);
IP_GLOBAL Ip_s32 ipnet_nat_input_hook(Ipcom_pkt **pktp, void *param);

IP_GLOBAL Ipnet_nat_mapping *ipnet_nat_add_mapping(Ip_bool incoming, Ipnet_nat_rule *nat_rule, Ipnet_nat_tuple *nat_tuple,
                                                   Ip_u32 timeout, Ipnet_nat_mapping *parent, Ip_bool napt,
                                                   Ipnet_nat_proxy_func proxy_func, void *proxy_cookie);

IP_GLOBAL Ipnet_nat_mapping *ipnet_nat_find_mapping(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_tuple *nat_tuple);

IP_GLOBAL char  *ipnet_nat_protocol_name(Ip_u8 protocol, char *protostr, Ip_u32 protostrlen);
IP_GLOBAL Ip_err ipnet_nat_init(void);

#ifdef IPNET_USE_NAT_PT
IP_GLOBAL Ip_s32 ipnet_nat_pt_translate(Ip_bool incoming, Ipnet_nat_mapping *nat_mapping, Ipcom_pkt **pktp,
                                        Ipnet_nat_tuple *nat_tuple, void *param);
IP_GLOBAL Ip_s32 ipnet_nat_pt_output_hook(Ipnet_ip4_output_param *param4, Ipcom_pkt **pktp);
#endif

IP_PUBLIC int ipnet_cmd_nat(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
