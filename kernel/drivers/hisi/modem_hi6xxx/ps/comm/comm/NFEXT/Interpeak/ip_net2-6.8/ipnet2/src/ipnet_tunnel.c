/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_tunnel.c,v $ $Revision: 1.133 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_tunnel.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Implements the following tunnel type:
 * - IPIP tunnel (RFC 1853)
 * - Minimal Encapsulation for IPv4 (RFC 2004)
 * - IPv6 tunnels (RFC 2473)
 * - Transmission of IPv6 over IPv4 Domains without Explicit Tunnels (6over4) (RFC 2529)
 * - Generic Packet Encapsulation (RFC 2784)
 * - 6to4 tunnels (RFC 3056)
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

#ifdef IPNET_USE_TUNNEL

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_netif.h"
#include "ipnet_tunnel.h"

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif

#define IPNET_GET_TUNNEL_PARAM(netif) (&((Ipnet_tunnel_plink *)(netif->ipcom.plink))->tp)


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_TUNNEL_OUTPUT_SANITY_CHECK(netif, pkt) \
    do { \
        if (pkt->start < netif->ipcom.link_hdr_size) \
        { \
            ipcom_pkt_free(pkt); \
            return -IP_ERRNO_EMSGSIZE; \
        } \
        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)) \
        { \
            ipcom_pkt_free(pkt); \
            return -IP_ERRNO_EHOSTDOWN; \
        } \
        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_RUNNING)) \
       { \
           ipcom_pkt_free(pkt); \
           return 0; \
       } \
    } while ((0))


/* Updates of all packet flags before inputing the packet into
   the stack output again */
#define IPNET_TUNNEL_PKT_FLAGS_OUTPUT_UPDATE(pkt) \
    IP_BIT_CLR((pkt)->flags, \
                 IPCOM_PKT_FLAG_HAS_IP_HDR \
               | IPCOM_PKT_FLAG_MORE_FRAG \
               | IPCOM_PKT_FLAG_BROADCAST \
               | IPCOM_PKT_FLAG_TUNNELED \
               | IPCOM_PKT_FLAG_FORWARDED \
               | IPCOM_PKT_FLAG_MULTICAST);



/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
   The format of the minimal forwarding header is as follows:

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   Protocol    |S|  reserved   |        Header Checksum        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                 Original Destination Address                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            (if present) Original Source Address               :
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#include "ipcom_align16.h"
typedef IP_PACKED1 struct Ipnet_pkt_min_encap_struct
{
    Ip_u8  proto;
    Ip_u8  flags;
    Ip_u16 chksum;
    Ip_u16 original_dst_addr[2];
    Ip_u16 original_src_addr[2];
}
IP_PACKED2 Ipnet_pkt_min_encap;
#include "ipcom_align16.h"

/* Length of header when S flag is cleared */
#define IPNET_TUNNEL_MIN_ENCAP_S0_LEN  8
/* Length of header when S flag is set */
#define IPNET_TUNNEL_MIN_ENCAP_S1_LEN  12


/* Tunnel link information */
typedef struct Ipnet_tunnel_plink_struct
{
    struct Ip_ip_tunnel_param tp;
    union Ipnet_tunnel_cache
    {
#ifdef IPCOM_USE_INET
        Ipnet_ip4_tx_cache ipv4;
#endif
#ifdef IPCOM_USE_INET6
        Ipnet_ip6_tx_cache ipv6;
#endif
    } cache;
    Ipnet_ppp_peer peer;
}
Ipnet_tunnel_plink;


/* NAT-T translation */
typedef struct Ipnet_tunnel_nat_t
{
    int     type;
    Ip_u8   *proto;
    Ip_u16  **cksum;
    Ip_u16  sport;
    Ip_u16  dport;
}
Ipnet_tunnel_nat_t;


#ifdef IPCOM_USE_INET
/* Search key for locating the correct tunnel interface for something
   tunneled over IPv4 */
struct Ipnet_tunnel_ip4_key {
    Ipnet_pkt_ip *outer_ip_hdr; /* Outer IPv4 header */
    Ip_u16        vr;           /* Virtual Router Index */
};
#endif

#ifdef IPCOM_USE_INET6
/* Search key for locating the correct tunnel interface for something
   tunneled over IPv6 */
struct Ipnet_tunnel_ip6_key {
    Ipnet_pkt_ip6 *outer_ip_hdr; /* Outer IPv6 header */
    Ip_u16         vr;           /* Virtual Router Index */
};
#endif


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
 *                    ipnet_tunnel_rfc3519_header
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_tunnel_nat_t(Ipcom_pkt               *pkt,
                   Ipnet_tunnel_nat_t      *nat_param)
{
    Ip_u8           *nat_t;
    Ipnet_pkt_udp   *udp_hdr;
    int             ret;

    /* Kind of translation */
    switch (nat_param->type)
    {
    case IP_UDP_NAT_T_RFC3519:
        /* 8 for UDP; 4 for RFC3519 header */
        if (pkt->start < (4 + IPNET_UDP_HDR_SIZE))
        {
            ret = ipnet_increase_hdr_space(&pkt, IP_FALSE);
            if (ret < 0)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 1, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                                  ipnet_tunnel_nat_t, IPCOM_WV_NETD_MSGSIZE,
                                  IPCOM_WV_IPNET_TUNNEL_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(ip4_output_hdr_space++);
                return ret;
            }
        }

        /* First; create the RFC3519 header */
        pkt->start -= 4;
        nat_t = &pkt->data[pkt->start];

        /* UDP HEADER
         * 4 byte NAT-T header;
         *  TYPE(1 byte) := 4
         *  NH(1 byte)   := 4, 47 or 55
         *  RESERVED(2 bytes) := 0
         */
        nat_t[0] = 4;
        nat_t[1] = *nat_param->proto;
        nat_t[2] = nat_t[3] = 0;
        goto udp_encap;

    case IP_UDP_NAT_T_DSMIPV6_TLV:
        {
            Ip_u16  len = 0;

            /* 8 for UDP; 4 for TLV header */
            if (pkt->start < 4)
            {
                ret = ipnet_increase_hdr_space(&pkt, IP_FALSE);
                if (ret < 0)
                {
                    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                      1, 1, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                                      ipnet_tunnel_nat_t, IPCOM_WV_NETD_MSGSIZE,
                                      IPCOM_WV_IPNET_TUNNEL_MODULE, IPCOM_WV_NETD_IP4);
                    IPNET_STATS(ip4_output_hdr_space++);
                    return ret;
                }
            }

            /* First; create the DSMIPv6 TLV header */
            pkt->start -= 4;

            /* UDP HEADER
             * 4 byte TLV header;
             *  TYPE(1 byte) := 0
             *  NH(1 byte) := Protocol Number
             *  LEN(2 bytes) := Length
             */
            nat_t = &pkt->data[pkt->start];
            nat_t[0] = 0;
            nat_t[1] = *nat_param->proto;
            len = (pkt->end - pkt->start - 4);
            IP_SET_HTONS(&nat_t[2], len);
        }
        /* FALL THROUGH */
    case IP_UDP_NAT_T_DSMIPV6:
udp_encap:
        /* 8 for UDP */
        if (pkt->start < (IPNET_UDP_HDR_SIZE))
        {
            ret = ipnet_increase_hdr_space(&pkt, IP_FALSE);
            if (ret < 0)
            {
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 1, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                                  ipnet_tunnel_nat_t, IPCOM_WV_NETD_MSGSIZE,
                                  IPCOM_WV_IPNET_TUNNEL_MODULE, IPCOM_WV_NETD_IP4);
                IPNET_STATS(ip4_output_hdr_space++);
                return ret;
            }
        }

        /* Setup the UDP header */
        pkt->start -= IPNET_UDP_HDR_SIZE;
        udp_hdr = (Ipnet_pkt_udp *) &pkt->data[pkt->start];

        /* Do setup the UDP header */
        udp_hdr->sport = nat_param->sport;
        udp_hdr->dport = nat_param->dport;
        udp_hdr->sum   = 0;
        udp_hdr->ulen  = (Ip_u16) ip_htons(pkt->end - pkt->start);

        /* Calculate the UDP body checksum */
        pkt->chk = ipcom_in_checksum_update(&pkt->data[pkt->start],
                                            pkt->end - pkt->start);

        /* Store the checksum field */
        *nat_param->cksum = &udp_hdr->sum;

        /* Set the protocol to UDP */
        *nat_param->proto = IP_IPPROTO_UDP;

        /* Done */
        return 0;
    }

    /**/
    return 0;
}


#ifdef IPNET_USE_GRE
/*
 *===========================================================================
 *                    ipnet_tunnel_min_encap_create_header
 *===========================================================================
 * Description: Creates a minimal encapsulation header and removes the
 *              original inner IPv4 header.
 * Parameters:  pkt - The packet where the minimal encapsulation header should
 *                    be written to.
 *              outer_ipv4_from_addr - The outer IPv4 address or IP_NULL if
 *                                     that address is unknown.
 *              ttl - Pointer to where the TTL value should be stored.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_min_encap_create_header(Ipcom_pkt                  *pkt,
                                     IP_CONST struct Ip_in_addr *outer_ipv4_from_addr,
                                     Ip_u8                      *ttl)
{
    Ipnet_pkt_ip        *inner_ipv4_hdr;
    int                  inner_ipv4_len;
    Ip_u8                inner_ipv4_proto;
    Ipnet_pkt_min_encap *min_encap_hdr;
    int                  min_encap_len;

    inner_ipv4_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->start];

    if ((inner_ipv4_hdr->v_hl & 0xf0) != 0x40)
    {
        IPCOM_LOG0(WARNING, "Tunnel: IP_IPPROTO_MOBILE is only supported if the inner header is IPv4");
        return -IP_ERRNO_EINVAL;
    }

    if (IPNET_ISFRAG(inner_ipv4_hdr->off))
    {
        /* Do ordinary IPIP tunneling; we can't deal with fragments */
        return 1;
    }

    if (*ttl == 0)
        *ttl = inner_ipv4_hdr->ttl;

    inner_ipv4_len = IPNET_IP4_GET_HDR_LEN(inner_ipv4_hdr) * sizeof(Ip_u32);
    inner_ipv4_proto = inner_ipv4_hdr->p;

    if (outer_ipv4_from_addr != IP_NULL
        && IP_GET_32ON16(inner_ipv4_hdr->src) == outer_ipv4_from_addr->s_addr)
    {
        /* Same source address for inner and outer, don't set the S flag */
        min_encap_len = IPNET_TUNNEL_MIN_ENCAP_S0_LEN;
        pkt->start += inner_ipv4_len - IPNET_TUNNEL_MIN_ENCAP_S0_LEN;
        min_encap_hdr = (Ipnet_pkt_min_encap *) &pkt->data[pkt->start];
        min_encap_hdr->flags = 0;
    }
    else
    {
        Ip_u32 src;

        min_encap_len = IPNET_TUNNEL_MIN_ENCAP_S1_LEN;
        pkt->start += inner_ipv4_len - IPNET_TUNNEL_MIN_ENCAP_S1_LEN;
        min_encap_hdr = (Ipnet_pkt_min_encap *) &pkt->data[pkt->start];

        src = IP_GET_32ON16(inner_ipv4_hdr->src);
        min_encap_hdr->original_dst_addr[0] = inner_ipv4_hdr->dst[0];
        min_encap_hdr->original_dst_addr[1] = inner_ipv4_hdr->dst[1];
        IP_SET_32ON16(min_encap_hdr->original_src_addr, src);
        min_encap_hdr->flags = 0x80; /* Set the S flag */
    }

    min_encap_hdr->proto  = inner_ipv4_proto;
    min_encap_hdr->chksum = 0;
    min_encap_hdr->chksum = ipcom_in_checksum(min_encap_hdr, min_encap_len);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_min_encap_input
 *===========================================================================
 * Description: Restores the full inner IPv4 header from the minimal
 *              encapsulation header.
 * Parameters:  netif - A tunnel interface.
 *              pkt - A inputed packet that has a minimal encapsulation header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_min_encap_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ip_bool              s_flag;
    int                  len;
    Ip_u32               dst = 0;
    Ip_u32               src = 0;
    Ipnet_pkt_min_encap *min_encap_hdr;
    Ipnet_pkt_ip        *ipv4_hdr;
    Ipnet_pkt_ip        *outer_ipv4_hdr;

    min_encap_hdr = (Ipnet_pkt_min_encap *) &pkt->data[pkt->start];
    if (IP_BIT_ISFALSE(min_encap_hdr->flags, 0x80))
    {
        s_flag = IP_FALSE;
        len = IPNET_TUNNEL_MIN_ENCAP_S0_LEN;
    }
    else
    {
        s_flag = IP_TRUE;
        len = IPNET_TUNNEL_MIN_ENCAP_S1_LEN;
        dst = IP_GET_32ON16(min_encap_hdr->original_dst_addr);
        src = IP_GET_32ON16(min_encap_hdr->original_src_addr);
    }

    /* Restore the complete IPv4 header */
    pkt->start -= IPNET_IP_HDR_SIZE - len;
    ipv4_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->start];
    outer_ipv4_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

    if (s_flag)
    {
        IP_SET_32ON16(ipv4_hdr->src, src);
        IP_SET_32ON16(ipv4_hdr->dst, dst);
    }
    else
    {
        ipv4_hdr->src[0] = outer_ipv4_hdr->src[0];
        ipv4_hdr->src[1] = outer_ipv4_hdr->src[1];
        /*  The destination address is already in place */
    }
    ipv4_hdr->p    = min_encap_hdr->proto;
    ipv4_hdr->ttl  = outer_ipv4_hdr->ttl;
    ipv4_hdr->tos  = outer_ipv4_hdr->tos;
    ipv4_hdr->v_hl = 0x45;
    ipv4_hdr->len  = (Ip_u16) ip_htons(pkt->end - pkt->start);
    ipv4_hdr->id   = 0;
    ipv4_hdr->off  = 0;
    ipv4_hdr->sum  = 0;
    ipv4_hdr->sum  = ipcom_in_checksum(ipv4_hdr, IPNET_IP_HDR_SIZE);

    if (netif->ipcom.link_tap)
        netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_HOST, ip_htons(IP_ETH_P_IP), pkt->start, pkt->start);

    return ipnet_ip4_input(netif, pkt);
}
#endif /* IPNET_USE_GRE */


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_tunnel_ip4_obj_hash
 *===========================================================================
 * Description: Computes a hash value for the specified outer IPv4 header.
 * Parameters:  netif - A tunnel interface.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_tunnel_ip4_obj_hash(Ipnet_netif *netif)
{
    unsigned                   hash_val;
    struct Ip_ip_tunnel_param *if_tp = IPNET_GET_TUNNEL_PARAM(netif);

    hash_val = ipcom_hash_update(&if_tp->remote_addr.sin.sin_addr,
                                 sizeof(if_tp->remote_addr.sin.sin_addr),
                                 if_tp->protocol + netif->vr_index);
    return ipcom_hash_update(&if_tp->local_addr.sin.sin_addr,
                             sizeof(if_tp->local_addr.sin.sin_addr),
                             hash_val);
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip4_key_hash
 *===========================================================================
 * Description: Computes a hash value for the specified outer IP header.
 * Parameters:  outer_ip_hdr - Outer IP header.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_tunnel_ip4_key_hash(struct Ipnet_tunnel_ip4_key *key)
{
    unsigned hash_val;
    Ip_u8    proto;
    Ipnet_pkt_ip *outer_ip_hdr = key->outer_ip_hdr;

    /* The tunnel always uses IPIP since it can be used both for tunneling IPv4 and IPv6 */
    proto = (outer_ip_hdr->p == IP_IPPROTO_IPV6 ? IP_IPPROTO_IPIP : outer_ip_hdr->p);

    hash_val = ipcom_hash_update(outer_ip_hdr->src, sizeof(outer_ip_hdr->src), proto + key->vr);
    return ipcom_hash_update(outer_ip_hdr->dst, sizeof(outer_ip_hdr->dst), hash_val);
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip4_hash_cmp
 *===========================================================================
 * Description: Compares if the outer IP header matches the specified interface.
 * Parameters:  netif - An interface
 *              outer_ip_hdr - Outer IP header.
 * Returns:     IP_TRUE if the interface matches the outer IP header.
 *
 */
IP_STATIC Ip_bool
ipnet_tunnel_ip4_hash_cmp(Ipnet_netif *netif, struct Ipnet_tunnel_ip4_key *key)
{
    struct Ip_ip_tunnel_param *if_tp = IPNET_GET_TUNNEL_PARAM(netif);
    Ipnet_pkt_ip              *outer_ip_hdr = key->outer_ip_hdr;
    Ip_u8                      proto;

    proto = (outer_ip_hdr->p == IP_IPPROTO_IPV6 ? IP_IPPROTO_IPIP : outer_ip_hdr->p);
    return netif->vr_index == key->vr
        && if_tp->protocol == proto
        && ipcom_memcmp(&if_tp->local_addr.sin.sin_addr,
                        outer_ip_hdr->dst, sizeof(Ip_u32)) == 0
        && ipcom_memcmp(&if_tp->remote_addr.sin.sin_addr,
                        outer_ip_hdr->src, sizeof(Ip_u32)) == 0;
}



/*
 *===========================================================================
 *                    ipnet_tunnel_tos
 *===========================================================================
 * Description: Extract the TOS bits from the inner header.
 * Parameters:  pkt - a packet
 * Returns:     The TOS bits.
 *
 */
IP_STATIC Ip_u8
ipnet_tunnel_tos(Ipcom_pkt *pkt)
{
#ifdef IPCOM_USE_INET6
    if ((pkt->data[pkt->start] & 0xf0) == 0x60)
        return IPNET_IP6_GET_CLASS(((Ipnet_pkt_ip6 *) &pkt->data[pkt->start]));
#endif
#ifdef IPCOM_USE_INET
    if ((pkt->data[pkt->start] & 0xf0) == 0x40)
        return ((Ipnet_pkt_ip *)&pkt->data[pkt->start])->tos;
#endif

    return 0;
}

/*
 *===========================================================================
 *                    ipnet_tunnel_ip4_output
 *===========================================================================
 * Description: Tunnel packets over IPv4.
 * Parameters:  netif       - The tunnel interface
 *              rt          - The route entry followed by this packet.
 *              plink       - The link parameters for this tunnel.
 *              proto       - The protocol ID to use in the outer header.
 *              tos         - TOS field to propagate to the outer header.
 *              pkt         - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_ip4_output(Ipnet_netif                 *netif,
                        Ipnet_route_entry           *rt,
                        Ipnet_tunnel_plink          *plink,
                        int                          proto,
                        Ip_u8                        tos,
                        Ipcom_pkt                   *pkt)
{
    Ipnet_ip4_output_param     param;
    int                        err;
    struct Ip_sockaddr_in     *gw;
    Ipnet_tunnel_nat_t         nat_t;
    struct Ip_ip_tunnel_param *tp = &plink->tp;

    (void)netif;
    nat_t.type  = IP_UDP_NAT_T_DISABLED;
    ipcom_memset(&param, 0, sizeof(param));
    /* Add outer IPv4 header. */
    if (tp->local_addr.sin.sin_addr.s_addr == IP_INADDR_ANY)
        param.from = IP_NULL;
    else
    {
        param.from = &tp->local_addr.sin.sin_addr;
        if (ipnet_ip4_get_addr_type(param.from->s_addr,
                                    pkt->vr_index,
                                    IP_NULL)
            != IPNET_ADDR_TYPE_UNICAST)
        {
            IPCOM_LOG1(WARNING, "Tunnel %s: Erronous tunnel source address (non local) specified", rt->netif->ipcom.name);
            err = -IP_ERRNO_EADDRNOTAVAIL;
            goto errout;
        }
    }

    /* The source port specified in the tunneling interface */
    nat_t.sport = tp->local_addr.sin.sin_port;

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_TUNNELEND)
        && rt->gateway != IP_NULL
        && rt->gateway->sa_family == IP_AF_INET
        && IPNET_IP4_GET_IPADDR(rt->hdr.key) != ((struct Ip_sockaddr_in *) rt->gateway)->sin_addr.s_addr)
    {
        /* The tunnel endpoint is stored in the gateway field of the route entry */
        gw = (struct Ip_sockaddr_in *) rt->gateway;
    }
    else
    {
        /* Gateway is the configured destinations */
        gw = &tp->remote_addr.sin;
    }

    /* Store the 'to' destination */
    param.to = &gw->sin_addr;

    /* Destination port */
    nat_t.dport = gw->sin_port;
    nat_t.proto = &param.proto;
    nat_t.cksum = &param.chksum_ptr;

    if (param.to->s_addr == IP_INADDR_ANY)
    {
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST | IPCOM_PKT_FLAG_MULTICAST))
            err = 0;
        else
        {
            IPCOM_LOG1(WARNING, "Tunnel %s: No tunnel endpoint specified", rt->netif->ipcom.name);
            err = -IP_ERRNO_ENETUNREACH;
        }
        goto errout;
    }

    /* Set header values */
    param.proto = (Ip_u8) proto;
    param.ttl   = tp->ttl;
    param.cache = &plink->cache.ipv4;
    param.tos   = tos;

    /* Are we using any form of NAT-T translation? */
    if (IP_UNLIKELY(nat_t.dport) != 0 && IP_UNLIKELY(nat_t.sport) != 0)
    {
        /* OK; We've got a UDP NAT-T translation; extract type */
        nat_t.type = gw->sin_zero[0];
    }

#ifdef IPNET_USE_GRE
    if (tp->protocol == IP_IPPROTO_MOBILE)
    {
        err = ipnet_tunnel_min_encap_create_header(pkt, param.from, &param.ttl);
        if (err < 0)
            goto errout;
        else if (err > 0)
            param.proto = IP_IPPROTO_IPIP;
    }
#endif

    if (IP_UNLIKELY(nat_t.type != IP_UDP_NAT_T_DISABLED))
    {
        err = ipnet_tunnel_nat_t(pkt, &nat_t);
        if (err < 0)
            goto errout;
    }

    IPNET_TUNNEL_PKT_FLAGS_OUTPUT_UPDATE(pkt);
    pkt->offset = 0;
    return ipnet_ip4_output(&param, pkt);

 errout:
    ipcom_pkt_free(pkt);
    return err;
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_tunnel_ip6_obj_hash
 *===========================================================================
 * Description: Computes a hash value for the specified outer IPv6 header.
 * Parameters:  netif - A tunnel interface.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_tunnel_ip6_obj_hash(Ipnet_netif *netif)
{
    unsigned                   hash_val;
    struct Ip_ip_tunnel_param *if_tp = IPNET_GET_TUNNEL_PARAM(netif);

    hash_val = ipcom_hash_update(&if_tp->remote_addr.sin6.sin6_addr,
                                 sizeof(if_tp->remote_addr.sin6.sin6_addr),
                                 if_tp->protocol + netif->vr_index);
    return ipcom_hash_update(&if_tp->local_addr.sin6.sin6_addr,
                             sizeof(if_tp->local_addr.sin6.sin6_addr),
                             hash_val);
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip6_key_hash
 *===========================================================================
 * Description: Computes a hash value for the specified outer IP header.
 * Parameters:  outer_ip_hdr - Outer IP header.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_tunnel_ip6_key_hash(struct Ipnet_tunnel_ip6_key *key)
{
    unsigned       hash_val;
    Ip_u8          proto;
    Ipnet_pkt_ip6 *outer_ip_hdr = key->outer_ip_hdr;

    proto = (outer_ip_hdr->nxt == IP_IPPROTO_IPV6
             ? IP_IPPROTO_IPIP
             : outer_ip_hdr->nxt);
    hash_val = ipcom_hash_update(&outer_ip_hdr->src, sizeof(outer_ip_hdr->src), proto + key->vr);
    return ipcom_hash_update(&outer_ip_hdr->dst, sizeof(outer_ip_hdr->dst), hash_val);
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip6_hash_cmp
 *===========================================================================
 * Description: Compares if the outer IP header matches the specified interface.
 * Parameters:  netif - An interface
 *              outer_ip_hdr - Outer IP header.
 * Returns:     IP_TRUE if the interface matches the outer IP header.
 *
 */
IP_STATIC Ip_bool
ipnet_tunnel_ip6_hash_cmp(Ipnet_netif *netif, struct Ipnet_tunnel_ip6_key *key)
{
    struct Ip_ip_tunnel_param *if_tp = IPNET_GET_TUNNEL_PARAM(netif);
    Ip_u8                      proto;
    Ipnet_pkt_ip6             *outer_ip_hdr = key->outer_ip_hdr;

    proto = (outer_ip_hdr->nxt == IP_IPPROTO_IPV6 ? IP_IPPROTO_IPIP : outer_ip_hdr->nxt);
    return key->vr == netif->vr_index
        && if_tp->protocol == proto
        && ipcom_memcmp(&if_tp->local_addr.sin6.sin6_addr,
                        &outer_ip_hdr->dst, sizeof(outer_ip_hdr->dst)) == 0
        && ipcom_memcmp(&if_tp->remote_addr.sin6.sin6_addr,
                        &outer_ip_hdr->src, sizeof(outer_ip_hdr->src)) == 0;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip6_create_addr
 *===========================================================================
 * Description: Creates an automatic IPv6 address.
 * Parameters:  netif - A tunnel interface for which an address should be
 *                      created.
 *              prefix - The first 64 bits of the address.
 *              in6_addr - Buffer where the created address will be stored.
 * Returns:     0 (success)
 *
 */
IP_STATIC int
ipnet_tunnel_ip6_create_addr(Ipnet_netif *netif,
                             IP_CONST struct Ip_in6_addr *prefix,
                             struct Ip_in6_addr *in6_addr)
{
    Ipnet_netif *eth_netif = IP_NULL;
    Ip_u32       i;
    struct Ip_ip_tunnel_param *tp = IPNET_GET_TUNNEL_PARAM(netif);

    in6_addr->in6.addr32[0] = prefix->in6.addr32[0];
    in6_addr->in6.addr32[1] = prefix->in6.addr32[1];

    if (tp->local_addr.sin.sin_addr.s_addr)
    {
        /* RFC 2893 chapter 3.1
           ...
           The Interface Identifier [14] for such an Interface SHOULD be the
           32-bit IPv4 address of that interface, with the bytes in the same
           order in which they would appear in the header of an IPv4 packet,
           padded at the left with zeros to a total of 64 bits.
           ... */
        in6_addr->in6.addr32[2] = 0;
        in6_addr->in6.addr32[3] = tp->local_addr.sin.sin_addr.s_addr;
        return 0;
    }

    IPNET_NETIF_FOR_EACH(eth_netif, i)
    {
        if (eth_netif->ipcom.type == IP_IFT_ETHER)
            break;
    }

    if (eth_netif != IP_NULL)
    {
        /* Use the MAC from an Ethernet interface */
        in6_addr->in6.addr8[8]  = eth_netif->ipcom.link_addr[0] ^ 0x02;
        in6_addr->in6.addr8[9]  = eth_netif->ipcom.link_addr[1];
        in6_addr->in6.addr8[10] = eth_netif->ipcom.link_addr[2];
        in6_addr->in6.addr8[11] = 0xff;
        in6_addr->in6.addr8[12] = 0xfe;
        in6_addr->in6.addr8[13] = eth_netif->ipcom.link_addr[3];
        in6_addr->in6.addr8[14] = eth_netif->ipcom.link_addr[4];
        in6_addr->in6.addr8[15] = eth_netif->ipcom.link_addr[5];
        return 0;
    }

    /* Last resort: use a random number */
    in6_addr->in6.addr32[2] = ipcom_random() & ip_htonl(~0x02000000);
    in6_addr->in6.addr32[3] = ipcom_random();
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip6_output
 *===========================================================================
 * Description: Tunnel packets over IPv6.
 * Parameters:  netif       - The tunnel interface
 *              rt          - The route to use
 *              plink       - The link parameters for this tunnel.
 *              proto       - The protocol ID to use in the outer header.
 *              tos         - TOS to propagate to outer header.
 *              pkt         - The packet to loopback. pkt->start is the offset to the
 *                            IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_ip6_output(Ipnet_netif                 *netif,
                        Ipnet_route_entry           *rt,
                        Ipnet_tunnel_plink          *plink,
                        int                          proto,
                        Ip_u8                        tos,
                        Ipcom_pkt                   *pkt)
{
    Ipnet_ip6_output_param     param;
    int                        err;
    struct Ip_ip_tunnel_param *tp = &plink->tp;
    Ipnet_tunnel_nat_t         nat_t;

    (void)netif;
    /* Add outer IPv6 header. */
    nat_t.type  = IP_UDP_NAT_T_DISABLED;
    ipcom_memset(&param, 0, sizeof(param));
    if (IP_IN6_IS_ADDR_UNSPECIFIED(&tp->local_addr.sin6.sin6_addr) == IP_FALSE)
    {
        param.from = param.real_from = &tp->local_addr.sin6.sin6_addr;
        if (ipnet_ip6_get_addr_type(param.from,
                                    pkt->vr_index,
                                    IP_NULL)
            != IPNET_ADDR_TYPE_UNICAST)
        {
            IPCOM_LOG1(WARNING, "Tunnel %s: Erronous tunnel source address (non local) specified", rt->netif->ipcom.name);
            err = -IP_ERRNO_EADDRNOTAVAIL;
            goto errout;
        }
    }

    nat_t.sport = tp->local_addr.sin6.sin6_port;
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_TUNNELEND)
        && rt->gateway != IP_NULL
        && rt->gateway->sa_family == IP_AF_INET6)
    {
        struct Ip_sockaddr_in6 *gw6 = ((struct Ip_sockaddr_in6 *) rt->gateway);;

        /* The tunnel endpoint is stored in the gateway field of the route entry */
        param.to        = &gw6->sin6_addr;
        param.scope_id  = gw6->sin6_scope_id;
        nat_t.dport     = gw6->sin6_port;
        if (IP_UNLIKELY(nat_t.sport != 0 && nat_t.dport != 0))
            nat_t.type  = gw6->sin6_flowinfo;
    }
    else
    {
        param.to       = &tp->remote_addr.sin6.sin6_addr;
        param.scope_id = tp->remote_addr.sin6.sin6_scope_id;
        nat_t.type     = 0;
    }

    if (IP_IN6_IS_ADDR_UNSPECIFIED(param.to))
    {
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST | IPCOM_PKT_FLAG_MULTICAST))
            err = 0;
        else
        {
            IPCOM_LOG1(WARNING, "Tunnel %s: No tunnel endpoint specified", rt->netif->ipcom.name);
            err = -IP_ERRNO_ENETUNREACH;
        }
        goto errout;
    }

    param.final_to   = param.to;
    param.nxt_hdr    = (Ip_u8) proto;
    param.hlim       = tp->ttl;
    param.cache      = &plink->cache.ipv6;
    param.tclass     = tos;

    /* Destination port */
    nat_t.proto = &param.nxt_hdr;
    nat_t.cksum = &param.chksum_ptr;

#ifdef IPNET_USE_GRE
    if (tp->protocol == IP_IPPROTO_MOBILE)
    {
        err = ipnet_tunnel_min_encap_create_header(pkt, IP_NULL, &param.hlim);
        if (err < 0)
            goto errout;
        else if (err > 0)
            param.nxt_hdr = IP_IPPROTO_IPV6;
    }
#endif

    if (IP_UNLIKELY(nat_t.type != IP_UDP_NAT_T_DISABLED))
    {
        err = ipnet_tunnel_nat_t(pkt, &nat_t);
        if (err < 0)
            goto errout;
    }

    IPNET_TUNNEL_PKT_FLAGS_OUTPUT_UPDATE(pkt);
    pkt->offset = 0;
    return ipnet_ip6_output(&param, pkt);

 errout:
    ipcom_pkt_free(pkt);
    return err;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_tunnel_output
 *===========================================================================
 * Description: Tunnel packets.
 * Parameters:  netif - The interface the packet was sent to.
 *              rt - The route followed.
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    Ipnet_tunnel_plink        *plink = netif->ipcom.plink;
    struct Ip_ip_tunnel_param *tp = &plink->tp;
    int                        outer_proto = IP_IPPROTO_IPIP;
    Ip_u8                      tos;

    ip_assert(tp->protocol == IP_IPPROTO_IPIP
              || tp->protocol == IP_IPPROTO_GRE
              || tp->protocol == IP_IPPROTO_MOBILE);

    if (pkt->start < netif->ipcom.link_hdr_size)
        /* Try to increase the header space, the sanity check
           will discard the packet is this fails */
        (void) ipnet_increase_hdr_space(&pkt, IP_TRUE);

    IPNET_TUNNEL_OUTPUT_SANITY_CHECK(netif, pkt);

    /* Extract frame type; could leave this to the tap handler, but I'd
     * rather do it explicitly */
    if (netif->ipcom.link_tap)
    {
        switch (pkt->data[pkt->ipstart] & 0xf0)
        {
        case 0x40:
            /* Let the tap handler copy the packet */
            netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_OUTGOING, ip_htons(IP_ETH_P_IP), pkt->ipstart, pkt->ipstart);
            break;
        case 0x60:
            netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_OUTGOING, ip_htons(IP_ETH_P_IPV6), pkt->ipstart, pkt->ipstart);
            break;
        }
    }

    /* Extract the TOS or TCLASS field from the packet;
     * Must do it now, since for example GRE adds its own
     * headers to it, which obscures the IP header start
     * for us */
    tos = ipnet_tunnel_tos(pkt);

#ifdef IPCOM_USE_INET6
    if ((pkt->data[pkt->start] & 0xf0) == 0x60)
        outer_proto = IP_IPPROTO_IPV6;
#endif

#ifdef IPNET_USE_GRE
    if (tp->protocol == IP_IPPROTO_GRE
        || tp->protocol == IP_IPPROTO_MOBILE)
    {
        Ip_u8 new_proto = tp->protocol;

        /* Check for change in the IFF_LINK0 flag */
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LINK0))
        {
            if (tp->protocol == IP_IPPROTO_GRE)
                new_proto = IP_IPPROTO_MOBILE;
        }
        else
        {
            if (tp->protocol == IP_IPPROTO_MOBILE)
                new_proto = IP_IPPROTO_GRE;
        }

        if (new_proto != tp->protocol)
        {
            Ipcom_hash *tunnel_ifs = IP_NULL;

            /* IFF_LINK0 flag has changed, update the IP protocol */
            switch (tp->local_addr.sa.sa_family)
            {
#ifdef IPCOM_USE_INET
            case IP_AF_INET:
                tunnel_ifs = ipnet->ip4.tunnel_ifs;
                break;
#endif
#ifdef IPCOM_USE_INET6
            case IP_AF_INET6:
                tunnel_ifs = ipnet->ip6.tunnel_ifs;
                break;
#endif
            }

            if (tunnel_ifs)
            {
                (void)ipcom_hash_remove(tunnel_ifs, netif);
                tp->protocol = new_proto;
                (void)ipcom_hash_add(tunnel_ifs, netif);
            }
        }

        if (tp->protocol == IP_IPPROTO_GRE)
        {
            int ret;

            ret = ipnet_gre_output_append(netif, tp, pkt);
            if (ret < 0)
            {
                ipcom_pkt_free(pkt);
                return ret;
            }
            outer_proto = IP_IPPROTO_GRE;
        }
        else
            outer_proto = IP_IPPROTO_MOBILE;
    }
#endif /* IPNET_USE_GRE */

    netif->obytes += pkt->end - pkt->start;
    netif->opackets++;

    switch (tp->local_addr.sa.sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipnet_tunnel_ip4_output(netif, rt, plink, outer_proto, tos, pkt);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipnet_tunnel_ip6_output(netif, rt, plink, outer_proto, tos, pkt);
#endif
    default:
        IP_PANIC2();
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EINVAL;
    }
}


/*
 *===========================================================================
 *                    ipnet_tunnel_sit_output
 *===========================================================================
 * Description: Tunnels IPv6 packets that uses IPv6-compatible-IPv4 addresses.
 * Parameters:  netif - The interface the packet was sent to.
 *              rt - The route followed.
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
#ifdef IPNET_USE_SIT
IP_STATIC int
ipnet_tunnel_sit_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    Ipnet_ip4_output_param     param;
    Ipnet_pkt_ip6             *inner_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];
    Ipnet_tunnel_plink        *plink = netif->ipcom.plink;
    struct Ip_in_addr          to;
    struct Ip_in_addr          from;

    IPCOM_UNUSED_ARG(rt);

    IPNET_TUNNEL_OUTPUT_SANITY_CHECK(netif, pkt);

    if (IPNET_IP6_GET_VER(inner_hdr) != 6
        || IP_IN6_IS_ADDR_V4COMPAT(&inner_hdr->dst) == IP_FALSE)
    {
        ipcom_pkt_free(pkt);
        return IP_IN6_IS_ADDR_MULTICAST(&inner_hdr->dst) ? 0 : -IP_ERRNO_EINVAL;
    }

    netif->obytes += pkt->end - pkt->start;
    netif->opackets++;

    ipcom_memset(&param, 0, sizeof(param));
    /* Add outer IPv4 header. */
    if (IP_IN6_IS_ADDR_V4COMPAT(&inner_hdr->src) == IP_FALSE)
        param.from  = IP_NULL;
    else
    {
        from.s_addr = IP_GET_32ON16(&inner_hdr->src.in6.addr32[3]);
        param.from = &from;
    }
    /* Copy the address to make sure it is located on the correct alignment */
    to.s_addr = IP_GET_32ON16(&inner_hdr->dst.in6.addr32[3]);
    param.to    = &to;
    param.proto = IP_IPPROTO_IPV6;
    param.ttl   = inner_hdr->hlim;
    param.cache = &plink->cache.ipv4;
    param.tos   = ipnet_tunnel_tos(pkt);

    IPNET_TUNNEL_PKT_FLAGS_OUTPUT_UPDATE(pkt);
    pkt->offset = 0;
    return ipnet_ip4_output(&param, pkt);
}
#endif /* IPNET_USE_SIT */


/*
 *===========================================================================
 *                    ipnet_tunnel_6to4_output
 *===========================================================================
 * Description: Tunnels IPv6 packets that uses 6to4 (2002:IPv4/48) addresses.
 * Parameters:  netif - The interface the packet was sent to.
 *              rt - The route followed.
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
#ifdef IPNET_USE_6TO4
IP_STATIC int
ipnet_tunnel_6to4_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    Ipnet_ip4_output_param     param;
    Ipnet_pkt_ip6             *inner_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];
    Ipnet_tunnel_plink        *plink = netif->ipcom.plink;
    struct Ip_in_addr          ipv4_src;
    struct Ip_in6_addr        *next_hop = IP_NULL;

    IPCOM_UNUSED_ARG(rt);

    IPNET_TUNNEL_OUTPUT_SANITY_CHECK(netif, pkt);

    if (IPNET_IP6_GET_VER(inner_hdr) == 6)
    {
        next_hop = &inner_hdr->dst;
        if (!IPNET_IP6_IS_ADDR_6TO4(&inner_hdr->dst))
        {
            /* Not sending directly to a 6to4 address, check if we are a 6to4 relay */
            if (ipnet_route_lookup_ecmp(IP_AF_INET6,
                                        pkt->vr_index,
                                        0,
                                        &inner_hdr->dst,
                                        0,
                                        netif->ipcom.ifindex,
                                        IP_NULL,
                                        &rt,
                                        IP_NULL) >= 0)
            {
                if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY))
                {
                    ip_assert(rt->gateway->sa_family == IP_AF_INET6);
                    next_hop = &((struct Ip_sockaddr_in6*) rt->gateway)->sin6_addr;
                    if (!IPNET_IP6_IS_ADDR_6TO4(next_hop))
                        next_hop = IP_NULL;
                    /* else: we are acting as a 6to4 relay */
                }
            }
        }
    }

    if (next_hop == IP_NULL)
    {
        ipcom_pkt_free(pkt);
        return IP_IN6_IS_ADDR_MULTICAST(&inner_hdr->dst) ? 0 : -IP_ERRNO_EINVAL;
    }

    netif->obytes += pkt->end - pkt->start;
    netif->opackets++;

    ipcom_memset(&param, 0, sizeof(param));
    /* Add outer IPv4 header. */
    if (IPNET_IP6_IS_ADDR_6TO4(&inner_hdr->src))
    {
        ipcom_memcpy(&ipv4_src, &inner_hdr->src.in6.addr16[1], sizeof(ipv4_src));
        if (ipnet_ip4_get_addr_type(ipv4_src.s_addr,
                                    pkt->vr_index,
                                    IP_NULL) == IPNET_ADDR_TYPE_UNICAST)
            param.from = &ipv4_src;
    }
    param.to    = (IP_CONST struct Ip_in_addr *) &next_hop->in6.addr16[1];
    param.proto = IP_IPPROTO_IPV6;
    param.ttl   = inner_hdr->hlim;
    param.cache = &plink->cache.ipv4;
    param.tos   = ipnet_tunnel_tos(pkt);

    IPNET_TUNNEL_PKT_FLAGS_OUTPUT_UPDATE(pkt);
    pkt->offset = 0;
    return ipnet_ip4_output(&param, pkt);
}
#endif /* IPNET_USE_6TO4 */


#ifdef IPNET_USE_6OVER4
/*
 *===========================================================================
 *                 ipnet_tunnel_6over4_ipv6_to_ipv4_mcast_addr
 *===========================================================================
 * Description: Returns the IPv4 multicast address that should be used
 *              when sending packets to the specified IPv6 multicast address.
 * Parameters:  ipv4_mcast - The IPv4 multicast address to use.
 *              ipv6_mcast - An IPv6 multicast address.
 * Returns:
 *
 */
IP_STATIC void
ipnet_tunnel_6over4_ipv6_to_ipv4_mcast_addr(struct Ip_in_addr *ipv4_mcast,
                                            const struct Ip_in6_addr *ipv6_mcast)
{
    Ip_u8 *ipv4_mcast_dst = (Ip_u8*) ipv4_mcast;

    ipv4_mcast_dst[0] = 239;
    ipv4_mcast_dst[1] = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.6over4.OLS", 192);
    ipv4_mcast_dst[2] = ipv6_mcast->in6.addr8[14];
    ipv4_mcast_dst[3] = ipv6_mcast->in6.addr8[15];
}


/*
 *===========================================================================
 *                    ipnet_tunnel_6over4_output
 *===========================================================================
 * Description: Tunnels IPv6 packets using IPv4 as a virtual Ethernet.
 * Parameters:  netif - The interface the packet was sent to.
 *              rt - The route followed.
 *              pkt - The packet to loopback. pkt->start is the offset to the
 *              IP header.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_6over4_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    Ipnet_ip4_output_param     param;
    Ipnet_pkt_ip6             *inner_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];
    Ipnet_tunnel_plink        *plink = netif->ipcom.plink;
    struct Ip_ip_tunnel_param *tp = &plink->tp;
    struct Ip_in_addr          ipv4_dst;
    Ipnet_ip4_addr_entry      *addr_entry;


    IPNET_TUNNEL_OUTPUT_SANITY_CHECK(netif, pkt);

    if (IPNET_IP6_GET_VER(inner_hdr) != 6)
    {
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EINVAL;
    }

    netif->obytes += pkt->end - pkt->start;
    netif->opackets++;

    ipcom_memset(&param, 0, sizeof(param));
    /* Add outer IPv4 header. */
    if (IP_IN6_IS_ADDR_MULTICAST(&inner_hdr->dst))
        ipnet_tunnel_6over4_ipv6_to_ipv4_mcast_addr(&ipv4_dst, &inner_hdr->dst);
    else
        /* The "+2" is because RFC 2529 chapter 5 states that the link address is
           +-------+-------+-------+-------+-------+-------+-------+-------+
           | Type  |Length | must be zero  |        IPv4 Address           |
           +-------+-------+-------+-------+-------+-------+-------+-------+
           which makes the link address 0:0:ipv4[0]:ipv4[1]:ipv4[2]:ipv4[3]
        */
        ipv4_dst.s_addr = IP_GET_32ON16(IP_SOCKADDR_DL_LLADDR(rt->gateway) + 2);

    param.from  = &tp->local_addr.sin.sin_addr;
    param.to    = &ipv4_dst;
    param.proto = IP_IPPROTO_IPV6;
    param.ttl   = tp->ttl;
    param.cache = &plink->cache.ipv4;
    param.tos    = ipnet_tunnel_tos(pkt);

    addr_entry = ipnet_ip4_get_addr_entry(param.from->s_addr, netif->vr_index, IP_NULL);
    if (addr_entry == IP_NULL)
    {
        /* The interface or the address of the physical interface has been removed */
        IPCOM_LOG1(WARNING, "Tunnel %s: Erronous tunnel source address (non local) specified", rt->netif->ipcom.name);
        return -IP_ERRNO_EADDRNOTAVAIL;
    }
    param.netif = addr_entry->netif;

    IPNET_TUNNEL_PKT_FLAGS_OUTPUT_UPDATE(pkt);
    pkt->offset = 0;
    return ipnet_ip4_output(&param, pkt);
}


/*
 *===========================================================================
 *                    ipnet_tunnel_6over4_create_addr
 *===========================================================================
 * Description: Creates an IPv6 address as part of the automatic address
 *              configuration.
 * Parameters:  netif - A 6over4 interface for which an address should be
 *                      created.
 *              prefix - The first 64 bits of the address.
 *              in6_addr - Buffer where the created address will be stored.
 * Returns:     0 (success)
 *
 */
IP_STATIC int
ipnet_tunnel_6over4_create_addr(Ipnet_netif *netif,
                                IP_CONST struct Ip_in6_addr *prefix,
                                struct Ip_in6_addr *in6_addr)
{
    struct Ip_ip_tunnel_param *tp = IPNET_GET_TUNNEL_PARAM(netif);

    in6_addr->in6.addr32[0] = prefix->in6.addr32[0];
    in6_addr->in6.addr32[1] = prefix->in6.addr32[1];
    in6_addr->in6.addr32[2] = 0;
    in6_addr->in6.addr32[3] = tp->local_addr.sin.sin_addr.s_addr;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_6over4_join_leave_mcast_grp
 *===========================================================================
 * Description: Joins or leaves a specific IPv6 multicast group.
 * Parameters:  netif - A 6over4 interface on which the address will be
 *                      joined/leaved.
 *              mcast_addr - The multicast address to join/leave.
 *              leave - IP_TRUE if the address should be leaved,
 *                     IP_FALSE if the address should be joined.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_6over4_join_leave_mcast_grp(Ipnet_netif *netif,
                                         struct Ip_in6_addr *mcast_addr,
                                         Ip_bool leave)
{
    struct Ip_in_addr          ipv4_mcast;
    Ipnet_netif               *out_if;
    Ipnet_ip4_addr_entry      *addr_entry;
    struct Ip_ip_tunnel_param *tp = IPNET_GET_TUNNEL_PARAM(netif);

    addr_entry = ipnet_ip4_get_addr_entry(tp->local_addr.sin.sin_addr.s_addr,
                                          netif->vr_index,
                                          IP_NULL);
    if (addr_entry == IP_NULL || addr_entry->type != IPNET_ADDR_TYPE_UNICAST)
    {
        IPCOM_LOG2(WARNING,
                   "6over4 tunnel: the address used as local IPv4 address (%s) for %s has been removed",
                   ipcom_inet_ntop(IP_AF_INET, &tp->local_addr.sin.sin_addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   netif->ipcom.name);
        /* No local interface has the address specifed as source */
        return -IP_ERRNO_EADDRNOTAVAIL;
    }

    out_if = addr_entry->netif;
    ipnet_tunnel_6over4_ipv6_to_ipv4_mcast_addr(&ipv4_mcast, mcast_addr);

    if (leave)
        return ipnet_ip4_remove_addr(out_if, ipv4_mcast.s_addr);
    return ipnet_ip4_add_addr(out_if, ipv4_mcast.s_addr, 0, IP_FALSE, IPNET_ADDR_TYPE_MULTICAST);
}
#endif /* IPNET_USE_6OVER4 */

/*
 *===========================================================================
 *                    ipnet_tunnel_sit_add_addresses
 *===========================================================================
 * Description: Adds an IPv4 compatible address for each IPv4 on all other
 *              interfaces.
 * Parameters:  netif - The tunnel interface to configure.
 *              other_netif - A network interface in the system.
 * Returns:
 *
 */
#ifdef IPNET_USE_SIT
IP_STATIC void
ipnet_tunnel_sit_add_addresses(Ipnet_netif *netif, Ipnet_netif *other_netif)
{
    Ipnet_ip4_addr_entry *addr4;
    struct Ip_in6_addr    addr6 = IP_IN6ADDR_ANY_INIT;

    if (other_netif == IP_NULL
        || netif == other_netif
        || IP_BIT_ISFALSE(other_netif->ipcom.flags, IP_IFF_UP))
        return;

    for (addr4 = other_netif->inet4_addr_list;
         addr4 != IP_NULL && addr4->type == IPNET_ADDR_TYPE_UNICAST;
         addr4 = addr4->next)
    {
        addr6.in6.addr32[3] = addr4->ipaddr_n;
        (void)ipnet_ip6_add_addr(netif,
                                 &addr6,
                                 0,
                                 IPCOM_ADDR_INFINITE,
                                 IPCOM_ADDR_INFINITE,
                                 96);
    }
}
#endif /* IPNET_USE_SIT */


/*
 *===========================================================================
 *                    ipnet_tunnel_configure
 *===========================================================================
 * Description: Performes automatic configuration for of the tunnel.
 * Parameters:  netif - The tunnel interface to configure.
 * Returns:
 *
 */
IP_STATIC void
ipnet_tunnel_configure(Ipnet_netif *netif)
{
    switch (ipnet_tunnel_type(netif))
    {
#ifdef IPNET_USE_GRE
    case IP_ARPHRD_IPGRE:
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_RUNNING))
            (void)netif->link_ioctl(netif, IP_SIOCCHGTUNNEL, netif->ipcom.plink);
        break;
#endif /* IPNET_USE_GRE */

#ifdef IPNET_USE_SIT
    case IP_ARPHRD_SIT:
        {
            Ip_u32                i;
            Ipnet_netif          *iter_netif;
            Ipnet_netif          *loopback;

            loopback = ipnet_loopback_get_netif(netif->vr_index);

            IPNET_NETIF_FOR_EACH_ON_VR(iter_netif, netif->vr_index, i)
            {
                if (iter_netif == loopback)
                    continue;
                ipnet_tunnel_sit_add_addresses(netif, iter_netif);
            }
            /* Add the loopback address last */
            ipnet_tunnel_sit_add_addresses(netif, loopback);
        }
        break;
#endif /* IPNET_USE_SIT */

    default:
        break;
    }
}


/*
 *===========================================================================
 *                    ipnet_tunnel_unconfigure
 *===========================================================================
 * Description: Removes any automatic configuration done for this tunnel.
 * Parameters:  netif - The tunnel interface to unconfigure.
 * Returns:
 *
 */
IP_STATIC void
ipnet_tunnel_unconfigure(Ipnet_netif *netif)
{
    switch (ipnet_tunnel_type(netif))
    {
#ifdef IPNET_USE_SIT
    case IP_ARPHRD_SIT:
        {
            Ipnet_ip6_addr_entry *addr;

            while (IP_NULL != (addr = netif->inet6_addr_list))
                (void)ipnet_ip6_remove_addr(netif, &addr->addr);
        }
        break;
#endif
    default:
        break;
    }

}

/*
 *===========================================================================
 *                    ipnet_tunnel_ioctl
 *===========================================================================
 * Description: IO control. Accept join/leave from multicast groups.
 * Parameters:  netif - The network interface on which to perform the operation.
 *              command - The command to perform.
 *              data - Data associated with the command.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
#ifdef IPCOM_USE_INET
    struct Ip_ifreq           *ifreq = data;
    struct Ip_sockaddr_in     *peer_sin = (struct Ip_sockaddr_in *) &ifreq->ip_ifr_dstaddr;
#endif
    Ipnet_tunnel_plink        *plink = netif->ipcom.plink;
    struct Ip_ip_tunnel_param *if_tp = &plink->tp;
    struct Ip_ip_tunnel_param *arg_tp = data;
    struct Ip_ip_tunnel_param iparg;
    Ipnet_netif               *outgoing_if = IP_NULL;
    int                        ret = 0;
    Ipcom_hash                *tunnel_ifs = IP_NULL;

    switch (if_tp->local_addr.sa.sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        tunnel_ifs = ipnet->ip4.tunnel_ifs;
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        tunnel_ifs = ipnet->ip6.tunnel_ifs;
        break;
#endif
    default:
        break;
    }

    switch (command)
    {
    case IP_SIOCXOPEN:
        if (netif->ipcom.arphrd_type == IP_ARPHRD_TUNNEL6OVER4
            && if_tp->local_addr.sa.sa_family != IP_AF_INET)
        {
            IPCOM_LOG1(WARNING, "6over4 tunnel: the local address must be set and have type AF_INET on %s",
                       netif->ipcom.name);
            return -IP_ERRNO_EINVAL;
        }

        ipnet_tunnel_configure(netif);
        ipnet_kioevent(netif, IP_EIOXUP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCXCLOSE:
        ipnet_tunnel_unconfigure(netif);
        ipnet_kioevent(netif, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

#ifdef IPCOM_USE_INET
    case IP_SIOCXADDMULTI_IN:
    case IP_SIOCXDELMULTI_IN:
#endif
#ifdef IPCOM_USE_INET6
    case IP_SIOCXADDMULTI_IN6:
    case IP_SIOCXDELMULTI_IN6:
#ifdef IPNET_USE_6OVER4
        if (netif->ipcom.arphrd_type == IP_ARPHRD_TUNNEL6OVER4)
            ret = ipnet_tunnel_6over4_join_leave_mcast_grp(netif,
                                                           data,
                                                           command == IP_SIOCXDELMULTI_IN6);
#endif
#endif /* IPCOM_USE_INET6 */
        break;

    case IP_X_SIOCGETTUNNEL:
        /* Linux style CHGTUNNEL */

        switch (netif->ipcom.arphrd_type)
        {
#ifdef IPCOM_USE_INET
        case IP_ARPHRD_SIT:
        case IP_ARPHRD_IPGRE:
        case IP_ARPHRD_TUNNEL:
            {
                struct Ip_ip_tunnel_parm  *tp4  = ifreq->ifr_ifru.ifru_data;
                ipcom_strcpy(tp4->name, netif->ipcom.name);
                ipcom_memset(&tp4->iph, 0, sizeof(tp4->iph));
                tp4->iph.ttl      = if_tp->ttl;
                tp4->iph.protocol = if_tp->protocol;
                tp4->iph.frag_off = ip_htons(IPNET_OFF_DF);
                tp4->o_flags      = if_tp->o_flags;
                tp4->i_flags      = if_tp->i_flags;
                tp4->iph.saddr    = if_tp->local_addr.sin.sin_addr.s_addr;
                tp4->iph.daddr    = if_tp->remote_addr.sin.sin_addr.s_addr;
            }
            break;
#endif
#ifdef IPCOM_USE_INET6
        case IP_ARPHRD_TUNNEL6:
        case IP_ARPHRD_IPGRE6:
            {
                struct Ip_ip6_tnl_parm    *tp6  = ifreq->ifr_ifru.ifru_data;

                ipcom_memset(tp6, 0, sizeof(*tp6));
                ipcom_strcpy(tp6->name, netif->ipcom.name);
                tp6->proto  = IP_IPPROTO_IPV6;

                IPNET_IP6_SET_ADDR(&tp6->raddr, &if_tp->remote_addr.sin6.sin6_addr);
                IPNET_IP6_SET_ADDR(&tp6->laddr, &if_tp->local_addr.sin6.sin6_addr);
            }
            break;
#endif
        default:
            return -IP_ERRNO_EINVAL;
        }

        break;

    case IP_SIOCGETTUNNEL:
        ipcom_memcpy(arg_tp, if_tp, sizeof(struct Ip_ip_tunnel_param));
        break;

    case IP_X_SIOCCHGTUNNEL:
        /* Linux style CHGTUNNEL */

        switch (netif->ipcom.arphrd_type)
        {
#ifdef IPCOM_USE_INET
        case IP_ARPHRD_SIT:
        case IP_ARPHRD_IPGRE:
        case IP_ARPHRD_TUNNEL:
            {
                struct Ip_ip_tunnel_parm  *tp4  = ifreq->ifr_ifru.ifru_data;
                ipcom_memset(&iparg, 0, sizeof(iparg));
                ipcom_strcpy(iparg.ifr_name, netif->ipcom.name);

                iparg.ttl      = tp4->iph.ttl;
                iparg.protocol = tp4->iph.protocol;
                iparg.o_flags  = tp4->o_flags;
                iparg.i_flags  = tp4->i_flags;

                iparg.local_addr.sin.sin_family       = IP_AF_INET;
                IPCOM_SA_LEN_SET(&iparg.local_addr.sin, sizeof(struct Ip_sockaddr_in));
                iparg.local_addr.sin.sin_addr.s_addr  = tp4->iph.saddr;

                iparg.remote_addr.sin.sin_family      = IP_AF_INET;
                IPCOM_SA_LEN_SET(&iparg.remote_addr.sin, sizeof(struct Ip_sockaddr_in));
                iparg.remote_addr.sin.sin_addr.s_addr = tp4->iph.daddr;
            }
            break;
#endif
#ifdef IPCOM_USE_INET6
        case IP_ARPHRD_TUNNEL6:
        case IP_ARPHRD_IPGRE6:
            {
                struct Ip_ip6_tnl_parm    *tp6  = ifreq->ifr_ifru.ifru_data;

                ipcom_memset(&iparg, 0, sizeof(iparg));
                ipcom_strcpy(iparg.ifr_name, netif->ipcom.name);

                iparg.protocol = tp6->proto;

                iparg.local_addr.sin6.sin6_family       = IP_AF_INET6;
                IPCOM_SA_LEN_SET(&iparg.local_addr.sin6, sizeof(struct Ip_sockaddr_in6));
                IPNET_IP6_SET_ADDR(&iparg.local_addr.sin6.sin6_addr, &tp6->laddr);

                iparg.remote_addr.sin6.sin6_family      = IP_AF_INET6;
                IPCOM_SA_LEN_SET(&iparg.remote_addr.sin6, sizeof(struct Ip_sockaddr_in6));
                IPNET_IP6_SET_ADDR(&iparg.remote_addr.sin6.sin6_addr, &tp6->raddr);
            }
            break;
#endif
        default:
            return -IP_ERRNO_EINVAL;
        }

        arg_tp = &iparg;

        /* FALL THROUGH */
    case IP_SIOCCHGTUNNEL:
        switch (netif->ipcom.arphrd_type)
        {
        case IP_ARPHRD_NONE:
        case IP_ARPHRD_SIT:
        case IP_ARPHRD_TUNNEL6TO4:
            IPCOM_LOG1(DEBUG, "Tunnel: no parameters can be changed on the %s interface", netif->ipcom.name);
            return -IP_ERRNO_EINVAL;
        case IP_ARPHRD_TUNNEL6OVER4:
            if (arg_tp->local_addr.sa.sa_family != IP_AF_INET)
            {
                IPCOM_LOG2(WARNING, "Tunnel: the local address must have domain IP_AF_INET, was %d != %d on %s",
                           arg_tp->local_addr.sa.sa_family, netif->ipcom.name);
                return -IP_ERRNO_EINVAL;
            }
            tunnel_ifs = IP_NULL;
            break;
        default:
            if (arg_tp->local_addr.sa.sa_family != arg_tp->remote_addr.sa.sa_family)
            {
                IPCOM_LOG3(WARNING, "Tunnel: the local and remote address must have the same domain %d != %d on %s",
                           arg_tp->local_addr.sa.sa_family, arg_tp->remote_addr.sa.sa_family, netif->ipcom.name);
                return -IP_ERRNO_EINVAL;
            }

            break;
        }

        /* Outgoing interface is used to get reasonable value for MTU,
           the local address is also verified to be a valid unicast
           address if the tunnel type is 6over4 */
        switch (arg_tp->local_addr.sa.sa_family)
        {
#ifdef IPCOM_USE_INET
        case IP_AF_INET:
            if (arg_tp->local_addr.sin.sin_addr.s_addr != IP_INADDR_ANY
                || netif->ipcom.arphrd_type == IP_ARPHRD_TUNNEL6OVER4)
            {
                Ipnet_ip4_addr_entry *addr_entry;

                addr_entry = ipnet_ip4_get_addr_entry(arg_tp->local_addr.sin.sin_addr.s_addr,
                                                      netif->vr_index,
                                                      IP_NULL);
                if (addr_entry == IP_NULL || addr_entry->type != IPNET_ADDR_TYPE_UNICAST)
                    /* No local interface has the address specifed as source */
                    return -IP_ERRNO_EADDRNOTAVAIL;
                outgoing_if = addr_entry->netif;
            }
            break;
#endif
#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            if (netif->ipcom.arphrd_type == IP_ARPHRD_TUNNEL6OVER4)
            {
                IPCOM_LOG1(ERR, "A 6over4 (%s) tunnel must have an IPv4 unicast local address",
                           netif->ipcom.name);
                return -IP_ERRNO_EINVAL;
            }

            if (!IP_IN6_IS_ADDR_UNSPECIFIED(&arg_tp->local_addr.sin6.sin6_addr))
            {
                Ipnet_ip6_addr_entry *addr_entry;

                addr_entry = ipnet_ip6_get_addr_entry(&arg_tp->local_addr.sin6.sin6_addr,
                                                      netif->vr_index,
                                                      IP_NULL);
                if (addr_entry == IP_NULL || addr_entry->type != IPNET_ADDR_TYPE_UNICAST)
                    /* No local interface has the address specifed as source */
                    return -IP_ERRNO_EADDRNOTAVAIL;
                outgoing_if = addr_entry->netif;
            }
            break;
#endif
        default:
            IPCOM_LOG2(ERR, "Unsupported local address family %d for tunnel %s",
                       arg_tp->local_addr.sa.sa_family, netif->ipcom.name);
            return -IP_ERRNO_EINVAL;
        }

        if (tunnel_ifs != IP_NULL)
            (void) ipcom_hash_remove(tunnel_ifs, netif);

        ipcom_memmove(&if_tp->local_addr,
                      &arg_tp->local_addr,
                      sizeof(if_tp->local_addr));

        if (netif->ipcom.arphrd_type != IP_ARPHRD_TUNNEL6OVER4)
            ipcom_memmove(&if_tp->remote_addr,
                          &arg_tp->remote_addr,
                          sizeof(if_tp->remote_addr));
        else
        {
            if_tp->remote_addr.sa.sa_family = IP_AF_UNSPEC;
            IPCOM_SA_LEN_SET(&if_tp->remote_addr.sa, sizeof(struct Ip_sockaddr));
        }
        /* else: remote address does not make sense for a 6over4 tunnel */

        /* Note: NetBSD uses 1280, Linux 1480 when tunneling over IPv4
           and 1460 when tunneling over IPv6 */
        ip_assert(netif->ipcom.arphrd_type != IP_ARPHRD_TUNNEL6OVER4 || outgoing_if != IP_NULL);
        if (netif->ipcom.arphrd_type == IP_ARPHRD_TUNNEL6OVER4
            || outgoing_if != IP_NULL)
            netif->ipcom.mtu = outgoing_if->ipcom.mtu;

#ifdef IPNET_USE_GRE
        if (netif->ipcom.arphrd_type == IP_ARPHRD_IPGRE ||
            netif->ipcom.arphrd_type == IP_ARPHRD_IPGRE6)
        {
            /* Note: The usage of the IFF_LINK0 flag is inverted compared to NetBSD */
            if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LINK0))
                if_tp->protocol = IP_IPPROTO_MOBILE;
            else
                if_tp->protocol = IP_IPPROTO_GRE;

            if (if_tp->protocol == IP_IPPROTO_GRE)
            {
                /* Note: NetBSD uses 1450, Linux 1476 (which is maximum on Eth) */
                netif->ipcom.mtu -= 4;
                if (IP_BIT_ISSET(if_tp->o_flags, IPCOM_TUNNEL_FLAG_GRE_CHECKSUM))
                    netif->ipcom.mtu -= 4;
            }
        }
#endif /* IPNET_USE_GRE */

#ifdef IPCOM_USE_INET
        if (arg_tp->local_addr.sa.sa_family == IP_AF_INET)
        {
#ifdef IPNET_USE_6OVER4
            if (netif->ipcom.arphrd_type == IP_ARPHRD_TUNNEL6OVER4)
            {
                int i;

                for (i = 0; i < ipnet->netif_6over4_count; i++)
                    if (netif == ipnet->netif_6over4[i])
                        break;
                if (i == ipnet->netif_6over4_count)
                {
                    void *ptr;
                    ptr = ipcom_realloc(ipnet->netif_6over4,
                                        (ipnet->netif_6over4_count + 1) * sizeof(void *));
                    if (ptr == IP_NULL)
                        return -IP_ERRNO_ENOMEM;

                    ipnet->netif_6over4 = ptr;
                    ipnet->netif_6over4[ipnet->netif_6over4_count++] = netif;
                }
                netif->ipcom.link_index     = outgoing_if->ipcom.ifindex;
                netif->ipcom.link_addr_size = 6; /* Two byte is zero and the next for is a IPv4 address */
                netif->ipcom.link_addr[0] = 0;
                netif->ipcom.link_addr[1] = 0;
                ipcom_memcpy(&netif->ipcom.link_addr[2],
                             &if_tp->local_addr.sin.sin_addr,
                             sizeof(if_tp->local_addr.sin.sin_addr));
                if_tp->ttl = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.6over4.BaseHopLimit", 8);
            }
            else
#endif /* IPNET_USE_6OVER4 */
            {
                if (ipnet->ip4.tunnel_ifs == IP_NULL)
                {
                    ipnet->ip4.tunnel_ifs =
                        ipcom_hash_new((Ipcom_hash_obj_func) ipnet_tunnel_ip4_obj_hash,
                                       (Ipcom_hash_key_func) ipnet_tunnel_ip4_key_hash,
                                       (Ipcom_hash_cmp_func) ipnet_tunnel_ip4_hash_cmp);
                }
                tunnel_ifs = ipnet->ip4.tunnel_ifs;
                if_tp->ttl = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.inet.BaseHopLimit", 64);
            }

            netif->ipcom.mtu          -= IPNET_IP_HDR_SIZE;
            netif->ipcom.link_hdr_size = IPNET_IP_HDR_SIZE;
        }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        if (arg_tp->local_addr.sa.sa_family == IP_AF_INET6)
        {
            if (ipnet->ip6.tunnel_ifs == IP_NULL)
            {
                ipnet->ip6.tunnel_ifs =
                    ipcom_hash_new((Ipcom_hash_obj_func) ipnet_tunnel_ip6_obj_hash,
                                   (Ipcom_hash_key_func) ipnet_tunnel_ip6_key_hash,
                                   (Ipcom_hash_cmp_func) ipnet_tunnel_ip6_hash_cmp);
            }
            tunnel_ifs = ipnet->ip6.tunnel_ifs;
            netif->ipcom.mtu          -= IPNET_IP6_HDR_SIZE;
            netif->ipcom.link_hdr_size = IPNET_IP6_HDR_SIZE;

            if_tp->ttl = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.inet6.BaseHopLimit", 64);
        }
#endif /* IPCOM_USE_INET6 */

        switch (netif->ipcom.arphrd_type)
        {
#ifdef IPNET_USE_GRE
        case IP_ARPHRD_IPGRE:
        case IP_ARPHRD_IPGRE6:
            netif->ipcom.link_hdr_size += IPNET_GRE_MAX_HDR_SIZE;
            break;
#endif
        default:
            break;
        }

        if (arg_tp->ttl)
            if_tp->ttl = arg_tp->ttl;
        if_tp->o_flags = arg_tp->o_flags;
        if_tp->i_flags = arg_tp->i_flags;

        if (netif->ipcom.arphrd_type != IP_ARPHRD_TUNNEL6OVER4)
        {
            int r = 0;
            if (tunnel_ifs == IP_NULL
                || (r = ipcom_hash_add(tunnel_ifs, netif)) != IPCOM_SUCCESS)
            {
                IPCOM_LOG2(ERR,
                           "Failed to configure tunnel interface %s, resulting in hash error code %u",
                           netif->ipcom.name,
                           r);

                ret = -IP_ERRNO_ENOMEM;
                goto conf_errout;
            }
        }

        netif->ipcom.mtu = IP_MAX(netif->ipcom.mtu, 1280);
        ipnet_kioevent(netif, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    conf_errout:
        ipnet_kioevent(netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_X_SIOCDELTUNNEL:
    case IP_SIOCIFDESTROY:
        {
            Ip_bool was_running = IP_FALSE;
            if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_RUNNING))
            {
                was_running = IP_TRUE;
                ipnet_kioevent(netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
            }
#ifndef IP_PORT_LKM
            ret = ipcom_if_detach(&netif->ipcom);
            if (ret < 0)
            {
                if (was_running)
                    ipnet_kioevent(netif, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
                return ret;
            }
#endif
        }

#ifdef IPNET_USE_GRE
        if (netif->ipcom.arphrd_type == IP_ARPHRD_IPGRE || netif->ipcom.arphrd_type == IP_ARPHRD_IPGRE6)
            ipnet_gre_destroy(netif);
#endif /* IPNET_USE_GRE */

#ifdef IPNET_USE_6OVER4
        if (netif->ipcom.arphrd_type == IP_ARPHRD_TUNNEL6OVER4)
        {
            int i;
            for (i = 0; i < ipnet->netif_6over4_count; i++)
                if (netif == ipnet->netif_6over4[i])
                    ipnet->netif_6over4[i] = ipnet->netif_6over4[--ipnet->netif_6over4_count];
            if (ipnet->netif_6over4_count == 0)
            {
                ipcom_free(ipnet->netif_6over4);
                ipnet->netif_6over4 = IP_NULL;
            }
        }
        else
#endif /* IPNET_USE_6OVER4 */
        if (tunnel_ifs != IP_NULL)
        {
            (void) ipcom_hash_remove(tunnel_ifs, netif);
            if (tunnel_ifs->elem == 0)
            {
                ipcom_hash_delete(tunnel_ifs);
#ifdef IPCOM_USE_INET
                if (if_tp->local_addr.sa.sa_family == IP_AF_INET)
                    ipnet->ip4.tunnel_ifs = IP_NULL;
#endif
#ifdef IPCOM_USE_INET6
                if (if_tp->local_addr.sa.sa_family == IP_AF_INET6)
                    ipnet->ip6.tunnel_ifs = IP_NULL;
#endif
            }
        }

        ipnet_if_clean_ppp_peer(&plink->peer);
#ifndef IP_PORT_LKM
        ret = ipcom_if_free(&netif->ipcom);
#endif
        break;

#ifdef IPCOM_USE_INET
    case IP_SIOCGIFDSTADDR:
        ipcom_memset(peer_sin, 0, sizeof(struct Ip_sockaddr_in));
        peer_sin->sin_family      = IP_AF_INET;
        IPCOM_SA_LEN_SET(peer_sin, sizeof(struct Ip_sockaddr_in));
        ipcom_memcpy(&peer_sin->sin_addr,
                     &plink->peer.peer4,
                     sizeof(struct Ip_in_addr));
        break;

    case IP_SIOCSIFDSTADDR:
        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_POINTOPOINT)
            || peer_sin->sin_family != IP_AF_INET
            || IPCOM_SA_LEN_GET(peer_sin) < sizeof(struct Ip_sockaddr_in))
            return -IP_ERRNO_EINVAL;
        ret = ipnet_if_set_ipv4_ppp_peer(&plink->peer, &peer_sin->sin_addr);
        break;
#endif /* IPCOM_USE_INET */

    case IP_SIOCGIFHWADDR:
        /* Switch to our if type */
        ifreq->ip_ifr_addr.sa_family = (Ip_sa_family_t)netif->ipcom.arphrd_type;
        break;

    default:
        return -IP_ERRNO_EINVAL;
    }
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_input
 *===========================================================================
 * Description: Handles tunnel packets from the input daemon.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_tunnel_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    int         outer_proto = -1;

    if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
    {
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EHOSTUNREACH;
    }

    pkt->ifindex  = netif->ipcom.ifindex;
    pkt->vr_index = netif->vr_index;

    netif->ibytes += pkt->end - pkt->start;
    netif->ipackets++;

    /* Store the outer header offsets */
    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_TUNNELED);

    /* Setup the LLA offsets */
    ipcom_pkt_set_int(pkt, IPNET_PKT_INFO_TUNNEL_IPSTART, pkt->ipstart, int);

    /* Store the offsets */
    switch (pkt->data[pkt->ipstart] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        pkt->lla_off    = ip_offsetof(Ipnet_pkt_ip, src) + pkt->ipstart;
        pkt->lla_size   = 4;
        break;
#endif

#ifdef IPCOM_USE_INET6
    case 0x60:
        pkt->lla_off    = ip_offsetof(Ipnet_pkt_ip6, src) + pkt->ipstart;
        pkt->lla_size   = sizeof(struct Ip_in6_addr);
        break;
#endif
    }

#ifdef IPCOM_USE_PACKET
    /* Give a copy of packet to all matching packet sockets. */
    if (ipnet->num_packet_sock > 0 && ipnet_packet_input(netif, pkt) == IP_FALSE)
        return 0;
#endif


#ifdef IPCOM_USE_INET
    if ((pkt->data[pkt->ipstart] & 0xf0) == 0x40)
    {
        Ipnet_pkt_ip *ippkt = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

        ipcom_pkt_set_info(pkt, IPNET_PKT_INFO_ENCAP_IP, sizeof(struct Ip_in_addr), &ippkt->src[0]);
        outer_proto = ippkt->p;
    }
#endif
#ifdef IPCOM_USE_INET6
    if ((pkt->data[pkt->ipstart] & 0xf0) == 0x60)
    {
        Ipnet_pkt_ip6 *ippkt = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];

        ipcom_pkt_set_info(pkt, IPNET_PKT_INFO_ENCAP_IP6, sizeof(struct Ip_in6_addr), &ippkt->src);
        outer_proto = ippkt->nxt;
    }
#endif

    switch (outer_proto)
    {
#ifdef IPCOM_USE_INET
    case IP_IPPROTO_IPIP:
        if (netif->ipcom.link_tap)
            netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_HOST, ip_htons(IP_ETH_P_IP), pkt->start, pkt->start);
        return ipnet_ip4_input(netif, pkt);
#endif

#ifdef IPCOM_USE_INET6
    case IP_IPPROTO_IPV6:
        if (netif->ipcom.link_tap)
            netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_HOST, ip_htons(IP_ETH_P_IPV6), pkt->start, pkt->start);
        return ipnet_ip6_input(netif, pkt);
#endif

#ifdef IPNET_USE_GRE
    case IP_IPPROTO_GRE:
        return ipnet_gre_input(netif, pkt);

    case IP_IPPROTO_MOBILE:
        return ipnet_tunnel_min_encap_input(netif, pkt);
#endif

    default:
        break;
    }

    IP_PANIC2();
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                      ipnet_eth_get_frame_info
 *===========================================================================
 * Description: Gets header information from the frame header.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              pkt - A compleate link layer fram. pkt->start is the offset
 *                  to the link layer header.
 *              pkt_type - Will be set to one of the IP_PACKET_xxx type,
 *                  can be IP_NULL if this value is not needed.
 *              frame_type - Will be set to the frame type (i.e. the IP proto field).
 *                  can be IP_NULL if this value is not needed.
 *              src - Will be set to the source address of the frame, can
 *                  be IP_NULL.
 *              dst - Will be set to the destination address of the frame,
 *                  can be IP_NULL.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_tunnel_get_frame_info(Ipnet_netif *netif,
                            Ipcom_pkt *pkt,
                            Ip_u8 *pkt_type,
                            Ip_u16 *frame_type,
                            Ip_u8 *src,
                            Ip_u8 *dst)
{
    IPCOM_UNUSED_ARG(netif);
    ip_assert(pkt != IP_NULL);

    switch (pkt->data[pkt->start] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        {
            Ipnet_pkt_ip *ipv4 = (Ipnet_pkt_ip *) &pkt->data[pkt->start];

            if (frame_type != IP_NULL)
                *frame_type = ipv4->p;

            if (src != IP_NULL)
                ipcom_memcpy(src, ipv4->src, sizeof(struct Ip_in_addr));
            if (dst != IP_NULL)
                ipcom_memcpy(dst, ipv4->dst, sizeof(struct Ip_in_addr));

        }
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case 0x60:
        {
            Ipnet_pkt_ip6 *ipv6 = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];

            if (frame_type != IP_NULL)
                *frame_type = ipv6->nxt;

            if (src != IP_NULL)
                ipcom_memcpy(src, &ipv6->src, sizeof(struct Ip_in6_addr));
            if (dst != IP_NULL)
                ipcom_memcpy(dst, &ipv6->dst, sizeof(struct Ip_in6_addr));
        }
        break;
#endif /* IPCOM_USE_INET6 */
    default:
        return -IP_ERRNO_EPROTONOSUPPORT;
    }

    if (pkt_type != IP_NULL)
        *pkt_type = IP_PACKET_HOST;

    return 0;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_tunnel_get_type
 *===========================================================================
 * Description: Returns the type of tunnel the interface implements.
 * Parameters:  name - A tunnel interface name.
 * Returns:     One of the IPNET_TUNNEL_TYPE_xxx constants.
 *
 */
IP_GLOBAL Ip_u16
ipnet_tunnel_get_type(char *name)
{
    if (ipcom_strncmp(name, "gif", 3) == 0)
        return IP_ARPHRD_TUNNEL;
#ifdef IPNET_USE_GRE
    if (ipcom_strncmp(name, "gre", 3) == 0)
        return IP_ARPHRD_IPGRE;
#endif
#ifdef IPNET_USE_SIT
    if (ipcom_strncmp(name, "sit", 3) == 0)
        return IP_ARPHRD_SIT;
#endif
#ifdef IPNET_USE_6TO4
    if (ipcom_strncmp(name, "6to4", 4) == 0)
        return IP_ARPHRD_TUNNEL6TO4;
#endif
#ifdef IPNET_USE_6OVER4
    if (ipcom_strncmp(name, "6over4", 6) == 0)
        return IP_ARPHRD_TUNNEL6OVER4;
#endif
    return IP_ARPHRD_NONE;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_get_type
 *===========================================================================
 * Description: Returns the type of tunnel the interface implements.
 * Parameters:  name - A tunnel interface name.
 * Returns:     One of the IPNET_TUNNEL_TYPE_xxx constants.
 *
 */
IP_GLOBAL Ip_u16
ipnet_tunnel_get_basetype(int vr, char *name, Ip_size_t *sz)
{
    Ipnet_netif *netif  =  ipnet_if_nametonetif(vr, name);
    Ip_size_t   tsz;

    /* Convenience */
    if (!sz)
        sz = &tsz;

    *sz = 0;

    /**/
    if (netif == IP_NULL)
    {
        if (ipcom_strcmp(name, "tunl0") == 0)
        {
            *sz = sizeof(struct Ip_ip_tunnel_parm);
            return IP_ARPHRD_TUNNEL;
        }
#ifdef IPCOM_USE_INET

#ifdef IPNET_USE_GRE
        else if (ipcom_strcmp(name, "gre0") == 0)
        {
            *sz = sizeof(struct Ip_ip_tunnel_parm);
            return IP_ARPHRD_IPGRE;
        }
#endif /* IPNET_USE_GRE */
#ifdef IPNET_USE_SIT
        else if (ipcom_strcmp(name, "sit0") == 0)
        {
            *sz = sizeof(struct Ip_ip_tunnel_parm);
            return IP_ARPHRD_SIT;
        }
#endif /* IPNET_USE_SIT */

#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        else if (ipcom_strcmp(name, "ip6tnl0") == 0)
        {
            *sz = sizeof(struct Ip_ip6_tnl_parm);
            return IP_ARPHRD_TUNNEL6;
        }
#ifdef IPNET_USE_GRE
        else if (ipcom_strcmp(name, "ip6gre0") == 0)
        {
            *sz = sizeof(struct Ip_ip6_tnl_parm);
            return IP_ARPHRD_IPGRE6;
        }
#endif /* IPNET_USE_GRE */

#endif /* IPCOM_USE_INET6 */
        return IP_ARPHRD_NONE;
    }

    return netif->ipcom.arphrd_type;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_get_type
 *===========================================================================
 * Description: Returns the type of tunnel the interface implements.
 * Parameters:  name - A tunnel interface name.
 * Returns:     One of the IPNET_TUNNEL_TYPE_xxx constants.
 *
 */
IP_GLOBAL char *
ipnet_tunnel_get_basename(int vr, Ip_u16 basetype, char *name)
{
    Ipnet_netif *netif;
    const char  *tbase;
    int         i;

    switch (basetype)
    {
    case IP_ARPHRD_TUNNEL:
        tbase = "tunl";
        break;
    case IP_ARPHRD_TUNNEL6:
        tbase = "ip6tnl";
        break;
    case IP_ARPHRD_IPGRE6:
        tbase = "ip6gre";
        break;
    case IP_ARPHRD_SIT:
        tbase = "sit";
        break;
    case IP_ARPHRD_IPGRE:
        tbase = "gre";
        break;
    default:
        return IP_NULL;
    }

    /* Find an empty slot */
    for (i = 0; i < 255; i++)
    {
        ipcom_snprintf(name, IP_IFNAMSIZ, "%s%d", tbase, i);
        netif = (Ipnet_netif*)ipcom_if_nametonetif(vr, name);

        if (netif == IP_NULL)
            return name;
        else
            IPNET_IF_UNLOCK(netif);
    }


    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_type
 *===========================================================================
 * Description: Returns the type of tunnel the interface implements.
 * Parameters:  netif - A tunnel instance
 * Returns:     One of the IPNET_TUNNEL_TYPE_xxx constants.
 *
 */
IP_GLOBAL Ip_u16
ipnet_tunnel_type(Ipnet_netif   *netif)
{
    if (netif->ipcom.type == IP_IFT_TUNNEL)
        return netif->ipcom.arphrd_type;

    return IP_ARPHRD_NONE;
}

/*
 *===========================================================================
 *                    ipnet_tunnel_ip4_input
 *===========================================================================
 * Description: Process packets tunneled over IPv4 (IPIP, IPV6, GRE).
 * Parameters:
 * Returns:     0 = success, 0< = error code.
 *
 */
#ifdef IPCOM_USE_INET
IP_GLOBAL int
ipnet_tunnel_ip4_input(Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip     *outer_ip_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
    struct Ip_in_addr dst;

    dst.s_addr = IPNET_IP4_GET_IPADDR(outer_ip_hdr->dst);
    if (IP_IN_CLASSD(dst.s_addr))
        /* Set the link multicast since the outer IPv4 header is the link
           layer for the inner IP header */
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_LINK_MULTICAST);

    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_IPV4);

    IPCOM_PKT_RESET_CHECKSUM(pkt);
    if (ipnet->ip4.tunnel_ifs != IP_NULL)
    {
        Ipnet_netif      *netif;
        struct Ip_in_addr src;
        struct Ipnet_tunnel_ip4_key key;

        /* Look for interface where both local and remote address of the
           tunnel is specified */
        key.outer_ip_hdr = outer_ip_hdr;
        key.vr           = pkt->vr_index;
        netif = ipcom_hash_get(ipnet->ip4.tunnel_ifs, &key);
        if (netif != IP_NULL)
            return netif->link_input(netif, pkt);

        /* Look for interface where the only the remote address is specified */
        IPNET_IP4_SET_IPADDR(outer_ip_hdr->dst, 0);
        netif = ipcom_hash_get(ipnet->ip4.tunnel_ifs, &key);
        IPNET_IP4_SET_IPADDR(outer_ip_hdr->dst, dst.s_addr);
        if (netif != IP_NULL)
            return netif->link_input(netif, pkt);

        /* Look for interface where the only the local address is specified */
        src.s_addr = IPNET_IP4_GET_IPADDR(outer_ip_hdr->src);
        IPNET_IP4_SET_IPADDR(outer_ip_hdr->src, 0);
        netif = ipcom_hash_get(ipnet->ip4.tunnel_ifs, &key);
        if (netif == IP_NULL)
        {
            /* Look for interface where neither the local nor the remote address
               is specified */
            IPNET_IP4_SET_IPADDR(outer_ip_hdr->dst, 0);
            netif = ipcom_hash_get(ipnet->ip4.tunnel_ifs, &key);
            IPNET_IP4_SET_IPADDR(outer_ip_hdr->dst, dst.s_addr);
        }

        IPNET_IP4_SET_IPADDR(outer_ip_hdr->src, src.s_addr);

        if (netif != IP_NULL)
            return netif->link_input(netif, pkt);
    }

#ifdef IPCOM_USE_INET6
    if (outer_ip_hdr->p == IP_IPPROTO_IPV6)
    {
#ifdef IPNET_USE_SIT
        Ipnet_pkt_ip6        *ip6 = (Ipnet_pkt_ip6 *) &pkt->data[pkt->start];
        Ipnet_ip6_addr_entry *addr;
        /* Try to find a tunnel that matches the destination address of
           the inner IPv6 header */

        addr = ipnet_ip6_get_addr_entry(&ip6->dst, pkt->vr_index, IP_NULL);
        if (addr != IP_NULL
            && addr->netif->ipcom.type == IP_IFT_TUNNEL
            && addr->netif->ipcom.arphrd_type == IP_ARPHRD_SIT)
            return addr->netif->link_input(addr->netif, pkt);
#endif /* IPNET_USE_SIT */

#ifdef IPNET_USE_6OVER4
        {
            int i;
            for (i = 0; i < ipnet->netif_6over4_count; i++)
                if (ipnet->netif_6over4[i]->ipcom.link_index == pkt->ifindex)
                    return ipnet->netif_6over4[i]->link_input(ipnet->netif_6over4[i], pkt);
        }
#endif /* IPNET_USE_6OVER4 */
#ifdef IPNET_USE_6TO4
        /* RFC 3056, chapter 5.3
           ...
           A simple decapsulation rule for incoming IPv4 packets with protocol
           type 41 MUST be implemented:

           ADDITIONAL DECAPSULATION RULE for 6to4 routers

               apply any security checks (see Section 8);
               remove the IPv4 header;
               submit the packet to local IPv6 routing.
           ...
        */
        return ipnet_tunnel_input(ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex), pkt);
#endif /* IPNET_USE_6TO4 */
    }
#endif /* IPCOM_USE_INET6 */

    ipcom_pkt_free(pkt);
    return -IP_ERRNO_ENXIO;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip4_rfc3519_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_tunnel_ip4_rfc3519_input(Ipcom_pkt *pkt)
{
    Ipnet_pkt_ip        *outer_ip_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

    /* Skip the UDP header */
    pkt->start += IPNET_UDP_HDR_SIZE;

    /* Switch the outer protocol to the 'tunneling protocol' */
    outer_ip_hdr->p = pkt->data[pkt->start + 1];

    /* Skip the RFC3519 UDP payload */
    pkt->start += 4;

    /* Go through the ordinary tunnel input */
    return ipnet_tunnel_ip4_input(pkt);
}


#endif /* IPCOM_USE_INET */

/*
 *===========================================================================
 *                    ipnet_tunnel_ip4_rfc3519_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_tunnel_dsmipv6_input(Ipcom_pkt *pkt)
{
    Ip_u8               proto;

    /* Skip the UDP header */
    pkt->start += IPNET_UDP_HDR_SIZE;

    /* Is this a TLV HEADER? */
    if ((pkt->data[pkt->start] & 0xf0) == 0x40)
        proto = IP_IPPROTO_IPIP;
    else if ((pkt->data[pkt->start] & 0xf0) == 0x60)
        proto = IP_IPPROTO_IPV6;
    else
    {
        Ip_u8 *tlv_hdr = &pkt->data[pkt->start];

        /* TLV - Set the outer type; don't bother with the length field */
        proto = tlv_hdr[1];

        /* Skip the DSMIPv6 TLV Header */
        pkt->start += 4;
    }

#ifdef IPCOM_USE_INET
    if ((pkt->data[pkt->ipstart] & 0xf0) == 0x40)
    {
        Ipnet_pkt_ip        *outer_ip_hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
        outer_ip_hdr->p = proto;
        /* Go through the ordinary tunnel input */
        return ipnet_tunnel_ip4_input(pkt);
    }
#endif
#ifdef IPCOM_USE_INET6
    if ((pkt->data[pkt->ipstart] & 0xf0) == 0x60)
    {
        Ipnet_pkt_ip6 *outer_ip_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];
        outer_ip_hdr->nxt = proto;
        /* Go through the ordinary tunnel input */
        return ipnet_tunnel_ip6_input(pkt);
    }
#endif

    ipcom_pkt_free(pkt);
    return -IP_ERRNO_ENXIO;
}


/*
 *===========================================================================
 *                    ipnet_tunnel_ip6_input
 *===========================================================================
 * Description: Process packets tunneled over IPv6 (IPIP, IPV6, GRE).
 * Parameters:
 * Returns:     0 = success, 0< = error code.
 *
 */
#ifdef IPCOM_USE_INET6
IP_GLOBAL int
ipnet_tunnel_ip6_input(Ipcom_pkt *pkt)
{
    Ipnet_netif   *netif;
    Ipnet_pkt_ip6 *outer_ip_hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];

    if (IP_IN6_IS_ADDR_MULTICAST(&outer_ip_hdr->dst))
        /* Set the link multicast since the outer IPv6 header is the link
           layer for the inner IP header */
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_LINK_MULTICAST);
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_IPV6);

    IPCOM_PKT_RESET_CHECKSUM(pkt);

    if (ipnet->ip6.tunnel_ifs != IP_NULL)
    {
        struct Ip_in6_addr          dst;
        struct Ip_in6_addr          src;
        struct Ipnet_tunnel_ip6_key key;

        key.outer_ip_hdr = outer_ip_hdr;
        key.vr           = pkt->vr_index;
        netif = ipcom_hash_get(ipnet->ip6.tunnel_ifs, &key);
        if (netif != IP_NULL)
            return netif->link_input(netif, pkt);

        IPNET_IP6_SET_ADDR(&dst, &outer_ip_hdr->dst);
        IPNET_IP6_SET_ADDR(&outer_ip_hdr->dst, &ip_in6addr_any);
        netif = ipcom_hash_get(ipnet->ip6.tunnel_ifs, &key);
        IPNET_IP6_SET_ADDR(&outer_ip_hdr->dst, &dst);
        if (netif != IP_NULL)
            return netif->link_input(netif, pkt);

        IPNET_IP6_SET_ADDR(&src, &outer_ip_hdr->src);
        IPNET_IP6_SET_ADDR(&outer_ip_hdr->src, &ip_in6addr_any);
        netif = ipcom_hash_get(ipnet->ip6.tunnel_ifs, &key);
        if (netif == IP_NULL)
        {
            IPNET_IP6_SET_ADDR(&outer_ip_hdr->dst, &ip_in6addr_any);
            netif = ipcom_hash_get(ipnet->ip6.tunnel_ifs, &key);
            IPNET_IP6_SET_ADDR(&outer_ip_hdr->dst, &dst);
        }
        IPNET_IP6_SET_ADDR(&outer_ip_hdr->src, &src);
        if (netif != IP_NULL)
            return netif->link_input(netif, pkt);

    }
    /* else: No tunnel interface defined */

    ipcom_pkt_free(pkt);
    return -IP_ERRNO_ENXIO;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_tunnel_setup
 *===========================================================================
 * Description: Initiates a new tunnel interface descriptor in IPNET.
 * Parameters:  netif - The new tunnel interface
 *              type           - Tunnel interface type.
 * Returns:     Pointer to interface descriptor.
 *
 */
IP_GLOBAL int
ipnet_tunnel_setup(Ipnet_netif *netif, int type)
{
    int                        ret = 0;
    Ipnet_tunnel_plink        *plink = netif->ipcom.plink;
    struct Ip_ip_tunnel_param *if_tp = &plink->tp;

    /* Store the type */
    netif->ipcom.arphrd_type = type;

    ipnet_if_init_ppp_peer(netif, &plink->peer);

    switch (type)
    {
    case IP_ARPHRD_TUNNEL:
    case IP_ARPHRD_TUNNEL6:
        if_tp->protocol = IP_IPPROTO_IPIP;
        break;

#ifdef IPNET_USE_GRE
    case IP_ARPHRD_IPGRE:
    case IP_ARPHRD_IPGRE6:
        if_tp->protocol = IP_IPPROTO_GRE;
        ret = ipnet_gre_setup(netif);
        break;
#endif

#ifdef IPNET_USE_SIT
    case IP_ARPHRD_SIT:
        if_tp->protocol = IP_IPPROTO_IPV6;
        IP_BIT_CLR(netif->ipcom.flags, IP_IFF_POINTOPOINT | IP_IFF_MULTICAST);
        IP_BIT_SET(netif->flags2, IPNET_IFF2_NO_IPV4_SUPPORT);
        netif->link_ip4_output      = ipnet_tunnel_sit_output;
        netif->link_ip6_output      = ipnet_tunnel_sit_output;
        netif->link_ip6_create_addr = IP_NULL;
        break;
#endif /* IPNET_USE_SIT */

#ifdef IPNET_USE_6TO4
    case IP_ARPHRD_TUNNEL6TO4:
        if_tp->protocol = IP_IPPROTO_IPV6;
        IP_BIT_CLR(netif->ipcom.flags, IP_IFF_POINTOPOINT | IP_IFF_MULTICAST);
        netif->link_ip4_output      = ipnet_tunnel_6to4_output;
        netif->link_ip6_output      = ipnet_tunnel_6to4_output;
        netif->link_ip6_create_addr = IP_NULL;
        break;
#endif /* IPNET_USE_6TO4 */

#ifdef IPNET_USE_6OVER4
    case IP_ARPHRD_TUNNEL6OVER4:
        if_tp->protocol = IP_IPPROTO_IPV6;
        IP_BIT_CLR(netif->ipcom.flags, IP_IFF_POINTOPOINT | IP_IFF_NOARP);
        IP_BIT_SET(netif->flags2, IPNET_IFF2_NO_IPV4_SUPPORT);
        netif->link_ip4_output      = ipnet_tunnel_6over4_output;
        netif->link_ip6_output      = ipnet_tunnel_6over4_output;
        netif->link_ip6_create_addr = ipnet_tunnel_6over4_create_addr;
        break;
#endif /* IPNET_USE_6TO4 */

    default:
        ret = -IP_ERRNO_EINVAL;
        break;
    }

    return ret;
}

/*
 *===========================================================================
 *                    ipnet_tunnel_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_tunnel_add(Ipnet_socket *sock, const char *name, Ip_u16 type, Ipnet_netif **netifp)
{
    Ipnet_netif               *netif;
    int                        ret;
    int                        ifindex = 0;

    if (netifp)
        *netifp = IP_NULL;

    netif = (Ipnet_netif *)ipcom_if_malloc(IP_IFT_TUNNEL);
    if (netif == IP_NULL)
    {
        ret = -IP_ERRNO_ENOMEM;
        goto done;
    }

    /* Set the interface name */
    ipcom_strncpy(netif->ipcom.name, name, IP_IFNAMSIZ);
    netif->vr_index = sock->vr_index;

    ret = ipnet_tunnel_setup(netif, type);
    if (ret < 0)
        goto badout;

    ret = ipcom_if_attach(&netif->ipcom);
    if (ret < 0)
        goto badout;
    ifindex = ret;

    if (netifp)
        *netifp = netif;

    if (type == IP_ARPHRD_SIT || type == IP_ARPHRD_TUNNEL6TO4)
    {
        /* SIT/6to4 interfaces is always running since the source and destination
         * addresses are extracted from the inner IPv6 packet */
        ipnet_kioevent(netif, IP_EIOXRUNNING, IP_NULL, 0);
    }

done:
    if (ret < 0)
        return ret;
    return ifindex;

 badout:
    (void)ipcom_if_free(&netif->ipcom);
    goto done;
}

/*
 *===========================================================================
 *                    ipnet_tunnel_create_bsd
 *===========================================================================
 * Description: Creates and attaches a new tunnel interface to IPNET.
 * Parameters:  ifreq - The name of the new tunnel interface
 *              or will contain the name of the new interface if ifreq->ifr_name
 *              is a zero length string.
 * Returns:     >0 = the interface index , 0< = error code.
 *
 */
IP_GLOBAL int
ipnet_tunnel_create_bsd(Ipnet_socket *sock, struct Ip_ifreq *ifreq)
{
    Ip_u16 type = ipnet_tunnel_get_type(ifreq->ifr_name);

    if (type == IP_ARPHRD_NONE)
        return -IP_ERRNO_EINVAL;

    return ipnet_tunnel_add(sock, ifreq->ifr_name, type, IP_NULL);
}


/*
 *===========================================================================
 *                    ipnet_tunnel_create_bsd
 *===========================================================================
 * Description: Creates and attaches a new tunnel interface to IPNET.
 * Parameters:  ifreq - The name of the new tunnel interface
 *              or will contain the name of the new interface if ifreq->ifr_name
 *              is a zero length string.
 * Returns:     >0 = the interface index , 0< = error code.
 *
 */
IP_GLOBAL int
ipnet_tunnel_create_linux(Ipnet_socket *sock, struct Ip_ifreq *ifreq)
{
    Ip_u16      basetype    = ipnet_tunnel_get_basetype(sock->vr_index, ifreq->ifr_name, IP_NULL);
    int         ret;
    char        *name;
    Ipnet_netif *netif;

    if (basetype == IP_ARPHRD_NONE)
        return -IP_ERRNO_EINVAL;

    name = (char *)ifreq->ip_ifr_data;
    if (*name == 0)
    {
        if (!ipnet_tunnel_get_basename(sock->vr_index, basetype, name))
            return -IP_ERRNO_EINVAL;
    }

    /* First element is name */
    if ((ret = ipnet_tunnel_add(sock, name, basetype, &netif)) >= 0)
        ret = ipnet_tunnel_ioctl(netif, IP_X_SIOCCHGTUNNEL, ifreq);

    return ret;
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_tunnel_if_init
 *===========================================================================
 * Description: Initializes a new tunnel interface.
 * Parameters:  [in] netif - The tunnel interface to initialize.
 * Returns:     0 = success, 0< = error code.
 *
 */
IP_PUBLIC int
ipnet_tunnel_if_init(Ipnet_netif *netif)
{
    ip_assert(netif->ipcom.type == IP_IFT_TUNNEL);

    netif->ipcom.plink = ipcom_calloc(1, sizeof(Ipnet_tunnel_plink));
    if (netif->ipcom.plink == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    /* Init tunnel interface */
    /* Correct MTU will be calculated in ioctl(IP_SIOCCHGTUNNEL) */
    netif->ipcom.arphrd_type    = IP_ARPHRD_NONE;
    netif->ipcom.mtu            = 1280;
    netif->link_ioctl           = ipnet_tunnel_ioctl;
    netif->ipcom.flags          = IP_IFF_POINTOPOINT | IP_IFF_NOARP | IP_IFF_MULTICAST | IP_IFF_SIMPLEX;
    netif->link_input           = ipnet_tunnel_input;
    netif->link_get_frame_info  = ipnet_tunnel_get_frame_info;
#ifdef IPCOM_USE_PACKET
    netif->link_raw_output      = IP_NULL;
#endif /* IPCOM_USE_PACKET */

#ifdef IPCOM_USE_INET
    netif->link_ip4_output      = ipnet_tunnel_output;
#endif

#ifdef IPCOM_USE_INET6
    netif->link_ip6_output      = ipnet_tunnel_output;
    netif->link_ip6_create_addr = ipnet_tunnel_ip6_create_addr;
    netif->inet6_if_id_bits     = 64;
#endif /* IPCOM_USE_INET6 */

    return 0;
}

#endif /* IPNET_USE_TUNNEL */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

