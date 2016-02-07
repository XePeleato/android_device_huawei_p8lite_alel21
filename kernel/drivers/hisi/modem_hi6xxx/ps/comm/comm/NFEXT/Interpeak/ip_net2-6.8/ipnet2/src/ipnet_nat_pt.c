/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_pt.c,v $ $Revision: 1.20 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_pt.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * The NAT-PT implementation. Some NAT-PT code is common with NAT and is
 * in the file ipnet_nat.c.
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

#ifdef IPNET_USE_NAT_PT

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_syslog.h>
#include <ipcom_shell.h>
#include <ipcom_err.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_nat.h"
#include "ipnet_nat_h.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ICMPv6 error type
 *===========================================================================
 */
#define IPNET_NAT_IS_ICMP6_ERROR(type)  (type == IPNET_ICMP6_DST_UNREACH || \
                                         type == IPNET_ICMP6_TIME_EXCEEDED || \
                                         type == IPNET_ICMP6_PARAM_PROB)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_nat_pt_pseudo_hdr
 *===========================================================================
 * Pseudo header for IPv6 used in UDP/TCP checksum calculations.
 *
 */
typedef struct
{
    struct Ip_in6_addr src_n;
    struct Ip_in6_addr dst_n;
    Ip_u32 length_n;
    Ip_u8 mbz[3];
    Ip_u8 protocol;
}
Ipnet_nat_pt_pseudo_hdr;


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
 *                    ipnet_nat_pt_scan_headers
 *===========================================================================
 * Description: Scans extension headers in an IPv6 packet.
 * Parameters:  pkt       - pointer to the packet.
 *              nat_tuple - pointer to the NAT tuple (can be IP_NULL).
 *              proto     - pointer to the returned protocol (can be IP_NULL).
 * Returns:     number of bytes of extension headers or -1 if failed.
 */
IP_STATIC Ip_s32
ipnet_nat_pt_scan_headers(Ipcom_pkt *pkt, Ipnet_nat_tuple *nat_tuple, Ip_u8 *proto)
{
    Ipnet_pkt_ip6 *ip6;
    Ip_pkt_ip6_frag *frag;
    Ip_u8 nxt;
    int start, end;

    ip6   = (Ipnet_pkt_ip6 *)&pkt->data[pkt->ipstart];
    nxt   = ip6->nxt;
    start = pkt->ipstart + IPCOM_IP6_HEADER_LENGTH;
    end   = pkt->end;
    while(nxt == IP_IPPROTO_HOPOPTS ||
          nxt == IP_IPPROTO_DSTOPTS ||
          nxt == IP_IPPROTO_ROUTING ||
          nxt == IP_IPPROTO_ESP ||
          nxt == IP_IPPROTO_AH ||
          nxt == IP_IPPROTO_FRAGMENT)
    {
        /* Check that there are at least 2 bytes for the next header and length field */
        if (end - start < 2)
            return -1;  /* Too short */
        if(nxt == IP_IPPROTO_FRAGMENT)
        {
            /* Check that there are enough bytes for the fragment header */
            if (end - start < (int)sizeof(Ip_pkt_ip6_frag))
                return -1;  /* Too short */

            if (nat_tuple != IP_NULL)
            {
                /* Get fragment offset and flags */
                frag = (Ip_pkt_ip6_frag *)&pkt->data[start];
                nat_tuple->fragoff = (Ip_u16)((ip_ntohs(frag->ip6f_offlg) & 0xfff8)>>3);
                nat_tuple->fragmf = (Ip_u8)(ip_ntohs(frag->ip6f_offlg) & 0x0001);
                nat_tuple->fragid = IP_GET_32ON16((void *)&frag->ip6f_ident);
                nat_tuple->fragid = ip_ntohl(frag->ip6f_ident);
            }
        }
        nxt = pkt->data[start];
        start += (pkt->data[start+1]+1)<<3;
        /* Check that there are enough bytes for the extension header */
        if (end - start < 0)
            return -1;  /* Too short */
    }

    if (proto != IP_NULL)
        *proto = nxt;
    return start - (pkt->ipstart + IPCOM_IP6_HEADER_LENGTH);
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_get_tuple
 *===========================================================================
 * Description: Populates a NAT tuple structure from an IPv6 packet.
 * Parameters:  pkt       - pointer to the packet.
 *              nat_tuple - pointer to the NAT tuple.
 * Returns:     0 = OK
 *             -1 = Cannot NAT
 */
IP_STATIC Ip_s32
ipnet_nat_pt_get_tuple(Ipcom_pkt *pkt, Ipnet_nat_tuple *nat_tuple)
{
    Ipnet_pkt_ip6 *ip6;
    int length, start, end;

    ip_assert(pkt);

    ipcom_memset(nat_tuple, 0, sizeof(*nat_tuple));

    ip6 = (Ipnet_pkt_ip6 *)&pkt->data[pkt->ipstart];

    /* Get tos */
    nat_tuple->tos = IPNET_IP6_GET_CLASS(ip6);

    /* Get ttl */
    nat_tuple->ttl = ip6->hlim;

    /* Get addr */
    ipcom_memcpy(&nat_tuple->pt_src, &ip6->src, sizeof(ip6->src));
    ipcom_memcpy(&nat_tuple->pt_dst, &ip6->dst, sizeof(ip6->dst));
    nat_tuple->src_n = nat_tuple->pt_src.in6.addr32[3];
    nat_tuple->dst_n = nat_tuple->pt_dst.in6.addr32[3];

    /* Scan extension headers */
    length = ipnet_nat_pt_scan_headers(pkt, nat_tuple, &nat_tuple->protocol);
    if (length < 0)
        return -1;
    start = pkt->ipstart + IPCOM_IP6_HEADER_LENGTH + length;
    end = pkt->end;

    /* Check transport length */
    if (nat_tuple->fragoff == 0)
    {
        if (nat_tuple->protocol == IP_IPPROTO_ICMPV6)
        {
            Ipnet_pkt_icmp *icmp;

            if(end - start < IPCOM_ICMP_HEADER_LENGTH)
                return -1;    /* Too short */
            icmp = (Ipnet_pkt_icmp *)&pkt->data[start];
            nat_tuple->icmp_type = icmp->type;
            if (icmp->type == IPNET_ICMP6_ECHO_REQUEST)
            {
                if(end - start < IPNET_NAT_ICMP_QUERY_LENGTH)
                    return -1;    /* Too short */
                nat_tuple->icmp_id_n = icmp->data.echo.id;
            }
            else if (IPNET_NAT_IS_ICMP6_ERROR(icmp->type))
            {
                if(end - start < 8 + IPCOM_IP6_HEADER_LENGTH + 8)
                    return -1;    /* Too short */
            }
        }
        else if(nat_tuple->protocol == IP_IPPROTO_UDP)
        {
            Ipcom_udp_header *udp;

            udp = (Ipcom_udp_header *)&pkt->data[start];
            if(end - start < IPCOM_UDP_HEADER_LENGTH)
                return -1;    /* Too short */

            nat_tuple->src_port_n = udp->src_port;
            nat_tuple->dst_port_n = udp->dst_port;
        }
        else if(nat_tuple->protocol == IP_IPPROTO_TCP)
        {
            Ipcom_tcp_header *tcp;

            tcp = (Ipcom_tcp_header *)&pkt->data[start];
            if(end - start < IPCOM_TCP_HEADER_LENGTH)
                return -1;    /* Too short */

            nat_tuple->src_port_n = tcp->src_port;
            nat_tuple->dst_port_n = tcp->dst_port;
            nat_tuple->tcp_flags  = ip_ntohs(tcp->offset_flags) & 0x003f;
        }
    }

    /* Move start to after extension headers */
    pkt->start = start;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_icmp_checksum
 *===========================================================================
 * Description: Calculate ICMP checksum for NAT-PT
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to NAT mapping.
 *              len         - length of packet excluding ip header.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_pt_icmp_checksum(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping,
                           Ipnet_nat_tuple *nat_tuple, int len)
{
    Ipnet_pkt_icmp *icmp;
    Ipnet_nat_pt_pseudo_hdr nat_pt_pseudo_hdr;

    icmp = (Ipnet_pkt_icmp *)&pkt->data[pkt->start];
    if(incoming == IP_TRUE)
    {
        /* Ipv6 pseudo header */
        ipcom_memcpy(&nat_pt_pseudo_hdr.src_n, &nat_mapping->inner.tuple.pt_dst, sizeof(nat_pt_pseudo_hdr.src_n));
        nat_pt_pseudo_hdr.src_n.in6.addr32[3] = nat_tuple->src_n;
        ipcom_memcpy(&nat_pt_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.pt_src, sizeof(nat_pt_pseudo_hdr.dst_n));
        nat_pt_pseudo_hdr.length_n = ip_htonl(len);
        ipcom_memset(nat_pt_pseudo_hdr.mbz, 0, sizeof(nat_pt_pseudo_hdr.mbz));
        nat_pt_pseudo_hdr.protocol = IP_IPPROTO_ICMPV6;

        icmp->cksum = 0;
        icmp->cksum = (Ip_u16)(~ipcom_in_checksum(&nat_pt_pseudo_hdr, sizeof(nat_pt_pseudo_hdr)));
 	    icmp->cksum = ipcom_in_checksum(icmp, len);
    }
    else
    {
        icmp->cksum = 0;
 	    icmp->cksum = ipcom_in_checksum(icmp, len);
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_udp_checksum
 *===========================================================================
 * Description: Calculate UDP checksum for NAT-PT
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to NAT mapping.
 *              len         - length of packet excluding ip header.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_pt_udp_checksum(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping,
                          Ipnet_nat_tuple *nat_tuple, int len)
{
    Ipcom_udp_header       *udp;
    Ipnet_nat_pseudo_hdr    nat_pseudo_hdr;
    Ipnet_nat_pt_pseudo_hdr nat_pt_pseudo_hdr;

    udp = (Ipcom_udp_header *)&pkt->data[pkt->start];
    if(incoming == IP_TRUE)
    {
        /* Ipv6 pseudo header */
        ipcom_memcpy(&nat_pt_pseudo_hdr.src_n, &nat_mapping->inner.tuple.pt_dst, sizeof(nat_pt_pseudo_hdr.src_n));
        nat_pt_pseudo_hdr.src_n.in6.addr32[3] = nat_tuple->src_n;
        ipcom_memcpy(&nat_pt_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.pt_src, sizeof(nat_pt_pseudo_hdr.dst_n));
        nat_pt_pseudo_hdr.length_n = ip_htonl(len);
        ipcom_memset(nat_pt_pseudo_hdr.mbz, 0, sizeof(nat_pt_pseudo_hdr.mbz));
        nat_pt_pseudo_hdr.protocol = IP_IPPROTO_UDP;

        udp->checksum = 0;
        udp->checksum = (Ip_u16)(~ipcom_in_checksum(&nat_pt_pseudo_hdr, sizeof(nat_pt_pseudo_hdr)));
 	    udp->checksum = ipcom_in_checksum(udp, len);
    }
    else
    {
        /* Ipv4 pseudo header */
        ipcom_memcpy(&nat_pseudo_hdr.src_n, &nat_mapping->inner.nat_addr_n, sizeof(nat_pseudo_hdr.src_n));
        ipcom_memcpy(&nat_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.pt_dst.in6.addr32[3], sizeof(nat_pseudo_hdr.dst_n));
        nat_pseudo_hdr.length_n = ip_htons((short)len);
        ipcom_memset(&nat_pseudo_hdr.mbz, 0, sizeof(nat_pseudo_hdr.mbz));
        nat_pseudo_hdr.protocol = IP_IPPROTO_UDP;

        udp->checksum = 0;
        udp->checksum = (Ip_u16)(~ipcom_in_checksum(&nat_pseudo_hdr, sizeof(nat_pseudo_hdr)));
 	    udp->checksum = ipcom_in_checksum(udp, len);
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_tcp_checksum
 *===========================================================================
 * Description: Calculate TCP checksum for NAT-PT
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to NAT mapping.
 *              len         - length of packet excluding ip header.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_pt_tcp_checksum(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping,
                          Ipnet_nat_tuple *nat_tuple, int len)
{
    Ipcom_tcp_header       *tcp;
    Ipnet_nat_pseudo_hdr    nat_pseudo_hdr;
    Ipnet_nat_pt_pseudo_hdr nat_pt_pseudo_hdr;

    tcp = (Ipcom_tcp_header *)&pkt->data[pkt->start];
    if(incoming == IP_TRUE)
    {
        /* Ipv6 pseudo header */
        ipcom_memcpy(&nat_pt_pseudo_hdr.src_n, &nat_mapping->inner.tuple.pt_dst, sizeof(nat_pt_pseudo_hdr.src_n));
        nat_pt_pseudo_hdr.src_n.in6.addr32[3] = nat_tuple->src_n;
        ipcom_memcpy(&nat_pt_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.pt_src, sizeof(nat_pt_pseudo_hdr.dst_n));
        nat_pt_pseudo_hdr.length_n = ip_htonl(len);
        ipcom_memset(nat_pt_pseudo_hdr.mbz, 0, sizeof(nat_pt_pseudo_hdr.mbz));
        nat_pt_pseudo_hdr.protocol = IP_IPPROTO_TCP;

        tcp->checksum = 0;
        tcp->checksum = (Ip_u16)(~ipcom_in_checksum(&nat_pt_pseudo_hdr, sizeof(nat_pt_pseudo_hdr)));
 	    tcp->checksum = ipcom_in_checksum(tcp, len);
    }
    else
    {
        /* Ipv4 pseudo header */
        ipcom_memcpy(&nat_pseudo_hdr.src_n, &nat_mapping->inner.nat_addr_n, sizeof(nat_pseudo_hdr.src_n));
        ipcom_memcpy(&nat_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.pt_dst.in6.addr32[3], sizeof(nat_pseudo_hdr.dst_n));
        nat_pseudo_hdr.length_n = ip_htons((short)len);
        ipcom_memset(&nat_pseudo_hdr.mbz, 0, sizeof(nat_pseudo_hdr.mbz));
        nat_pseudo_hdr.protocol = IP_IPPROTO_TCP;

        tcp->checksum = 0;
        tcp->checksum = (Ip_u16)(~ipcom_in_checksum(&nat_pseudo_hdr, sizeof(nat_pseudo_hdr)));
 	    tcp->checksum = ipcom_in_checksum(tcp, len);
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_embedded_checksum_update
 *===========================================================================
 * Description: Smart update of TCP/UDP/ICMP checksum for NAT-PT.
 * Parameters:  nat_mapping - pointer to NAT mapping.
 *              len         - length of packet excluding ip header.
 *              protocol    - packet protocol number.
 *              checksum    - pointer to the calculated checksum.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_pt_embedded_checksum_update(Ipnet_nat_mapping *nat_mapping,
                                      int len,
                                      Ip_u8 protocol,
                                      Ip_u16 *checksum)

{
    Ipnet_nat_pseudo_hdr nat_pseudo_hdr;
    Ipnet_nat_pt_pseudo_hdr nat_pt_pseudo_hdr;

    if(protocol == IP_IPPROTO_TCP || protocol == IP_IPPROTO_UDP)
    {
        /* Remove IPv4 pseudo header */
        ipcom_memcpy(&nat_pseudo_hdr.src_n, &nat_mapping->inner.nat_addr_n, sizeof(nat_pseudo_hdr.src_n));
        ipcom_memcpy(&nat_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.dst_n, sizeof(nat_pseudo_hdr.dst_n));
        nat_pseudo_hdr.length_n = ip_htons((short)len);
        ipcom_memset(&nat_pseudo_hdr.mbz, 0, sizeof(nat_pseudo_hdr.mbz));
        nat_pseudo_hdr.protocol = protocol;

        /* Add IPv6 pseudo header */
        ipcom_memcpy(&nat_pt_pseudo_hdr.src_n, &nat_mapping->inner.tuple.pt_src, sizeof(nat_pt_pseudo_hdr.src_n));
        ipcom_memcpy(&nat_pt_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.pt_dst, sizeof(nat_pt_pseudo_hdr.dst_n));
        nat_pt_pseudo_hdr.length_n = ip_htonl(len);
        ipcom_memset(nat_pt_pseudo_hdr.mbz, 0, sizeof(nat_pt_pseudo_hdr.mbz));
        nat_pt_pseudo_hdr.protocol = protocol;

        /* Update the checksum */
        ipnet_nat_checksum_update((Ip_u8 *)checksum,
                                  (Ip_u8 *)&nat_pseudo_hdr,
                                  sizeof(nat_pseudo_hdr),
                                  (Ip_u8 *)&nat_pt_pseudo_hdr,
                                  sizeof(nat_pt_pseudo_hdr));
    }
    else if(protocol == IP_IPPROTO_ICMP)
    {
        /* Add IPv6 pseudo header */
        ipcom_memcpy(&nat_pt_pseudo_hdr.src_n, &nat_mapping->inner.tuple.pt_src, sizeof(nat_pt_pseudo_hdr.src_n));
        ipcom_memcpy(&nat_pt_pseudo_hdr.dst_n, &nat_mapping->inner.tuple.pt_dst, sizeof(nat_pt_pseudo_hdr.dst_n));
        nat_pt_pseudo_hdr.length_n = ip_htonl(len);
        ipcom_memset(nat_pt_pseudo_hdr.mbz, 0, sizeof(nat_pt_pseudo_hdr.mbz));
        nat_pt_pseudo_hdr.protocol = IP_IPPROTO_ICMPV6;
        ipnet_nat_checksum_update((Ip_u8 *)checksum,
                                  IP_NULL,
                                  0,
                                  (Ip_u8 *)&nat_pt_pseudo_hdr,
                                  sizeof(nat_pt_pseudo_hdr));
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_translate_embedded_icmp
 *===========================================================================
 * Description: Translate an embedded icmp error message for NAT-PT
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              len         - pointer to the length of the packet.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to the NAT mapping.
 * Returns:     1 = Translated.
 *              0 = No translation.
 *             -1 = Drop
 */
IP_STATIC Ip_s32
ipnet_nat_pt_translate_embedded_icmp(Ip_bool incoming, int *len, Ipcom_pkt **pktp, Ipnet_nat_mapping *nat_mapping)
{
    Ipcom_pkt        *new_pkt;
    Ipnet_pkt_icmp   *icmp;
    Ipnet_pkt_icmp   *icmp_embed;
    Ipcom_ip_header  *iph_embed;
    Ipnet_pkt_ip6    *ip6_embed;
    Ipcom_udp_header *udp_embed;
    Ipcom_tcp_header *tcp_embed;
    Ip_u16 hlen_embed, oldsport;
    Ip_u8 ttl, tos, oldtypecode[2], newtypecode[2];
    int plen_embed, diff;

    icmp = (Ipnet_pkt_icmp *)&(*pktp)->data[(*pktp)->start];
    if(incoming == IP_FALSE)
    {
        if (icmp->type == IPCOM_ICMP_ECHO_REQUEST)
            return 0;
        else
            return -1;
    }
    else
    {
        if (icmp->type == IPNET_ICMP6_ECHO_REPLY)
            return 0;
    }

    switch(icmp->type)
    {
        case IPCOM_ICMP_UNREACH:
            icmp->type = IPNET_ICMP6_DST_UNREACH;
            icmp->data.failing_pkt.next_hop_mtu[0] = 0;
            icmp->data.failing_pkt.next_hop_mtu[1] = 0;
            switch(icmp->code)
            {
                case IPNET_NAT_ICMP4_CODE_DST_UNREACH_NET:
                case IPNET_NAT_ICMP4_CODE_DST_UNKNOWN_NET:
                case IPNET_NAT_ICMP4_CODE_DST_UNKNOWN_HOST:
                case IPNET_NAT_ICMP4_CODE_DST_ISOLATED:
                case IPNET_NAT_ICMP4_CODE_DST_UNREACH_TOS_NET:
                case IPNET_NAT_ICMP4_CODE_DST_UNREACH_TOS_HOST:
                    icmp->code = IPNET_ICMP6_DST_UNREACH_NOROUTE;
                    break;
                case IPNET_NAT_ICMP4_CODE_DST_UNREACH_HOST:
                    icmp->code = IPNET_ICMP6_DST_UNREACH_NOROUTE;
                    break;
                case IPNET_NAT_ICMP4_CODE_DST_UNREACH_PROTO:
                    icmp->type = IPNET_ICMP6_PARAM_PROB;
                    icmp->code = IPNET_ICMP6_PARAM_PROB_NXT_HDR;
                    IP_SET_HTONL(icmp->data.failing_pkt.next_hop_mtu, ip_offsetof(Ipnet_pkt_ip6, nxt));
                    break;
                case IPNET_NAT_ICMP4_CODE_DST_UNREACH_PORT:
                    icmp->code = IPNET_ICMP6_DST_UNREACH_NOPORT;
                    break;
                case IPNET_NAT_ICMP4_CODE_DST_NEEDFRAG:
                    return -1;
                case IPNET_NAT_ICMP4_CODE_DST_SRCFAIL:
                    icmp->code = IPNET_ICMP6_DST_UNREACH_NOROUTE;
                    break;
                case IPNET_NAT_ICMP4_CODE_DST_PROHIBITED_NET:
                case IPNET_NAT_ICMP4_CODE_DST_PROHIBITED_HOST:
                    icmp->code = IPNET_ICMP6_DST_UNREACH_ADMIN;
                    break;
                default:
                    return -1;
            }
            break;
        case IPCOM_ICMP_TIMEXCEED:
            icmp->type = IPNET_ICMP6_TIME_EXCEEDED;
            break;
        case IPCOM_ICMP_PARAMPROB:
            icmp->type = IPNET_ICMP6_PARAM_PROB;
            icmp->code = IPNET_ICMP6_PARAM_PROB_ERR_HDR;
            break;
        default:
            return -1;
    }

    iph_embed  = (Ipcom_ip_header *)icmp->data.failing_pkt.ip;
    hlen_embed = (Ip_u16)((iph_embed->vers_len & 0x0f)<<2);
    diff       = IPCOM_IP6_HEADER_LENGTH - hlen_embed;
    plen_embed = (Ip_u16)(ip_ntohs(iph_embed->length) - hlen_embed);
    tos = iph_embed->tos;
    ttl = iph_embed->ttl;
    if(iph_embed->protocol == IP_IPPROTO_UDP)
    {
        udp_embed = (Ipcom_udp_header *)((Ip_u8 *)iph_embed + hlen_embed);

        /* Update pseudo checksum */
        ipnet_nat_pt_embedded_checksum_update(nat_mapping, plen_embed, iph_embed->protocol, &udp_embed->checksum);

        /* Update source port */
        oldsport = udp_embed->src_port;
        udp_embed->src_port = nat_mapping->inner.tuple.src_port_n;

        /* Update checksum for udp source port */
        ipnet_nat_checksum_update((Ip_u8 *)&udp_embed->checksum,
                                  (Ip_u8 *)&oldsport,
                                  sizeof(oldsport),
                                  (Ip_u8 *)&udp_embed->src_port,
                                  sizeof(udp_embed->src_port));
    }
    else if(iph_embed->protocol == IP_IPPROTO_TCP)
    {
        tcp_embed = (Ipcom_tcp_header *)((Ip_u8 *)iph_embed + hlen_embed);


        /* Check if packet contains enough data to update TCP checksum */
        if (*len >= 8 + hlen_embed + IPCOM_TCP_HEADER_LENGTH)
        {
            /* Update pseudo checksum */
            ipnet_nat_pt_embedded_checksum_update(nat_mapping, plen_embed, iph_embed->protocol, &tcp_embed->checksum);
        }

        /* Update source port */
        oldsport = tcp_embed->src_port;
        tcp_embed->src_port = nat_mapping->inner.tuple.src_port_n;

        /* Check if packet contains enough data to update TCP checksum */
        if (*len >= 8 + hlen_embed + IPCOM_TCP_HEADER_LENGTH)
        {
            /* Update checksum for tcp source port */
            ipnet_nat_checksum_update((Ip_u8 *)&tcp_embed->checksum,
                                      (Ip_u8 *)&oldsport,
                                      sizeof(oldsport),
                                      (Ip_u8 *)&tcp_embed->src_port,
                                      sizeof(tcp_embed->src_port));
        }
    }
    else if(iph_embed->protocol == IP_IPPROTO_ICMP)
    {
        icmp_embed = (Ipnet_pkt_icmp *)((Ip_u8 *)iph_embed + hlen_embed);
        if(icmp_embed->type == IPCOM_ICMP_ECHO_REQUEST)
        {
            /* Update pseudo checksum */
            ipnet_nat_pt_embedded_checksum_update(nat_mapping, plen_embed, iph_embed->protocol, &icmp_embed->cksum);

            /* Update type */
            ipcom_memcpy(oldtypecode, &icmp_embed->type, sizeof(oldtypecode));
            icmp_embed->type = IPNET_ICMP6_ECHO_REQUEST;
            ipcom_memcpy(newtypecode, &icmp_embed->type, sizeof(newtypecode));
            /* Update checksum for type */
            ipnet_nat_checksum_update((unsigned char *)&icmp_embed->cksum,
                                      (unsigned char *)oldtypecode, sizeof(oldtypecode),
                                      (unsigned char *)newtypecode, sizeof(newtypecode));
            /* Update echo id */
            oldsport = icmp_embed->data.echo.id;
            icmp_embed->data.echo.id = nat_mapping->inner.tuple.icmp_id_n;
            /* Update checksum for echo id */
            ipnet_nat_checksum_update((unsigned char *)&icmp_embed->cksum,
                                      (unsigned char *)&oldsport, sizeof(oldsport),
                                      (unsigned char *)&icmp_embed->data.echo.id, sizeof(icmp_embed->data.echo.id));
        }
    }

    /* Check if there is room for the increased packet size */
    if ((*pktp)->end + diff > (*pktp)->maxlen)
    {
        new_pkt = ipcom_pkt_malloc(IPNET_NAT_LINK_HDR_SIZE + IPCOM_IP6_HEADER_LENGTH + *len + diff,
                                   IP_FLAG_FC_STACKCONTEXT);
        if(new_pkt == IP_NULL)
            return -1;

        new_pkt->ipstart = IPNET_NAT_LINK_HDR_SIZE;
        new_pkt->start   = new_pkt->ipstart + IPCOM_IP6_HEADER_LENGTH;
        new_pkt->end     = new_pkt->start + *len + diff;
        new_pkt->ifindex = (*pktp)->ifindex;
        new_pkt->flags   = (*pktp)->flags;
        ipcom_memcpy(&new_pkt->data[new_pkt->start], &(*pktp)->data[(*pktp)->start], *len);
        ipcom_pkt_free(*pktp);
        *pktp = new_pkt;
        ip_assert((*pktp)->end <= (*pktp)->maxlen);
        /* Reset pointers */
        icmp = (Ipnet_pkt_icmp *)&(*pktp)->data[(*pktp)->start];
        iph_embed  = (Ipcom_ip_header *)icmp->data.failing_pkt.ip;
    }

    /* Move payload in order to fit an IPv6 header */
    ipcom_memmove((Ip_u8 *)iph_embed + IPCOM_IP6_HEADER_LENGTH,
                  (Ip_u8 *)iph_embed + hlen_embed,
                   *len - 8 - hlen_embed);

    /* Write Ipv6 header */
    ip6_embed = (Ipnet_pkt_ip6 *)iph_embed;
    IPNET_IP6_SET_VER_CLASS_FLOW(ip6_embed, 6, tos, 0);
    ip6_embed->plen = ip_htons(plen_embed);
    ip6_embed->nxt  = iph_embed->protocol == IP_IPPROTO_ICMP ? IP_IPPROTO_ICMPV6 : iph_embed->protocol;
    ip6_embed->hlim = ttl;
    IPNET_IP6_SET_ADDR(&ip6_embed->src, &nat_mapping->inner.tuple.pt_src);
    IPNET_IP6_SET_ADDR(&ip6_embed->dst, &nat_mapping->inner.tuple.pt_dst);

    /* Signal the change in length */
    *len += diff;
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_translate_icmp6_id
 *===========================================================================
 * Description: Translate the ICMPv6 id.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to the NAT mapping.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_pt_translate_icmp6_id(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping)
{
    Ipnet_pkt_icmp *icmp;

    icmp = (Ipnet_pkt_icmp *)&pkt->data[pkt->start];
    if(incoming == IP_FALSE && icmp->type == IPNET_ICMP6_ECHO_REQUEST)
    {
        icmp->type = IPCOM_ICMP_ECHO_REQUEST;
        icmp->data.echo.id = nat_mapping->inner.nat_port_n;
    }
    else if(incoming == IP_TRUE && icmp->type == IPCOM_ICMP_ECHO_REPLY)
    {
        icmp->type = IPNET_ICMP6_ECHO_REPLY;
        icmp->data.echo.id = nat_mapping->inner.tuple.icmp_id_n;
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_loop_rules
 *===========================================================================
 * Description: Loop through NAT-PT rules and match packets against mappings
 *              and rules.
 * Parameters:  param4 - pointer to the output parameters.
 *              pktp   - pointer to pointer to packet.
 * Returns:     IPNET_NAT_PT_TRANSLATED
 *              IPNET_NAT_NOMATCH
 *              IPNET_NAT_INVALID
 *              IPNET_NAT_DROP
 */
IP_STATIC Ip_s32
ipnet_nat_pt_loop_rules(Ipnet_ip4_output_param *param4, Ipcom_pkt **pktp)
{
    Ip_s32 ret, i = 0;
    Ipnet_nat_tuple nat_tuple;
    Ipnet_nat_mapping *nat_mapping;
    Ipnet_nat_rule *nat_rule;
    int origstart;

    /* Get the tuple */
    origstart = (*pktp)->start;
    ret = ipnet_nat_pt_get_tuple(*pktp, &nat_tuple);
    if(ret < 0)
    {
        IPCOM_LOG0(INFO, "ipnet_nat_pt_loop_rules() :: INCOMING v6 packet is invalid");
        ipnet->nat.inner.stats.invalid_in++;
        (*pktp)->start = origstart;
        return IPNET_NAT_INVALID;   /* Packet is invalid in some way */
    }

    /* Change to IPv4 */
    if(nat_tuple.protocol == IP_IPPROTO_ICMPV6)
        nat_tuple.protocol = IP_IPPROTO_ICMP;
    if(nat_tuple.fragoff == 0 && nat_tuple.icmp_type == IPNET_ICMP6_ECHO_REQUEST)
        nat_tuple.icmp_type = IPCOM_ICMP_ECHO_REQUEST;

    /* Search for existing mapping */
    nat_mapping = ipnet_nat_find_mapping(IP_FALSE, *pktp, &nat_tuple);
    if(nat_mapping != IP_NULL)
    {
        /* Perform the network address translation */
        ipnet_nat_update_mapping(IP_FALSE, nat_mapping, &nat_tuple);
        return ipnet_nat_pt_translate(IP_FALSE, nat_mapping, pktp, &nat_tuple, param4);
    }

    /* Match packet against nat rules */
    if (nat_tuple.fragoff == 0)
    {
	    nat_rule = IPCOM_LIST_FIRST(&ipnet->nat.head_rule);
	    while(nat_rule != IP_NULL)
	    {
	        i++;
            ret = ipnet_nat_match_rule(IP_FALSE, nat_rule, (*pktp)->ifindex, &nat_tuple);
            if(ret == 1)
            {
                IPCOM_LOG1(DEBUG, "ipnet_nat_pt_loop_rules() :: INCOMING v6 packet matched rule @%d", i);
                break;
            }
            /* Try next rule */
            nat_rule = IPCOM_LIST_NEXT(&nat_rule->list_rule);
	    }

        if(nat_rule != IP_NULL)
        {
            /* Add a mapping */
            nat_mapping = ipnet_nat_add_mapping(IP_FALSE,
                                                nat_rule,
                                                &nat_tuple,
                                                0,
                                                IP_NULL,
                                                IP_FALSE,       /* Rule selects port */
                                                nat_rule->proxy_func,
                                                nat_rule->proxy_cookie);
            if(nat_mapping == IP_NULL)
            {
                IPCOM_LOG0(WARNING, "ipnet_nat_pt_loop_rules() :: Failed to add mapping. (dropping packet)");
                ipnet->nat.inner.stats.dropped_in++;
                (*pktp)->start = origstart;
                return IPNET_NAT_DROP;
            }
            return ipnet_nat_pt_translate(IP_FALSE, nat_mapping, pktp, &nat_tuple, param4);
        }
    }

    ipnet->nat.inner.stats.no_rule_match_in++;
    (*pktp)->start = origstart;
    return IPNET_NAT_NOMATCH;
}


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_pt_translate
 *===========================================================================
 * Description: Translate a packet for NAT-PT.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              nat_mapping - pointer to NAT mapping.
 *              pktp        - pointer to pointer to the packet.
 *              nat_tuple   - pointer to packet tuple.
 *              param       - pointer to the output parameters.
 * Returns:     IPNET_NAT_PT_TRANSLATED
 *              IPNET_NAT_DROP
 */
IP_GLOBAL Ip_s32
ipnet_nat_pt_translate(Ip_bool incoming,
                       Ipnet_nat_mapping *nat_mapping,
                       Ipcom_pkt **pktp,
                       Ipnet_nat_tuple *nat_tuple,
                       void *param)
{
    Ipcom_ip_header  *iph;
    Ipnet_pkt_ip6    *ip6;
    Ipnet_ip4_output_param *param4;
    Ipnet_ip6_output_param *param6;
    Ip_bool mod = IP_FALSE;
    Ip_s32 ret, delta = 0;
    int len, hlen;
#ifdef IPCOM_USE_SYSLOG
    char protostr[16];
#endif
    static struct Ip_in6_addr dstaddr6;

    ip_assert(nat_mapping != IP_NULL);
    ip_assert(pktp != IP_NULL);
    ip_assert(*pktp != IP_NULL);
    ip_assert(nat_tuple != IP_NULL);
    ip_assert(param != IP_NULL);

    /* Translate the tcp/udp port */
    if(nat_tuple->fragoff == 0)
    {
        switch(nat_tuple->protocol)
        {
        case IP_IPPROTO_TCP:
            ipnet_nat_translate_tcpudp_port(incoming, *pktp, nat_mapping);
            break;
        case IP_IPPROTO_UDP:
            ipnet_nat_translate_tcpudp_port(incoming, *pktp, nat_mapping);
            break;
        case IP_IPPROTO_ICMP:
            ipnet_nat_pt_translate_icmp6_id(incoming, *pktp, nat_mapping);
            break;
        default:
            break;
        }
    }

    /* Call the application proxy */
    if(nat_mapping->proxy_func != IP_NULL && nat_tuple->protocol != IP_IPPROTO_ICMP)
    {
        /* Options can be skipped since the header is not used anyway */
        hlen = incoming == IP_TRUE ? IPCOM_IP_HEADER_LENGTH : IPNET_IP6_HDR_SIZE;
        ret = ipnet_nat_proxy(incoming, pktp, nat_tuple, nat_mapping, hlen, &mod, &delta);
        if (ret < 0)
        {
            /* Drop packet */
            IPCOM_LOG1(INFO, "ipnet_nat_pt_translate() :: Proxy %s dropped packet",
                       nat_mapping->rule->inner.proxy_label);
            if(incoming == IP_FALSE)
                ipnet->nat.inner.stats.dropped_out++;
            else
                ipnet->nat.inner.stats.dropped_in++;
            return IPNET_NAT_DROP;
        }
    }

    len = (*pktp)->end - (*pktp)->start;
    if(nat_tuple->fragoff == 0)
    {
        if(nat_tuple->protocol == IP_IPPROTO_TCP)
        {
            Ipcom_tcp_header *tcp = (Ipcom_tcp_header *)&(*pktp)->data[(*pktp)->start];

            /* Save, translate and update the sequence number */
            if(delta != 0)
            {
                Ip_u32 seq_num;

                seq_num = IP_GET_32ON16((void *)&tcp->seq_num);
                seq_num = ip_ntohl(seq_num);
                ipnet_nat_translate_sequence_number(incoming, *pktp, nat_mapping);
                ipnet_nat_update_sequence_number(incoming, nat_mapping, seq_num, delta);
            }
            else
            {
                ipnet_nat_translate_sequence_number(incoming, *pktp, nat_mapping);
            }

            /* Translate acknowledgment number */
            ipnet_nat_translate_ack_number(incoming, *pktp, nat_mapping);

            if (nat_tuple->fragmf == 0)
            {
                /* Calculate checksum */
                ipnet_nat_pt_tcp_checksum(incoming, *pktp, nat_mapping, nat_tuple, len);
            }
            else
            {
                ip_assert(incoming == IP_FALSE);
                IPCOM_LOG1(INFO, "ipnet_nat_pt_translate() :: Dropping %s fragment",
                                   ipnet_nat_protocol_name(nat_tuple->protocol, protostr, sizeof(protostr)));
                ipnet->nat.inner.stats.dropped_out++;
                return IPNET_NAT_DROP;
            }
        }
        else if(nat_tuple->protocol == IP_IPPROTO_UDP)
        {
            Ipcom_udp_header *udp = (Ipcom_udp_header *)&(*pktp)->data[(*pktp)->start];

            /* Fix UDP length */
            if(delta != 0)
            {
                Ip_u16 tmp;

                tmp = udp->length;
                tmp = ip_ntohs(tmp);
                tmp = (Ip_u16)((Ip_s32)tmp + delta);
                ip_assert(tmp == (Ip_u16)len);
                udp->length = ip_htons(tmp);
            }

            if (nat_tuple->fragmf == 0)
            {
                /* Calculate checksum */
                ipnet_nat_pt_udp_checksum(incoming, *pktp, nat_mapping, nat_tuple, len);
            }
            else
            {
                /* Handle UDP fragments by setting checksum to zero */
                udp->checksum = 0;
            }
        }
        else if(nat_tuple->protocol == IP_IPPROTO_ICMP)
        {
#ifdef IPCOM_USE_SYSLOG
            Ipnet_pkt_icmp *icmp = (Ipnet_pkt_icmp *)&(*pktp)->data[(*pktp)->start];
#endif

            /* Translate the icmp error embedded datagram */
            ret = ipnet_nat_pt_translate_embedded_icmp(incoming, &len, pktp, nat_mapping);
            if (ret < 0)
            {
                IPCOM_LOG3(INFO, "ipnet_nat_pt_translate() :: Dropping %s icmp error message. type = %d code = %d",
                            incoming == IP_FALSE ? "OUTGOING" : "INCOMING", icmp->type, icmp->code);
                if(incoming == IP_FALSE)
                    ipnet->nat.inner.stats.dropped_out++;
                else
                    ipnet->nat.inner.stats.dropped_in++;
                return IPNET_NAT_DROP;
            }

            if (nat_tuple->fragmf == 0)
            {
                /* Calculate checksum */
                ipnet_nat_pt_icmp_checksum(incoming, *pktp, nat_mapping, nat_tuple, len);
            }
            else
            {
                ip_assert(incoming == IP_FALSE);
                IPCOM_LOG1(INFO, "ipnet_nat_pt_translate() :: Dropping %s fragment",
                                   ipnet_nat_protocol_name(nat_tuple->protocol, protostr, sizeof(protostr)));
                ipnet->nat.inner.stats.dropped_out++;
                return IPNET_NAT_DROP;
            }
        }
    }
    else
    {
        ip_assert(incoming == IP_FALSE);
        if (nat_tuple->protocol == IP_IPPROTO_TCP || nat_tuple->protocol == IP_IPPROTO_ICMP)
        {
            IPCOM_LOG1(INFO, "ipnet_nat_pt_translate() :: Dropping %s fragment",
                               ipnet_nat_protocol_name(nat_tuple->protocol, protostr, sizeof(protostr)));
            ipnet->nat.inner.stats.dropped_out++;
            return IPNET_NAT_DROP;
        }
    }

    if(incoming == IP_FALSE)
    {
        /* Populate IPv4 output parameters */
        param4 = param;
        ipcom_memset(param4, 0, sizeof(*param4));
        param4->from = (struct Ip_in_addr *)&nat_mapping->inner.nat_addr_n;
        param4->to = (struct Ip_in_addr *)&nat_mapping->inner.tuple.pt_dst.in6.addr32[3];
        param4->proto = nat_tuple->protocol;
        param4->tos = nat_tuple->tos;
        param4->ttl = nat_tuple->ttl;

        /* Write Ipv4 header */
        (*pktp)->end     = (*pktp)->start + len;
        (*pktp)->start  -= IPCOM_IP_HEADER_LENGTH;
        (*pktp)->ipstart = (*pktp)->start;
        iph = (Ipcom_ip_header *)&(*pktp)->data[(*pktp)->ipstart];
        ipcom_memset(iph, 0, IPCOM_IP_HEADER_LENGTH);
        iph->vers_len = 0x45;
        iph->tos      = nat_tuple->tos;
        iph->length   = ip_htons((Ip_u16)len + IPCOM_IP_HEADER_LENGTH);
        if (nat_tuple->fragmf != 0 || nat_tuple->fragoff != 0)
        {
            /* Attempt to create a unique identifier from the fragment identifier */
            iph->id  = (Ip_u16)(nat_tuple->fragid & 0x000000ff);
            iph->id += (Ip_u16)((nat_tuple->fragid & 0xff000000) >> 16);
        }
        else
        {
            iph->id = (Ip_u16) ipcom_atomic_get(&ipnet->ip4.id);
        }
        iph->frag     = ip_htons(nat_tuple->fragoff);
        if (nat_tuple->fragmf)
            IP_BIT_SET(iph->frag, IPNET_OFF_MF);
        iph->ttl      = nat_tuple->ttl;
        iph->protocol = param4->proto;
        IPNET_IP4_SET_IPADDR(&iph->src, param4->from->s_addr);
        IPNET_IP4_SET_IPADDR(&iph->dst, param4->to->s_addr);
        iph->checksum = 0;
        iph->checksum = ipcom_in_checksum(iph, IPCOM_IP_HEADER_LENGTH);
    }
    else
    {
        /* Populate IPv6 output parameters */
        param6 = param;
        ipcom_memset(param6, 0, sizeof(*param6));
        dstaddr6.in6.addr32[0] = nat_mapping->inner.tuple.pt_dst.in6.addr32[0];
        dstaddr6.in6.addr32[1] = nat_mapping->inner.tuple.pt_dst.in6.addr32[1];
        dstaddr6.in6.addr32[2] = nat_mapping->inner.tuple.pt_dst.in6.addr32[2];
        dstaddr6.in6.addr32[3] = nat_tuple->src_n;
        param6->from = &dstaddr6;
        param6->real_from = param6->from;
        param6->to = &nat_mapping->inner.tuple.pt_src;
        param6->final_to = param6->to;
        param6->nxt_hdr = nat_tuple->protocol == IP_IPPROTO_ICMP ? IP_IPPROTO_ICMPV6 : nat_tuple->protocol;
        param6->tclass = nat_tuple->tos;
        param6->hlim = nat_tuple->ttl - 1;

        /* Write Ipv6 header */
        if ((*pktp)->start - IPNET_IP6_HDR_SIZE < 0)
        {
            Ipcom_pkt *new_pkt;

            new_pkt = ipcom_pkt_malloc(len + IPNET_IP6_HDR_SIZE,
                                       IP_FLAG_FC_STACKCONTEXT);
            if(new_pkt == IP_NULL)
            {
                IPCOM_LOG1(INFO, "ipnet_nat_pt_translate() :: Could not create new packet with length = %d",
                           len + IPNET_IP6_HDR_SIZE);
                return IPNET_NAT_DROP;
            }
            new_pkt->start   = IPNET_IP6_HDR_SIZE;
            new_pkt->ifindex = (*pktp)->ifindex;
            new_pkt->flags   = (*pktp)->flags;
            ipcom_memcpy(&new_pkt->data[new_pkt->start], &(*pktp)->data[(*pktp)->start], len);
            ipcom_pkt_free(*pktp);
            *pktp = new_pkt;
        }
        (*pktp)->end = (*pktp)->start + len;
        (*pktp)->start  -= IPNET_IP6_HDR_SIZE;
        (*pktp)->ipstart = (*pktp)->start;
        ip6 = (Ipnet_pkt_ip6 *)&(*pktp)->data[(*pktp)->ipstart];
        ipcom_memset(ip6, 0, IPNET_IP6_HDR_SIZE);
        IPNET_IP6_SET_VER_CLASS_FLOW(ip6, 6, param6->tclass, param6->flowinfo);
        ip6->plen = ip_htons((Ip_u16)len);
        ip6->nxt  = param6->nxt_hdr;
        ip6->hlim = param6->hlim;
        IPNET_IP6_SET_ADDR(&ip6->src, param6->from);
        IPNET_IP6_SET_ADDR(&ip6->dst, param6->to);
    }

    if(incoming == IP_FALSE)
        ipnet->nat.inner.stats.translated_out++;
    else
        ipnet->nat.inner.stats.translated_in++;

    IP_BIT_CLR((*pktp)->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);  /* Checksum already calculated above */
    IP_BIT_SET((*pktp)->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
    IP_BIT_CLR((*pktp)->flags, IPCOM_PKT_FLAG_FORWARDED);    /* Avoid a fastpath cache entry to be created */
    IP_BIT_SET((*pktp)->flags, IPCOM_PKT_FLAG_NAT);          /* Avoid NAT output hook to be called */
    return IPNET_NAT_PT_TRANSLATED;
}


/*
 *===========================================================================
 *                    ipnet_nat_pt_output_hook
 *===========================================================================
 * Description: Output hook for NAT-PT.
 * Parameters:  param4 - Ipv4 output parameters.
 *              pktp   - pointer to pointer to packet.
 * Returns:     IPNET_NAT_PT_TRANSLATED
 *              IPNET_NAT_NOMATCH
 *              IPNET_NAT_INVALID
 *              IPNET_NAT_DROP
 */
IP_GLOBAL Ip_s32
ipnet_nat_pt_output_hook(Ipnet_ip4_output_param *param4, Ipcom_pkt **pktp)
{
    Ip_s32 ret;

    /* Check that nat is enabled and that least one rule is defined */
    if(ipnet->nat.inner.open == 0 || ipnet->nat.head_rule.size == 0)
        return IPNET_NAT_NOMATCH;

    ret = ipnet_nat_pt_loop_rules(param4, pktp);
    return ret;
}    /*lint !e550*/


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_nat_pt_empty_file;
#endif /* #ifdef IPNET_USE_NAT_PT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
