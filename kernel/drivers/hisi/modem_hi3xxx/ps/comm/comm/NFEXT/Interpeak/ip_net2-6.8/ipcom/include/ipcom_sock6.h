/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sock6.h,v $ $Revision: 1.79 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_sock6.h,v $
 *     $Author: jhorteli $ $Date: 2009-02-25 14:31:21 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

#ifndef IPCOM_SOCK6_H
#define IPCOM_SOCK6_H

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

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_cstyle.h"
#include "ipcom_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* IPv6 Non-IP Protocol Next Header field values
 * Note: Also see ipcom_sock.h for additonal IP_IPPROTO_xxx defines.
 */
#define IP_IPPROTO_HOPOPTS   0   /* IPv6 Hop-by-Hop options */
#define IP_IPPROTO_ROUTING  43   /* IPv6 Routing header */
#define IP_IPPROTO_FRAGMENT 44   /* IPv6 fragment header */
#define IP_IPPROTO_NONE     59   /* IPv6 no next header */
#define IP_IPPROTO_DSTOPTS  60   /* IPv6 Destination options */


/* Max length of an address created by ipcom_inet_ntop(IP_AF_INET6, ...) */
#define IP_INET6_ADDRSTRLEN   46

/* Macro initializers for standard addresses */
#define IP_IN6ADDR_ANY_INIT \
    {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}}
#define IP_IN6ADDR_LOOPBACK_INIT \
    {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}

/*
 *===========================================================================
 *                      IPv6 address macros
 *===========================================================================
 */

#define IP_IN6_ARE_ADDR_EQUAL(addr1, addr2) \
    (ipcom_memcmp(addr1, addr2, sizeof(struct Ip_in6_addr)) == 0)

#define IP_IN6_IS_ADDR_UNSPECIFIED(addr) \
    (IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[0]) == 0 && \
     IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[1]) == 0 && \
     IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[2]) == 0 && \
     IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[3]) == 0)

#define IP_IN6_IS_ADDR_LOOPBACK(addr) \
    (IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[0]) == 0 && \
     IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[1]) == 0 && \
     IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[2]) == 0 && \
     IP_GET_32ON16(&((struct Ip_in6_addr *)(addr))->in6.addr32[3]) == ip_ntohl(1))

#define IP_IN6_IS_ADDR_V4COMPAT(addr) \
    (IP_GET_32ON16(&(addr)->in6.addr32[0]) == 0 && \
     IP_GET_32ON16(&(addr)->in6.addr32[1]) == 0 && \
     IP_GET_32ON16(&(addr)->in6.addr32[2]) == 0 && \
     IP_GET_32ON16(&(addr)->in6.addr32[3]) != 0 && /* Must not return TRUE for "::" and "::1" */ \
     IP_GET_32ON16(&(addr)->in6.addr32[3]) != ip_ntohl(1))

#define IP_IN6_IS_ADDR_V4MAPPED(addr) \
    (IP_GET_32ON16(&(addr)->in6.addr32[0]) == 0 && \
     IP_GET_32ON16(&(addr)->in6.addr32[1]) == 0 && \
     IP_GET_32ON16(&(addr)->in6.addr32[2]) == ip_ntohl(0x0000ffff))

/* RFC 4291 definition of global unicast address */
#define IP_IN6_IS_ADDR_AGGR_GLOB_UNICAST(addr) \
    !IP_IN6_IS_ADDR_UNSPECIFIED(addr) \
    && !IP_IN6_IS_ADDR_LOOPBACK(addr) \
    && !IP_IN6_IS_ADDR_LINK_LOCAL(addr) \
    && !IP_IN6_IS_ADDR_MULTICAST(addr)

#define IP_IN6_IS_ADDR_LINK_LOCAL(addr) \
    ((ip_ntohs((addr)->in6.addr16[0]) & 0xffc0) == 0xfe80)

#define IP_IN6_IS_ADDR_SITE_LOCAL(addr) \
    ((ip_ntohs((addr)->in6.addr16[0]) & 0xffc0) == 0xfec0)


#define IP_IN6_IS_ADDR_MULTICAST(addr)   (((Ip_u8 *)(addr))[0] == 0xff) /* note: avoid union use due to gcc/sparc behaviour */


#define IP_IN6_IS_ADDR_MC_NODELOCAL(addr) \
    (IP_IN6_IS_ADDR_MULTICAST(addr) && ((((Ip_u8 *)(addr))[1] & 0xf) == 0x1))

#define IP_IN6_IS_ADDR_MC_LINKLOCAL(addr) \
    (IP_IN6_IS_ADDR_MULTICAST(addr) && ((((Ip_u8 *)(addr))[1] & 0xf) == 0x2))

#define IP_IN6_IS_ADDR_MC_SITELOCAL(addr) \
    (IP_IN6_IS_ADDR_MULTICAST(addr) && ((((Ip_u8 *)(addr))[1] & 0xf) == 0x5))

#define IP_IN6_IS_ADDR_MC_ORGLOCAL(addr) \
    (IP_IN6_IS_ADDR_MULTICAST(addr) && ((((Ip_u8 *)(addr))[1] & 0xf) == 0x8))

#define IP_IN6_IS_ADDR_MC_GLOBAL(addr) \
    (IP_IN6_IS_ADDR_MULTICAST(addr) && ((((Ip_u8 *)(addr))[1] & 0xf) == 0xe))


/* Interpeak extensions */
#define IPCOM_IN6_IS_ADDR_SCOPED(addr) \
    (IP_IN6_IS_ADDR_LINK_LOCAL(addr) || IP_IN6_IS_ADDR_MC_LINKLOCAL(addr) || \
     IP_IN6_IS_ADDR_SITE_LOCAL(addr) || IP_IN6_IS_ADDR_MC_SITELOCAL(addr))

/* Check for RFC 4193 unique local IPv6 unicast addresses */
#define IPCOM_IN6_IS_ADDR_UNIQUE_LOCAL_UNICAST(addr) \
   ((((Ip_u8 *)(addr))[0] & 0xfe) == 0xfc)


/*
 *===========================================================================
 *                         setsockopt/getsockopt
 *===========================================================================
 * IP_IPPROTO_IPV6 level socket options.
 */

#define IP_IPV6_V6ONLY            1   /* int; make IP_AF_INET6 sockets accept IPv6 only, not
                                         IPv6 mapped IPv4. This option is off by default. */
#define IP_IPV6_UNICAST_HOPS      4   /* int; set/get IPv6 hop limit */
#define IP_IPV6_CHECKSUM          7   /* int; checksum offset for raw socket  */
#define IP_IPV6_MULTICAST_IF      9   /* unsigned int; controls which interface multicast packets sent from
                                         this socket should use */
#define IP_IPV6_MULTICAST_HOPS   10   /* int; set/get IPv6 multicast hop limit */
#define IP_IPV6_MULTICAST_LOOP   11   /* unsigned int; Controls if multicast packets should be looped back */
#define	IP_IPV6_JOIN_GROUP       12   /* struct Ip_ipv6_mreq; add an IPv6 group membership */
#define IP_IPV6_ADD_MEMBERSHIP   IP_IPV6_JOIN_GROUP
#define	IP_IPV6_LEAVE_GROUP      13   /* struct Ip_ipv6_mreq; drop an IPv6 group membership */
#define IP_IPV6_DROP_MEMBERSHIP  IP_IPV6_LEAVE_GROUP
#define IP_IPV6_JOIN_ANYCAST     14
#define IP_IPV6_LEAVE_ANYCAST    15


    /* 'sticky' means that the ancillary data will be applied to each sent packet
        on that socket unless the ancillary data is explicitly specified when
        calling ipcom_sendmsg() (in which case the specified data is used instead) */
#define IP_IPV6_PKTINFO          20   /* struct Ip_in6_pktinfo; set/get sticky packet info */
#define IP_IPV6_TCLASS           22   /* int; set/get sticky traffic class */
#define IP_IPV6_NEXTHOP          23   /* struct Ip_sockaddr_in6; set/get sticky address of the next hop */
#define IP_IPV6_RTHDR            24   /* struct Ip_ip6_rthdr; set/get sticky route header option */
#define IP_IPV6_HOPOPTS          25   /* struct Ip_ip6_hbh; set/get sticky hop by hop option */
#define IP_IPV6_DSTOPTS          26   /* struct Ip_ip6_dest; set/get sticky destination option */
#define IP_IPV6_RTHDRDSTOPTS     27   /* struct Ip_ip6_dest; set/get sticky destination option after routing option */
#define IP_IPV6_SAINFO           28   /* struct Ip_ip6_sainfo; set/get sticky SA info options */

#define IP_IPV6_RECVPKTINFO      30   /* int; turn "get packet information ancillary data" on/off */
#define IP_IPV6_RECVHOPLIMIT     31   /* int; turn "get the hoplimit ancillary data" on/off */
#define IP_IPV6_RECVTCLASS       32   /* int; turn "get the traffic class ancillary data" on/off */
#define IP_IPV6_RECVRTHDR        34   /* int; turn "get the routing header ancillary data" on/off  */
#define IP_IPV6_RECVHOPOPTS      35   /* int; turn "get the hop by hop option header ancillary data" on/off */
#define IP_IPV6_RECVDSTOPTS      36   /* int; turn "get the destination option header ancillary data" on/off */
#define IP_IPV6_RECVSAINFO       37   /* int; turn "get the SA information" on/off */
#define IP_IPV6_ADDR_PREFERENCES 38   /* int; set/get sticky address preference flags (IP_IPV6_PREFER_SRC_xxx)
                                         Implemented from "draft-chakrabarti-ipv6-addrselect-api-03" */
/* NOTE: 42-47 is used by MCAST_xxx macros */
#define IP_IPV6_FW               51   /* Ipfirewall_ctrl; Firewall control */
#define IP_IPV6_RECVIF           100   /* int; turn "get link layer address(as a struct Ip_sockaddr_dl)
                                          of the interface the packet was received on from ipcom_recvmsg()" on/off */
#define IP_IPV6_X_SENDERLINKADDR 101   /* int; ipcom_recvmsg() returns a struct Ip_sockaddr_dl
                                          that will contain the link address of the sender */
#define IP_IPV6_X_UDPENCAP       102   /* struct Ip_sockaddr; The outer UDP encap source for a UDP Tunneled Packet  */

    /* NOTE: These options can NOT be used with getsockopt/setsockopt!
   Only sent/rcvd with ancillary data as described by RFC3542 */
#define IP_IPV6_HOPLIMIT         21   /* int; IPv6 header hoplimit. */


/*
 *===========================================================================
 *                         setsockopt/getsockopt
 *===========================================================================
 * IP_IPPROTO_IPV6 level multicast socket options.
 */
#define IP_MRT6_BASE      0x60000000
#define IP_MRT6_INIT      (IP_MRT6_BASE + 0)    /* int; mrouted is starting */
#define IP_MRT6_DONE      (IP_MRT6_BASE + 1)    /* int; mrouted is shutting down */
#define IP_MRT6_ADD_MIF   (IP_MRT6_BASE + 2)    /* struct Ip_mif6ctl; add virtual interface */
#define IP_MRT6_DEL_MIF   (IP_MRT6_BASE + 3)    /* struct Ip_mif6ctl; delete virtual interface */
#define IP_MRT6_ADD_MFC   (IP_MRT6_BASE + 4)    /* struct Ip_mf6cctl; add a multicast forwarding entry */
#define IP_MRT6_DEL_MFC   (IP_MRT6_BASE + 5)    /* struct Ip_mf6cctl; delete a multicast forwarding entry */
#define IP_MRT6_PIM       (IP_MRT6_BASE + 6)    /* int; enable PIM */


/*
 *===========================================================================
 *                         setsockopt/getsockopt
 *===========================================================================
 * IP_IPPROTO_ICMPV6 level socket options.
 */
#define IP_ICMP6_FILTER         1   /* struct Ip_icmp6_filter; set/get the ICMPv6 message filter */


/*
 *===========================================================================
 *                         setsockopt/getsockopt
 *===========================================================================
 * IP_IPPROTO_IPV6 level and IP_IPV6_ADDR_PREFERENCES type flags.
 */
#define IP_IPV6_PREFER_SRC_HOME       (1u << 0) /* Prefer Home Address as source (not implemented) */
#define IP_IPV6_PREFER_SRC_COA        (1u << 1) /* Prefer Care-Of_address as source (not implemented) */
#define IP_IPV6_PREFER_SRC_TMP        (1u << 2) /* Prefer Temporary address as source */
#define IP_IPV6_PREFER_SRC_PUBLIC     (1u << 3) /* Prefer Public address as source */
#define IP_IPV6_PREFER_SRC_CGA        (1u << 4) /* Prefer CGA address as source */
#define IP_IPV6_PREFER_SRC_NONCGA     (1u << 5) /* Prefer a non-CGA address as source (not implemented) */
#define IP_IPV6_PREFER_SRC_LARGESCOPE (1u << 6) /* Prefer larger scope source */
#define IP_IPV6_PREFER_SRC_SMALLSCOPE (1u << 7) /* Prefer smaller scope source */


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ip_in6_addr
 *===========================================================================
 * 128-bit IP6 address.
 */
#include "ipcom_align16.h"
IP_PACKED1 struct Ip_in6_addr
{
    union Ip_in6_addr_union
    {
        Ip_u8    addr8[16];
        Ip_u16   addr16[8];
        Ip_u32   addr32[4];
    } in6;
}
IP_PACKED2;

#ifndef IP_PORT_LAS
#ifndef s6_addr
#define s6_addr   in6.addr8
#endif
#endif

IP_EXTERN IP_CONST struct Ip_in6_addr ip_in6addr_any;
IP_EXTERN IP_CONST struct Ip_in6_addr ip_in6addr_loopback;


/*
 *===========================================================================
 *                    Ip_sockaddr_in6
 *===========================================================================
 */
IP_PACKED1 struct Ip_sockaddr_in6
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8	            sin6_len;	     /* length of this struct */
#endif
    Ip_sa_family_t      sin6_family;	 /* IP_AF_INET6 (8bits) */
    Ip_in_port_t        sin6_port;	     /* Transport layer port */
    Ip_u32	            sin6_flowinfo;	 /* IP6 flow information */

    struct Ip_in6_addr  sin6_addr;	     /* IPv6 address */

    Ip_u32	            sin6_scope_id;	 /* interface scope id */
}
IP_PACKED2;
#include "ipcom_align16.h"

/* rfc2553: Ip_sockaddr_in6 has sin6_len field. */
#ifdef IPCOM_USE_SA_LEN
#define SIN6_LEN
#endif

/*
 *===========================================================================
 *                    Ip_in6_pktinfo
 *===========================================================================
 * Structure for ipcom_socketioctl() IP_IPV6_PKTINFO ioctl.
 */
struct Ip_in6_pktinfo
{
    struct Ip_in6_addr  ipi6_addr;      /* local src/dst IPv6 address of sent/recv packet */
    Ip_u32              ipi6_ifindex;   /* send/recv interface index */
};


/*
 *===========================================================================
 *                    Ip_in6_sainfo
 *===========================================================================
 * Structure for ipcom_socketioctl() IP_IPV6_SAINFO ioctl.
 */
struct Ip_in6_sainfo
{
    struct Ip_in6_addr  ipsa6_dst;      /* Destination address  */
    Ip_u32              ipsa6_spi;      /* SPI used             */
    Ip_u16              ipsa6_protocol; /* Protocol used        */
    Ip_u16              ipsa6_nesting;  /* 0 if it's the final packet, > 0 if
                                           it's a tunnel */
};


/*
 *===========================================================================
 *          Ip_ipv6_mreq - IPv6 multicast socket options structure
 *===========================================================================
 * Argument structure for IP_IPV6_JOIN_GROUP and IP_IPV6_LEAVE_GROUP.
 */
struct Ip_ipv6_mreq
{
    struct Ip_in6_addr    ipv6mr_multiaddr;   /* IPv6 multicast addr */
    unsigned int          ipv6mr_interface;   /* interface index, or 0 */
};

#define ipv6mr_acaddr	ipv6mr_multiaddr

/*
 *===========================================================================
 *                         IPv6 Extension Headers
 *===========================================================================
 */

/* Generic Extension Header */
typedef struct Ip_pkt_ip6_ext_hdr_struct
{
    /* Next header field */
    Ip_u8  nxt;
    /* Length of of extension in 8-octets units,
       not counting the first 8-octets */
    Ip_u8  len;
}
Ip_pkt_ip6_ext_hdr;


/* Hop-by-Hop options header */
typedef struct Ip_ip6_hbh
{
    Ip_u8   ip6h_nxt;        /* next header */
    Ip_u8   ip6h_len;        /* length in units of 8 octets */
    /* followed by options */
}
Ip_pkt_ip6_hbh;


/* Destination options header */
typedef struct Ip_ip6_dest
{
    Ip_u8  ip6d_nxt;        /* next header */
    Ip_u8  ip6d_len;        /* length in units of 8 octets */
    /* followed by options */
}
Ip_pkt_ip6_dest;


/* Routing header */
typedef struct Ip_ip6_rthdr
{
    Ip_u8  ip6r_nxt;        /* next header */
    Ip_u8  ip6r_len;        /* length in units of 8 octets */
    Ip_u8  ip6r_type;       /* routing type */
    Ip_u8  ip6r_segleft;    /* segments left */
    /* followed by routing type specific data */
}
Ip_pkt_ip6_rthdr;


#define IP_IPV6_RTHDR_TYPE_0    0 /* IPv6 Routing header type 0 */
#define IP_IPV6_RTHDR_TYPE_2    2 /* IPv6 Routing header type 2 */


/* Type 0 Routing header */
typedef struct Ip_ip6_rthdr0
{
    Ip_u8  ip6r0_nxt;       /* next header */
    Ip_u8  ip6r0_len;       /* length in units of 8 octets */
    Ip_u8  ip6r0_type;      /* always zero */
    Ip_u8  ip6r0_segleft;   /* segments left */
    Ip_u32 ip6r0_reserved;  /* reserved field */
    /* followed by up to 127 struct Ip_in6_addr */
}
Ip_pkt_ip6_rthdr0;

/* Type 2 Routing header */
typedef struct Ip_ip6_rthdr2
{
    Ip_u8               ip6r2_nxt;       /* next header */
    Ip_u8               ip6r2_len;       /* length in units of 8 octets */
    Ip_u8               ip6r2_type;      /* always zero */
    Ip_u8               ip6r2_segleft;   /* segments left */
    Ip_u32              ip6r2_reserved;  /* reserved field */
    struct Ip_in6_addr  ip6r2_home;      /* Home address */
}
Ip_pkt_ip6_rthdr2;


/* Fragment header */
typedef struct Ip_ip6_frag
{
    Ip_u8   ip6f_nxt;       /* next header */
    Ip_u8   ip6f_reserved;  /* reserved field */
    Ip_u16  ip6f_offlg;     /* offset, reserved, and flag */
    Ip_u32  ip6f_ident;     /* identification */
}
Ip_pkt_ip6_frag;

#ifdef IP_LITTLE_ENDIAN
    #define IP_IP6F_OFF_MASK       0xf8ff  /* mask out offset from ip6f_offlg */
    #define IP_IP6F_RESERVED_MASK  0x0600  /* reserved bits in ip6f_offlg */
    #define IP_IP6F_MORE_FRAG      0x0100  /* more-fragments flag */
#else
    #define IP_IP6F_OFF_MASK       0xfff8  /* mask out offset from
    ip6f_offlg */
    #define IP_IP6F_RESERVED_MASK  0x0006  /* reserved bits in
    ip6f_offlg */
    #define IP_IP6F_MORE_FRAG      0x0001  /* more-fragments flag */
#endif

#define IP_IP6F_SIZE             sizeof(Ip_pkt_ip6_frag)
#define IP_IP6F_GET_OFFSET(hdr)  (ip_ntohs((hdr)->ip6f_offlg & IP_IP6F_OFF_MASK))



/*
 *===========================================================================
 *                      Extension Header Options
 *===========================================================================
 */

/* Generic Option Header */
typedef struct Ip_ip6_opt
{
    Ip_u8  ip6o_type;   /* Option type, IP_IP6OPT_xxx  */
    Ip_u8  ip6o_len;    /* Length in octets */
}
Ip_pkt_ip6_opt;

/* Names and values from RFC 3542 */
#define IP_IP6OPT_TYPE(o)                       ((o) & 0xc0)
#define IP_IP6OPT_TYPE_SKIP                     0x00   /* skip over this option and continue processing the header */
#define IP_IP6OPT_TYPE_DISCARD                  0x40   /* discard the packet */
#define IP_IP6OPT_TYPE_FORCEICMP                0x80   /* discard the packet and, regardless of whether or not the
                                                          packet's Destination Address was a multicast address, send an
                                                          ICMP Parameter Problem, Code 2, message to the packet's
                                                          Source Address, pointing to the unrecognized Option Type */
#define IP_IP6OPT_TYPE_ICMP                     0xc0   /* discard the packet and, only if the packet's Destination
                                                          Address was not a multicast address, send an ICMP Parameter
                                                          Problem, Code 2, message to the packet's Source Address,
                                                          pointing to the unrecognized Option Type.*/
#define IP_IP6OPT_MUTABLE                       0x20   /* Option Data may change en-route */

#define IP_IP6OPT_PAD1                          0x00  /* 00 0 00000 */
#define IP_IP6OPT_PADN                          0x01  /* 00 0 00001 */

#define IP_IP6OPT_JUMBO                         0xc2  /* 11 0 00010 (not supported) */
#define IP_IP6OPT_NSAP_ADDR                     0xc3  /* 11 0 00011 (not supported) */
#define IP_IP6OPT_TUNNEL_LIMIT                  0x04  /* 00 0 00100 (not supported) */
#define IP_IP6OPT_ROUTER_ALERT                  0x05  /* 00 0 00101 */
#define IP_IP6OPT_DSTOPT_HOME_ADDRESS           0x09  /* 11 0 01001 */


/* Router Alert Option */
struct Ip_ip6_opt_router
{
    Ip_u8   ip6or_type;
    Ip_u8   ip6or_len;
    Ip_u8   ip6or_value[2];
};

/* Router alert values (in network byte order) */
#define IP_IP6_ALERT_MLD     ip_htons(0x0000)   /* Datagram contains a Multicast Listener Discovery
                                                   message */
#define IP_IP6_ALERT_RSVP    ip_htons(0x0001)   /* Datagram contains RSVP message */
#define IP_IP6_ALERT_AN      ip_htons(0x0002)   /* Datagram contains an Active Networks message */


/*
 *===========================================================================
 *          Ip_icmp6_filter - IPv6 icmp message filter
 *===========================================================================
 * Argument structure for IP_ICMP6_FILTER.
 */
struct Ip_icmp6_filter
{
    Ip_u32  icmp6_filt[8];  /* 8*32 = 256 bits */
};

/*
 *===========================================================================
 *                    IP_ICMP6_FILTER_WILLPASS
 *===========================================================================
 * Description: Returns if the specified type is passed.
 * Parameters:  [in] type - An ICMPv6 message type.
 *              [in] filterp - The filter to check.
 * Returns:     IP_TRUE if the message type is passed.
 *
 */
#define IP_ICMP6_FILTER_WILLPASS(type, filterp) \
    ((((filterp)->icmp6_filt[(type) >> 5]) & \
        (1 << ((type) & 31))) != 0)


/*
 *===========================================================================
 *                    IP_ICMP6_FILTER_WILLBLOCK
 *===========================================================================
 * Description: Returns if the specified type is blocked.
 * Parameters:  [in] type - An ICMPv6 message type.
 *              [in] filterp - The filter to check.
 * Returns:     IP_TRUE if the message type is blocked.
 *
 */
#define IP_ICMP6_FILTER_WILLBLOCK(type, filterp) \
    ((((filterp)->icmp6_filt[(type) >> 5]) & \
        (1 << ((type) & 31))) == 0)


/*
 *===========================================================================
 *                    IP_ICMP6_FILTER_SETPASS
 *===========================================================================
 * Description: Modifies the filter to pass the specified message.
 * Parameters:  [in] type - The message type of message to pass to socket(s).
 *              [out] filterp - The filter to modify
 * Returns:
 *
 */
#define IP_ICMP6_FILTER_SETPASS(type, filterp) \
    ((((filterp)->icmp6_filt[(type) >> 5]) |= \
        (1 << ((type) & 31))))


/*
 *===========================================================================
 *                    IP_ICMP6_FILTER_SETBLOCK
 *===========================================================================
 * Description: Modifies the filter to block the specified message.
 * Parameters:  [in] type - The message type of message to pass to socket(s).
 *              [out] filterp - The filter to modify
 * Returns:
 *
 */
#define IP_ICMP6_FILTER_SETBLOCK(type, filterp) \
    ((((filterp)->icmp6_filt[(type) >> 5]) &= \
        ~(1 << ((type) & 31))))


/*
 *===========================================================================
 *                    IP_ICMP6_FILTER_SETPASSALL
 *===========================================================================
 * Description: Modifies the filter to pass all messages.
 * Parameters:  [out] filterp - The filter to modify
 * Returns:
 *
 */
#define IP_ICMP6_FILTER_SETPASSALL(filterp) \
    ipcom_memset((filterp), 0xFF, sizeof(struct Ip_icmp6_filter))


/*
 *===========================================================================
 *                    IP_ICMP6_FILTER_SETBLOCKALL
 *===========================================================================
 * Description: Modifies the filter to block all messages.
 * Parameters:  [out] filterp - The filter to modify
 * Returns:
 *
 */
#define IP_ICMP6_FILTER_SETBLOCKALL(filterp) \
    ipcom_memset((filterp), 0, sizeof(struct Ip_icmp6_filter))


/*
 ****************************************************************************
 * 7                    PORT OVERRIDE INCLUDE
 ****************************************************************************
 */
#if IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
#include "ipcom_psock.h"
#endif


/*
 ****************************************************************************
 * 8                    PORT SPECIFIC TYPES
 ****************************************************************************
 * These types must be after the include of ipcom_psock.h since sizes may
 * be altered in ipcom_psock.h.
 */

struct Ip_ip6_tnl_parm {
	char    name[IP_IFNAMSIZ];	/* name of tunnel device */
	Ip_s32  link;		        /* ifindex of underlying L2 interface */
	Ip_u8   proto;		        /* tunnel protocol */
	Ip_u8   encap_limit;	    /* encapsulation limit for tunnel */
	Ip_u8   hop_limit;		    /* hop limit for tunnel */
	Ip_u32  flowinfo;        	/* traffic class and flowlabel for tunnel */
	Ip_u32  flags;		        /* tunnel flags */
	struct Ip_in6_addr laddr;	/* local tunnel end-point address */
	struct Ip_in6_addr raddr;	/* remote tunnel end-point address */
};

#define IP_IPV6_TLV_TNL_ENCAP_LIMIT         4
#define IP_IPV6_DEFAULT_TNL_ENCAP_LIMIT     4

#define IP_IP6_TNL_F_IGN_ENCAP_LIMIT        0x1
#define IP_IP6_TNL_F_USE_ORIG_TCLASS        0x2
#define IP_IP6_TNL_F_USE_ORIG_FLOWLABEL     0x4
#define IP_IP6_TNL_F_MIP6_DEV               0x8
#define IP_IP6_TNL_F_RCV_DSCP_COPY          0x10

/*
 *===========================================================================
 *                         Ip_in6_ifreq
 *===========================================================================
 * Interface request structure used for socket ioctl's.  All interface
 * ioctl's must have parameter definitions which begin with ifr_name. The
 * remainder may be interface specific.
 */
struct Ip_in6_ifreq
{
    char    ifr_name[IP_IFNAMSIZ];             /* if name, e.g. "eth0" */

    union
    {
        struct Ip_sockaddr_in6  ifru_addr;      /* address */
        struct Ip_sockaddr_in6  ifru_dstaddr;   /* other end of p-to-p link */
        unsigned  ifru_prefixlen;               /* address prefix length */
    }
    ifr_ifru;
};


/*
 *===========================================================================
 *                         Ip_in6_addrlifetime
 *===========================================================================
 * For use with IP_SIOCAIFADDR_IN6 ioctl.
 */
struct Ip_in6_addrlifetime
{
    Ip_time_t   ia6t_expire;    /* valid lifetime expiration time or IPCOM_ADDR_INFINTE */
    Ip_time_t   ia6t_preferred; /* preffered lifetime expiration time or IPCOM_ADDR_INFINTE */
    Ip_u32      ia6t_vltime;    /* valid lifetime (currently not used) */
    Ip_u32      ia6t_pltime;    /* prefix lifetime (currently not used) */
};


/*
 *===========================================================================
 *                         Ip_in6_aliasreq
 *===========================================================================
 * For use with IP_SIOCAIFADDR_IN6 ioctl.
 */
struct Ip_in6_aliasreq
{
    char                     ifra_name[IP_IFNAMSIZ];    /* The interface name. i.e. eth0 */
    struct  Ip_sockaddr_in6  ifra_addr;         /* The address to set */
    struct  Ip_sockaddr_in6  ifra_dstaddr;      /* Peer address for PPP interfaces */
    struct  Ip_sockaddr_in6  ifra_prefixmask;   /* Mask to get the nework ID */
    int                      ifra_flags;        /* IP_IN6_IFF_xxx flags */
    struct Ip_in6_addrlifetime  ifra_lifetime;
};

/* ifra_flags */
#define IP_IN6_IFF_TEMPORARY            0x01    /* RW: This is a temporary address      */
#define IP_IN6_IFF_TENTATIVE            0x02    /* RW: Do duplicate address detection on                                                   this address before assigning it     */
#define IP_IN6_IFF_HOMEADDRESS          0x04    /* RW: This is a homeaddress            */
#define IP_IN6_IFF_DEPRECATED           0x08    /* RO: Deprecated address               */
#define IP_IN6_IFF_AUTOMATIC            0x10    /* RO: Created automatically by IPNET   */
#define IP_IN6_IFF_AUTONOMOUS           0x20    /* RW: Created based on prefix adv      */
#define IP_IN6_IFF_ANYCAST              0x40    /* RW: Anycast address                  */
#define IP_IN6_IFF_CGA                  0x80    /* RW: Cryptographically Generated Address */



/*
 *===========================================================================
 *                    IPv6 multicast routing
 *===========================================================================
 */

typedef Ip_u16 Ip_mifi_t; /* Multicast interface index type */

#define IP_MAXMIFS   64  /* Maximum number of multicast interfaces that can be installed */

/* Control structure used by IP_SIOCGETMIFCNT_IN6 ioctl.
 * Used by a multicast routing daemon to get src-grp packet counts.
 */
struct Ip_sioc_sg_req6
{
    struct Ip_sockaddr_in6 src;  /* Source address                           */
    struct Ip_sockaddr_in6 grp;  /* Multicast group address                  */
    Ip_u64 pktcnt;               /* No of packets routed                     */
    Ip_u64 bytecnt;              /* No of bytes routed                       */
    Ip_u64 wrong_if;             /* No of packets arrived on wrong interface */
};


/* Control structure used by IP_SIOCGETSGCNT_IN6 ioctl.
 * Used by a routing daemon to get multicast interface
 * packet counts.
 */
struct Ip_sioc_mif_req6
{
    Ip_mifi_t mifi;     /* Multicast interface index */
    Ip_u64   icount;    /* Input packet count        */
    Ip_u64   ocount;    /* Output packet count       */
    Ip_u64   ibytes;    /* Input byte count          */
    Ip_u64   obytes;    /* Output byte count         */
};


#ifndef IP_IF_SETSIZE
#define IP_IF_SETSIZE 256
#endif

#define IP_MRT6_NIFBITS (sizeof(Ip_if_mask) * 8) /* Bits per mask */

typedef Ip_u32 Ip_if_mask;

typedef struct Ip_if_set_struct
{
    Ip_if_mask ifs_bits[(((IP_IF_SETSIZE)+((IP_MRT6_NIFBITS)-1))/(IP_MRT6_NIFBITS))];
} Ip_if_set;


 /* IPv6 multicast interface control structure.
  * Argument for IP_MRT6_ADD_MIF and IP_MRT6_DEL_MIF.
  */
struct Ip_mif6ctl
{
    Ip_mifi_t  mif6c_mifi;  /* The index of the mif to be added */
    Ip_u8      mif6c_flags; /* IP_MIFF_ flags as defined below */
    Ip_u16     mif6c_pifi;  /* The index of the physical interface */
};

#define IP_MRT6_MIFF_REGISTER 0x1 /* Mif represents a register end-point */


/* IPv6 multicast route control structure.
 * Argument for IP_MRT6_ADD_MFC and IP_MRT6_ADD_MFC.
 */
struct Ip_mf6cctl
{
    struct Ip_sockaddr_in6 mf6cc_origin;
    struct Ip_sockaddr_in6 mf6cc_mcastgrp;
    Ip_mifi_t              mf6cc_parent;
    Ip_if_set              mf6cc_ifset;
};


/* Macros to operate on the mf6cc_ifset bit array */
#define	IP_MRT6_IF_SET(n, p)	\
((p)->ifs_bits[(n)/IP_MRT6_NIFBITS] |= (1 << ((n) % IP_MRT6_NIFBITS)))
#define	IP_MRT6_IF_CLR(n, p)	\
((p)->ifs_bits[(n)/IP_MRT6_NIFBITS] &= ~(1 << ((n) % IP_MRT6_NIFBITS)))
#define	IP_MRT6_IF_ISSET(n, p) \
((p)->ifs_bits[(n)/IP_MRT6_NIFBITS] & (1 << ((n) % IP_MRT6_NIFBITS)))
#define	IP_MRT6_IF_COPY(f, t) ipcom_memcpy(t, f, sizeof(*(f)))
#define	IP_MRT6_IF_ZERO(p) ipcom_memset(p, 0, sizeof(*(p)))


/*
 *===========================================================================
 *                         Ip_mrt6msg
 *===========================================================================
 * Description: Message structure used for communication from the multicast
 *              forwarding engine to the multicast router daemon.
 *              Is overlaid onto an MLD message header.
 */
struct Ip_mrt6msg
{
    Ip_u8              im6_mbz;     /* Used for user level encapsulation */
    Ip_u8              im6_msgtype; /* Message type as defined below     */
    Ip_u16             im6_mif;     /* Receiving mif                     */
    Ip_u32             im6_pad;     /* Padding                           */
    struct Ip_in6_addr im6_src;     /* Source address                    */
    struct Ip_in6_addr im6_dst;     /* Destination address               */
};

/* Message types for Ip_mrt6msg */
#define IP_MRT6MSG_NOCACHE  1  /* Received packet that did not match any route entry */
#define IP_MRT6MSG_WRONGMIF 2  /* Packet arrived on the wrong interface */
#define IP_MRT6MSG_WHOLEPKT 3  /* PIM register processing */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_inet6_rth_space
 *===========================================================================
 * Description: Returns the number of bytes required to hold a Routing
 *              header of the specified type containing the specified number of
 *              segments (addresses).  For an IPv6 Type 0 Routing header, the number
 *              of segments must be between 0 and 127, inclusive.  The return value
 *              is just the space for the Routing header.  When the application uses
 *              ancillary data it must pass the returned length to IP_CMSG_SPACE() to
 *              determine how much memory is needed for the ancillary data object
 *              (including the Ip_cmsghdr structure).
 * Parameters:  [in] type - The type of routing header (IP_IPV6_RTHDR_TYPE_xxx)
 *              [in] segments - Maximum number of segments that will be included in
 *              this routing header.
 * Returns:     Number of bytes required or if the return value is 0, then either
 *              the type of the Routing header is not supported by this
 *              implementation or the number of segments is  invalid for this
 *              type of Routing header.
 */
IP_PUBLIC Ip_socklen_t
ipcom_inet6_rth_space(int type, int segments);


/*
 *===========================================================================
 *                    ipcom_inet6_rth_init
 *===========================================================================
 * Description: Initializes the buffer pointed to by bp to contain a
 *              Routing header of the specified type and sets 'ip6r_len' based
 *              on the segments parameter. The 'ip6r_segleft' field is set to
 *              zero; ipcom_inet6_rth_add() will increment it
 * Parameters:  [out] bp - The buffer to initialize
 *              [in] bp_len - The length of the 'bp' buffer.
 *              [in] type - The type of routing header to create (IP_IPV6_RTHDR_TYPE_xxx)
 *              [in] segments - Number of segments this buffer will contain.
 * Returns:     Upon success the return value is the pointer to the buffer 'bp',
 *              and this is then used as the first argument to the ipcom_inet6_rth_add()
 *              function.  Upon an error the return value is IP_NULL.
 */
IP_PUBLIC void *
ipcom_inet6_rth_init(void *bp, Ip_socklen_t bp_len, int type, int segments);


/*
 *===========================================================================
 *                    ipcom_inet6_rth_add
 *===========================================================================
 * Description: Adds the IPv6 address pointed to by addr to the end of
 *              the Routing header being constructed.
 * Parameters:  [in, out] bp - The buffer to which the address should be added.
 *              [in] addr - The address to add.
 * Returns:     If successful, the segleft member of the Routing Header is updated to
 *              account for the new address in the Routing header and the return
 *              value of the function is 0.  Upon an error the return value of the
 *              function is -1.
 */
IP_PUBLIC int
ipcom_inet6_rth_add(void *bp, const struct Ip_in6_addr *addr);


/*
 *===========================================================================
 *                    ipcom_inet6_rth_reverse
 *===========================================================================
 * Description: Takes a Routing header extension header (pointed to by
 *              the first argument) and writes a new Routing header that sends
 *              datagrams along the reverse of that route.  The function reverses the
 *              order of the addresses and sets the segleft member in the new Routing
 *              header to the number of segments.  Both arguments are allowed to
 *              point to the same buffer (that is, the reversal can occur in place).
 * Parameters:  [in] in - The buffer to reverse.
 *              [out] out - The buffer where the reversed Routing header should be placed.
 * Returns:     The return value of the function is 0 on success, or -1 upon an
 *              error.
 */
IP_PUBLIC int
ipcom_inet6_rth_reverse(const void *in, void *out);


/*
 *===========================================================================
 *                    ipcom_inet6_rth_segments
 *===========================================================================
 * Description: Returns the number of segments (addresses) contained in
 *              the Routing header described by 'bp'.
 * Parameters:  [in] bp - A routing header.
 * Returns:     The number of segments or -1 upon an error.
 */
IP_PUBLIC int
ipcom_inet6_rth_segments(const void *bp);


/*
 *===========================================================================
 *                    ipcom_inet6_rth_getaddr
 *===========================================================================
 * Description: Returns a pointer to the IPv6 address specified by
 *              index (which must have a value between 0 and one less than
 *              the value returned by ipcom_inet6_rth_segments()) in the
 *              Routing header described by 'bp'.
 * Parameters:  [in] bp - A routing header.
 *              [in] i - The index of the address to return.
 * Returns:     Pointer to the address at the specified index or IP_NULL
 *              if an error occurred.
 */
IP_PUBLIC struct Ip_in6_addr *
ipcom_inet6_rth_getaddr(const void *bp, int i);


/*
 *===========================================================================
 *                    ipcom_inet6_opt_init
 *===========================================================================
 * Description: Returns the number of bytes needed for the empty extension
 *              header i.e., without any options.
 *              If extbuf is not IP_NULL it also initializes the extension header
 *              to have the correct length field. In that case if the extlen
 *              value is not a positive (i.e., non-zero) multiple of 8 the
 *              function fails and returns -1.
 * Parameters:  [out] extbuf - An empty extension header.
 *              [in] extlen - The length of 'extbuf'.
 * Returns:     >0 = The length of the empty header.
 *              -1 = failed, error code in ipcom_errno.
 */
IP_PUBLIC int
ipcom_inet6_opt_init(void *extbuf, Ip_socklen_t extlen);


/*
 *===========================================================================
 *                    ipcom_inet6_opt_append
 *===========================================================================
 * Description: Offset should be the length returned by ipcom_inet6_opt_init()
 *              or a previous ipcom_inet6_opt_append().
 * Parameters:  [out] extbuf - If extbuf is not IP_NULL then, in addition to
 *              returning the length, the function inserts any needed pad option,
 *              initializes the option (setting the type and length fields) and
 *              returns a pointer to the location for the option content in databufp.
 *              [in] extlen - The length of 'extbuf'.
 *              [in] offset - Should be the length returned by ipcom_inet6_opt_init()
 *              or ipcom_inet6_opt_append().
 *              [in] type - The 8-bit option type.
 *              [in] len - the length of the option data (i.e., excluding the
 *              option type and option length fields).
 *              [in] align - must have a value of 1, 2, 4, or 8. The align
 *              value cannot exceed the value of len.
 *              [out] databufp - Will return a pointer to the location for the
 *              option content.
 * Returns:     The updated total length taking into account adding an option
 *              with length 'len' and alignment 'align'. -1 is returned if any
 *              of the parameters is invalid.
 */
IP_PUBLIC int
ipcom_inet6_opt_append(void *extbuf, Ip_socklen_t extlen, int offset,
    Ip_u8 type, Ip_socklen_t len, unsigned int align, void **databufp);


/*
 *===========================================================================
 *                    ipcom_inet6_opt_finish
 *===========================================================================
 * Description: Returns the updated total length taking into account the
 *              final padding of the extension header to make it a multiple
 *              of 8 bytes.
 * Parameters:  [out] extbuf - If extbuf is not IP_NULL then initializes the
 *              option by inserting a Pad1 or PadN option of the proper length.
 *              [in] extlen - The length of extbuf.
 *              [in] offset - Should be the length returned by
 *              ipcom_inet6_opt_init() or ipcom_inet6_opt_append().
 * Returns:     The final total length or -1 if the necessary pad does not
 *              fit in the extension header buffer.
 */
IP_PUBLIC int
ipcom_inet6_opt_finish(void *extbuf, Ip_socklen_t extlen, int offset);


/*
 *===========================================================================
 *                    ipcom_inet6_opt_set_val
 *===========================================================================
 * Description: Inserts data items of various sizes in the data portion of
 *              the option.
 * Parameters:  databuf - [out] A pointer returned by ipcom_inet6_opt_append().
 *              offset - [in] specifies where in the data portion of the
 *              option the value should be inserted; the first byte after the
 *              option type and length is accessed by specifying an offset of
 *              zero.
 *              [in] val - Pointer to the data to be inserted.
 *              [in] vallen - The length of the option data.
 * Returns:     The function returns the offset for the next field (i.e.,
 *              offset + vallen) which can be used when composing option
 *              content with multiple fields.
 */
IP_PUBLIC int
ipcom_inet6_opt_set_val(void *databuf, int offset, void *val, Ip_socklen_t vallen);


/*
 *===========================================================================
 *                    ipcom_inet6_opt_next
 *===========================================================================
 * Description: Parses received option extension headers returning the
 *              next option. PAD1 or PADN options are not returned.
 * Parameters:  [in] extbuf - An extension header.
 *              [in] extlen - Length of extension header buffer.
 *              [in] offset - Should either be zero (for the first option) or
 *              the length returned by a previous call to ipcom_inet6_opt_next()
 *              or ipcom_inet6_opt_find()
 *              [out] typep - The type of the returned option.
 *              [out] lenp - The length of the option data (i.e., excluding
 *              the option type and option length fields).
 *              [out] databufp - Points the data field of the option.
 * Returns:     The updated "previous" length computed by advancing past the
 *              option that was returned.  This returned "previous" length
 *              can then be passed to subsequent calls to ipcom_inet6_opt_next().
 *              When there are no more options or if the option extension
 *              header is malformed the return value is -1.
 */
IP_PUBLIC int
ipcom_inet6_opt_next(void *extbuf, Ip_socklen_t extlen, int offset,
    Ip_u8 *typep, Ip_socklen_t *lenp, void **databufp);


/*
 *===========================================================================
 *                    ipcom_inet6_opt_find
 *===========================================================================
 * Description: If an option of the specified type is located, the function
 *              returns the updated "previous" total length computed by
 *              advancing past the option that was returned and past any
 *              options that didn't match the type.
 *              This returned "previous" length can then be passed to subsequent
 *              calls to ipcom_inet6_opt_find() for finding the next occurrence
 *              of the same option type.
 *              If an option of the specified type is not located, the return
 *              value is -1.  If the option extension header is malformed,
 *              the return value is -1.
 * Parameters:  [in] extbuf - An extension header.
 *              [in] extlen - Length of extension header buffer.
 *              [in] offset - Should either be zero or the length returned
 *              by a previous call to ipcom_inet6_opt_next() or
 *              ipcom_inet6_opt_find(). The search starts from that offset.
 *              [in] type - The type to return.
 *              [out] lenp - The length of the option data (i.e., excluding
 *              the option type and option length fields).
 *              [out] databufp - Points the data field of the option.
 * Returns:     Returns the specified options, if it exists.
 */
IP_PUBLIC int
ipcom_inet6_opt_find(void *extbuf, Ip_socklen_t extlen, int offset,
    Ip_u8 type, Ip_socklen_t *lenp, void **databufp);


/*
 *===========================================================================
 *                    ipcom_inet6_opt_get_val
 *===========================================================================
 * Description: Gets a option from an extension header.
 * Parameters:  [in] databuf - Should be a pointer returned by
 *              ipcom_inet6_opt_next() or ipcom_inet6_opt_find().
 *              [in] offset - Specifies from where in the data portion of the
 *              option the value should be extracted; the first byte after
 *              the option type and length is accessed by specifying an
 *              offset of zero.
 *              [out] val - Should point to the destination for the extracted
 *              data.
 *              [in] vallen - The length of the 'val' buffer.
 * Returns:     The offset for the next field (i.e., offset + vallen) which
 *              can be used when extracting option content with multiple fields.
 */
IP_PUBLIC int
ipcom_inet6_opt_get_val(void *databuf, int offset, void *val, Ip_socklen_t vallen);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
