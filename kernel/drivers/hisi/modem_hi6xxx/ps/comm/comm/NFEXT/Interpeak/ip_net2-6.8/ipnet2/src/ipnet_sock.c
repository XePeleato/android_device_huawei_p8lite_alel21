/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sock.c,v $ $Revision: 1.441.16.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sock.c,v $
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
 * Functions for socket API.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_SKIP_NATIVE_SOCK_API

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
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
#include <ipcom_inet.h>
#include <ipcom_sock2.h>
#include <ipcom_spinlock.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>

#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_usr_sock.h"

#ifdef IPNET_USE_ROUTESOCK
#include "ipnet_routesock.h"
#endif

#ifdef IPNET_USE_NETLINKSOCK
#include "ipnet_netlink.h"
#endif

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPSCTP
#include <ipsctp.h>
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif


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

typedef struct Ipnet_bound_socket_struct
{
    Ip_u32 count;          /* Number of sockets that match [domain, local port, proto] */
    Ip_u16 vr;             /* The virtual router the socket must be assigned to */
    Ip_u16 domain;         /* The domain this socket is part of */
    Ip_u16 proto;          /* The protocol the socket uses */
    Ip_u16 local_port;     /* The local port the socket is bound to */
    Ip_u8  has_reuse_addr; /* !=0 if all matching sockets has reuse addr set */
    Ip_u8  has_reuse_port; /* !=0 if all matching sockets has reuse port set */
} Ipnet_bound_socket;


/* Key when looking up socket operations */
struct Ipnet_sock_ctor_key
{
    int     domain;    /* Domain to lookup */
    int     type;      /* Type to lookup */
    int     proto;     /* Proto to lookup or -1 for wildcard */
    Ip_bool no_domain; /* Only used when determine error code */
    Ip_bool no_type;   /* Only used when determine error code */
};


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IP_PORT_OSE5
Ip_err ipcom_crt_block_wait(Ipcom_socket *sock, Ip_s32 milli_seconds, Ip_bool snd);
void   ipcom_crt_block_post(Ipcom_socket *sock, Ip_bool snd);
#endif

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
IP_GLOBAL int  ipcom_windnet_socklib_socket_init(Ipnet_socket *sock);
IP_GLOBAL void ipcom_windnet_socklib_socket_free(Ipnet_socket *sock);
#ifdef IPNET_SO_WAKEUP_HOOK
IP_GLOBAL void ipcom_windnet_socklib_sowakeupHook(Ipnet_socket *sock, int how);
#endif
#endif

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
 *                    ipnet_sock_bound_obj_hash
 *===========================================================================
 * Description: Calculates a hash value for the bound socket parameters.
 * Parameters:  bs - Info about port, domain, local port, local address.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_sock_bound_obj_hash(Ipnet_bound_socket *bs)
{
    Ip_u32 x[2];

    x[0] = bs->domain + (bs->proto << 8) + (bs->vr << 16);
    x[1] = bs->local_port;
    return ipcom_hash_update(x, sizeof(x), 0);
}


/*
 *===========================================================================
 *                    ipnet_sock_bound_key_hash
 *===========================================================================
 * Description: Computes a hash value for socket lookup search key.
 * Parameters:  key - The search key.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_sock_bound_key_hash(Ipnet_socket_lookup_key *key)
{
    Ip_u32 x[2];

    x[0] = key->domain + (key->proto << 8) + (key->vr << 16);
    x[1] = key->local_port;
    return ipcom_hash_update(x, sizeof(x), 0);
}


/*
 *===========================================================================
 *                    ipnet_sock_bound_hash_cmp
 *===========================================================================
 * Description: Compares if a socket matches the specified lookup key.
 * Parameters:  bs - Info about port, domain, local port, local address.
 *              key - The search key.
 * Returns:     IP_TRUE the bound socket(s) and the key matches.
 *
 */
IP_STATIC Ip_bool
ipnet_sock_bound_hash_cmp(Ipnet_bound_socket *bs, Ipnet_socket_lookup_key *key)
{
    return bs->domain == key->domain
        && bs->proto == key->proto
        && bs->vr == key->vr
        && bs->local_port == key->local_port;
}


/*
 *===========================================================================
 *                    ipnet_sock_obj_hash
 *===========================================================================
 * Description: Calculates a hash value for the socket.
 * Parameters:  sock - A IPNET socket descriptor.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_sock_obj_hash(Ipnet_socket *sock)
{
    unsigned hash_val;
    Ip_u32   x[2];

    x[0] = sock->ipcom.domain + (sock->proto << 8) + (sock->vr_index << 16);
    x[1] = sock->sport + (sock->dport << 16);
    hash_val = ipcom_hash_update(x, sizeof(x), 0);
    switch (sock->ipcom.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        hash_val = ipcom_hash_update(&sock->ip4->saddr_n, sizeof(struct Ip_in_addr), hash_val);
        hash_val = ipcom_hash_update(&sock->ip4->daddr_n, sizeof(struct Ip_in_addr), hash_val);
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        hash_val = ipcom_hash_update(&sock->ip6->saddr, sizeof(struct Ip_in6_addr), hash_val);
        hash_val = ipcom_hash_update(&sock->ip6->daddr, sizeof(struct Ip_in6_addr), hash_val);
        break;
#endif
    default:
        break;
    }
    return hash_val;
}


/*
 *===========================================================================
 *                    ipnet_sock_key_hash
 *===========================================================================
 * Description: Computes a hash value for socket lookup search key.
 * Parameters:  key - The search key.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_sock_key_hash(Ipnet_socket_lookup_key *key)
{
    unsigned hash_val;
    Ip_u32   x[2];

    x[0] = key->domain + (key->proto << 8) + (key->vr << 16);
    x[1] = key->local_port + (key->peer_port << 16);
    hash_val = ipcom_hash_update(x, sizeof(x), 0);
    switch (key->domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        hash_val = ipcom_hash_update(&key->local_addr.in, sizeof(key->local_addr.in), hash_val);
        hash_val = ipcom_hash_update(&key->peer_addr.in, sizeof(key->peer_addr.in), hash_val);
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        hash_val = ipcom_hash_update(&key->local_addr.in6, sizeof(key->local_addr.in6), hash_val);
        hash_val = ipcom_hash_update(&key->peer_addr.in6, sizeof(key->peer_addr.in6), hash_val);
        break;
#endif
    default:
        break;
    }
    return hash_val;
}


/*
 *===========================================================================
 *                      ipnet_sock_hash_cmp
 *===========================================================================
 * Description: Checks if a socket matches a lookup key.
 * Parameters:  sock - A socket.
 *              key - A socket lookup key-
 * Returns:     IP_TRUE the socket(s) and the key matches.
 *
 */
IP_STATIC Ip_bool
ipnet_sock_hash_cmp(Ipnet_socket *sock, Ipnet_socket_lookup_key *key)
{
    if (sock->ipcom.domain != key->domain
        || sock->proto != key->proto
        || sock->vr_index != key->vr
        || sock->sport != key->local_port
        || sock->dport != key->peer_port)
        return IP_FALSE;

    switch (sock->ipcom.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        if (sock->ip4->saddr_n != key->local_addr.in.s_addr
            || sock->ip4->daddr_n != key->peer_addr.in.s_addr)
            return IP_FALSE;
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        if (IP_IN6_ARE_ADDR_EQUAL(&sock->ip6->saddr, &key->local_addr.in6) == IP_FALSE
            || IP_IN6_ARE_ADDR_EQUAL(&sock->ip6->daddr, &key->peer_addr.in6) == IP_FALSE)
            return IP_FALSE;
        break;
#endif
    default:
        break;
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_key_from_sock
 *===========================================================================
 * Description: Creates a search key from the specifed socket.
 * Parameters:  sock - A socket.
 *              key - The search key.
 * Returns:
 *
 */
IP_STATIC void
ipnet_key_from_sock(const Ipnet_socket *sock, Ipnet_socket_lookup_key *key)
{
    key->domain     = sock->ipcom.domain;
    key->vr         = sock->vr_index;
    key->proto      = sock->proto;
    key->local_port = sock->sport;
    key->peer_port  = sock->dport;
    switch (sock->ipcom.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        key->local_addr.in.s_addr = sock->ip4->saddr_n;
        key->peer_addr.in.s_addr  = sock->ip4->daddr_n;
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        key->local_addr.in6 = sock->ip6->saddr;
        key->peer_addr.in6  = sock->ip6->daddr;
        break;
#endif
    default:
        break;
    }
}


/*
 *===========================================================================
 *                    ipnet_bound_sock_from_sock
 *===========================================================================
 * Description: Allocates and initializes a new bound socket structure.
 * Parameters:  sock - A socket.
 * Returns:     The new bound socket structure.
 *
 */
IP_STATIC Ipnet_bound_socket *
ipnet_bound_sock_from_sock(const Ipnet_socket *sock)
{
    Ipnet_bound_socket *bs;

    bs = ipcom_slab_alloc(ipnet->slabs.bound_socket);
    if (bs == IP_NULL)
        return IP_NULL;
    bs->count          = 1;
    bs->has_reuse_addr = (Ip_u8) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_REUSEADDR);
    bs->has_reuse_port = (Ip_u8) IP_BIT_ISTRUE(sock->flags, IPNET_SOCKET_FLAG_REUSEPORT);
    bs->domain         = sock->ipcom.domain;
    bs->vr             = sock->vr_index;
    bs->proto          = sock->proto;
    bs->local_port     = sock->sport;
    return bs;
}


#ifdef IPCOM_USE_INET6


/*
 *===========================================================================
 *                    ipnet_find_sa_headers
 *===========================================================================
 * Description: Tries to find the SA related headers in the packet.
 * Parameters:  ip6hdr      - The IPv6 header for the packet.
 *              msg         - The message
 *              pcmsg       - Pointer to the control message
 *              pmcmsg_len  - Pointer to the current len of the message
 *              nesting     - Nesting level.
 * Returns:     Pointer to the extension header or IP_NULL if it was not found.
 *
 */
IP_STATIC void
ipnet_find_sa_headers(Ipnet_pkt_ip6         *ip6hdr,
                      struct Ip_msghdr      *msg,
                      struct Ip_cmsghdr    **pcmsg,
                      Ip_socklen_t          *pcmsg_len,
                      Ip_u16                 nesting)
{
    struct Ip_in6_sainfo   *saptr;
    struct Ip_cmsghdr      *cmsg = *pcmsg;
    Ip_socklen_t            cmsg_len = *pcmsg_len;
    Ip_pkt_ip6_ext_hdr     *ext;
    int                     ext_type;
    Ip_bool                 give_up = IP_FALSE;
    Ip_socklen_t            len = 0;

    ext_type = ip6hdr->nxt;
    ext = (Ip_pkt_ip6_ext_hdr *) (ip6hdr + 1);

    while (!give_up)
    {
        switch (ext_type)
        {
        case IP_IPPROTO_HOPOPTS:
        case IP_IPPROTO_ROUTING:
        case IP_IPPROTO_FRAGMENT:
        case IP_IPPROTO_DSTOPTS:
            break;
        case IP_IPPROTO_AH:
            len = sizeof(*saptr);
            cmsg_len += IP_CMSG_SPACE(len);
            if (cmsg_len > msg->msg_controllen)
                goto truncated;

            cmsg->cmsg_len   = IP_CMSG_LEN(len);
            cmsg->cmsg_level = IP_IPPROTO_IPV6;
            cmsg->cmsg_type  = IP_IPV6_SAINFO;

            saptr = (struct Ip_in6_sainfo *) IP_CMSG_DATA(cmsg);
            saptr->ipsa6_protocol = IP_IPPROTO_AH;
            saptr->ipsa6_nesting  = nesting;
            saptr->ipsa6_spi      = ip_ntohl(IP_GET_32ON16(((Ip_u8 *) ext) + 4));
            ipcom_memcpy(&saptr->ipsa6_dst, &ip6hdr->dst, sizeof(struct Ip_in6_addr));

            cmsg = IP_CMSG_NXTHDR(msg, cmsg);
            break;
        case IP_IPPROTO_ESP:
            len = sizeof(*saptr);
            cmsg_len += IP_CMSG_SPACE(len);
            if (cmsg_len > msg->msg_controllen)
                goto truncated;

            cmsg->cmsg_len   = IP_CMSG_LEN(len);
            cmsg->cmsg_level = IP_IPPROTO_IPV6;
            cmsg->cmsg_type  = IP_IPV6_SAINFO;

            saptr = (struct Ip_in6_sainfo *) IP_CMSG_DATA(cmsg);
            saptr->ipsa6_protocol = IP_IPPROTO_ESP;
            saptr->ipsa6_nesting  = nesting;
            saptr->ipsa6_spi      = ip_ntohl(IP_GET_32ON16(((Ip_u8 *) ext)));
            ipcom_memcpy(&saptr->ipsa6_dst, &ip6hdr->dst, sizeof(struct Ip_in6_addr));

            cmsg = IP_CMSG_NXTHDR(msg, cmsg);
            break;
        default:
            give_up = IP_TRUE;
            break;
        }

        ext_type = ext->nxt;
        ext = (Ip_pkt_ip6_ext_hdr *) ((Ip_u8 *) ext + (ext->len + 1) * 8);
    }

truncated:
    *pcmsg = cmsg;
    *pcmsg_len = cmsg_len;
}


/*
 *===========================================================================
 *                    ipnet_find_extension_header
 *===========================================================================
 * Description: Tries to find the specifed extension header.
 * Parameters:  ip6hdr - The IPv6 header for the packet.
 *              type - The extension header type to find.
 * Returns:     Pointer to the extension header or IP_NULL if it was not found.
 *
 */
IP_STATIC Ip_pkt_ip6_ext_hdr *
ipnet_find_extension_header(Ipnet_pkt_ip6 *ip6hdr, int type)
{
    Ip_pkt_ip6_ext_hdr *ext;
    int                 ext_type;
    Ip_bool             give_up = IP_FALSE;

    ext_type = ip6hdr->nxt;
    ext = (Ip_pkt_ip6_ext_hdr *) (ip6hdr + 1);

    while (!give_up)
    {
        if (type == ext_type)
            return ext;
        switch (ext_type)
        {
        case IP_IPPROTO_HOPOPTS:
        case IP_IPPROTO_ROUTING:
        case IP_IPPROTO_FRAGMENT:
        case IP_IPPROTO_DSTOPTS:
        case IP_IPPROTO_AH:
        case IP_IPPROTO_ESP:
            break;
        default:
            give_up = IP_TRUE;
            break;
        }

        ext_type = ext->nxt;
        ext = (Ip_pkt_ip6_ext_hdr *) ((Ip_u8 *) ext + (ext->len + 1) * 8);
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_add_ancillary_ipv6_data
 *===========================================================================
 * Description: Adds ancillary data to the message.
 * Parameters:  msg - The message that will receive the ancillary data.
 *              sock - The socket used.
 *              pkt - The packet from which the data should be extracted.
 * Returns:
 *
 */
IP_STATIC void
ipnet_add_ancillary_ipv6_data(struct Ip_msghdr *msg,
                              struct Ip_cmsghdr **pcmsg,
                              Ip_socklen_t *pcmsg_len,
                              Ipnet_socket *sock,
                              Ipcom_pkt *pkt)
{
#ifdef IPCOM_USE_INET
    Ipnet_pkt_ip6       ip6hdrbuf;
#endif
    struct Ip_cmsghdr  *cmsg = *pcmsg;
    Ip_socklen_t        cmsg_len = *pcmsg_len;
    Ip_socklen_t        len = 0;
    Ipnet_pkt_ip6      *ip6hdr = (Ipnet_pkt_ip6 *) &pkt->data[pkt->ipstart];
    Ip_pkt_ip6_ext_hdr *ext;
    int                 i;
    struct Ipnet_flag_proto_stype_tuple {
        int flag;           /* The socket flag that must be set to return the header */
        int proto;          /* The proto number of the header */
        int sockopt_type;   /* The socket options type that the ancillary data will have */
    } fpst[] = {
        { IPNET_SOCKET_FLAG_IP6_RECVHOPOPTS,IP_IPPROTO_HOPOPTS, IP_IPV6_HOPOPTS },
        { IPNET_SOCKET_FLAG_IP6_RECVDSTOPTS,IP_IPPROTO_DSTOPTS, IP_IPV6_DSTOPTS },
        { IPNET_SOCKET_FLAG_IP6_RECVRTHDR,  IP_IPPROTO_ROUTING, IP_IPV6_RTHDR },
        { 0,                            0,                  0 }
    };

#ifdef IPCOM_USE_INET
    if (IPNET_IP6_GET_VER(ip6hdr) == 4)
    {
        Ipnet_pkt_ip *iphdr = (Ipnet_pkt_ip *)ip6hdr;

        /* Using IPv6 mapped IPv4 address, let's create a dummy IPv6
           header matching the IPv4 header to support this part of
           RFC 3542
           ...
           13.  IPv6-Specific Options with IPv4-Mapped IPv6 Addresses
           ...
           In general, attempting to specify an IPv6-only option, such as the
           Hop-by-Hop options, Destination options, or Routing header on an IPv6
           socket that is using IPv4-mapped IPv6 addresses, will probably result
           in an error.  Some implementations, however, may provide access to
           the packet information (source/destination address, send/receive
           interface, and hop limit) on an IPv6 socket that is using IPv4-mapped
           IPv6 addresses.
           ...
           This will make all the packet information available.
        */
        ip6hdr = &ip6hdrbuf;
        ipcom_memset(ip6hdr, 0, sizeof(*ip6hdr));
        IPNET_IP6_SET_VER_CLASS_FLOW(ip6hdr, 6, iphdr->tos, 0);
        ip6hdr->hlim = iphdr->ttl;
        ip6hdr->dst.in6.addr16[5] = 0xffff;
        ipcom_memcpy(&ip6hdr->dst.in6.addr32[3], iphdr->dst, 4);
    }
#endif /* IPCOM_USE_INET*/

    if (IP_BIT_ISSET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVSAINFO))
    {
        /* Grab the tunneled packet */
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_TUNNELED))
        {
            int tunnel_ipstart;

            tunnel_ipstart = ipcom_pkt_get_int(pkt,
                                               IPNET_PKT_INFO_TUNNEL_IPSTART,
                                               int);
            /* Only if it's V6 */
            if ((pkt->data[tunnel_ipstart] & 0xf0) == 0x60)
            {
                /* Parse any SA related headers in the outer packet */
                ipnet_find_sa_headers((Ipnet_pkt_ip6 *) &pkt->data[tunnel_ipstart],
                                      msg,
                                      &cmsg,
                                      &cmsg_len,
                                      1);
            }
        }

        /* Parse any SA related headers in the inner packet */
        ipnet_find_sa_headers((Ipnet_pkt_ip6 *) ip6hdr,
                              msg,
                              &cmsg,
                              &cmsg_len,
                              0);
    }

    if (IP_BIT_ISSET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVENCAP))
    {
        union Ip_in_addr_union  *s_addr;
        Ipnet_pkt_udp           *s_udp  = ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_ENCAP_UDP);
        union Ip_sockaddr_union s_un;

        ipcom_memset(&s_un, 0, sizeof(s_un));

#ifdef IPCOM_USE_INET
        if ((s_addr = ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_ENCAP_IP)) != IP_NULL)
        {
            s_un.sin.sin_family = IP_AF_INET;
            IPCOM_SA_LEN_SET(&s_un, sizeof(s_un.sin));

            s_un.sin.sin_addr.s_addr    = s_addr->in.s_addr;
            s_un.sin.sin_port           = s_udp? s_udp->sport : 0;
        }
        else
#endif
        if ((s_addr = ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_ENCAP_IP6)) != IP_NULL)
        {
            s_un.sin6.sin6_family = IP_AF_INET6;
            IPCOM_SA_LEN_SET(&s_un, sizeof(s_un.sin6));

            IPNET_IP6_SET_ADDR(&s_un.sin6.sin6_addr, &s_addr->in6);
            s_un.sin6.sin6_port = s_udp? s_udp->sport : 0;
        }

        if (s_un.sa.sa_family != IP_AF_UNSPEC)
        {
            len = IPCOM_SA_LEN_GET(&s_un.sa);

            cmsg_len += IP_CMSG_SPACE(len);
            if (cmsg_len > msg->msg_controllen)
                goto truncated;

            cmsg->cmsg_len   = IP_CMSG_LEN(len);
            cmsg->cmsg_level = IP_IPPROTO_IPV6;
            cmsg->cmsg_type  = IP_IPV6_X_UDPENCAP;
            ipcom_memcpy(IP_CMSG_DATA(cmsg), &s_un, IPCOM_SA_LEN_GET(&s_un.sa));
            cmsg = IP_CMSG_NXTHDR(msg, cmsg);
        }
    }

    if (IP_BIT_ISSET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVHOPLIMIT))
    {
        int *hoplimit_ptr;

        len = sizeof(int);
        cmsg_len += IP_CMSG_SPACE(len);
        if (cmsg_len > msg->msg_controllen)
            goto truncated;

        cmsg->cmsg_len   = IP_CMSG_LEN(len);
        cmsg->cmsg_level = IP_IPPROTO_IPV6;
        cmsg->cmsg_type  = IP_IPV6_HOPLIMIT;
        hoplimit_ptr = (int *) IP_CMSG_DATA(cmsg);
        *hoplimit_ptr = ip6hdr->hlim;
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO))
    {
        struct Ip_in6_pktinfo *pktinfo;
        Ipnet_netif           *netif;

        len = sizeof(struct Ip_in6_pktinfo);
        cmsg_len += IP_CMSG_SPACE(len);
        if (cmsg_len > msg->msg_controllen)
            goto truncated;

        cmsg->cmsg_len   = IP_CMSG_LEN(len);
        cmsg->cmsg_level = IP_IPPROTO_IPV6;
#ifdef IP_PORT_LKM
        cmsg->cmsg_type = IP_IPV6_RECVPKTINFO;
#else
        cmsg->cmsg_type = IP_IPV6_PKTINFO;
#endif
        pktinfo = (struct Ip_in6_pktinfo *) IP_CMSG_DATA(cmsg);
        IPNET_IP6_SET_ADDR(&pktinfo->ipi6_addr, &ip6hdr->dst);

        netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);
        if (netif != IP_NULL)
            pktinfo->ipi6_ifindex = netif->ipcom.ifindex;
        else
            pktinfo->ipi6_ifindex = 0;
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

    if (IP_BIT_ISSET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_RECVTCLASS))
    {
        int *tclass_ptr;

        len = sizeof(int);
        cmsg_len += IP_CMSG_SPACE(len);
        if (cmsg_len > msg->msg_controllen)
            goto truncated;

        cmsg->cmsg_len   = IP_CMSG_LEN(len);
        cmsg->cmsg_level = IP_IPPROTO_IPV6;
        cmsg->cmsg_type  = IP_IPV6_TCLASS;
        tclass_ptr = (int *) IP_CMSG_DATA(cmsg);
        *tclass_ptr = IPNET_IP6_GET_CLASS(ip6hdr);
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

    for (i = 0; fpst[i].flag != 0; i++)
    {
        if (IP_BIT_ISSET(sock->ip6->flags, fpst[i].flag) && cmsg != IP_NULL)
        {
            ext = ipnet_find_extension_header(ip6hdr, fpst[i].proto);
            if (ext == IP_NULL)
                /* This header is not present */
                continue;

            len = (ext->len + 1) * 8;
            cmsg_len += IP_CMSG_SPACE(len);

            if (cmsg_len > msg->msg_controllen)
                goto truncated;

            cmsg->cmsg_len   = IP_CMSG_LEN(len);
            cmsg->cmsg_level = IP_IPPROTO_IPV6;
            cmsg->cmsg_type  = fpst[i].sockopt_type;
            ipcom_memcpy(IP_CMSG_DATA(cmsg), ext, len);
            cmsg = IP_CMSG_NXTHDR(msg, cmsg);
        }
    }

 truncated:
    *pcmsg = cmsg;
    *pcmsg_len = cmsg_len;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_add_ancillary_ipv4_data
 *===========================================================================
 * Description: Adds ancillary data to the message.
 * Parameters:  msg - The message that will receive the ancillary data.
 *              sock - The socket used.
 *              pkt - The packet from which the data should be extracted.
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC void
ipnet_add_ancillary_ipv4_data(struct Ip_msghdr *msg,
                              struct Ip_cmsghdr **pcmsg,
                              Ip_socklen_t *pcmsg_len,
                              Ipnet_socket *sock,
                              Ipcom_pkt *pkt)
{
    struct Ip_cmsghdr *cmsg = *pcmsg;
    Ip_socklen_t       cmsg_len = *pcmsg_len;
    Ip_socklen_t       len = 0;
    Ipnet_pkt_ip      *ip4hdr = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];

    if (IP_BIT_ISSET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTTL))
    {
        int *ttl_ptr;

        len = sizeof(int);
        cmsg_len += IP_CMSG_SPACE(len);
        if (cmsg_len > msg->msg_controllen)
            goto truncated;

        cmsg->cmsg_len   = IP_CMSG_LEN(len);
        cmsg->cmsg_level = IP_IPPROTO_IP;
        cmsg->cmsg_type  = IP_IP_TTL;
        ttl_ptr = (int *) IP_CMSG_DATA(cmsg);
        *ttl_ptr = ip4hdr->ttl;
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

    if (IP_BIT_ISSET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVTOS))
    {
        int *tos_ptr;

        len = sizeof(int);
        cmsg_len += IP_CMSG_SPACE(len);
        if (cmsg_len > msg->msg_controllen)
            goto truncated;

        cmsg->cmsg_len   = IP_CMSG_LEN(len);
        cmsg->cmsg_level = IP_IPPROTO_IP;
        cmsg->cmsg_type  = IP_IP_TOS;
        tos_ptr = (int *) IP_CMSG_DATA(cmsg);
        *tos_ptr = ip4hdr->tos;
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

    if (IP_BIT_ISSET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_RECVDSTADDR))
    {
        struct Ip_in_addr *to_addr;

        len = sizeof(struct Ip_in_addr);
        cmsg_len += IP_CMSG_SPACE(len);
        if (cmsg_len > msg->msg_controllen)
            goto truncated;

        cmsg->cmsg_len   = IP_CMSG_LEN(len);
        cmsg->cmsg_level = IP_IPPROTO_IP;
        cmsg->cmsg_type  = IP_IP_RECVDSTADDR;
        to_addr = (struct Ip_in_addr *) IP_CMSG_DATA(cmsg);
        to_addr->s_addr = IP_GET_32ON16(ip4hdr->dst);
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_RECVPKTINFO))
    {
        /* Pkt info socket option */
        if (sock->ipcom.domain == IP_AF_INET)
        {
            struct Ip_in_pktinfo *pktinfo;
            Ipnet_netif          *netif;

            len = sizeof(struct Ip_in_pktinfo);
            cmsg_len += IP_CMSG_SPACE(len);
            if (cmsg_len > msg->msg_controllen)
                goto truncated;

            cmsg->cmsg_len   = IP_CMSG_LEN(len);
            cmsg->cmsg_level = IP_IPPROTO_IP;
            cmsg->cmsg_type  = IP_IP_PKTINFO;
            pktinfo = (struct Ip_in_pktinfo *) IP_CMSG_DATA(cmsg);
            ipcom_memset(pktinfo, 0, sizeof(struct Ip_in_pktinfo));

            pktinfo->ipi_addr.s_addr = IP_GET_32ON16(ip4hdr->dst);

            netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);
            if (netif != IP_NULL)
            {
#ifdef IP_PORT_LKM
                Ipnet_ip4_addr_entry *ifa = netif->inet4_addr_list;

                pktinfo->ipi_spec_dest.s_addr = ifa ? ifa->ipaddr_n : 0;
#endif /* IP_PORT_LKM */
                pktinfo->ipi_ifindex = netif->ipcom.ifindex;
            }
        }
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

 truncated:
    *pcmsg = cmsg;
    *pcmsg_len = cmsg_len;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_bind_check_reuse
 *===========================================================================
 * Description: Checks if the source port is reused and if it is, whenever
 *              the reuse is ok or not.
 * Parameters:  sock - The socket for which to check port reuse.
 *              is_unicast - IP_TRUE if the address we are trying to bind
 *                           to is an unicast address, IP_FALSE otherwise.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_bind_check_reuse(Ipnet_socket *sock, Ip_bool is_unicast)
{
    /*
     * Check if there is a socket bound to same local port&addr ->
     * EADDRINUSE (Consider socket option SO_REUSEADDR and
     * SO_REUSEPORT).
     */
    Ipnet_socket_lookup_key  key;
    Ipnet_bound_socket      *bs;
    int                      reuse;
    void                    *saddr = IP_NULL;
    IP_CONST void           *any_addr = IP_NULL;
    Ipnet_socket            *matched_sock;
    Ip_u8                    has_reuse_port;
    Ip_u8                    has_reuse_addr;

    ipnet_key_from_sock(sock, &key);
    bs = ipcom_hash_get(ipnet->bound_sockets, &key);

    if (bs == IP_NULL)
        return 0;

    reuse = 0;
    has_reuse_port = bs->has_reuse_port;
    has_reuse_addr = bs->has_reuse_addr;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_REUSEPORT) && has_reuse_port)
        reuse |= IPNET_SOCKET_FLAG_REUSEPORT;
    else
        bs->has_reuse_port = IP_FALSE;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_REUSEADDR) && has_reuse_addr)
        reuse |= IPNET_SOCKET_FLAG_REUSEADDR;
    else
        bs->has_reuse_addr = IP_FALSE;

    if (reuse)
    {
#if defined(IPTCP) || defined(IPSCTP)
#ifdef IPTCP
        if (IP_NULL != sock->tcb)
            /* Treat reuse port as reuse addr for TCP */
            reuse = IPNET_SOCKET_FLAG_REUSEADDR;
        else
#endif
#ifdef IPSCTP
        if (IP_NULL != sock->sctp_inst)
            /* Treat reuse port as reuse addr for SCTP */
            reuse = IPNET_SOCKET_FLAG_REUSEADDR;
        else
#endif
#endif /* IPTCP || IPSCTP */
            if (!is_unicast)
                /*
                 * UNIX Network Programming, 3 ed, chapter 7.5
                 *
                 * SO_REUSEADDR is considered equivalent to
                 * SO_REUSEPORT if the IP addres being bond is a
                 * multicast address.
                 *
                 * Most implementations seem to have this behavior for
                 * all non-unicast addresses.
                 */
            reuse = IPNET_SOCKET_FLAG_REUSEPORT;
    }

    if (IP_BIT_ISSET(reuse, IPNET_SOCKET_FLAG_REUSEPORT))
        return 0;
    /* else: reuse addr */

#ifdef IPCOM_USE_INET
    if (sock->ipcom.domain == IP_AF_INET)
    {
        saddr = &sock->ip4->saddr_n;
        if (sock->ip4->saddr_n == ip_inaddr_any.s_addr)
            any_addr = saddr;
        else
            any_addr = &ip_inaddr_any;
    }
#endif
#ifdef IPCOM_USE_INET6
    if (sock->ipcom.domain == IP_AF_INET6)
    {
        saddr = &sock->ip6->saddr;
        if (IP_IN6_IS_ADDR_UNSPECIFIED(saddr))
            any_addr = saddr;
        else
            any_addr = &ip_in6addr_any;
    }
#endif

    if (!reuse)
    {
        if (any_addr == saddr)
            /* This socket tried to bind to the ANY address when other
               sockets is already bound to this port, will only work if
               the reuse flag is set. */
            goto eaddrinuse;

        matched_sock = sock->ops->lookup(sock->vr_index, sock->proto,
                                         any_addr, sock->sport,
                                         IP_NULL, 0);
        if (matched_sock)
            /* Socket bound to the ANY address is treated as if it has
               been bound to all local addresses when it comes to reuse */
            goto eaddrinuse;
    }

    matched_sock = sock->ops->lookup(sock->vr_index, sock->proto,
                                     saddr, sock->sport,
                                     IP_NULL, 0);
    if (matched_sock == IP_NULL)
        /* No other socket bound to the same [domain, vr, protocol,
           local addr, local port] */
        return 0;

#ifdef IPCOM_USE_INET
    if (sock->ipcom.domain == IP_AF_INET
#ifdef IPSCTP /* SCTP can have more than one local address */
        && (IP_IPPROTO_SCTP != sock->proto)
#endif
        && (matched_sock->ipcom.domain != IP_AF_INET
            || sock->ip4->saddr_n != matched_sock->ip4->saddr_n))
        /* This socket is not bound to the ANY address and its local
           address does not match any other bound socket */
        return 0;
#endif
#ifdef IPCOM_USE_INET6
    if (sock->ipcom.domain == IP_AF_INET6
#ifdef IPSCTP /* SCTP can have more than one local address */
        && (IP_IPPROTO_SCTP != sock->proto)
#endif
        && IP_IN6_ARE_ADDR_EQUAL(&sock->ip6->saddr, &matched_sock->ip6->saddr) == IP_FALSE)
        /* This socket is not bound to the ANY address and its local
           address does not match any other bound socket */
        return 0;
#endif

#if defined(IPTCP) || defined(IPSCTP)
    if (reuse && IP_BIT_ISSET(matched_sock->flags, IPNET_SOCKET_FLAG_CONNECTED))
    {
#ifdef IPTCP
        if (IP_NULL != sock->tcb)
            return 0;
#endif
#ifdef IPSCTP
        if (IP_NULL != sock->sctp_inst)
            return 0;
#endif
    }
#endif

 eaddrinuse:
    /* bind not ok. */
    bs->has_reuse_addr = has_reuse_addr;
    bs->has_reuse_port = has_reuse_port;
    return IPNET_ERRNO(EADDRINUSE);
}


/*
 *===========================================================================
 *                    ipnet_sock_get_port_from_sockaddr
 *===========================================================================
 * Description: Returns the port stored in the socket address structure.
 * Parameters:  sa - A socket address.
 * Returns:     The port in host byte order.
 *
 */
IP_STATIC Ip_u16
ipnet_sock_get_port_from_sockaddr(IP_CONST struct Ip_sockaddr *sa)
{
    switch (sa->sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ip_ntohs(((struct Ip_sockaddr_in *)sa)->sin_port);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ip_ntohs(((struct Ip_sockaddr_in6 *)sa)->sin6_port);
#endif
    default:
        IP_PANIC();
        return 0;
    }
}


/*
 *===========================================================================
 *                         ipnet_sock_is_unicast
 *===========================================================================
 * Description: Returns if an address is an IPv4/IPv6 unicast address.
 * Parameters:  vr - virtual router index
 *              sa - the address to test.
 * Returns:     IP_TRUE if the address is unicast, IP_FALSE otherwise.
 *
 */
IP_STATIC Ip_bool
ipnet_sock_is_unicast(Ip_u16 vr, IP_CONST struct Ip_sockaddr *sa)
{
#ifdef IPCOM_USE_INET
    if (sa->sa_family == IP_AF_INET)
    {
        struct Ip_sockaddr_in *sin = (struct Ip_sockaddr_in *) sa;
        return (ipnet_ip4_get_addr_type(sin->sin_addr.s_addr, vr, IP_NULL)
                == IPNET_ADDR_TYPE_UNICAST);
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (sa->sa_family == IP_AF_INET6)
    {
        struct Ip_sockaddr_in6 *sin6 = (struct Ip_sockaddr_in6 *) sa;
        return (ipnet_ip6_get_addr_type(&sin6->sin6_addr, vr, IP_NULL)
                == IPNET_ADDR_TYPE_UNICAST);
    }
#endif /* IPCOM_USE_INET6 */
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_sock_opt_obj_hash
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_sock_opt_obj_hash(Ipnet_sock_ops *ops)
{
    unsigned h;
    h = ipcom_hash_update(&ops->domain, sizeof(ops->domain), ops->type);
    return ipcom_hash_update(&ops->proto, sizeof(ops->proto), h);
}


/*
 *===========================================================================
 *                    ipnet_sock_opt_key_hash
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_sock_opt_key_hash(struct Ipnet_sock_ctor_key *key)
{
    unsigned h;
    h = ipcom_hash_update(&key->domain, sizeof(key->domain), key->type);
    return ipcom_hash_update(&key->proto, sizeof(key->proto), h);
}


/*
 *===========================================================================
 *                    ipnet_sock_opt_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_sock_opt_cmp(Ipnet_sock_ops *ops, struct Ipnet_sock_ctor_key *key)
{
    return ops->domain == key->domain
        && ops->type == key->type
        && ops->proto == key->proto;
}


/*
 *===========================================================================
 *                    ipnet_sock_create_failed_cb
 *===========================================================================
 * Description: Callback to determine which error code to return when
 *              ipcom_socket() failes.
 * Parameters:  ops - A socket operation structure.
 *              key - The key used when looking for a matching ops.
 * Returns:
 *
 */
IP_STATIC void
ipnet_sock_create_failed_cb(Ipnet_sock_ops *ops, struct Ipnet_sock_ctor_key *key)
{
    if (ops->domain == key->domain)
    {
        key->no_domain = IP_FALSE;
        if (ops->type == key->type)
            key->no_type = IP_FALSE;
    }
}


/*
 *===========================================================================
 *                      ipnet_sock_pkt_dtor
 *===========================================================================
 * Description: Deallocates a packet from the socket send buffer.
 * Parameters:  pkt - packet to deallocate.
 * Returns:
 *
 */
IP_STATIC void
ipnet_sock_pkt_dtor(Ipcom_pkt *pkt)
{
    Ipnet_socket                *sock;
    Ipnet_pkt_info_sock_snd_buf *snd_buf_info;
    Ip_bool                     empty  = IP_FALSE;

    snd_buf_info = ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_SOCK_SND_BUF);
    ip_assert(snd_buf_info != IP_NULL);
    sock = snd_buf_info->sock;

    ip_assert(ipnet->sockets[sock->ipcom.fd] == sock);

    if (ipcom_atomic_get(&sock->snd_bytes) == snd_buf_info->payload_len)
    {
        empty = IP_TRUE;
        IPNET_DATA_LOCK(ipnet->sockets_lock);
    }

    /* Due to the design, this if case cannot be true unless 'empty' is TRUE.and
     * the lock held
     */
    if (ipcom_atomic_sub_and_return(&sock->snd_bytes, snd_buf_info->payload_len) == 0
        && IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CLOSED)
        && ipcom_atomic_get(&sock->ref_count) == 0)
    {
        Ip_bool need_close;
        /* Need to check this with sockets_lock held, another task
           might be running ipnet_do_close() on this socket
           co-currently as with this context.
           ipnet->sockets[sock->ipcom.fd] will only be equal to sock
           if ipnet_do_close() took the path that did not close the
           socket. */
        need_close = (ipnet->sockets[sock->ipcom.fd] == sock);

        IPNET_DATA_UNLOCK(ipnet->sockets_lock);

        if (need_close)
        {
            /* Socket has been closed but cleanup was postponed until
               all outstanding packets on this socket has been
               freed. This was the last outstanding packet, so it is
               now safe to free this socket */
            if (ipnet_is_stack_task())
                ipnet_sock_free(sock, IP_TRUE);
            else
            {
                Ipnet_sig *sig = (Ipnet_sig *) sock->async_free;

                /* Cannot free the socket in this context since
                   ipnet_sock_free() might modified shared IPNET data
                   that is not protected by a mutual exclusion
                   lock. */
                sig->sig_type = IPNET_SIG_SOCKETFREE_ASYNC;
                sig->d.close  = sock->ipcom.fd;
                ipcom_pipe_send(ipnet->task.pipe, sig);
            }
        }
        /* else: ipnet_do_close() moved the sock->ref_count to zero
           and read the sock->snd_bytes variable between the point
           where this context decremented sock->snd_bytes and read
           sock->ref_count. ipnet_do_close() closed the socket in this
           case. */
        return;
    }

    if (empty)
        IPNET_DATA_UNLOCK(ipnet->sockets_lock);

    ip_assert(ipcom_atomic_get(&sock->snd_bytes) >= 0);

    if (ipcom_atomic_get(&sock->waiting_writers))
    {
        /* At least one waiting writer waiting on this socket */
        if (ipnet_is_stack_task())
            ipnet_sock_data_avail(sock, 0, IP_SHUT_WR);
        else
        {
            Ipnet_sig *sig;

            /* The caller is not the IPNET daemon, must schedule an
               asynchronous poll notification since
               ipnet_sock_data_avail() can only be called by the
               IPNET daemon */
            sig = ipcom_slab_alloc(ipnet->slabs.poll);
            if (sig != IP_NULL)
            {
                sig->sig_type    = IPNET_SIG_POLL_ASYNC;
                sig->d.poll.fd   = sock->ipcom.fd;
                sig->d.poll.what = IPNET_SOCK_POLL_WRITE;
                ipcom_atomic_set(&sig->garbage_tag, 0);
                if (!ipcom_pipe_try_send(ipnet->task.pipe, sig))
                    ipcom_slab_free(ipnet->slabs.poll, sig);
            }
        }
    }
}


/*
 *===========================================================================
 *                      ipnet_poll_timeout
 *===========================================================================
 * Description: Timeout handler for a poll request.
 * Parameters:  sig - a poll signal that has timed out
 * Returns:
 *
 */
IP_STATIC void
ipnet_poll_timeout(Ipnet_sig *sig)
{
    struct Ipnet_sig_poll *poll = &sig->d.poll;

    if (ipcom_atomic_add_and_return(poll->responses_sent, 1) == 1)
    {
        sig->sig_type = IPNET_SIG_POLL_TIMEOUT;
        ipcom_pipe_send(sig->response_pipe, sig);
    }
}


/*
 *===========================================================================
 *                    ipnet_signal_ready_sock
 *===========================================================================
 * Description: Goes through all pending user tasks waiting on the
 *                    socket and signals all task where the socket is
 *                    now ready.
 * Parameters:  sock - the socket that the pending tasks are waiting
 *                    to be ready.
 *              what - what to check for
 *              is_sock_ready - function that knows how to check if
 *                    the socket is ready.
 * Returns:
 *
  */
IP_STATIC void
ipnet_signal_ready_sock(Ipnet_socket *sock,
                        enum Ipnet_sock_poll what,
                        Ipnet_is_sock_ready_cb is_sock_ready)
{
    Ipcom_list *list_node;
    Ipnet_sig  *poll_signal;

    for (list_node = ipcom_list_first(&sock->poll_list_head);
         list_node != IP_NULL;
         list_node = ipcom_list_next(list_node))
    {
        poll_signal = ipcom_list_entry(list_node, Ipnet_sig, d.poll.list);

        if (what == poll_signal->d.poll.what
            && is_sock_ready(sock, poll_signal->d.poll.lowat))
        {
            if (ipcom_atomic_add_and_return(poll_signal->d.poll.responses_sent, 1) == 1)
                ipcom_pipe_send(poll_signal->response_pipe, poll_signal);
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_sock_has_waiting
 *===========================================================================
 * Description: Checks if the specified socket has any user task
 *                    waiting for the socket to become ready in some way.
 * Parameters:  sock - socket to check
 *              what - that to check for
 * Returns:     IP_TRUE  = socket has waiting readers.
 *              IP_FALSE = socket does not have waiting readers.
 *
 */
IP_STATIC Ip_bool
ipnet_sock_has_waiting(Ipnet_socket *sock, enum Ipnet_sock_poll what)
{
    Ipcom_list *list_node;
    Ipnet_sig  *poll_signal;

    ip_assert(ipnet_is_stack_task());

    if (sock->poll_list_head.size == 0)
        return IP_FALSE;

    list_node = ipcom_list_first(&sock->poll_list_head);
    while (list_node != IP_NULL)
    {
        poll_signal = ipcom_list_entry(list_node, Ipnet_sig, d.poll.list);

        if (what == poll_signal->d.poll.what)
            return IP_TRUE;

        list_node = ipcom_list_next(list_node);
    }
    return IP_FALSE;
}


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   ipnet_sock_get_bound_socket_size
 *===========================================================================
 * Description: Returns the size of the Ipnet_bound_socket structure
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u32
ipnet_sock_get_bound_socket_size()
{
    return sizeof(Ipnet_bound_socket);
}


/*
 *===========================================================================
 *                    ipnet_sock_has_waiting_writers
 *===========================================================================
 * Description: Checks if the specified socket has any user task
 *                    waiting to write on this socket.
 * Parameters:  sock - a socket.
 * Returns:     IP_TRUE  = socket has waiting writers.
 *              IP_FALSE = socket does not have waiting writers.
 *
 */
IP_GLOBAL Ip_bool
ipnet_sock_has_waiting_writers(Ipnet_socket *sock)
{
    return ipnet_sock_has_waiting(sock, IPNET_SOCK_POLL_WRITE);
}


/*
 *===========================================================================
 *                     ipnet_is_sock_readable
 *===========================================================================
 * Description: Checks if the specified socket is readable.
 * Parameters:  sock - the socket to check.
 *              lowat - minimum number of bytes that must be present
 *                     in the receive queue to cont the socket as
 *                     readable. 0 means to use the socket lo
 *                     watermask value.
 * Returns:     IP_TRUE  = socket is readable
 *              IP_FALSE = socket is not readable
 *
 */
IP_GLOBAL Ip_bool
ipnet_is_sock_readable(Ipnet_socket *sock, int lowat)
{
    static Ip_socklen_t lowat_len = sizeof(lowat);

    if (sock->ipcom.so_errno)
        /* Sockets with pending error is always readable */
        return IP_TRUE;

    if (lowat == 0
        && (sock->ops->getopt == IP_NULL
            || sock->ops->getopt(sock, IP_SOL_SOCKET, IP_SO_RCVLOWAT, &lowat, &lowat_len) != 0))
        lowat = 1;

#ifdef IPTCP
    if (sock->tcb != IP_NULL
        && sock->ipcom.rcv_bytes == lowat
        && IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_OOBINLINE)
        && ipnet_is_sock_exceptional(sock, 0))
        /* There is only 'lowat' bytes available and one of them are
           the out-of-bound so this socket is not readable yet  */
        return IP_FALSE;
#endif

    return (sock->ipcom.rcv_bytes >= lowat
#ifdef IPSCTP
            && (sock->sctp_inst == IP_NULL
               || (ipcom_atomic_get(&sock->sctp_inst->wait_readable) == 0))
#endif
           )
#ifdef IPTCP
        || (sock->tcb != IP_NULL
            && ((sock->tcb->backlog_done != IP_NULL
                 && sock->tcb->backlog_done[0] != IP_SOCKERR)
                || IP_BIT_ISSET(sock->tcb->flags, IPTCP_TCPC_FLAG_CONNABORTED)))
#endif
#ifdef IPSCTP
        || (sock->sctp_inst != IP_NULL
            && ((sock->sctp_inst->backlog_done != IP_NULL
                 && sock->sctp_inst->backlog_done[0] != IP_SOCKERR)
                || IP_BIT_ISSET(sock->sctp_inst->inst_flags, IPSCTP_SOCKET_CONNABORTED)))
#endif
        || IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_READ_CLOSED)
        || IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_PEER_WRITE_CLOSED);
}


/*
 *===========================================================================
 *                    ipnet_is_sock_writable
 *===========================================================================
 * Description: Checks if the specified socket is writable.
 * Parameters:  sock - the socket to check.
 *              lowat - minimum number of bytes that must be available
 *                     in the transmit queue to cont the socket as
 *                     writable. 0 means to use the socket lo
 *                     watermark value.
 * Returns:     IP_TRUE  = socket is writable
 *              IP_FALSE = socket is not writable
 *
 */
IP_GLOBAL Ip_bool
ipnet_is_sock_writable(Ipnet_socket *sock, int lowat)
{
    static Ip_socklen_t lowat_len = sizeof(lowat);

    if (lowat == 0
        && (sock->ops->getopt == IP_NULL
            || sock->ops->getopt(sock, IP_SOL_SOCKET, IP_SO_SNDLOWAT, &lowat, &lowat_len) != 0))
        lowat = 1;

    if (sock->ipcom.so_errno
        || IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_WRITE_CLOSED))
        return IP_TRUE;

    if ((IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED)
         || ((sock->ipcom.type != IP_SOCK_STREAM)
#ifdef IPSCTP
             && (IP_SOCK_SEQPACKET != sock->ipcom.type) /* SCTP is also based on connection */
#endif
             ))
        && sock->send_max_bytes - ipcom_atomic_get(&sock->snd_bytes) >= lowat)
        return IP_TRUE;

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_is_sock_exceptional
 *===========================================================================
 * Description: Checks if the specified socket is exceptional.
 * Parameters:  sock - the socket to check.
 *              lowat - unused. Added so this function has the same
 *                    signature as the other ipnet_is_sock_xxx().
 * Returns:     IP_TRUE  = socket is exceptional
 *              IP_FALSE = socket is not exceptional.
 *
 */
IP_GLOBAL Ip_bool
ipnet_is_sock_exceptional(Ipnet_socket *sock, int lowat)
{
    int in_urgent_mode;

    IPCOM_UNUSED_ARG(lowat);

    if (sock->ipcom.so_errno
        || (sock->ops->ioctl != IP_NULL
            && sock->ops->ioctl(sock, IP_X_SIOCINURG, &in_urgent_mode) == 0
            && in_urgent_mode))
        return IP_TRUE;
    return IP_FALSE;
}


/*
 *===========================================================================
 *                      ipnet_sys_poll
 *===========================================================================
 * Description: Register a watch on a socket that looks for the socket
 *                      to become readable, writable or exceptional.
 * Parameters:  sig - an IPNET_SIG_POLL signal.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_poll(Ipnet_sig *sig)
{
    struct Ipnet_sig_poll *poll = &sig->d.poll;
    Ipnet_socket          *sock = ipnet->sockets[poll->fd];
    Ipnet_is_sock_ready_cb is_sock_ready;

    ip_assert(sock != IP_NULL);

    ipcom_list_insert_last(&sock->poll_list_head, &poll->list);

    switch (poll->what)
    {
    case IPNET_SOCK_POLL_READ:
        is_sock_ready = ipnet_is_sock_readable;
        break;
    case IPNET_SOCK_POLL_WRITE:
        is_sock_ready = ipnet_is_sock_writable;
        ipcom_atomic_inc(&sock->waiting_writers);
        break;
    default:
        ip_assert(poll->what == IPNET_SOCK_POLL_EXCEPT);
        is_sock_ready = ipnet_is_sock_exceptional;
        break;
    }

    /* Check if the socket has become ready between now and when the
       signal was sent */
    if (is_sock_ready(sock, poll->lowat))
    {
        /* It is ready now, wakeup the polling task */
        ipnet_signal_ready_sock(sock, poll->what, is_sock_ready);
        return;
    }

    if (poll->timeout != IP_NULL
        && *poll->ptmo == IP_NULL
        && ipcom_atomic_get(poll->responses_sent) == 0)
        (void)ipnet_timeout_schedule(IPNET_TIMEVAL_TO_MSEC(*poll->timeout),
                                     (Ipnet_timeout_handler) ipnet_poll_timeout,
                                     sig,
                                     poll->ptmo);
}


/*
 *===========================================================================
 *                      ipnet_sys_poll_async
 *===========================================================================
 * Description: Checks if a socket is ready and notifies any pending
 *                      processes if it is.
 * Parameters:  sig - an IPNET_SIG_POLL_ASYNC signal.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_poll_async(Ipnet_sig *sig)
{
    int how;

    switch (sig->d.poll.what)
    {
    case IPNET_SOCK_POLL_READ:
        how = IP_SHUT_RD;
        break;
    case IPNET_SOCK_POLL_WRITE:
        how = IP_SHUT_WR;
        break;
    default:
#ifdef IPTCP
        how = IP_SHUT_EX;
#else
        how = 0;
#endif /* IPTCP */
        break;
    }

    /* Only poll if there's a socket available with the corresponding FD */
    if (ipnet->sockets[sig->d.poll.fd] != IP_NULL)
        ipnet_sock_data_avail(ipnet->sockets[sig->d.poll.fd],
                              0,
                              how);

    ipcom_slab_free(ipnet->slabs.poll, sig);
}


/*
 *===========================================================================
 *                      ipnet_sys_poll_done
 *===========================================================================
 * Description: Unregister a watch on a socket.
 * Parameters:  sig - a IPNET_SIG_POLL_DONE signal.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sys_poll_done(Ipnet_sig *sig)
{
    struct Ipnet_sig_poll_done *poll_done = &sig->d.poll_done;
    Ipcom_list                 *list_node;
    Ipnet_sig                  *poll_sig;

    for (list_node = ipcom_list_first(&poll_done->session_list_head);
         list_node != IP_NULL;
         list_node = ipcom_list_first(&poll_done->session_list_head))
    {
        ipcom_list_remove(list_node);

        poll_sig = ipcom_list_entry(list_node, Ipnet_sig, d.poll.session_list);

        if (poll_sig->d.poll.list.head != IP_NULL)
            ipcom_list_remove(&poll_sig->d.poll.list);
        /* else: the poll was interrupted before the poll signal was
           processed by the stack daemon. */

        if (poll_sig->d.poll.what == IPNET_SOCK_POLL_WRITE)
        {
            Ipnet_socket *sock = ipnet->sockets[poll_sig->d.poll.fd];

            /* One less waiting write on the socket */
            if (sock != IP_NULL)
                ipcom_atomic_dec(&sock->waiting_writers);
            /* else: socket has been closed */
        }

        ipcom_slab_free(ipnet->slabs.poll, poll_sig);
    }

    ipnet_timeout_cancel(sig->d.poll_done.tmo);
    ipcom_slab_free(ipnet->slabs.poll, sig);
}


/*
 *===========================================================================
 *                    ipnet_sock_pkt_drain
 *===========================================================================
 * Description: Tries to locate packets that can be freed since the stack
 *              is a low packet situation.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_pkt_drain(void)
{
    int           i;
    Ipnet_socket *sock;

    for (i = 0; i < ipnet_conf_max_sockets; i++)
    {

        sock = ipnet->sockets[i];
        if (sock == IP_NULL
            || IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_DRAINABLE))
            continue;

        while (sock->rcv_head != sock->rcv_tail)
        {
            struct Ip_msghdr msg;

            /* Socket allows packet drain and has more than one packet queued */
            ipcom_memset(&msg, 0, sizeof(msg));
            (void)ipnet_usr_sock_recvmsg(sock, &msg, 0);
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_sock_register_ops
 *===========================================================================
 * Description: Registers operations for a new domain/type/proto tuple.
 * Parameters:  ops - The socket operations to register.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_register_ops(Ipnet_sock_ops *ops)
{
    if (ipnet->socket_ops == IP_NULL)
    {
        ipnet->socket_ops = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_sock_opt_obj_hash,
                                           (Ipcom_hash_key_func) ipnet_sock_opt_key_hash,
                                           (Ipcom_hash_cmp_func) ipnet_sock_opt_cmp);
        if (ipnet->socket_ops == IP_NULL)
        {
            IPCOM_LOG0(CRIT, "Failed to create socket operation hash table (out of memory?)");
            IP_PANIC();
            return IPNET_ERRNO(ENOMEM);
        }
    }

    if (ops->domain < 0)
    {
        IPCOM_LOG1(ERR, "Tried to register an invalid domain: %d", ops->domain);
        return IPNET_ERRNO(EPFNOSUPPORT);
    }

    if (ops->type < 0)
    {
        IPCOM_LOG1(ERR, "Tried to register an invalid type: %d", ops->domain);
        return IPNET_ERRNO(ESOCKTNOSUPPORT);
    }

    if (ops->proto < -1)
    {
        IPCOM_LOG1(ERR, "Tried to register an invalid protocol: %d", ops->domain);
        return IPNET_ERRNO(EINVAL);
    }

    if (ipnet->largest_socket_size < sizeof(Ipnet_socket) + ops->extra_sock_space)
        ipnet->largest_socket_size = sizeof(Ipnet_socket) + ops->extra_sock_space;

    switch (ipcom_hash_add(ipnet->socket_ops, ops))
    {
    case IPCOM_SUCCESS:
        IPCOM_LOG3(DEBUG, "New socket constructor added, domain:%2d, type:%2d, proto:%2d",
                   ops->domain, ops->type, ops->proto);
        return 0;
    case IPCOM_ERR_DUPLICATE:
        return IPNET_ERRNO(EEXIST);
    case IPCOM_ERR_NO_MEMORY:
        IP_PANIC2();
        return IPNET_ERRNO(ENOMEM);
    default:
        return IPNET_ERRNO(EINVAL);
    }
}


/*
 *===========================================================================
 *                    ipnet_sock_alloc_private_data
 *===========================================================================
 * Description: Returns a pointer to a memory space inside the socket buffer
 *              that can be used by the layer. Available space for this
 *              buffer is allocated during socket creation via the
 *              'extra_sock_space' variable.
 * Parameters:
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_GLOBAL void *
ipnet_sock_alloc_private_data(Ipnet_socket *sock, Ip_size_t size)
{
    void *d;

    if (sock->sock_end == IP_NULL)
        sock->sock_end = sock + 1;

    d = sock->sock_end;
    sock->sock_end = (Ip_u8 *) sock->sock_end + size;
    return d;
}


/*
 *===========================================================================
 *                    ipnet_sock_create_lookup_tables
 *===========================================================================
 * Description: Allocates the hash tableles used for socket lookups.
 * Parameters:
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_GLOBAL int
ipnet_sock_create_lookup_tables(void)
{
    ipnet->bound_sockets = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_sock_bound_obj_hash,
                                          (Ipcom_hash_key_func) ipnet_sock_bound_key_hash,
                                          (Ipcom_hash_cmp_func) ipnet_sock_bound_hash_cmp);
    ipnet->reachable_sockets = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_sock_obj_hash,
                                              (Ipcom_hash_key_func) ipnet_sock_key_hash,
                                              (Ipcom_hash_cmp_func) ipnet_sock_hash_cmp);

    if (ipnet->bound_sockets && ipnet->reachable_sockets)
        return 0;
    IPCOM_LOG0(CRIT, "Failed to allocate memory for the socket lookup hash tables");
    return IPNET_ERRNO(ENOMEM);
}


/*
 *===========================================================================
 *                    ipnet_sock_rx_cache_invalidate
 *===========================================================================
 * Description: Invalidate sockets rx cache
 * Parameters:  domain - socket domain to invalidate
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_rx_cache_invalidate(int domain)
{
#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET && ipnet->ip4.rx_cache.sock != IP_NULL)
    {
        IP_BIT_CLR(ipnet->ip4.rx_cache.sock->flags, IPNET_SOCKET_FLAG_RX_CACHED);
        ipnet->ip4.rx_cache.sock = IP_NULL;
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6 && ipnet->ip6.rx_cache.sock != IP_NULL)
    {
        IP_BIT_CLR(ipnet->ip6.rx_cache.sock->flags, IPNET_SOCKET_FLAG_RX_CACHED);
        ipnet->ip6.rx_cache.sock = IP_NULL;
    }
#endif /* IPCOM_USE_INET6 */
}


/*
 *===========================================================================
 *                    ipnet_sock_set_reachable
 *===========================================================================
 * Description: Inserts the socket into the reachable sockets table.
 * Parameters:  sock - The socket to make reachable.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_set_reachable(Ipnet_socket *sock)
{
    Ipnet_socket           *matching_sock;
    Ipnet_socket_lookup_key key;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_REACHABLE))
        return 0;

#ifdef IPSCTP
    /* directly return if SCTP socket */
    if (IP_IPPROTO_SCTP == sock->proto)
        goto set_rx_cache;
#endif

    ipnet_key_from_sock(sock, &key);
    matching_sock = ipcom_hash_get(ipnet->reachable_sockets, &key);
    if (matching_sock == IP_NULL)
    {
        if (ipcom_hash_add(ipnet->reachable_sockets, sock) != IPCOM_SUCCESS)
            return IPNET_ERRNO(ENOMEM);
    }
    else
    {
        sock->prev = matching_sock;
        sock->next = matching_sock->next;
        matching_sock->next = sock;
        if (sock->next)
            sock->next->prev = sock;
    }
#ifdef IPSCTP
set_rx_cache: /* add for SCTP protocol */
#endif
    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_REACHABLE);

    /* Must invalidate the rx cache since the new socket might change
       the socket selection */
    ipnet_sock_rx_cache_invalidate((int)sock->ipcom.domain);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_set_unreachable
 *===========================================================================
 * Description: Removes the socket from the reachable socket table, a socklookup
 *              can will no longer find this socket.
 * Parameters:  sock - The socket to make unreachable.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_set_unreachable(Ipnet_socket *sock)
{
    if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_REACHABLE))
        return;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_RX_CACHED))
    {
#ifdef IPCOM_USE_INET
        if (sock->ipcom.domain == IP_AF_INET && sock == ipnet->ip4.rx_cache.sock)
            ipnet->ip4.rx_cache.sock = IP_NULL;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if (sock->ipcom.domain == IP_AF_INET6 && sock == ipnet->ip6.rx_cache.sock)
            ipnet->ip6.rx_cache.sock = IP_NULL;
#endif /* IPCOM_USE_INET6 */
        IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_RX_CACHED);
    }

#ifdef IPSCTP
    /* directly return if SCTP socket */
    if (IP_IPPROTO_SCTP == sock->proto)
        goto clear_out;
#endif

    if (sock->prev == IP_NULL)
#ifdef IPNET_ASSERT
        ip_assert(ipcom_hash_remove(ipnet->reachable_sockets, sock) == IPCOM_SUCCESS);
#else
        (void)ipcom_hash_remove(ipnet->reachable_sockets, sock);
#endif
    else
        sock->prev->next = sock->next;

    if (sock->next)
    {
        sock->next->prev = sock->prev;
        if (sock->prev == IP_NULL)
            /* Insert the new head item into the hash table */
            (void)ipcom_hash_add(ipnet->reachable_sockets, sock->next);
    }

#ifdef IPSCTP
clear_out :
#endif
    sock->prev = IP_NULL;
    sock->next = IP_NULL;
    IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_REACHABLE);
}


/*
 *===========================================================================
 *                    ipnet_sock_add_to_rx_cache
 *===========================================================================
 * Description: Adds a fast receive path to the specified socket.
 * Parameters:  sock - The socket to which the packet should be delivered.
 *              func - The function that knows how to deliver the packet.
 * Returns:
 */
IP_GLOBAL void
ipnet_sock_add_to_rx_cache(Ipnet_socket *sock,
                           IP_CONST void *local_addr,
                           Ip_u16 local_port,
                           IP_CONST void *peer_addr,
                           Ip_u16 peer_port,
                           Ipnet_rx_cache_fast_input_f func)
{
    Ip_u32 tldata = peer_port + (local_port << 16);

    /* The tldata field assumes TCP or UDP */
    ip_assert(sock->proto == IP_IPPROTO_TCP || sock->proto == IP_IPPROTO_UDP);

#ifdef IPCOM_USE_INET
    if (sock->ipcom.domain == IP_AF_INET)
    {
        IP_CONST struct Ip_in_addr *src4 = local_addr;
        IP_CONST struct Ip_in_addr *dst4 = peer_addr;

        if (ipnet->ip4.rx_cache.sock != IP_NULL)
            IP_BIT_CLR(ipnet->ip4.rx_cache.sock->flags, IPNET_SOCKET_FLAG_RX_CACHED);

        ipnet->ip4.rx_cache.dst.s_addr      = IPNET_IP4_GET_IPADDR(dst4);
        ipnet->ip4.rx_cache.src.s_addr      = IPNET_IP4_GET_IPADDR(src4);
        ipnet->ip4.rx_cache.tldata          = ip_htonl(tldata);
        ipnet->ip4.rx_cache.proto           = sock->proto;
        ipnet->ip4.rx_cache.sock            = sock;
        ipnet->ip4.rx_cache.fast_input_func = func;
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_RX_CACHED);
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (sock->ipcom.domain == IP_AF_INET6)
    {
        IP_CONST struct Ip_in6_addr *src6 = local_addr;
        IP_CONST struct Ip_in6_addr *dst6 = peer_addr;

        if (ipnet->ip6.rx_cache.sock != IP_NULL)
            IP_BIT_CLR(ipnet->ip6.rx_cache.sock->flags, IPNET_SOCKET_FLAG_RX_CACHED);

        IPNET_IP6_SET_ADDR(&ipnet->ip6.rx_cache.dst, dst6);
        IPNET_IP6_SET_ADDR(&ipnet->ip6.rx_cache.src, src6);
        ipnet->ip6.rx_cache.tldata          = ip_htonl(tldata);
        ipnet->ip6.rx_cache.proto           = sock->proto;
        ipnet->ip6.rx_cache.sock            = sock;
        ipnet->ip6.rx_cache.fast_input_func = func;
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_RX_CACHED);
    }
#endif /* IPCOM_USE_INET6 */
}


/*
 *===========================================================================
 *                    ipnet_sock_unconnect
 *===========================================================================
 * Description: Unconnects the socket.
 * Parameters:  sock - The socket to unconnect.
 * Returns:     1 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_sock_unconnect(Ipnet_socket *sock)
{
    int ret;

    IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);
    sock->dport = 0;
    ret = ipnet_sock_set_reachable(sock);
    if (ret < 0)
        return ret;
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_sock_unbind
 *===========================================================================
 * Description: Unbind the specific IP_AF_INET or IP_AF_INET6 socket.
 * Parameters:  sock - The socket to unbind.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL void
ipnet_sock_unbind(Ipnet_socket *sock)
{
    Ipnet_socket_lookup_key  key;
    Ipnet_bound_socket      *bs;

    ipnet_sock_set_unreachable(sock);

#ifdef IPCOM_USE_INET
    if (sock->ipcom.domain == IP_AF_INET)
    {
        /* Remove the reference from the IPv4 address this socket was bound to */
        Ipnet_ip4_addr_entry *addr_entry = sock->addr_entry;
        if (addr_entry != IP_NULL && addr_entry->socket_list == sock)
            addr_entry->socket_list = sock->addr_next;

        ipnet_sock_invalidate_ip4_cache(sock);
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (sock->ipcom.domain == IP_AF_INET6)
    {
        /* Remove the reference from the IPv6 address this socket was bound to */
        Ipnet_ip6_addr_entry *addr_entry = sock->addr_entry;
        if (addr_entry != IP_NULL && addr_entry->socket_list == sock)
            addr_entry->socket_list = sock->addr_next;

        ipnet_sock_invalidate_ip6_cache(sock);
    }
#endif /* IPCOM_USE_INET6 */

    /* Remove this socket from the list of sockets bound to the address */
    if (sock->addr_next != IP_NULL)
        sock->addr_next->addr_prev = sock->addr_prev;
    if (sock->addr_prev != IP_NULL)
        sock->addr_prev->addr_next = sock->addr_next;
    sock->addr_entry = IP_NULL;
    sock->addr_next  = IP_NULL;
    sock->addr_prev  = IP_NULL;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_BOUND))
    {
        /* This operation can only must be matched with a successful bind call */
        ipnet_key_from_sock(sock, &key);
        bs = ipcom_hash_get(ipnet->bound_sockets, &key);
        if (bs != IP_NULL && --bs->count == 0)
        {
            if (ipcom_hash_remove(ipnet->bound_sockets, bs) != IPCOM_SUCCESS)
                IP_PANIC();
            ipcom_slab_free(ipnet->slabs.bound_socket, bs);
        }
    }

    sock->sport = 0;
    IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_BOUND);
}


/*
 *===========================================================================
 *                    ipnet_sock_bind
 *===========================================================================
 * Description: Binds a IP_AF_INET or IP_AF_INET6 socket to a specific
 *              socket address and possibly port.
 * Parameters:  sock - A pointer to the socket data.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 *              check_reuse - Set to IP_TRUE if the name should be checked
 *              for address reuse.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_sock_bind(Ipnet_socket *sock,
                IP_CONST struct Ip_sockaddr *name,
                Ip_socklen_t namelen,
                Ip_bool check_reuse)
{
    Ipnet_sock_inet_ops *ops = (Ipnet_sock_inet_ops *) sock->ops;
    int                  err;

    ipnet_sock_set_unreachable(sock);

    /* Call the transport layer bind (UDP and TCP)
       or network layer bind (RAW) */
    err = ops->network_bind(sock, name, namelen, check_reuse);

    if (err >= 0)
    {
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_BOUND);
        err = ipnet_sock_set_reachable(sock);
    }

    if (err < 0)
        ipnet_sock_unbind(sock);

    return err;
}


/*
 *===========================================================================
 *                    ipnet_sock_bind_to_port
 *===========================================================================
 * Description: Binds the socket to a specific port.
 * Parameters:  sock - A pointer to the socket data.
 *              name - The address the socket should be bound to.
 *              check_reuse - IP_TRUE if the port should be checked for reuse.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_sock_bind_to_port(Ipnet_socket *sock,
                        IP_CONST struct Ip_sockaddr *name,
                        Ip_bool check_reuse)
{
    Ipnet_socket_lookup_key key;
    Ipnet_bound_socket     *bs;

    sock->sport = ipnet_sock_get_port_from_sockaddr(name);

    /* If port 0, pick an ephemeral port */
    if (sock->sport == 0)
    {
        sock->sport = ipnet_next_ephemeral_port(sock);
        if (sock->sport == 0)
            return IPNET_ERRNO(EADDRINUSE);
    }

    if (check_reuse)
    {
        int err;

        err = ipnet_bind_check_reuse(sock,
                                     ipnet_sock_is_unicast(sock->vr_index,
                                                           name));
        if (err < 0)
            return err;
    }

    ipnet_key_from_sock(sock, &key);
    bs = ipcom_hash_get(ipnet->bound_sockets, &key);
    if (bs)
        bs->count++;
    else
    {
        bs = ipnet_bound_sock_from_sock(sock);
        if (bs == IP_NULL || ipcom_hash_add(ipnet->bound_sockets, bs) != IPCOM_SUCCESS)
            return IPNET_ERRNO(ENOMEM);
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_bind_addr_removed
 *===========================================================================
 * Description: The address used by the socket(s) in the passed list has been
 *              removed.
 * Parameters:  sock_list - The list of sockets bound to the removed address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_bind_addr_removed(Ipnet_socket *sock_list)
{
    Ipnet_socket *sock = sock_list;
    Ipnet_socket *next_sock;

    while (sock != IP_NULL)
    {
        next_sock = sock->addr_next;
        sock->addr_next  = IP_NULL;
        sock->addr_prev  = IP_NULL;
        sock->addr_entry = IP_NULL;
        if (ipcom_sysvar_get_as_int0("ipnet.sock.AddrNotify", 1) > 0)
        {
#ifdef IPTCP
            if (sock->tcb != IP_NULL)
                (void)iptcp_drop_connection(sock, -IP_ERRNO_ENETDOWN);
            else
#endif /* IPTCP */
                ipnet_sock_data_avail(sock, IP_ERRNO_ENETDOWN, IP_SHUT_RDWR);
        }
        sock = next_sock;
    }
}


/*
 *===========================================================================
 *                    ipnet_sock_change_addr_entry
 *===========================================================================
 * Description: The address the sockets in the old list is bound to has been
 *              removed, but there are an identical address assigned to
 *              another interface which can be used
 * Parameters:  old_sock_list - The list of socket bound to the removed address.
 *              pnew_sock_list - Pointer to the head of the socket list on
 *              the new address entry to use, this list may already contain
 *              sockets.
 *              addr_entry - Pointer to the new address entry.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_change_addr_entry(Ipnet_socket *old_sock_list, Ipnet_socket **pnew_sock_list, void *addr_entry)
{
    Ipnet_socket *sock;
    Ipnet_socket *sock_last = IP_NULL;

    for (sock = old_sock_list; sock != IP_NULL; sock = sock->addr_next)
    {
        /* Reassign all sockets in the old list to the new address entry */
        sock->addr_entry = addr_entry;
        sock_last = sock;
    }

    sock = *pnew_sock_list;

    if (sock == IP_NULL)
        *pnew_sock_list = old_sock_list;
    else
    {
        /* Insert the old_sock_list after the first entry in the new list */
        sock_last->addr_next = sock->addr_next;
        if (sock->addr_next != IP_NULL)
            sock->addr_next->addr_prev = sock_last;
        sock->addr_next = old_sock_list;
        old_sock_list->addr_prev = sock;
    }
}


/*
 *===========================================================================
 *                    ipnet_next_ephemeral_port
 *===========================================================================
 * Description: Returns the next ephemeral port.
 * Parameters:  sock - The socket that will use the port.
 * Returns:     The next free ephemeral port or 0 if no port could be found.
 *              The port is retured in host byte order.
 *
 */
IP_GLOBAL Ip_u16
ipnet_next_ephemeral_port(Ipnet_socket *sock)
{
    Ipnet_socket_lookup_key key;
    Ip_u16                  eport;
    Ip_u16                  initial_port;
    Ip_u16                  portrange;
    Ip_u16                  portbase;
    Ip_bool                 all_ports_tested = IP_FALSE;

    portbase = (Ip_u16) ipcom_sysvar_get_as_int0("ipnet.sock.AnonPortMin", 49152);
    portrange = (Ip_u16) ipcom_sysvar_get_as_int0("ipnet.sock.AnonPortMax", 65535);
    portrange -= portbase;

    /*
     * Select a port at random within the range of ephemeral ports.
     */
    initial_port = portbase + ipcom_rand() % (portrange + 1);
    eport = initial_port;

    ipcom_memset(&key, 0, sizeof(key));
    key.domain = sock->ipcom.domain;
    key.vr     = sock->vr_index;
    key.proto  = sock->proto;

    do
    {
        key.local_port = eport;
        if (ipcom_hash_get(ipnet->bound_sockets, &key) == IP_NULL)
            /*
             * This port is unused
             */
            break;

        if (eport++ >= portbase + portrange)
            eport = portbase;

        all_ports_tested = (eport == initial_port);
    } while (!all_ports_tested);

    return all_ports_tested ? 0 : eport;
}


/*
 *===========================================================================
 *                    ipnet_queue_received_packet
 *===========================================================================
 * Description: Queues a packet on the passed socket. The start field of
 *              the packet should specify beginning of application data.
 * Parameters:  pkt - packet to queue.
 *              sock - socket to queue the packet on.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_queue_received_packet(Ipcom_pkt *pkt, Ipnet_socket *sock)
{
    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_READ_CLOSED))
    {
        IPCOM_LOG1(DEBUG2, "ipnet_queue_received_packet: socket %d closed", sock->ipcom.fd);
        return IPNET_ERRNO(EPERM);
    }

    if (sock->bind_to_ifindex != 0)
    {
        switch (sock->ipcom.domain)
        {
#if defined(IP_SO_USELOOPBACK) && (defined(IPNET_USE_ROUTESOCK) || defined(IPNET_USE_NETLINKSOCK))
        case IP_AF_ROUTE:
        case IP_AF_NETLINK:
            if (sock->bind_to_ifindex == pkt->ifindex)
                /* This socket has set IP_SO_USELOOPBACK */
                return IPNET_ERRNO(EINVAL);
            break;
#endif /* defined(IP_SO_USELOOPBACK) && (defined(IPNET_USE_ROUTESOCK) || defined(IPNET_USE_NETLINKSOCK)) */
        default:
            if (sock->bind_to_ifindex != pkt->ifindex)
            {
                /* This socket is bound to a interface (with IP_SO_BINDTODEVICE)
                 * that does not match this packet */
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                  1, 3, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                  ipnet_queue_received_packet, IPCOM_WV_NETD_BADSOCK,
                                  IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
                IPNET_STATS(pkt_input_bound_to_other_if++);
                return IPNET_ERRNO(EINVAL);
            }
            break;
        }
    }

    ipcom_spinlock_lock(sock->rcv_lock);
    /* Use pkt->maxlen instead of pkt->end-pkt->start for datagram
       packets to avoid DoS attack where huge amount of zero length
       UDP packets is sent. pkt->end-pkt->start==0 for UDP packets
       without payload, so those packets could used up all available
       memory regardless of the size of the socket receive buffer.
       Every socket is allowed to queue one single packet regardless
       of packet size and socket receive buffer size.
       This is done so packet that does not change the socket receive
       buffer size or change it to a very small size does not break.
    */
    if (sock->rcv_head != IP_NULL)
    {
#if defined(IPNET_USE_NETLINKSOCK)
        /* If there's room in the last buffer to store this packet, copy it in */
        if (IP_UNLIKELY(sock->ipcom.domain == IP_AF_NETLINK))
        {
            /* Store in tail in last buffer if possible */
            if ((sock->rcv_tail->maxlen - sock->rcv_tail->end) >= (pkt->end - pkt->start))
            {
                ipcom_memcpy(&sock->rcv_tail->data[sock->rcv_tail->end], &pkt->data[pkt->start], pkt->end - pkt->start);
                sock->rcv_tail->end += pkt->end - pkt->start;

                ipcom_pkt_free(pkt);
                ipcom_spinlock_unlock(sock->rcv_lock);
                ipnet_sock_data_avail(sock, 0, IP_SHUT_RD);
                return 0;
            }
        }
#endif /* defined(IPNET_USE_NETLINKSOCK)) */

        if (sock->ipcom.rcv_bytes + pkt->maxlen > sock->recv_max_bytes)
        {
            ipcom_spinlock_unlock(sock->rcv_lock);
            IPCOM_LOG4(DEBUG2,
                       "ipnet_queue_received_packet: socket %d receive buffer full (%d:%d), packet is %d",
                       sock->ipcom.fd,
                       sock->ipcom.rcv_bytes,
                       sock->recv_max_bytes,
                       pkt->maxlen);
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 4, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_queue_received_packet, IPCOM_WV_NETD_FULLSOCK,
                              IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
            IPNET_STATS(pkt_queue_emsgsize++);

            /* Let the receiving process run if it has the same priority
               as the stack task */
            ipcom_proc_yield();

            return IPNET_ERRNO(EMSGSIZE);
        }
    }

    /* Add the packet to the socket receive queue */
    sock->ipcom.rcv_bytes += pkt->maxlen;
    if (sock->rcv_head != IP_NULL)
        sock->rcv_tail->next = pkt;
    else
        sock->rcv_head = pkt;
    sock->rcv_tail = pkt;
    pkt->next = IP_NULL;

    ipcom_spinlock_unlock(sock->rcv_lock);
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_SOCKRECVQ);

    ipnet_sock_data_avail(sock, 0, IP_SHUT_RD);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_data_avail
 *===========================================================================
 * Description: Is called each time data arrives to the socket so that
 *              ipcom_recvfrom() and ipcom_accept() can be called without
 *              blocking.
 * Parameters:  sock - The socket on which data arrived.
 *              so_errno: 0 = data, -1 = close else socket errno.
 *              how - Determines if processes/tasks sleeping on read and/or
 *              write should be woken.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_data_avail(Ipnet_socket *sock, int so_errno, int how)
{
    Ipcom_socket_eventcb event_cb = sock->ipcom.event_cb;

#ifdef IPSCTP
    /* Set the flag to make SCTP socket readable */
    if (sock->sctp_inst && (how == IP_SHUT_RD || how == IP_SHUT_RDWR))
    {
        ipcom_atomic_set(&sock->sctp_inst->wait_readable, 0);
    }
#endif

    if (so_errno != 0)
    {

        /* so_errno == -1 means a close event. */
        if (so_errno != -1 && sock->ipcom.so_errno == 0)
        {
            IPCOM_LOG5(DEBUG, "errno '%s' set on socket %d, domain %d, type %d, proto %d",
                       ipcom_strerror(so_errno),
                       sock->ipcom.fd,
                       sock->ipcom.domain,
                       sock->ipcom.type,
                       sock->proto);
            sock->ipcom.so_errno = so_errno;
            ip_assert(sock->ipcom.so_errno > 0);
        }

        if (event_cb != IP_NULL
            && IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_CLOSED))
        {
            event_cb(&sock->ipcom,
                     IP_NULL,
                     sock->ipcom.so_errno ? IP_SOEVENT_CB_ERRNO : IP_SOEVENT_CB_CLOSE);
#ifndef IP_PORT_LKM
            /* There must be no more events after the close event. */
            if (sock->ipcom.so_errno == 0)
            {
                sock->ipcom.event_cb = IP_NULL;
                event_cb             = IP_NULL;
            }
#endif
        }
    }

    if (how == IP_SHUT_RD || how == IP_SHUT_RDWR || sock->ipcom.so_errno)
    {

        if (event_cb != IP_NULL)
            event_cb(&sock->ipcom, IP_NULL, IP_SOEVENT_CB_SELREAD);
        ipnet_signal_ready_sock(sock,
                                IPNET_SOCK_POLL_READ,
                                ipnet_is_sock_readable);
    }

    if (how == IP_SHUT_WR || how == IP_SHUT_RDWR || sock->ipcom.so_errno)
    {
        if (event_cb != IP_NULL)
            event_cb(&sock->ipcom, IP_NULL, IP_SOEVENT_CB_SELWRITE);
        ipnet_signal_ready_sock(sock,
                                IPNET_SOCK_POLL_WRITE,
                                ipnet_is_sock_writable);
    }

#ifdef IPTCP
    if (how == IP_SHUT_EX || sock->ipcom.so_errno)
    {
        if (event_cb != IP_NULL)
            event_cb(&sock->ipcom, IP_NULL, IP_SOEVENT_CB_SELEXCEPT);
        ipnet_signal_ready_sock(sock,
                                IPNET_SOCK_POLL_EXCEPT,
                                ipnet_is_sock_exceptional);
    }
#endif /* IPTCP */

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
#ifdef IPNET_SO_WAKEUP_HOOK
        ipcom_windnet_socklib_sowakeupHook(sock, how);
#endif
#endif
}


/*
 *===========================================================================
 *                    ipnet_sock_free
 *===========================================================================
 * Description: Frees all memory allocated by a socket.
 * Parameters:  sock - The socket to free.
 *              clear_sockets_arr - IP_TRUE = this socket has not been
 *                  removed from ipnet->sockets[] yet.
 * Returns:
 */
IP_GLOBAL void
ipnet_sock_free(Ipnet_socket *sock, Ip_bool clear_sockets_arr)
{
    Ipcom_pkt            *pkt;
    Ipcom_socket_eventcb  event_cb = sock->ipcom.event_cb;

    if (clear_sockets_arr)
    {
        IPNET_DATA_LOCK(ipnet->sockets_lock);
        ipnet->sockets[sock->ipcom.fd] = IP_NULL;
        IPNET_DATA_UNLOCK(ipnet->sockets_lock);
    }
    else
        ip_assert(ipnet->sockets[sock->ipcom.fd] == IP_NULL);

    if (sock->ops->destroy != IP_NULL)
        sock->ops->destroy(sock);

    if (event_cb != IP_NULL)
        event_cb(&sock->ipcom, IP_NULL, IP_SOEVENT_CB_CLOSE);

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
    ipcom_windnet_socklib_socket_free(sock);
#endif

    /* Free any pending packets */
    while (sock->rcv_head != IP_NULL)
    {
        pkt = sock->rcv_head;
        sock->rcv_head = pkt->next;
        ipcom_pkt_free(pkt);
    }

#if defined(IPCOM_USE_INET) && defined(IPCOM_USE_INET6)
    if (sock->ipcom.domain == IP_AF_INET6)
        /* Free memory that was used for IPv6 mapped IPv4 addresses */
        ipcom_free(sock->ip4);
#endif

    ip_assert(sock->addr_entry == IP_NULL);
    ip_assert(sock->addr_next == IP_NULL);
    ip_assert(sock->addr_prev == IP_NULL);

    ipcom_spinlock_delete(&sock->rcv_lock);
    ipcom_free(sock->rcv_timeout);
    ipcom_slab_free(ipnet->slabs.socket, sock);
}


/*
 *===========================================================================
 *                    ipnet_do_close
 *===========================================================================
 * Description: Closes the socket and frees all resouces held by it.
 * Parameters:  sock - The socket to close.
 * Returns:     0 = success, <0 error code,
 */
IP_GLOBAL int
ipnet_do_close(Ipnet_socket *sock)
{
    int ret = 0;
    int how;

    ip_assert(sock != IP_NULL);
    ip_assert(ipnet->sockets[sock->ipcom.fd] == sock);

    switch (sock->flags & (IPNET_SOCKET_FLAG_READ_CLOSED | IPNET_SOCKET_FLAG_WRITE_CLOSED))
    {
    case 0:
        how = IP_SHUT_RDWR;
        break;
    case IPNET_SOCKET_FLAG_READ_CLOSED:
        how = IP_SHUT_WR;
        break;
    case IPNET_SOCKET_FLAG_WRITE_CLOSED:
        how = IP_SHUT_RD;
        break;
    default:
        how = 0;
        break;
    }

    if (how)
        ret = ipnet_sys_shutdown(sock->ipcom.fd, how, IP_FALSE);
    if (sock->ops->close != IP_NULL)
        sock->ops->close(sock);

    (void) ipnet_sock_drop_all_multicast_memberships(sock,
                                                     IP_NULL,
                                                     IP_TRUE,
                                                     IP_TRUE);

    IPNET_DATA_LOCK(ipnet->sockets_lock);
    ip_assert(ipnet->sockets[sock->ipcom.fd] == sock);
    if (ipcom_atomic_sub_and_return(&sock->ref_count, 1) > 0)
        IPNET_DATA_UNLOCK(ipnet->sockets_lock);
    else
    {
        /* This was the last reference held on this socket */
        if (ipcom_atomic_get(&sock->snd_bytes) == 0)
        {
            /* This was the last reference to the socket and there is
               no packets waiting for transmission in this socket. */
            ipnet->sockets[sock->ipcom.fd] = IP_NULL;
            IPNET_DATA_UNLOCK(ipnet->sockets_lock);
            ipnet_sock_free(sock, IP_FALSE);
        }
        else
        {
            IPNET_DATA_UNLOCK(ipnet->sockets_lock);
            if (sock->ipcom.type == IP_SOCK_RAW
                || sock->ipcom.type == IP_SOCK_DGRAM)
                /* There is no session to tear-down for these socket
                   types, unbind them right now. Socket types that
                   keep some kind of session has to unbind them self
                   when appropriate. */
                ipnet_sock_unbind(sock);
        }
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sock_drop_all_multicast_memberships
 *===========================================================================
 * Description: Drops all multicast memberships this socket has.
 * Parameters:  sock - a socket.
 *              netif - IP_NULL to dropp all multicast memberships or the
 *              interface for which all multicast memberships should be dropped.
 *              drop_ip4 - IP_TRUE if all IPv4 multicast groups should be dropped.
 *              drop_ip6 - IP_TRUE if all IPv6 multicast groups should be dropped.
 * Returns:     Number of multicast memberships that were dropped.
 */
IP_GLOBAL int
ipnet_sock_drop_all_multicast_memberships(Ipnet_socket *sock,
                                          Ipnet_netif *netif,
                                          Ip_bool drop_ip4,
                                          Ip_bool drop_ip6)
{
    int drop_count = 0;
    int fd = sock->ipcom.fd;

#if defined(IPCOM_USE_INET)
    IPCOM_UNUSED_ARG(drop_ip6);
    if (drop_ip4 && sock->ipcom.domain == IP_AF_INET)
    {
        Ipnet_ip4_sock_mcast_data *mcd;
        struct Ip_in_addr          maddr;

        while (ipnet->sockets[fd] != IP_NULL)
        {
            mcd = sock->ip4->multicast_addr_list;

            while (mcd != IP_NULL && netif != IP_NULL && mcd->netif != netif)
                mcd = mcd->next;

            if (mcd == IP_NULL)
                break;

            drop_count++;
            /* Must be copied since the 'mcd' structure will be freed */
            maddr = mcd->maddr;
            (void)ipnet_sock_ip4_mcast_delete_data(sock,
                                                   mcd->netif,
                                                   &maddr);
        }
    }
#endif /* IPCOM_USE_INET */

#if defined(IPCOM_USE_INET6)
    IPCOM_UNUSED_ARG(drop_ip4);
    if (drop_ip6 && sock->ipcom.domain == IP_AF_INET6)
    {
        Ipnet_ip6_sock_mcast_data *mcd;
        struct Ip_in6_addr         maddr;

        while (ipnet->sockets[fd] != IP_NULL)
        {
            mcd = sock->ip6->multicast_addr_list;

            while (mcd != IP_NULL && netif != IP_NULL && mcd->netif != netif)
                mcd = mcd->next;

            if (mcd == IP_NULL)
                break;

            drop_count++;
            /* Must be copied since the 'mcd' structure will be freed */
            maddr = mcd->maddr;
            (void)ipnet_sock_ip6_mcast_delete_data(sock,
                                                   mcd->netif,
                                                   &maddr);
        }
    }
#endif /* IPCOM_USE_INET6 */

    return drop_count;
}


/*
 *===========================================================================
 *                    ipnet_sock_get_route
 *===========================================================================
 * Description: Gets the route that would be used when sending on this socket.
 * Parameters:  sock - The socket to use when sending.
 * Returns:     IP_NULL or the route that would be used.
 *
 */
IP_GLOBAL Ipnet_route_entry *
ipnet_sock_get_route(Ipnet_socket *sock)
{
    Ipnet_route_policy_selector rps;
    Ipnet_route_entry          *rt;
    union Ip_sockaddr_union     name;
    union Ip_sockaddr_union     peer;
    Ip_socklen_t                name_len = sizeof(name);
    Ip_socklen_t                peer_len = sizeof(peer);

    if (ipnet_sys_getname(sock->ipcom.fd, &name.sa, &name_len, IP_TRUE) < 0
        || ipnet_sys_getname(sock->ipcom.fd, &peer.sa, &peer_len, IP_FALSE) < 0)
        return IP_NULL;

    rps.fields = IPNET_PR_RULE_DADDR | IPNET_PR_RULE_SADDR | IPNET_PR_RULE_PROTO;
#ifdef IPCOM_USE_INET
    if (sock->ipcom.domain == IP_AF_INET)
    {
        rps.daddr  = &peer.sin.sin_addr;
        rps.saddr  = &name.sin.sin_addr;
    }
#endif
#ifdef IPCOM_USE_INET6
    if (sock->ipcom.domain == IP_AF_INET6)
    {
        rps.daddr  = &peer.sin6.sin6_addr;
        rps.saddr  = &name.sin6.sin6_addr;
    }
#endif
    rps.proto = sock->proto;

    if (ipnet_route_lookup_ecmp(sock->ipcom.domain,
                                sock->vr_index,
                                IPNET_RTL_FLAG_DONTCLONE,
                                rps.daddr,
                                0,
                                0,
                                &rps,
                                &rt,
                                IP_NULL) < 0)
        /* No route to the peer */
        return IP_NULL;

    return rt;
}

/*
 *===========================================================================
 *                    ipnet_sock_get_ops
 *===========================================================================
 * Description: Find the socket operation structure by specified domain+type+
 *              protocol.
 * Parameters:  sa_family - the socket domain type.
 *              type - the socket type.
 *              protocol - the protocol type.
 * Returns:     NOT NULL = the result socket operation structure,
 *              NULL = no match.
 *
 */
IP_GLOBAL Ipnet_sock_ops *
ipnet_sock_get_ops(int domain, int type, int protocol)
{
    struct Ipnet_sock_ctor_key  k;

    k.domain = domain;
    k.type   = type;
    k.proto  = protocol;

    return (Ipnet_sock_ops *)ipcom_hash_get(ipnet->socket_ops, &k);
}


/*
 ****************************************************************************
 * 11                  HANDLER FOR PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sys_socket
 *===========================================================================
 * Description: Creates a new socket.
 * Parameters:  domain - The address domain.
 *              type - The socket type.
 *              protocol - The protocol the stack must use for
 *                    communication when using this socket.
 *              kernel_context - IP_TRUE if the socket is create by
 *                    the stack kernel, IP_FALSE if it is created in
 *                    user context.
 * Returns:     0 >= The new socket descriptor.
 *              0 < error, error code can be read from ipcom_errno.
 *
 */
IP_GLOBAL Ip_fd
ipnet_sys_socket(int domain, int type, int protocol, Ip_bool user_context)
{
    struct Ipnet_sock_ctor_key  k;
    Ipnet_sock_ops             *ops;
    Ipnet_socket               *sock = IP_NULL;
    int                         i;
    int                         ret = IPNET_ERRNO(EMFILE);

    k.domain = domain;
    k.type   = type;
    k.proto  = protocol;

    ops = ipcom_hash_get(ipnet->socket_ops, &k);
    if (ops == IP_NULL)
    {
        /* Try wildcard on protocol */
        k.proto = -1;
        ops = ipcom_hash_get(ipnet->socket_ops, &k);
    }

    if (ops == IP_NULL)
    {
        /* No matching socket */
        k.no_domain = IP_TRUE;
        k.no_type   = IP_TRUE;
        ipcom_hash_for_each(ipnet->socket_ops,
                            (Ipcom_hash_foreach_cb_func) ipnet_sock_create_failed_cb,
                            &k);
        if (k.no_domain)
            return IPNET_ERRNO(EAFNOSUPPORT);
        if (k.no_type)
            return IPNET_ERRNO(ESOCKTNOSUPPORT);
        return IPNET_ERRNO(EPROTONOSUPPORT);
    }


    /* Find an available socket descriptor slot. */
    for (i = 1; i <= ipnet_conf_max_sockets - IPNET_SOCKET_STARTFD; i++)
    {
        Ip_fd fd;
        Ip_fd last_fd;

        last_fd = ipnet->last_fd;

        fd = (last_fd - IPNET_SOCKET_STARTFD + i)
            % (ipnet_conf_max_sockets - IPNET_SOCKET_STARTFD) + IPNET_SOCKET_STARTFD;
        if (ipnet->sockets[fd] != IP_NULL)
            continue;

        ret = IPNET_ERRNO(ENOMEM);
        sock = ipcom_slab_alloc(ipnet->slabs.socket);
        if (sock == IP_NULL)
            goto errout;

        if (ipcom_spinlock_create(&sock->rcv_lock) != IPCOM_SUCCESS)
            goto errout;

        /* Init socket */
        ipcom_atomic_set(&sock->ref_count, 1);
        sock->ipcom.fd        = fd;
        sock->ipcom.domain    = (Ip_u16) domain;
        sock->proto           = (Ip_u16) protocol;
        sock->ipcom.type      = (Ip_u16) type;
        sock->max_hdrspace    = (Ip_u16) ops->hdr_space;
        sock->multi_hop_limit = 1;
        sock->ipcom.pid       = ipnet_get_effective_pid();
        sock->send_max_bytes  = ipcom_sysvar_get_as_int0("ipnet.sock.DefaultSendBufSize", 10000);
        sock->recv_max_bytes  = ipcom_sysvar_get_as_int0("ipnet.sock.DefaultRecvBufSize", 10000);
        sock->ops             = ops;
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_LOOP_MULTICAST);
        sock->vr_index        = ipnet_get_effective_vr();
        sock->pkt_dtor        = ipnet_sock_pkt_dtor;
        sock->snd_route_cache_id = ipnet->route_cache_id - 1;

        /* Initialize the lists of user tasks that are waiting for
           the socket to be readable, writable and/or exceptional */
        ipcom_list_init(&sock->poll_list_head);

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
        ret = ipcom_windnet_socklib_socket_init(sock);
        if (ret < 0)
            goto errout;
#endif
        if (ops->init != IP_NULL)
        {
            ret = ops->init(sock, user_context);
            if (ret < 0)
                goto errout;
        }
        ipnet->last_fd     = fd;
        ipnet->sockets[fd] = sock;

        return fd;
    }

    /* No free socket slot found. */
 errout:
    if (sock != IP_NULL)
    {
#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
        ipcom_windnet_socklib_socket_free(sock);
#endif
        ipcom_slab_free(ipnet->slabs.socket, sock);
    }
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sys_shutdown
 *===========================================================================
 * Description: Shuts down the read and/or the write end of a socket.
 * Parameters:  fd - The file descriptor to shut down.
 *              how - How the socket should be shut down.
 *              from_user_signal - from user shutdown flag.
 * Returns:     0 = success, -1 = error
 */
IP_GLOBAL int
ipnet_sys_shutdown(Ip_fd fd, int how, Ip_bool from_user_signal)
{
    Ipnet_socket *sock;
    int           ret;
    int           ret2;
    int           so_errno = -1; /* Close event */

    IPNET_FD_TO_SOCKET(fd, sock);

    /* if SCTP UDP-like socket, just return.
     * ?? WHY UDP CAN USE THIS API ?? */
#ifdef IPSCTP
    if ((IP_TRUE == from_user_signal)
        && (IP_IPPROTO_SCTP == sock->proto)
        && (IP_NULL != sock->sctp_inst) /* must not NULL */
        && (ipsctp_style(sock, UDP)))
        return IPNET_ERRNO(EOPNOTSUPP);
#endif

    /* Make sure next send is synchronous */
    --sock->snd_route_cache_id;

    /* Make sure that read and/or write end close is only done once */
    switch (how)
    {
    case IP_SHUT_RDWR:
        ret  = ipnet_sys_shutdown(fd, IP_SHUT_RD, from_user_signal);
        ret2 = ipnet_sys_shutdown(fd, IP_SHUT_WR, from_user_signal);
        return ret == 0 ? ret2 : ret;

    case IP_SHUT_RD:
        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_READ_CLOSED))
            return IPNET_ERRNO(ESHUTDOWN);
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_READ_CLOSED);
        break;

    case IP_SHUT_WR:
        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_WRITE_CLOSED))
            return IPNET_ERRNO(ESHUTDOWN);
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_WRITE_CLOSED);

        if (ipnet_sock_has_waiting_writers(sock))
            so_errno = IP_ERRNO_EPIPE;

        if (sock->snd_head != IP_NULL)
        {
            /* The octest stored in sock->snd_bytes has been allocated
               in the send buffer. Undo that allocation. */
            ipcom_atomic_sub(&sock->snd_bytes,
                             sock->snd_head->end - sock->snd_head->start);
            ipcom_pkt_free(sock->snd_head);
            sock->snd_head = IP_NULL;
        }
        if (sock->snd_to != IP_NULL)
        {
            ipcom_free(sock->snd_to);
            sock->snd_to = IP_NULL;
        }
        break;

    default:
        return IPNET_ERRNO(EINVAL);
    }

    if (sock->ops->shutdown != IP_NULL)
        sock->ops->shutdown(sock, how);

    ipnet_sock_data_avail(sock, so_errno, how);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_sys_socketclose
 *===========================================================================
 * Description: Closes a socket.
 * Parameters:  fd - The socket to close.
 * Returns:     0= success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_sys_socketclose(Ip_fd fd)
{
    Ipnet_socket *sock;

    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);
    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_CLOSED);
    return ipnet_do_close(sock);
}


/*
 *===========================================================================
 *                    ipnet_sys_listen
 *===========================================================================
 * Description: Places a socket a state where it is listening for incoming
 *              connection(s).
 * Parameters:  fd - the socket to create a backlog for.
 *              backlog - The maximum length of the queue of pending
 *                    connections. Set to 0 to allow the stack to
 *                    select a reasonably sized backlog.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sys_listen(Ip_fd fd, int backlog)
{
    Ipnet_socket *sock;

    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);

    if (sock->ops->listen == IP_NULL)
        return IPNET_ERRNO(EOPNOTSUPP);

    return sock->ops->listen(sock, backlog);
}


/*
 *===========================================================================
 *                    ipnet_sys_accept
 *===========================================================================
 * Description: Accepts an incoming connection attempt on a socket.
 * Parameters:  fd - A socket that is in listen state (ipcom_listen()) has
 *              been called.
 *              name - Will hold the address of the peer (set to IP_NULL if
 *              that information is not wanted).
 *              addrlen - Must be initialized to the length of the 'name'
 *              buffer, will hold the actual 'name' length upon return.
 *              Set to IP_NULL if 'name' is IP_NULL.
 * Returns:     0>= the socket to use for communication (success),
 *              <0 = error.
 *
 */
IP_GLOBAL int
ipnet_sys_accept(Ip_fd fd, struct Ip_sockaddr *name, Ip_socklen_t *namelen)
{
    Ipnet_socket  *sock;

    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);

    if (sock->ops->accept == IP_NULL)
        return IPNET_ERRNO(EOPNOTSUPP);

    return sock->ops->accept(sock, name, namelen);
}

/*
 *===========================================================================
 *                    ipnet_sys_bind
 *===========================================================================
 * Description: Assigns a name to a unnamed socket.
 * Parameters:  fd - The socket to name.
 *              name - The socket name.
 *              namelen - The length of the name buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_sys_bind(Ip_fd fd, IP_CONST struct Ip_sockaddr *name, Ip_socklen_t namelen)
{
    Ipnet_socket *sock;

    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);
    ip_assert2(name != IP_NULL);
    ip_assert2(namelen > 0);

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);

#if defined(IPNET_COMPAT_OLDSOCK) && defined(IPCOM_USE_SA_LEN)
    if (name->sa_family == 0 && name->sa_len < IP_AF_MAX)
        ((struct Ip_sockaddr *)name)->sa_family = name->sa_len;
    ((struct Ip_sockaddr *)name)->sa_len = (Ip_u8) namelen;
#endif

#ifdef IPCOM_USE_SA_LEN
    ip_assert(name->sa_len > 0);
#endif

    if (sock->ops->bind == IP_NULL)
    {
        /* This domain/type/proto does not support bind */
        IPCOM_LOG1(WARNING, "socket %d does not support the bind operation", sock->ipcom.fd);
        return IPNET_ERRNO(EOPNOTSUPP);
    }

    if (name->sa_family != sock->ipcom.domain)
    {
        /* Wrong domain in socket name */
        IPCOM_LOG3(WARNING, "domain miss-match on socket %d in bind, socket domain %d, name domain %d",
                   sock->ipcom.fd, sock->ipcom.domain, name->sa_family);
        return IPNET_ERRNO(EINVAL);
    }

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_BOUND))
    {
        /* Socket already bound. */
        IPCOM_LOG1(NOTICE, "socket %d already bound", sock->ipcom.fd);
        return IPNET_ERRNO(EINVAL);
    }

    return sock->ops->bind(sock, name, namelen, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipnet_sys_connect
 *===========================================================================
 * Description: Connects the socket to a specified peer.
 * Parameters:  fd - The socket to connect.
 *              to - The address of the peer.
 *              tolen - The length of the to buffer.
 * Returns:     0= connection was successful, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sys_connect(Ip_fd fd, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    Ipnet_socket *sock = IP_NULL;
    int           ret;

    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);

    if (to == IP_NULL)
        return IPNET_ERRNO(EINVAL);

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);

    if (sock->ipcom.so_errno != 0)
        return -sock->ipcom.so_errno;

    if (sock->ops->connect == IP_NULL)
    {
        /* This domain does not support connect */
        IPCOM_LOG1(NOTICE, "socket %d does not support the connect call", sock->ipcom.fd);
        return IPNET_ERRNO(EOPNOTSUPP);
    }

#if defined(IPNET_COMPAT_OLDSOCK) && defined(IPCOM_USE_SA_LEN)
    if (to->sa_family == 0 && to->sa_len < IP_AF_MAX)
        ((struct Ip_sockaddr *)to)->sa_family = to->sa_len;
    ((struct Ip_sockaddr *)to)->sa_len = (Ip_u8) tolen;
#endif /* IPNET_COMPAT_OLDSOCK */

    if (sock->ipcom.type == IP_SOCK_STREAM
        && IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED))
        return IPNET_ERRNO(EISCONN);

#ifdef IPSCTP /* SCTP UDP-style socket allow multi-connect */
    if ((IP_IPPROTO_SCTP != sock->proto)
        && (IP_SOCK_SEQPACKET != sock->ipcom.type))
#endif
        IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);

    if (to->sa_family != sock->ipcom.domain && to->sa_family != IP_AF_UNSPEC)
        return IPNET_ERRNO(EAFNOSUPPORT);

    ret = sock->ops->connect(sock, to, tolen);
    if (ret != 0
#ifdef IPSCTP /* SCTP UDP-style socket allow multi-connect */
        && ((IP_IPPROTO_SCTP != sock->proto)
            && (IP_SOCK_SEQPACKET != sock->ipcom.type))
#endif
        && ret != -IP_ERRNO_EISCONN
        && ret != -IP_ERRNO_EWOULDBLOCK
        && ret != -IP_ERRNO_EINPROGRESS)
        IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);
    return ret;
}


/*
 *===========================================================================
 *                    ipcom_pkt_hdrspace
 *===========================================================================
 * Description: Returns the minimum number of octets that has to be reserved
 *              before the transport layer payload in the packet when using
 *              the zero copy API.
 * Parameters:  fd - The socket descriptor to use when sending this packet.
 * Returns:     >=0 = the number of octets that must be reserved for
 *              transport, network and link header.
 *              =IP_SOCKERR = error, get the error from ipcom_errno.
 *
 */
IP_GLOBAL int
ipcom_pkt_hdrspace(Ip_fd fd)
{
    Ipnet_socket *sock;

    IPNET_FD_TO_SOCKET(fd, sock);

    if (sock->ipcom.type == IP_SOCK_STREAM)
        return IPNET_ERRNO(EPROTONOSUPPORT);

    return ipcom_conf_max_link_hdr_size + sock->max_hdrspace;
}


/*
 *===========================================================================
 *                      ipnet_sys_sendmsg
 *===========================================================================
 * Description: Sends a packet to the address to which the socket is connected
 *              or to the 'to_param' address if the socket is not connected.
 * Parameters:  fd - The descriptor for the socket to use.
 *              msg - Send parameters.
 *              pkt - data to send.
 * Returns:     >=0 = number of bytes sent
                 <0 = error code.
 *
 */
IP_GLOBAL Ip_ssize_t
ipnet_sys_sendmsg(Ip_fd fd, IP_CONST struct Ip_msghdr *msg, Ipcom_pkt *pkt)
{
    Ipnet_socket    *sock;
    int              ret;

    ip_assert2(fd >= IPNET_SOCKET_STARTFD);
    ip_assert2(fd < ipnet_conf_max_sockets);
    ip_assert2(ipnet->sockets[fd] != IP_NULL);
    ip_assert2(msg != IP_NULL);

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 7, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                      ipcom_sendmsg, IPCOM_WV_NETD_INFO_SEND,
                      IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
    IPNET_STATS(sock_sendmsg++);

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);

    ret = -sock->ipcom.so_errno;
    if (IP_UNLIKELY(ret))
        goto errout;

    if (IP_UNLIKELY(IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_WRITE_CLOSED)))
    {
        ret = IPNET_ERRNO(ESHUTDOWN);
        goto errout;
    }

#if defined(IPNET_COMPAT_OLDSOCK) && defined(IPCOM_USE_SA_LEN)
    if (msg->msg_name != IP_NULL)
    {
        struct Ip_sockaddr *to = (struct Ip_sockaddr *) msg->msg_name;

        if (to->sa_family == 0 && to->sa_len < IP_AF_MAX)
            ((struct Ip_sockaddr *)to)->sa_family = to->sa_len;
        ((struct Ip_sockaddr *)to)->sa_len = (Ip_u8) msg->msg_namelen;
    }
#endif /* IPNET_COMPAT_OLDSOCK */
    ip_assert(msg->msg_name == IP_NULL || msg->msg_namelen > 0);

    if (IP_UNLIKELY(IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_BOUND))
        && sock->ops->bind != IP_NULL)
    {
        union Ip_sockaddr_union name;

        /* Do an implicit bind */
        ipcom_memset(&name, 0, sizeof(name));
        name.sa.sa_family = (Ip_sa_family_t)sock->ipcom.domain;
        ret = sock->ops->bind(sock, &name.sa, sizeof(name), IP_TRUE);
        if (ret < 0)
            goto errout;
    }

    ip_assert(sock->ops->send != IP_NULL);
    ret = sock->ops->send(sock, msg, pkt);
    if (IP_LIKELY(ret >= 0))
        sock->snd_route_cache_id = ipnet->route_cache_id;
    return ret;

 errout:
    ipcom_pkt_free(pkt);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sys_getname
 *===========================================================================
 * Description: Returns the socket address of the local side (if bound) or
 *              the peer side (if connected).
 * Parameters:  fd - Socket descriptor.
 *              name - Will hold the socket address if the call succeeds.
 *              namelenp - The length of the 'name' buffer, will be updated
 *              to hold the actual size of 'name'.
 *              local - Set to IP_TRUE if the name of local size should be
 *              returned and IP_FALSE if the peer socket name should be returned.
 * Returns:     0 = success, <0 negative error code.
 *
 */
IP_GLOBAL int
ipnet_sys_getname(Ip_fd fd,
                  struct Ip_sockaddr *name,
                  Ip_socklen_t *namelenp,
                  Ip_bool local)
{
    Ipnet_socket  *sock;

    ip_assert(name != IP_NULL);
    ip_assert(namelenp != IP_NULL);

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);

    if (sock->ops->getname == IP_NULL)
        return IPNET_ERRNO(EOPNOTSUPP);

    return sock->ops->getname(sock, name, namelenp, local);
}



/*
 *===========================================================================
 *                    ipnet_sys_get_ancillary_data
 *===========================================================================
 * Description: Gets ancillary data that has is appropriate for the
 *                    specified socket.
 * Parameters:  sig - signal payload for getting ancillary data.
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_sys_get_ancillary_data(struct Ipnet_sig_ancillary_data *sig)
{
    Ipnet_socket      *sock;
    struct Ip_cmsghdr *cmsg;
    Ip_socklen_t       cmsg_len = 0;
    Ip_socklen_t       len = 0;
    struct Ip_msghdr  *msg = sig->msg;
    Ipcom_pkt         *pkt = sig->pkt;

    IPNET_FD_TO_SOCKET(sig->fd, sock);

    cmsg = IP_CMSG_FIRSTHDR(msg);
    if (cmsg == IP_NULL)
        return 0;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_RECV_IF))
    {
        /* Recv if socket option */
        len = ipnet_if_to_sockaddr_dl(ipnet_if_indextonetif(pkt->vr_index,
                                                            pkt->ifindex),
                                      IP_NULL);
        cmsg_len += IP_CMSG_SPACE(len);
        if (cmsg_len > msg->msg_controllen)
            goto truncated;

        cmsg->cmsg_len = IP_CMSG_LEN(len);
#ifdef IPCOM_USE_INET
        if (sock->ipcom.domain == IP_AF_INET)
        {
            cmsg->cmsg_level = IP_IPPROTO_IP;
            cmsg->cmsg_type  = IP_IP_RECVIF;
        }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if (sock->ipcom.domain == IP_AF_INET6)
        {
            cmsg->cmsg_level = IP_IPPROTO_IPV6;
            cmsg->cmsg_type  = IP_IPV6_RECVIF;
        }
#endif /* IPCOM_USE_INET6 */
        (void)ipnet_if_to_sockaddr_dl(ipnet_if_indextonetif(pkt->vr_index,
                                                            pkt->ifindex),
                                      IP_CMSG_DATA(cmsg));
        cmsg = IP_CMSG_NXTHDR(msg, cmsg);
    }

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_SENDER_LINK_ADDR))
    {
        /* Sender link address */
        int                    start;
        Ipnet_netif           *netif;
        struct Ip_sockaddr_dl *dl;

        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
            netif = ipnet_loopback_get_netif(pkt->vr_index);
        else
            netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);

        if (netif != IP_NULL)
        {
            len = ipnet_if_to_sockaddr_dl(netif, IP_NULL);
            cmsg_len += IP_CMSG_SPACE(len);
            if (cmsg_len > msg->msg_controllen)
                goto truncated;

            cmsg->cmsg_len = IP_CMSG_LEN(len);
#ifdef IPCOM_USE_INET
            if (sock->ipcom.domain == IP_AF_INET)
            {
                cmsg->cmsg_level = IP_IPPROTO_IP;
                cmsg->cmsg_type  = IP_IP_X_SENDERLINKADDR;
            }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
            if (sock->ipcom.domain == IP_AF_INET6)
            {
                cmsg->cmsg_level = IP_IPPROTO_IPV6;
                cmsg->cmsg_type  = IP_IPV6_X_SENDERLINKADDR;
            }
#endif /* IPCOM_USE_INET6 */

            dl = IP_CMSG_DATA(cmsg);
            (void)ipnet_if_to_sockaddr_dl(netif, dl);

            /* Get the sender link address */
            start = pkt->start;
            pkt->start = pkt->ipstart - netif->ipcom.link_hdr_size;
            (void)netif->link_get_frame_info(netif,
                                             pkt,
                                             IP_NULL,
                                             IP_NULL,
                                             (Ip_u8 *) IP_SOCKADDR_DL_LLADDR(dl),
                                             IP_NULL);
            pkt->start = start;

            cmsg = IP_CMSG_NXTHDR(msg, cmsg);
        }
        /* else: interface has been detached */
    }

#ifdef IPCOM_USE_INET
    if (sock->ipcom.domain == IP_AF_INET)
        ipnet_add_ancillary_ipv4_data(msg, &cmsg, &cmsg_len, sock, pkt);
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (sock->ipcom.domain == IP_AF_INET6)
        ipnet_add_ancillary_ipv6_data(msg, &cmsg, &cmsg_len, sock, pkt);
#endif /* IPCOM_USE_INET6 */

 truncated:
    if (msg->msg_controllen < cmsg_len)
    {
        IP_BIT_SET(msg->msg_flags, IP_MSG_CTRUNC);
        cmsg_len -= IP_CMSG_SPACE(len);
    }

    msg->msg_controllen = cmsg_len;
    return 0;
}


#ifdef IP_PORT_LKM
/*
 *===========================================================================
 *                    ipnet_sys_pkt_recv_raw_pkt
 *===========================================================================
 * Description: Receives a raw packet from a socket.
 *              This function is not allowed to lock neither the code locks
 *              nor any data locks, since it is called recursively from
 *              locked context.
 * Parameters:  fd - A socket descriptor.
 *              flags -
 * Returns:     A reference to a packet
 *              IP_NULL - failure
 *
 */
IP_GLOBAL Ipcom_pkt*
ipnet_pkt_recv_raw_pkt(Ip_fd fd, int flags)
{
    Ipcom_pkt    *pkt = IP_NULL;
    Ipnet_socket *sock;

    if (IP_UNLIKELY(fd < IPNET_SOCKET_STARTFD)
        || IP_UNLIKELY(fd >= ipnet_conf_max_sockets)
        || IP_UNLIKELY(IP_NULL == (sock = ipnet->sockets[fd]))
        || IP_UNLIKELY(IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CLOSED)))
    {
        IPNET_STATS(sock_ebadf++);
        return IP_NULL;
    }

    ipcom_spinlock_lock(sock->rcv_lock);

    if (sock->ipcom.so_errno)
        goto errout;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_READ_CLOSED))
        goto errout;

    pkt = sock->rcv_head;
    if (pkt == IP_NULL)
    {
        ipcom_errno_set(-IP_ERRNO_EWOULDBLOCK);
        goto errout;
    }

    if (IP_LIKELY(IP_BIT_ISFALSE(flags, IP_MSG_PEEK)))
    {
        /* Unlink the first packet from the queue head. */
        if (sock->ipcom.type == IP_SOCK_STREAM)
            sock->ipcom.rcv_bytes -= (pkt->end - pkt->start);
        else
            sock->ipcom.rcv_bytes -= pkt->maxlen;

        sock->rcv_head = pkt->next;
        if (sock->rcv_head == IP_NULL)
            sock->rcv_tail = IP_NULL;
    }

    if (sock->ipcom.type == IP_SOCK_DGRAM)
    {
        /* Datagram payload starts after IP header */
        pkt->start -= sizeof(Ipnet_pkt_udp);
    }

errout:

    ipcom_spinlock_unlock(sock->rcv_lock);

    return pkt;
}

#endif /* IP_PORT_LKM */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

