/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_routesock.c,v $ $Revision: 1.156 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_routesock.c,v $
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

#ifdef IPNET_USE_ROUTESOCK

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_syslog.h>

#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_route.h"
#include "ipnet_routesock.h"
#include "ipnet_usr_sock.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_ROUTESOCK_INIT_PKT(pkt, size) \
    do { \
        (pkt)->start = (int) ((Ip_ptrdiff_t) (pkt)->data & 0x3); \
        if ((pkt)->start) \
            (pkt)->start = 4 - (pkt)->start; \
        (pkt)->end   = (int) (size) + pkt->start; \
    } while ((0))


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

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
IP_STATIC int ipnet_routesock_input(Ipcom_pkt *pkt);

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

/*
 *===========================================================================
 *                    ipnet_sock_route_lookup
 *===========================================================================
 * Description: Returns the route socket matching the parametres. Source is
 *              always this host and the destination is the peer.
 * Parameters:  vr - The virtual route to use.
 *              version - The protocol version for the routing packet.
 * Returns:     A pointer to the matching socket or IP_NULL if no socket match.
 *
 */
IP_STATIC Ipnet_socket *
ipnet_sock_route_lookup(Ip_u16 vr, int version,
                        IP_CONST void *unused1, Ip_u16 unused2,
                        IP_CONST void *unused3, Ip_u16 unused4)
{
    Ipnet_socket_lookup_key key;

    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;

    ipcom_memset(&key, 0, sizeof(key));
    key.domain = IP_AF_ROUTE;
    key.proto  = (Ip_u16) version;
    key.vr     = vr;
    return ipcom_hash_get(ipnet->reachable_sockets, &key);
}


/*
 *===========================================================================
 *                    ipnet_sock_route_init
 *===========================================================================
 * Description: The route domain socket initialization.
 * Parameters:  sock - The socket that is beeing created.
 *              user_context - not used.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_route_init(Ipnet_socket *sock, Ip_bool user_context)
{
    IPCOM_UNUSED_ARG(user_context);
    ipnet->num_route_sock++;
    ipnet_sock_set_reachable(sock);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_route_destroy
 *===========================================================================
 * Description: Destroys the route socket.
 * Parameters:  sock - The socket beeing closed.
 * Returns:     0 = success
 *
 */
IP_STATIC void
ipnet_sock_route_destroy(Ipnet_socket *sock)
{
    ipnet_sock_unbind(sock);
    ipnet->num_route_sock--;
}


/*
 *===========================================================================
 *                    ipnet_sock_route_send
 *===========================================================================
 * Description: Inputs a rouet socket message to the stack.
 * Parameters:  sock - The socket to use when sending.
 *              msg - The send parameters.
 *              pkt - The packet to send.
 * Returns:     >=0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sock_route_send(Ipnet_socket *sock,
                      IP_CONST struct Ip_msghdr *msg,
                      Ipcom_pkt *pkt)
{
    int        length, ret;

    IPCOM_UNUSED_ARG(sock);
    IPCOM_UNUSED_ARG(msg);

    length = pkt->end - pkt->start;
    ret = ipnet_routesock_input(pkt);
    return ret < 0 ? ret : length;
}


/*
 *===========================================================================
 *                    ipnet_routesock_output
 *===========================================================================
 * Description: Write the packet to all affected sockets.
 * Parameters:  pkt - The route socket to write.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_output(Ipcom_pkt *pkt)
{
    struct Ipnet_rt_msghdr *rtm;

    rtm = (struct Ipnet_rt_msghdr *)&pkt->data[pkt->start];
    if (rtm->rtm_type <= IPNET_RTM_LOCK)
        pkt->ifindex = rtm->rtm_pid;
    ip_assert(pkt->end - pkt->start == rtm->rtm_msglen);

    return ipnet_raw_input(pkt, IP_TRUE, 0, IP_NULL, 0, IP_NULL, 0,
                           (Ipnet_sock_lookup_f) ipnet_sock_route_lookup);
}


/*
 *===========================================================================
 *                    ipnet_routesock_get_addrs_from_rt_msg
 *===========================================================================
 * Description: Extracts the pointers for all addresses in the message.
 * Parameters:  rtm - The message to extract from.
 *              addr - Will hold the pointers to the addresses.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_get_addrs_from_rt_msg(struct Ipnet_rt_msghdr *rtm,
                                      struct Ip_sockaddr *addr[IPNET_RTAX_MAX])
{
    Ip_u32              size;
    struct Ip_sockaddr *rtm_addr;
    int                 i;
    int                 sa_len;

    size = rtm->rtm_msglen;

    if (size < sizeof(struct Ipnet_rt_msghdr))
        /* Cannot be a valid Ipnet_rt_msghdr structure */
        return -IP_ERRNO_EINVAL;

    rtm_addr = (struct Ip_sockaddr *) (rtm + 1);
    size -= sizeof(struct Ipnet_rt_msghdr);

    for (i = 0; i < IPNET_RTAX_MAX; i++)
    {
        if (IP_BIT_ISFALSE(rtm->rtm_addrs, 1 << i))
        {
            addr[i] = IP_NULL;
            continue;
        }

        if (size < sizeof(Ip_u32))
            return -IP_ERRNO_EINVAL;

        addr[i] = rtm_addr;
        sa_len = IPCOM_SA_LEN_GET(addr[i]) ? IP_ROUNDUP(IPCOM_SA_LEN_GET(addr[i]), sizeof(Ip_u32)) : sizeof(Ip_u32);
        rtm_addr = (struct Ip_sockaddr *) ((Ip_u8*) rtm_addr + sa_len);
        size -= sa_len;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_routesock_get_rt
 *===========================================================================
 * Description: Get the route corresponding the information in the 'rtm' message.
 * Parameters:  [in] rtm - A route socket message.
 *              [out] re - Will contain the route.
 *              [out] domain - Will contain the domain.
 * Returns:     IPNET_ROUTE_PERFECT_MATCH = perfect match,
 *              IPNET_ROUTE_MASK_MATCH = mask match,
 *              <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_get_rt(Ip_u16 vr,
                       struct Ipnet_rt_msghdr *rtm,
                       Ipnet_route_entry **re,
                       int *domain,
                       Ip_bool use_gw)
{
    int                 ret;
    struct Ip_sockaddr *addr[IPNET_RTAX_MAX];

    ip_assert(re != IP_NULL);
    *re = IP_NULL;

    ret = ipnet_routesock_get_addrs_from_rt_msg(rtm, addr);
    if (ret != 0)
        return ret;

    if (addr[IPNET_RTAX_DST] == IP_NULL)
        /* Cannot be a valid Ipnet_rt_msghdr structure */
        return -IP_ERRNO_EINVAL;

    *domain = addr[IPNET_RTAX_DST]->sa_family;
#ifdef IPCOM_USE_INET
    if (*domain == IP_AF_INET)
    {
        struct Ip_in_addr *dst;
        struct Ip_in_addr *mask = IP_NULL;

        dst = &((struct Ip_sockaddr_in *) addr[IPNET_RTAX_DST])->sin_addr;
        if (addr[IPNET_RTAX_NETMASK])
            mask = &((struct Ip_sockaddr_in *) addr[IPNET_RTAX_NETMASK])->sin_addr;

        ret = ipnet_route_raw_lookup2(IP_AF_INET,
                                      vr,
                                      rtm->rtm_table,
                                      IPNET_RTL_FLAG_DONTCLONE,
                                      dst,
                                      0,
                                      mask,
                                      use_gw ? addr[IPNET_RTAX_GATEWAY] : IP_NULL,
                                      rtm->rtm_index,
                                      re);
    }
    else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (*domain == IP_AF_INET6)
    {
        struct Ip_sockaddr_in6 *sin6_dst = (struct Ip_sockaddr_in6 *) addr[IPNET_RTAX_DST];
        struct Ip_in6_addr     *mask = IP_NULL;

        if (addr[IPNET_RTAX_NETMASK])
            mask = &((struct Ip_sockaddr_in6 *) addr[IPNET_RTAX_NETMASK])->sin6_addr;

        ipnet_ip6_extract_scoped_addr(addr[IPNET_RTAX_GATEWAY], rtm->rtm_index);

        ret = ipnet_route_raw_lookup2(IP_AF_INET6,
                                      vr,
                                      rtm->rtm_table,
                                      IPNET_RTL_FLAG_DONTCLONE,
                                      &sin6_dst->sin6_addr,
                                      sin6_dst->sin6_scope_id,
                                      mask,
                                      addr[IPNET_RTAX_GATEWAY],
                                      rtm->rtm_index,
                                      re);
    }
    else
#endif /* IPCOM_USE_INET6 */
    {
        return -IP_ERRNO_EAFNOSUPPORT;
    }

    if (ret < 0)
        *re = IP_NULL;
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_routesock_create_addrs
 *===========================================================================
 * Description: Construct a rtm_addrs value from a route entry.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_routesock_create_addrs(int domain, Ipnet_route_entry *rt, Ip_u16 *size, int *addrs)
{
    Ip_u16 domain_addr_size;

    ip_assert(rt);
    ip_assert(size);

    if (domain == IP_AF_INET)
        domain_addr_size = sizeof(struct Ip_sockaddr_in);
    else
        domain_addr_size = sizeof(struct Ip_sockaddr_in6);

    *size = (Ip_u16) sizeof(struct Ipnet_rt_msghdr) + domain_addr_size;
    *addrs = IPNET_RTA_DST;

    if (rt->netif)
    {
        *size += (Ip_u16) (ipnet_if_to_sockaddr_dl(rt->netif, IP_NULL) + domain_addr_size);
        *addrs |= IPNET_RTA_IFA | IPNET_RTA_IFP;
    }

    if (rt->gateway)
    {
        *size += IPCOM_SA_LEN_GET(rt->gateway);
        IP_BIT_SET(*addrs, IPNET_RTA_GATEWAY);
    }

    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_MASK))
    {
        *size += domain == IP_AF_INET ? sizeof(struct Ip_sockaddr_in) : sizeof(struct Ip_sockaddr_in6);
        IP_BIT_SET(*addrs, IPNET_RTA_NETMASK);
    }
}


/*
 *===========================================================================
 *                    ipnet_routesock_is_valid_addr
 *===========================================================================
 * Description: Determines if the address is OK to dump via routesock.
 * Parameters:  domain - address domain
 *              addr - The address entry.
 * Returns:     IP_TRUE, IP_FALSE
 *
 */

IP_STATIC Ip_bool
ipnet_routesock_is_valid_addr(int domain, void *addr)
{
#ifdef IPQUAGGA
    Ip_bool ret = IP_FALSE;

    if (domain == IP_AF_INET)
    {
        Ipnet_ip4_addr_entry *addr4 = (Ipnet_ip4_addr_entry *)addr;

        if (addr4->type == IPNET_ADDR_TYPE_UNICAST)
            ret = IP_TRUE;
    }
    else if (domain == IP_AF_INET6)
    {
        Ipnet_ip6_addr_entry *addr6 = (Ipnet_ip6_addr_entry *)addr;

        if (addr6->type == IPNET_ADDR_TYPE_UNICAST)
            ret = IP_TRUE;
    }

    return ret;
#else
    (void)domain;
    (void)addr;

    return IP_TRUE;
#endif
}


/*
 *===========================================================================
 *                    ipnet_routesock_output_addr
 *===========================================================================
 * Description: Create a route socket message about an address.
 * Parameters:  type - The type of message to create
 *              (IPNET_RTM_NEWADDR or IPNET_RTM_DELADDR).
 *              netif - The affected interface.
 *              domain - The address domain.
 *              addr - The address entry.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC void
ipnet_routesock_output_addr(int type, Ipnet_netif *netif, int domain, void *addr)
{
    Ipcom_pkt               *pkt;
    Ip_size_t                size = 0;
    struct Ipnet_ifa_msghdr *ifa;

    if (!ipnet_routesock_is_valid_addr(domain, addr))
        return;

    (void)ipnet_sysctl_if_add_addrs_one(IP_NULL, netif, domain, addr, &size);

    pkt = ipcom_pkt_malloc(size, IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RTSOCK);
    IPNET_ROUTESOCK_INIT_PKT(pkt, size);
    ifa = (struct Ipnet_ifa_msghdr *) &pkt->data[pkt->start];
    (void)ipnet_sysctl_if_add_addrs_one(ifa, netif, domain, addr, &size);
    ifa->ifam_type = (Ip_u8)type;

    pkt->vr_index = netif->vr_index;
    (void)ipnet_routesock_output(pkt);
}


/*
 *===========================================================================
 *                    ipnet_routesock_output_rt
 *===========================================================================
 * Description: Create a message and send it to all routing sockets.
 * Parameters:
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_output_rt(Ip_u8 type,
                          Ip_pid_t pid,
                          int seq,
                          Ip_u16 vr,
                          Ip_u32 table,
                          Ipnet_route_entry *rt,
                          Ip_u16 size,
                          int addrs,
                          int domain,
                          int err)
{
    Ipcom_pkt              *pkt;
    struct Ipnet_rt_msghdr *rtm_reply;
    struct Ip_sockaddr     *rtm_addr;
    struct Ip_sockaddr     *addr;
    int                     i;
    Ip_u8                   rtm_addr_buf[IP_INET6_ADDRSTRLEN];

    pkt = ipcom_pkt_malloc(size + 3,
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return -IP_ERRNO_ENOBUFS;
    pkt->vr_index = vr;
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RTSOCK);

    /* Make sure that 'rtm_reply' is on a 4-byte boundary */
    IPNET_ROUTESOCK_INIT_PKT(pkt, 0);
    rtm_reply = (struct Ipnet_rt_msghdr *) &pkt->data[pkt->start];

    ipcom_memset(rtm_reply, 0, size);
    rtm_reply->rtm_version = IPNET_RTM_VERSION;
    rtm_reply->rtm_type    = type;
    rtm_reply->rtm_errno   = -err;
    rtm_reply->rtm_table   = table;
    rtm_reply->rtm_pid     = pid;
    rtm_reply->rtm_seq     = seq;
    rtm_addr = (struct Ip_sockaddr *) (rtm_reply + 1);

    if (err == 0)
    {
        /* Reset the PID and sequence number of the last operation */
        rt->pid = 0;
        rt->seq = 0;

        rtm_reply->rtm_index = (rt->netif ? rt->netif->ipcom.ifindex : 0);
        ipcom_memcpy(&rtm_reply->rtm_rmx, &rt->metrics, sizeof(struct Ipnet_rt_metrics));
        if (rtm_reply->rtm_rmx.rmx_expire == IPCOM_ADDR_INFINITE)
            rtm_reply->rtm_rmx.rmx_expire = 0;
        else
            rtm_reply->rtm_rmx.rmx_expire = rtm_reply->rtm_rmx.rmx_expire - IPNET_SECONDS_ABS;
        rtm_reply->rtm_inits = ~0u;

        rtm_reply->rtm_flags  = rt->hdr.flags & IPNET_RTF_PUBLIC_FLAGS;
        rtm_reply->rtm_use    = rt->use;

        for (i = 0; i < IPNET_RTAX_MAX; i++)
        {
            if (IP_BIT_ISFALSE(addrs, 1 << i))
                continue;

            addr = (struct Ip_sockaddr *) &rtm_addr_buf[0];
            switch (i)
            {
            case IPNET_RTAX_DST:
                ipnet_route_key_to_sockaddr(domain, rt->hdr.key, addr);
                break;
            case IPNET_RTAX_GATEWAY:
                addr = rt->gateway;
                break;
            case IPNET_RTAX_NETMASK:
                ipnet_route_key_to_sockaddr(domain, rt->hdr.mask, addr);
                break;
            case IPNET_RTAX_IFP:
                (void)ipnet_if_to_sockaddr_dl(rt->netif, (struct Ip_sockaddr_dl *) addr);
                break;
            case IPNET_RTAX_IFA:
#ifdef IPCOM_USE_INET
                if (domain == IP_AF_INET)
                {
                    struct Ipnet_ipv4_key      *ipv4_dst = (struct Ipnet_ipv4_key *) rt->hdr.key;
                    IP_CONST struct Ip_in_addr *in;

                    in = ipnet_ip4_get_src_addr(vr, &ipv4_dst->addr, rt, IP_NULL);
                    if (in == IP_NULL)
                    {
                        size -= sizeof(struct Ip_sockaddr_in);
                        IP_BIT_CLR(addrs, 1 << i);
                        continue;
                    }
                    ipnet_route_key_to_sockaddr(IP_AF_INET, in, addr);
                }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
                if (domain == IP_AF_INET6)
                {
                    struct Ipnet_ipv6_key       *ipv6_dst = (struct Ipnet_ipv6_key *) rt->hdr.key;
                    struct Ipnet_ipv6_key        ipv6_ifa;
                    IP_CONST struct Ip_in6_addr *in6;

                    in6 = ipnet_ip6_get_src_addr(vr,
                                                 &ipv6_dst->addr,
                                                 rt->netif->ipcom.ifindex,
                                                 rt,
                                                 IP_NULL,
                                                 IP_NULL,
                                                 0);

                    if (in6 == IP_NULL)
                    {
                        size -= sizeof(struct Ip_sockaddr_in6);
                        IP_BIT_CLR(addrs, 1 << i);
                        continue;
                    }
                    IPNET_IP6_SET_ADDR(&ipv6_ifa.addr, in6);
                    if (ipnet_ip6_is_scoped_addr(in6))
                        ipv6_ifa.scope_id = ipnet_ip6_get_zone(rt->netif->ipcom.ifindex,
                                                               in6);
                    else
                        ipv6_ifa.scope_id = 0;
                    ipnet_route_key_to_sockaddr(IP_AF_INET6, &ipv6_ifa, addr);
                }
#endif /* IPCOM_USE_INET6 */
                break;
            default:
                IP_PANIC();
                return 0;
            }
            ipcom_memcpy(rtm_addr, addr, IPCOM_SA_LEN_GET(addr));
            rtm_addr = (struct Ip_sockaddr *) ((Ip_u8 *) rtm_addr + IPCOM_SA_LEN_GET(addr));
        }
    }
    rtm_reply->rtm_addrs = addrs;
    rtm_reply->rtm_msglen = size;
    pkt->end = pkt->start + (int) size;
    (void)ipnet_routesock_output(pkt);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_routesock_output_get
 *===========================================================================
 * Description: Respond on a IPNET_RTM_GET request.
 * Parameters:  rtm - The user message requesting the IPNET_RTM_GET.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_output_get(Ip_u16 vr, struct Ipnet_rt_msghdr *rtm)
{
    Ip_u16             size;
    int                err;
    int                addrs;
    int                domain;
    Ipnet_route_entry *re;

    err = ipnet_routesock_get_rt(vr, rtm, &re, &domain, IP_TRUE);
    if (err < 0)
    {
        addrs = 0;
        size = sizeof(struct Ipnet_rt_msghdr);
    }
    else
    {
        err = 0;
        ipnet_routesock_create_addrs(domain, re, &size, &addrs);
    }
    return ipnet_routesock_output_rt(IPNET_RTM_GET,
                                     rtm->rtm_pid,
                                     rtm->rtm_seq,
                                     vr,
                                     rtm->rtm_table,
                                     re, size, addrs,
                                     domain, err);
}


/*
 *===========================================================================
 *                    ipnet_routesock_output_err_reply
 *===========================================================================
 * Description: Creates and sends a reply to a request that is a copy of
 *              the request with possibly a changed rtm_errno value.
 * Parameters:  vr - The virtual router that can see this packet.
 *              rtm - The message to respond to.
 *              err - Value to place in rtm_errno.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_output_err_reply(Ip_u16 vr, struct Ipnet_rt_msghdr *rtm, int err)
{
    int                     retval;
    struct Ipnet_rt_msghdr *rtm_reply;
    Ipcom_pkt              *pkt;

    pkt = ipcom_pkt_malloc(rtm->rtm_msglen + 3,
                           IP_FLAG_FC_BLOCKOK | IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return -IP_ERRNO_ENOBUFS;
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RTSOCK);
    IPNET_ROUTESOCK_INIT_PKT(pkt, rtm->rtm_msglen);
    pkt->vr_index = vr;
    rtm_reply = (struct Ipnet_rt_msghdr *) &pkt->data[pkt->start];

    ipcom_memcpy(rtm_reply, rtm, rtm->rtm_msglen);
    if (rtm_reply->rtm_pid == 0)
        rtm_reply->rtm_pid = ipnet_get_effective_pid();
    rtm_reply->rtm_errno = err;

    retval = ipnet_routesock_output(pkt);
    if(retval < 0)
        return retval;
    return -err;
}


/*
 *===========================================================================
 *                    ipnet_routesock_input_add_rt
 *===========================================================================
 * Description: Act on a IPNET_RTM_ADD request.
 * Parameters:  rtm - The user message requesting the IPNET_RTM_ADD.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_input_add_rt(Ip_u16 vr, struct Ipnet_rt_msghdr *rtm)
{
    struct Ipnet_route_add_param param;
    struct Ip_sockaddr          *addr[IPNET_RTAX_MAX];
    int                          ret;
#ifdef IPCOM_USE_INET
    struct Ip_in_addr            mask;
#endif
#ifdef IPCOM_USE_INET6
    struct Ipnet_ipv6_key        dst6;
    struct Ipnet_ipv6_key        mask6;
#endif /* IPCOM_USE_INET6 */

    ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
    if (ipnet_routesock_get_addrs_from_rt_msg(rtm, addr) < 0)
        /* Invalid message */
        return -IP_ERRNO_EINVAL;

    if (addr[IPNET_RTAX_DST] == IP_NULL)
        /* No destination */
        return ipnet_routesock_output_err_reply(vr, rtm, IP_ERRNO_EDESTADDRREQ);

    if (addr[IPNET_RTAX_NETMASK] != IP_NULL &&
        addr[IPNET_RTAX_DST]->sa_family != addr[IPNET_RTAX_NETMASK]->sa_family)
        /* The address family of the mask does not match the destination */
        return ipnet_routesock_output_err_reply(vr, rtm, IP_ERRNO_EINVAL);

    param.netif   = ipnet_if_indextonetif((Ip_u16)(ipnet_has_priv(IPNET_PRIV_CHVR)
                                           ? IPCOM_VR_ANY
                                           : vr),
                                          rtm->rtm_index);
    param.flags   = rtm->rtm_flags;
    param.pid     = rtm->rtm_pid;
    param.seq     = rtm->rtm_seq;
    param.metrics = &rtm->rtm_rmx;
    if (rtm->rtm_rmx.rmx_expire == 0)
        rtm->rtm_rmx.rmx_expire = IPCOM_ADDR_INFINITE;

    if (IP_BIT_ISSET(param.flags, IPNET_RTF_GATEWAY | IPNET_RTF_TUNNELEND))
    {
        if (addr[IPNET_RTAX_GATEWAY] == IP_NULL)
            return ipnet_routesock_output_err_reply(vr, rtm, IP_ERRNO_EINVAL);
    }
    param.gateway = addr[IPNET_RTAX_GATEWAY];

#ifdef IPCOM_USE_INET
    if (addr[IPNET_RTAX_DST]->sa_family == IP_AF_INET)
    {
        param.key = &((struct Ip_sockaddr_in *) addr[IPNET_RTAX_DST])->sin_addr;
        if (addr[IPNET_RTAX_NETMASK] != IP_NULL)
        {
            struct Ip_sockaddr_in *sin_mask = (struct Ip_sockaddr_in *) addr[IPNET_RTAX_NETMASK];

            ipcom_memset(&mask, 0, sizeof(mask));
            if (IPCOM_SA_LEN_GET(sin_mask)
                > (unsigned) ip_offsetof(struct Ip_sockaddr_in, sin_addr))
                ipcom_memcpy(&mask,
                             &sin_mask->sin_addr,
                             IP_MIN(sizeof(struct Ip_in_addr),
                                    IPCOM_SA_LEN_GET(sin_mask)
                                    - (unsigned) ip_offsetof(struct Ip_sockaddr_in, sin_addr)));
            param.netmask = &mask;
        }
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (addr[IPNET_RTAX_DST]->sa_family == IP_AF_INET6)
    {
        ipnet_route_sockaddr_to_key(IP_AF_INET6, addr[IPNET_RTAX_DST], &dst6);
        param.key = &dst6;
        if (addr[IPNET_RTAX_NETMASK] != IP_NULL)
        {
            ipnet_route_sockaddr_to_key(IP_AF_INET6, addr[IPNET_RTAX_NETMASK], &mask6);
            param.netmask = &mask6;
        }
    }
#endif /* IPCOM_USE_INET6 */

    param.domain       = addr[IPNET_RTAX_DST]->sa_family;
    param.vr           = vr;
    param.table        = rtm->rtm_table;
    param.no_ref_count = IP_TRUE;
    ret = ipnet_route_add(&param);
    if (ret < 0)
        return ipnet_routesock_output_err_reply(vr, rtm, -ret);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_routesock_input_delete_rt
 *===========================================================================
 * Description: Act on a IPNET_RTM_DELETE request.
 * Parameters:  rtm - The user message requesting the IPNET_RTM_DELETE.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_input_delete_rt(Ip_u16 vr, struct Ipnet_rt_msghdr *rtm)
{
    struct Ip_sockaddr *addr[IPNET_RTAX_MAX];
    Ipnet_route_entry  *re;
    int                 domain;
    int                 ret;
    IP_CONST void      *mask;

    ret = ipnet_routesock_get_rt(vr, rtm, &re, &domain, IP_TRUE);
    if (ret < 0)
        return ipnet_routesock_output_err_reply(vr, rtm, -ret);
    if (ret != IPNET_ROUTE_PERFECT_MATCH)
        return ipnet_routesock_output_err_reply(vr, rtm, IP_ERRNO_ESRCH);

    if (ipnet_routesock_get_addrs_from_rt_msg(rtm, addr) < 0)
        /* Invalid message */
        return -IP_ERRNO_EINVAL;

    mask = IP_NULL;
    if (addr[IPNET_RTAX_NETMASK] != IP_NULL)
    {
#ifdef IPCOM_USE_INET
        if (domain == IP_AF_INET)
            mask = &((struct Ip_sockaddr_in *) addr[IPNET_RTAX_NETMASK])->sin_addr;
        else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if (domain == IP_AF_INET6)
            mask = &((struct Ip_sockaddr_in6 *) addr[IPNET_RTAX_NETMASK])->sin6_addr;
        else
#endif /* IPCOM_USE_INET6 */
        {
            IP_PANIC();
        }
    }
    ret = ipnet_route_delete2(domain,
                              vr,
                              rtm->rtm_table,
                              re->hdr.key,
                              mask,
                              addr[IPNET_RTAX_GATEWAY],
                              rtm->rtm_index,
                              rtm->rtm_pid,
                              rtm->rtm_seq,
                              IP_FALSE);

    if (ret != 0)
        return ipnet_routesock_output_err_reply(vr, rtm, IP_ERRNO_ESRCH);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_routesock_input_change_rt
 *===========================================================================
 * Description: Act on a IPNET_RTM_CHANGE request (change metrics, gateway or flags).
 * Parameters:  rtm - The user message requesting the IPNET_RTM_CHANGE.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_input_change_rt(Ip_u16 vr, struct Ipnet_rt_msghdr *rtm)
{
    Ipnet_route_entry  *rt;
    int                 domain;
    int                 ret;
    struct Ip_sockaddr *addr[IPNET_RTAX_MAX];
    Ip_s32              rtm_inits;

    if (ipnet_routesock_get_addrs_from_rt_msg(rtm, addr) < 0)
        /* Invalid message */
        return -IP_ERRNO_EINVAL;

    ret = ipnet_routesock_get_rt(vr, rtm, &rt, &domain, IP_TRUE);
    if (ret != IPNET_ROUTE_PERFECT_MATCH)
    {
        if (ret >= 0)
            ret = -IP_ERRNO_ESRCH;
        return ipnet_routesock_output_err_reply(vr, rtm, -ret);
    }

    /* Change metric if flag is set */
    rtm_inits = IP_GET_32ON16(&rtm->rtm_inits);
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_HOPCOUNT))
        rt->metrics.rmx_hopcount = rtm->rtm_rmx.rmx_hopcount;
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_RTT))
        rt->metrics.rmx_rtt = rtm->rtm_rmx.rmx_rtt;
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_RTTVAR))
        rt->metrics.rmx_rttvar = rtm->rtm_rmx.rmx_rttvar;
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_MTU))
        rt->metrics.rmx_mtu = rtm->rtm_rmx.rmx_mtu;
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_EXPIRE))
    {
        if (rtm->rtm_rmx.rmx_expire == 0)
            rtm->rtm_rmx.rmx_expire = IPCOM_ADDR_INFINITE;
        ipnet_route_set_lifetime(rt, rtm->rtm_rmx.rmx_expire);
    }
#ifdef IP_PORT_OSE5
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_PROTO))
        rt->metrics.rmx_proto = rtm->rtm_rmx.rmx_proto;
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_USR1))
        rt->metrics.rmx_usr1 = rtm->rtm_rmx.rmx_usr1;
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_USR2))
        rt->metrics.rmx_usr2 = rtm->rtm_rmx.rmx_usr2;
    if (IP_BIT_ISSET(rtm_inits, IPNET_RTV_USR3))
        rt->metrics.rmx_usr3 = rtm->rtm_rmx.rmx_usr3;
#endif /* IP_PORT_OSE5 */

    if (ipnet_if_indextonetif(vr, rtm->rtm_index) != IP_NULL)
        rt->netif = ipnet_if_indextonetif(vr, rtm->rtm_index);

    rt->hdr.flags = (rtm->rtm_flags & IPNET_RTF_PUBLIC_FLAGS)
        | (rt->hdr.flags & ~IPNET_RTF_PUBLIC_FLAGS);

    ipnet_route_has_changed(rt);

    rt->pid = rtm->rtm_pid;
    rt->seq = rtm->rtm_seq;

    IPNET_ROUTESOCK(ipnet_routesock_rt_change(rt));
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_routesock_input_lock_rt
 *===========================================================================
 * Description: Act on a IPNET_RTM_LOCK request.
 * Parameters:  rtm - The user message requesting the IPNET_RTM_LOCK.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_input_lock_rt(struct Ipnet_rt_msghdr *rtm)
{
    IPCOM_UNUSED_ARG(rtm);

    /* NOT IMPLEMENTED */
    IP_PANIC2();
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_routesock_input_new_vr
 *===========================================================================
 * Description: Adds a new route table.
 * Parameters:  rtm - The user message requesting the IPNET_RTM_NEWTAB.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_input_new_vr(Ip_u16 vr, struct Ipnet_rt_msghdr *rtm)
{
    int err;

    err = ipnet_route_add_table(vr, rtm->rtm_table, IP_NULL, IP_NULL);
    return ipnet_routesock_output_err_reply(vr, rtm, err >= 0 ? 0 : -err);
}


/*
 *===========================================================================
 *                    ipnet_routesock_input_delete_vr
 *===========================================================================
 * Description: Adds a new route table.
 * Parameters:  rtm - The user message requesting the IPNET_RTM_DELTAB.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_input_delete_vr(Ip_u16 vr, struct Ipnet_rt_msghdr *rtm)
{
    int err;

    if (IPCOM_ROUTE_TABLE_IS_DEFAULT(rtm->rtm_table))
        err = ipnet_route_delete_vr(vr);
    else
        err = ipnet_route_delete_table(vr, rtm->rtm_table);

    return ipnet_routesock_output_err_reply(vr, rtm, -err);

}


/*
 *===========================================================================
 *                    ipnet_routesock_input
 *===========================================================================
 * Description: Incoming route socket request.
 * Parameters:  pkt - the data sent to the socket.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_routesock_input(Ipcom_pkt *pkt)
{
    struct Ipnet_rs_msghdr *msghdr;
    int                     ret;

    ip_assert(((Ip_ptrdiff_t) &pkt->data[pkt->start] & 0x3) == 0);

    if (pkt->end - pkt->start < (int) sizeof(struct Ipnet_rs_msghdr))
    {
        ret = -IP_ERRNO_EINVAL;
        goto cleanup;
    }

    msghdr = (struct Ipnet_rs_msghdr *) &pkt->data[pkt->start];
    if (msghdr->version != IPNET_RTM_VERSION)
    {
        ret = -IP_ERRNO_EPROTONOSUPPORT;
        goto cleanup;
    }

    if (pkt->end - pkt->start < msghdr->msglen)
    {
        ret = -IP_ERRNO_EINVAL;
        goto cleanup;
    }

    switch (msghdr->type)
    {
    case IPNET_RTM_ADD:
        ret = ipnet_routesock_input_add_rt(pkt->vr_index, (struct Ipnet_rt_msghdr *) msghdr);
        break;
    case IPNET_RTM_DELETE:
        ret = ipnet_routesock_input_delete_rt(pkt->vr_index, (struct Ipnet_rt_msghdr *) msghdr);
        break;
    case IPNET_RTM_GET:
        ret = ipnet_routesock_output_get(pkt->vr_index, (struct Ipnet_rt_msghdr *) msghdr);
        break;
    case IPNET_RTM_CHANGE:
        ret = ipnet_routesock_input_change_rt(pkt->vr_index, (struct Ipnet_rt_msghdr *) msghdr);
        break;
    case IPNET_RTM_LOCK:
        ret = ipnet_routesock_input_lock_rt((struct Ipnet_rt_msghdr *) msghdr);
        break;
    case IPNET_RTM_NEWVR:
        ret = ipnet_routesock_input_new_vr(pkt->vr_index, (struct Ipnet_rt_msghdr *) msghdr);
        break;
    case IPNET_RTM_DELVR:
        ret = ipnet_routesock_input_delete_vr(pkt->vr_index, (struct Ipnet_rt_msghdr *) msghdr);
        break;
    default:
        ret = -IP_ERRNO_EINVAL;
        break;
    }
cleanup:
    ipcom_pkt_free(pkt);
    return IP_MIN(ret, 0);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_routesock_addr_add
 *===========================================================================
 * Description: A new address was added, create a IPNET_RTM_NEWADDR.
 * Parameters:  netif - The interface on which the address was added.
 *              domain - The domain of the new address.
 *              addr - The address entry.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_addr_add(Ipnet_netif *netif, int domain, void *addr)
{
    if (ipnet->num_route_sock == 0)
        return;
    ipnet_routesock_output_addr(IPNET_RTM_NEWADDR, netif, domain, addr);
}


/*
 *===========================================================================
 *                    ipnet_routesock_addr_delete
 *===========================================================================
 * Description: A address was deleted, create a IPLTIE_RTM_DELADDR.
 * Parameters:  netif - The interface on which the address was deleted.
 *              domain - The domain of the deleted address.
 *              addr - The address entry.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_addr_delete(Ipnet_netif *netif, int domain, void *addr)
{
    if (ipnet->num_route_sock == 0)
        return;
    ipnet_routesock_output_addr(IPNET_RTM_DELADDR, netif, domain, addr);
}


/*
 *===========================================================================
 *                    ipnet_routesock_if_change
 *===========================================================================
 * Description: Status of a network interface has changed, create a IPLTIE_RTM_IFINFO.
 * Parameters:  netif - The interface.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_if_change(Ipnet_netif *netif)
{
    Ipcom_pkt *pkt;
    Ip_size_t  size;

    if (ipnet->num_route_sock == 0)
        return;

    size = ipnet_sysctl_create_ifinfo(IP_NULL, netif);

    pkt = ipcom_pkt_malloc(size, IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;

    /**/
    pkt->vr_index = netif->vr_index;

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RTSOCK);
    IPNET_ROUTESOCK_INIT_PKT(pkt, size);

    (void)ipnet_sysctl_create_ifinfo(pkt->data, netif);

    pkt->vr_index = netif->vr_index;
    (void)ipnet_routesock_output(pkt);
}


/*
 *===========================================================================
 *                    ipnet_routesock_if_announce
 *===========================================================================
 * Description: Announce arrival/departure of a network interface by creating
 *              and sending a IPNET_RTM_IFANNOUNCE message.
 * Parameters:  netif - The attached/deatached interface.
 *
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_if_announce(Ipnet_netif *netif, Ip_u16 what)
{
    Ipcom_pkt *pkt;
    struct Ipnet_if_announcemsghdr  *ifan;

    if (ipnet->num_route_sock == 0)
        return;

    pkt = ipcom_pkt_malloc(sizeof(struct Ipnet_if_announcemsghdr), IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;

    pkt->vr_index = netif->vr_index;

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RTSOCK);
    IPNET_ROUTESOCK_INIT_PKT(pkt, sizeof(struct Ipnet_if_announcemsghdr));

    ifan = (struct Ipnet_if_announcemsghdr *)&pkt->data[pkt->start];
    ifan->ifan_msglen = sizeof(struct Ipnet_if_announcemsghdr);
    ifan->ifan_version = IPNET_RTM_VERSION;
    ifan->ifan_type = IPNET_RTM_IFANNOUNCE;
    ifan->ifan_index = netif->ipcom.ifindex;
    ipcom_strcpy(ifan->ifan_name, netif->ipcom.name);
    ifan->ifan_what = what;

    (void)ipnet_routesock_output(pkt);
}


/*
 *===========================================================================
 *                    ipnet_routesock_rt_add
 *===========================================================================
 * Description: A new route was added, create IPNET_RTM_ADD message.
 * Parameters:  rt - The added route entry
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_rt_add(Ipnet_route_entry *rt)
{
    Ip_u16 size;
    int    addrs;

    if (ipnet->num_route_sock == 0 || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE))
        return;
#ifndef IPNET_USE_ROUTESOCK_LINKINFO
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO) && rt->pid == 0)
        return;
#endif

    ipnet_routesock_create_addrs(IPNET_ROUTE_GET_FAMILY(rt->head), rt, &size, &addrs);
    (void)ipnet_routesock_output_rt(IPNET_RTM_ADD, rt->pid, rt->seq,
                                    IPNET_ROUTE_GET_VR(rt->head),
                                    IPNET_ROUTE_GET_TABLE(rt->head),
                                    rt, size, addrs, IPNET_ROUTE_GET_FAMILY(rt->head), 0);
}


/*
 *===========================================================================
 *                    ipnet_routesock_rt_delete
 *===========================================================================
 * Description: A route was deleted, create IPNET_RTM_DELETE message.
 * Parameters:  rt - The removed route entry
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_rt_delete(Ipnet_route_entry *rt)
{
    Ip_u16 size;
    int    addrs;

    if (ipnet->num_route_sock == 0 || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE))
        return;
#ifndef IPNET_USE_ROUTESOCK_LINKINFO
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO) && rt->pid == 0)
        return;
#endif

    ipnet_routesock_create_addrs(IPNET_ROUTE_GET_FAMILY(rt->head), rt, &size, &addrs);
    (void)ipnet_routesock_output_rt(IPNET_RTM_DELETE, rt->pid, rt->seq,
                                    IPNET_ROUTE_GET_VR(rt->head),
                                    IPNET_ROUTE_GET_TABLE(rt->head),
                                    rt, size, addrs, IPNET_ROUTE_GET_FAMILY(rt->head), 0);
}


/*
 *===========================================================================
 *                    ipnet_routesock_rt_change
 *===========================================================================
 * Description: Notifies all IP_AF_ROUTE socket listener about the change
 *              in the route parameters.
 * Parameters:  rt - The route entry that has been changed.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_rt_change(Ipnet_route_entry *rt)
{
    Ip_u16 size;
    int    addrs;

    if (ipnet->num_route_sock == 0 || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE))
        return;
    ipnet_routesock_create_addrs(IPNET_ROUTE_GET_FAMILY(rt->head), rt, &size, &addrs);
    (void)ipnet_routesock_output_rt(IPNET_RTM_CHANGE, rt->pid, rt->seq,
                                    IPNET_ROUTE_GET_VR(rt->head),
                                    IPNET_ROUTE_GET_TABLE(rt->head),
                                    rt, size, addrs, IPNET_ROUTE_GET_FAMILY(rt->head), 0);
}


/*
 *===========================================================================
 *                    ipnet_routesock_rt_miss
 *===========================================================================
 * Description: A route lookup failed, create IPNET_RTM_MISS message.
 * Parameters:  rtab - The route table used during the lookup.
 *              key - The search key that did not match anything.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_rt_miss(Ipcom_route *rtab, IP_CONST void *key)
{
    Ipcom_pkt              *pkt;
    Ip_size_t               size;
    struct Ipnet_rt_msghdr *rtm;
    int                     domain;

    domain = IPNET_ROUTE_GET_FAMILY(rtab);
    size = sizeof(struct Ipnet_rt_msghdr);

    if (domain == IP_AF_INET)
        size += sizeof(struct Ip_sockaddr_in);
    else
        size += sizeof(struct Ip_sockaddr_in6);

    pkt = ipcom_pkt_malloc(size, IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;
    pkt->vr_index = IPNET_ROUTE_GET_VR(rtab);
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RTSOCK);
    IPNET_ROUTESOCK_INIT_PKT(pkt, size);
    rtm = (struct Ipnet_rt_msghdr *) pkt->data;

    ipcom_memset(rtm, 0, size);
    rtm->rtm_msglen  = (Ip_u16) size;
    rtm->rtm_version = IPNET_RTM_VERSION;
    rtm->rtm_type    = IPNET_RTM_MISS;
    rtm->rtm_addrs   = IPNET_RTA_DST;

    ipnet_route_key_to_sockaddr(domain, key, (struct Ip_sockaddr *) (rtm + 1));

    (void)ipnet_routesock_output(pkt);
}


/*
 *===========================================================================
 *                    ipnet_routesock_rt_redirect
 *===========================================================================
 * Description: The stack was told to use a different route.
 * Parameters:  rtab - The route table to use.
 *              ifindex - The interface the redirect message was received on.
 *              dst - The final destination.
 *              new_gw - A better first hop gateway.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_rt_redirect(Ipcom_route *rtab, Ip_u32 ifindex,
                            IP_CONST void *dst, IP_CONST void *new_gw)
{
    Ipcom_pkt              *pkt;
    Ip_size_t               size;
    struct Ipnet_rt_msghdr *rtm;
    int                     domain;

    if (ipnet->num_route_sock == 0)
        return;

    domain = IPNET_ROUTE_GET_FAMILY(rtab);
    size = sizeof(struct Ipnet_rt_msghdr);

    if (domain == IP_AF_INET)
        size += sizeof(struct Ip_sockaddr_in) * 2;
    else
        size += sizeof(struct Ip_sockaddr_in6) * 2;

    pkt = ipcom_pkt_malloc(size, IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;
    pkt->vr_index = IPNET_ROUTE_GET_VR(rtab);
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RTSOCK);
    IPNET_ROUTESOCK_INIT_PKT(pkt, size);
    rtm = (struct Ipnet_rt_msghdr *) pkt->data;

    ipcom_memset(rtm, 0, size);
    rtm->rtm_msglen  = (Ip_u16) size;
    rtm->rtm_version = IPNET_RTM_VERSION;
    rtm->rtm_type    = IPNET_RTM_REDIRECT;
    rtm->rtm_addrs   = IPNET_RTA_DST | IPNET_RTA_GATEWAY;

    if (domain == IP_AF_INET)
    {
        struct Ip_sockaddr_in *inet;
        struct Ip_sockaddr_in *gw;

        inet = (struct Ip_sockaddr_in *) (rtm + 1);
        inet->sin_family = IP_AF_INET;
        IPCOM_SA_LEN_SET(inet, sizeof(struct Ip_sockaddr_in));
        ipcom_memcpy(&inet->sin_addr, dst, sizeof(inet->sin_addr));

        gw = inet + 1;
        gw->sin_family = IP_AF_INET;
        IPCOM_SA_LEN_SET(gw, sizeof(struct Ip_sockaddr_in));
        ipcom_memcpy(&gw->sin_addr, new_gw, sizeof(gw->sin_addr));
    }
    else
    {
        struct Ip_sockaddr_in6 *inet6;
        struct Ip_sockaddr_in6 *gw;

        inet6 = (struct Ip_sockaddr_in6 *) (rtm + 1);
        inet6->sin6_family = IP_AF_INET6;
        IPCOM_SA_LEN_SET(inet6, sizeof(struct Ip_sockaddr_in6));
        ipcom_memcpy(&inet6->sin6_addr, dst, sizeof(inet6->sin6_addr));
        if (IP_IN6_IS_ADDR_LINK_LOCAL(&inet6->sin6_addr))
            inet6->sin6_scope_id = ifindex;

        gw = inet6 + 1;
        gw->sin6_family = IP_AF_INET6;
        IPCOM_SA_LEN_SET(gw, sizeof(struct Ip_sockaddr_in6));
        ipcom_memcpy(&gw->sin6_addr, new_gw, sizeof(gw->sin6_addr));
        if (IP_IN6_IS_ADDR_LINK_LOCAL(&gw->sin6_addr))
            gw->sin6_scope_id = ifindex;
    }

    (void)ipnet_routesock_output(pkt);
}


/*
 *===========================================================================
 *                    ipnet_routesock_rt_resolve
 *===========================================================================
 * Description: Kernel failed to resolve this address to a link-layer address,
 *              request any route socket listener to create the mapping.
 * Parameters:  rt - The address for which no link layer address was found.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_routesock_rt_resolve(Ipnet_route_entry *rt)
{
    Ip_u16              size;
    int                 addrs;
    struct Ip_sockaddr *gw;

    if (ipnet->num_route_sock == 0 || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE))
        return;
    /* No not include the gateway in RTM_RESOLVE messages
       (is just a unresolved link-layer address)*/
    gw = rt->gateway;
    rt->gateway = IP_NULL;
    ipnet_routesock_create_addrs(IPNET_ROUTE_GET_FAMILY(rt->head), rt, &size, &addrs);
    rt->gateway = gw;
    (void)ipnet_routesock_output_rt(IPNET_RTM_RESOLVE, 0, 0,
                                    IPNET_ROUTE_GET_VR(rt->head),
                                    IPNET_ROUTE_GET_TABLE(rt->head),
                                    rt, size, addrs, IPNET_ROUTE_GET_FAMILY(rt->head), 0);
}


/*
 *===========================================================================
 *                    ipnet_sock_route_register
 *===========================================================================
 * Description: Registers the IP_AF_ROUTE domain.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_route_register(void)
{
    Ipnet_sock_ops *ops;

    ops = ipcom_calloc(1, sizeof(*ops));
    if (ops == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "Could not register the IP_AF_ROUTE domain, out of memory");
        IP_PANIC();
        return -IP_ERRNO_ENOMEM;
    }

    ipnet_usr_sock_get_ops(ops);
    ops->domain     = IP_AF_ROUTE;
    ops->type       = IP_SOCK_RAW;
    ops->destroy    = ipnet_sock_route_destroy;
    ops->init       = ipnet_sock_route_init;
    ops->lookup     = ipnet_sock_route_lookup;
    ops->send       = ipnet_sock_route_send;

    return ipnet_sock_register_ops(ops);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* IPNET_USE_ROUTESOCK */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

