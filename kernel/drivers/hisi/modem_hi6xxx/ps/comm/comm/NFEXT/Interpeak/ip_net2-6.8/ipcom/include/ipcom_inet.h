/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_inet.h,v $ $Revision: 1.40 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_inet.h,v $
 *     $Author: hsun $ $Date: 2009-04-16 03:03:46 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_INET_H
#define IPCOM_INET_H

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

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_IS_GT(s1, s2)    ((Ip_s32) ((s1) - (s2)) > 0)
#define IPCOM_IS_GEQ(s1, s2)   ((Ip_s32) ((s1) - (s2)) >= 0)
#define IPCOM_IS_LT(s1, s2)    ((Ip_s32) ((s1) - (s2)) < 0)
#define IPCOM_IS_LEQ(s1, s2)   ((Ip_s32) ((s1) - (s2)) <= 0)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         Ip_addr4
 *===========================================================================
 */

#ifndef _IPCOM_IPA_TYPE_
#define _IPCOM_IPA_TYPE_
typedef Ip_u32 Ip_addr4;   /* 32 bit IPv4 address in network byte order. */
#endif

typedef union Ipa_union_union
{
    Ip_addr4 addr32;
    Ip_u16   addr16[2];
    Ip_u8    addr8[4];
}
Ipa_union;


/*
 *===========================================================================
 *                         Ipcom_sock_addr
 *===========================================================================
 * Ipcom_sock_addr is used to store an IPv4 address, a port number and optionally
 * the interface index of an address. Set 'ifindex' to IP_INVAL_IFINDEX if
 * unknown.
 */

typedef struct Ipcom_sock_addr_struct
{
    Ip_addr4   ipaddr_n;
    Ip_u32     ifindex;    /* where applicable. */
    Ip_u16     port_n;
}
Ipcom_sock_addr;

/* Use the below macro if 'ifindex' is unknown. */
#define IP_INVAL_IFINDEX   0xffffffff


/*
 *===========================================================================
 *                         Ipcom_sock_packet
 *===========================================================================
 * Interpeak packet structure including the buffer as well as the source and
 * destination address.
 */
typedef struct Ipcom_sock_packet_struct
{
  /* next pointer for packet linking. */
  struct Ipcom_sock_packet_struct *next;

  /* Source address (from) and destination address (to). */
  Ipcom_sock_addr  from;
  Ipcom_sock_addr  to;

  /* 'packet' buffer. */
  void        *buf;
  Ip_size_t     maxlen;
  Ip_size_t     len;
}
Ipcom_sock_packet;


/*
 *===========================================================================
 *                         Ipcom_ip_header
 *===========================================================================
 * IPv4 header.
 */
#define IPCOM_IP_HEADER_LENGTH    20

#include "ipcom_align16.h"
typedef IP_PACKED1 struct Ipcom_ip_header_struct
{
    Ip_u8    vers_len;    /* 4-bit version and 4-bit header length. */
    Ip_u8    tos;         /* Type Of Service. */
    Ip_u16   length;      /* total length of IP datagram in bytes. */

    Ip_u16   id;          /* Identification field. */
    Ip_u16   frag;        /* 3-bit flags and 13-bit fragment offset. */

    Ip_u8    ttl;         /* Time To Live - number of routing hops. */
    Ip_u8    protocol;    /* IP Protocol number. */
    Ip_u16   checksum;    /* IP header checksum, 16-bit one's complement sum. */

    Ipa_union   src;      /* Source IP address. */
    Ipa_union   dst;      /* Destination IP address. */
}
IP_PACKED2 Ipcom_ip_header;

/* frag field. */
#define IPCOM_ISFRAG(frag)  ((frag) & (IPCOM_IPFRAG_MF | IPCOM_IPFRAG_OFFMASK))
#ifdef IP_LITTLE_ENDIAN
#define IPCOM_IPFRAG_RF            0x0080    /* Reserved fragment flag. */
#define IPCOM_IPFRAG_DF            0x0040    /* Dont fragment flag. */
#define IPCOM_IPFRAG_MF            0x0020    /* More fragments flag. */
#define IPCOM_IPFRAG_OFFMASK       0xff1f    /* Mask for fragmenting bits. */
#else
#define IPCOM_IPFRAG_RF            0x8000    /* Reserved fragment flag. */
#define IPCOM_IPFRAG_DF            0x4000    /* Dont fragment flag. */
#define IPCOM_IPFRAG_MF            0x2000    /* More fragments flag. */
#define IPCOM_IPFRAG_OFFMASK       0x1fff    /* Mask for fragmenting bits. */
#endif


/*
 *===========================================================================
 *                         Ipcom_ip6_header
 *===========================================================================
 * IPv6 header.
 */
#define IPCOM_IP6_HEADER_LENGTH    40

typedef IP_PACKED1 struct Ipcom_ip6_header_struct
{
    Ip_u32   ver_class_flow;  /* 4 bit version, 8 bit class, 20 bit flow. */
    Ip_u16   plen;            /* Payload length. */
    Ip_u8    nxt;             /* Next Header. */
    Ip_u8    hlim;            /* Hop limit. */

#ifdef IPCOM_USE_INET6
    struct Ip_in6_addr  src;  /* IPv6 source address. */
    struct Ip_in6_addr  dst;  /* IPv6 destination address. */
#else
    Ip_u8  xsrc[16];
    Ip_u8  xdst[16];
#endif
}
IP_PACKED2 Ipcom_ip6_header;

#define IP_IP6_GET_VER(ip6)      (IP_GET_NTOHL(&(ip6)->ver_class_flow) >> 28)


/*
 *===========================================================================
 *                         Ipcom_udp_header
 *===========================================================================
 * UDP header.
 */
#define IPCOM_UDP_HEADER_LENGTH    8

typedef IP_PACKED1 struct Ipcom_udp_header_struct
{
    Ip_u16   src_port;           /* Source UDP Port */
    Ip_u16   dst_port;           /* Destination UDP Port */
    Ip_u16   length;             /* Length of UDP header + data = 8+X */
    Ip_u16   checksum;           /* UDP Checksum (0 = No checksum) */
}
IP_PACKED2 Ipcom_udp_header;


/*
 *===========================================================================
 *                         Ipcom_sctp_header
 *===========================================================================
 * SCTP header.
 */

#define IPCOM_SCTP_HEADER_LENGTH    12

typedef IP_PACKED1 struct Ipcom_sctp_header_struct
{
    Ip_u16  sport;            /* source port */
    Ip_u16  dport;            /* destination port */
    Ip_u32  vtag;             /* packet verification tag */
    Ip_u32  crc;              /* CRC-32c Checksum */
}
IP_PACKED2 Ipcom_sctp_header;

/*
 *===========================================================================
 *                         Ipcom_tcp_header
 *===========================================================================
 * TCP header.
 * NOTE: fixed standard/minimum size, can acctually vary in size!
 */

/* Size of a TCP header without options. */
#define IPCOM_TCP_HEADER_LENGTH    20

typedef IP_PACKED1 struct Ipcom_tcp_header_struct
{
    Ip_u16   src_port;           /* Source TCP Port */
    Ip_u16   dst_port;           /* Destination TCP Port */

    Ip_u32   seq_num;
    Ip_u32   ack_num;

    Ip_u16   offset_flags;
    Ip_u16   window;

    Ip_u16   checksum;
    Ip_u16   urgent_pointer;
}
IP_PACKED2 Ipcom_tcp_header;


/*
 *===========================================================================
 *                         Ipcom_icmp_header
 *===========================================================================
 * ICMP header.
 */

/* Size of common fields in an ICMP header */
#define IPCOM_ICMP_HEADER_LENGTH    4

typedef IP_PACKED1 struct Ipcom_icmp_header_struct
{
    Ip_u8    type;      /* Type field */
    Ip_u8    code;      /* Code field */

    Ip_u16   checksum;  /* Checksum field */

    union Ipcom_icmp_data_union
    {
        struct Ipcom_icmp_data_echo_struct
        {
            Ip_u16    id;
            Ip_u16    seq;
            Ip_u16    data[2];
        } echo;

        struct Ipcom_icmp_data_unreach_struct
        {
            Ip_u16    unused[2];
            Ip_u16    iph[10];
            Ip_u16    data[4];
        } unreach;

        struct Ipcom_icmp_data_redirect_struct
        {
            Ip_u32    gateway_addr;
            Ip_u16    ip[10];
            Ip_u16    transport_hdr[4];
        } redirect;

        struct Ipcom_icmp_data_failing_pkt_struct
        {
            Ip_u16    unused[2];
            Ip_u16    ip[10];
            Ip_u16    transport_hdr[4];
        } failing_pkt;
    }
    data;
}
IP_PACKED2 Ipcom_icmp_header;
#include "ipcom_align16.h"

/* ICMP v4 'type': */
#define IPCOM_ICMP_ECHO_REPLY        0
#define IPCOM_ICMP_UNREACH           3
#define IPCOM_ICMP_SOURCEQUENCH      4
#define IPCOM_ICMP_REDIRECT          5
#define IPCOM_ICMP_ECHO_REQUEST      8
#define IPCOM_ICMP_TIMEXCEED         11
#define IPCOM_ICMP_PARAMPROB         12
#define IPCOM_ICMP_TSTAMP_REQUEST    13
#define IPCOM_ICMP_TSTAMP_REPLY      14
#define IPCOM_ICMP_INFO_REQUEST      15
#define IPCOM_ICMP_INFO_REPLY        16
#define IPCOM_ICMP_MASK_REQUEST      17
#define IPCOM_ICMP_MASK_REPLY        18

    /* ICMP v6 'type': */
#define IPCOM_ICMP6_ECHO_REQUEST     128
#define IPCOM_ICMP6_ECHO_REPLY       129


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_ipproto_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_ipproto_name(Ip_u8 protocol);


/*
 *===========================================================================
 *                    ipcom_getsockaddrbyaddr
 *===========================================================================
 * Description:  Convert the presentation form of a address to a socket address.
 * Parameters:   domain - Address domain (IP_AF_LINK, IP_AF_INET or IP_AF_INET6).
 *               straddr - The presentation form of the address.
 *               sa - Will hold the socket address.
 * Returns:      0 for success else negative errno code (ipcom_perrno.h).
 *
 */
IP_PUBLIC int
ipcom_getsockaddrbyaddr(int domain, IP_CONST char *straddr, struct Ip_sockaddr *sa);


/*
 *===========================================================================
 *                    ipcom_getsockaddrbyaddrname
 *===========================================================================
 * Description:  Resolves the 'addr_or_name' to IP address stored in socket
 *               address.
 * Parameters:   domain - IP_AF_INET or IP_AF_INET6.
 *               no_dns - IP_TRUE if address should be resolved using DNS.
 *               addr_or_name - Must be a IP address or a DNS name.
 *               sa - Socket address structure to hold the IP address.
 * Returns:      0 for success else negative errno code (ipcom_perrno.h).
 *
 */
IP_PUBLIC int
ipcom_getsockaddrbyaddrname(int domain, Ip_bool no_dns, IP_CONST char *addr_or_name,
                            struct Ip_sockaddr *sa);


/*
 *===========================================================================
 *                    ipcom_getsockaddrbyaddr
 *===========================================================================
 * Description:  Converts a prefix in the form addr/len to a numerical form.
 * Parameters:   domain - Address domain (IP_AF_LINK, IP_AF_INET or IP_AF_INET6).
 *               straddr - The presentation form of the prefix.
 *               inprefix - Points to a buffer that can hold the domain address.
 *               prefixlen - The length of the prefix.
 * Returns:      0 for success else negative errno code (ipcom_perrno.h).
 *
 */
IP_PUBLIC int
ipcom_getinaddrprefixbyaddr(int domain, IP_CONST char *straddr, void *inprefix, int *prefixlen);


/*
 *===========================================================================
 *                    ipcom_mask_to_prefixlen
 *===========================================================================
 * Description: Converts binary representation of a mask to the corresponding
 *              prefix len value (i.e. 255.255.0.0 -> 16).
 * Parameters:  mask - The mask in binary form
 *              mask_bit_len - The size of the mask in bits
 * Returns:     The prefix length value for the specified mask.
 *
 */
IP_PUBLIC int
ipcom_mask_to_prefixlen(IP_CONST void *mask, int mask_bit_len);


/*
 *===========================================================================
 *                    ipcom_sockaddr_to_prefixlen
 *===========================================================================
 * Description: Returns the number of leading '1' in the mask.
 * Parameters:  sa - Socket address structure containing the mask.
 * Returns:     The prefix length value for the address in sockaddr 'sa'.
 *
 */
IP_PUBLIC int
ipcom_sockaddr_to_prefixlen(struct Ip_sockaddr *sa);



/*
 *===========================================================================
 *                    ipcom_in_checksum
 *===========================================================================
 * Description:   Calculate a one complement checksum.
 * Parameters:    buf and bytes.
 * Returns:       A one complement standard internet checksum.
 *
 */
IP_PUBLIC Ip_u16
ipcom_in_checksum(void *buf, Ip_size_t bytes);


/*
 *===========================================================================
 *                    ipcom_in_checksum_update
 *===========================================================================
 * Description:   Computes the internet checksum (RFC 1071). NOTE
 *                it does not compute the 1 complement checksum.
 * Parameters:    buf and bytes.
 * Returns:       A standard internet checksum..
 *
 */
IP_PUBLIC Ip_u32
ipcom_in_checksum_update(void *buf, Ip_size_t bytes);


/*
 *===========================================================================
 *                    ipcom_in_checksum_finish
 *===========================================================================
 * Description:   Computes the one complement internet checksum (RFC 1071).
 * Parameters:    sum - The accumulated 32 bit internet sum.
 * Returns:       A one complement standard internet checksum.
 *
 */
IP_PUBLIC Ip_u16
ipcom_in_checksum_finish(Ip_u32 sum);


/*
 *===========================================================================
 *                    ipcom_in_checksum_memcpy
 *===========================================================================
 * Description: Copies data and calculate the internet checksum at the same
 *              time.
 * Parameters:  dst - Destination buffer.
 *              src - Source buffer.
 *              len - Number of octets to copy.
 * Returns:     The intermediate internet checksum, use
 *              ipcom_in_checksum_finish() to get the actual checksum.
 *
 */
IP_PUBLIC Ip_u32
ipcom_in_checksum_memcpy(void *dst, void *src, Ip_size_t len);


/*
 *===========================================================================
 *                         ipcom_stackd
 *===========================================================================
 * Description: Prototypes for external stack daemon.
 * Parameters:
 * Returns:     No of milliseconds to next stack timeout.
 *
 */
IP_PUBLIC Ip_s32 ipcom_stackd_init(void);
IP_FASTTEXT IP_PUBLIC Ip_s32 ipcom_stackd_event(Ip_bool is_timeout);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

