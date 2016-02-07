/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_netlink.c,v $ $Revision: 1.84.18.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_netlink.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation Jonas Rendel <Jonas.Rendel@interpeak.se>
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


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */
#define IPCOM_USE_CLIB_PROTO
#include <ipcom.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>
#include <ipcom_sock2.h>
#include <ipcom_route.h>
#include <ipcom_errno.h>

#include "ipnet_h.h"
#include "ipnet_ctrl.h"
#include "ipnet_route.h"
#include "ipnet_netlink.h"
#include "ipnet_netlink_h.h"
#include "ipcom_sock.h"
#include "ipnet_eth.h"
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#include "ipcom_sock6.h"
#endif

#include "ipnet_usr_sock.h"

#ifdef IPNET_USE_NETLINKSOCK
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
 *                    ipnet_netlink_sock_lookup
 *===========================================================================
 * Description: Returns the route socket matching the parametres. Source is
 *              always this host and the destination is the peer.
 * Parameters:  rtab_index - The route table index the socket must be bound to.
 *              version - The protocol version for the routing packet.
 * Returns:     A pointer to the matching socket or IP_NULL if no socket match.
 *
 */
IP_STATIC Ipnet_socket *
ipnet_netlink_sock_lookup(Ip_u16 vr, int proto,
                          IP_CONST void *unused1, Ip_u16 unused2,
                          IP_CONST void *unused3, Ip_u16 unused4)
{
    Ipnet_socket_lookup_key key;

    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;

    ipcom_memset(&key, 0, sizeof(key));
    key.domain = IP_AF_NETLINK;
    key.proto  = (Ip_u16) proto;
    key.vr     = (Ip_u16) vr;
    return ipcom_hash_get(ipnet->reachable_sockets, &key);
}


/*
 *===========================================================================
 *                    ipnet_netlink_sock_init
 *===========================================================================
 * Description: The netlink domain socket initialization.
 * Parameters:  sock - The socket that is beeing created.
 *              user_context - not used.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_netlink_sock_init(Ipnet_socket *sock, Ip_bool user_context)
{
    (void) user_context;

    sock->nl = ipnet_sock_alloc_private_data(sock, sizeof(Ipnet_netlink_socket));
    ipnet->num_netlink_sock++;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_netlink_destroy
 *===========================================================================
 * Description: Destroys the netlink socket.
 * Parameters:  sock - The socket beeing closed.
 * Returns:     0 = success
 *
 */
IP_STATIC void
ipnet_netlink_sock_destroy(Ipnet_socket *sock)
{
    (void) sock;
    ipnet->num_netlink_sock--;
    ipnet_sock_unbind(sock);
}



/*
 *===========================================================================
 *                    ip_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipnet_socket *
ipnet_netlink_sock_lookup_pid(Ip_u16 vr, int proto, Ip_pid_t pid)
{
    Ipnet_socket          *nl_sock, *nl_start_sock;

    /* Grab the first socket in the proto family */
    nl_sock = nl_start_sock = ipnet_netlink_sock_lookup(vr,
                                                        proto,
                                                        IP_NULL,
                                                        0,
                                                        IP_NULL,
                                                        0);
    /* Verify uniqueness */
    while (nl_sock != IP_NULL)
    {
        /* Does this socket match the PID we're currently trying to get? */
        if (nl_sock->nl->nl_src_pid == pid)
            return nl_sock;

        /* Check next socket */
        nl_sock = nl_sock->next;
    }

    return IP_NULL;
}

/*
 *===========================================================================
 *                    ipnet_netlink_sock_bind
 *===========================================================================
 * Description: Binds a netlink socket.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_netlink_sock_bind(struct Ipnet_socket_struct *sock,
                        IP_CONST struct Ip_sockaddr *name,
                        Ip_socklen_t namelen,
                        Ip_bool user_context)
{
    struct Ip_sockaddr_nl *name_nl;
    Ip_pid_t              mypid;

    IPCOM_UNUSED_ARG(user_context);

    (void)user_context;

    if (namelen < sizeof(struct Ip_sockaddr_nl))
        return -IP_ERRNO_EINVAL;

    name_nl = (struct Ip_sockaddr_nl *)name;

    mypid = name_nl->nl_pid? name_nl->nl_pid : ipnet_get_effective_pid();
    while (ipnet_netlink_sock_lookup_pid(sock->vr_index, sock->proto, mypid))
    {
        if (name_nl->nl_pid != 0)
            return -IP_ERRNO_EADDRINUSE;
        else
        {
            /* First clash; shift it a bit */
            if (mypid == ipnet_get_effective_pid())
                mypid = mypid >> ((sizeof(Ip_pid_t) - 1) << 3) | mypid << 8;
            else
                mypid++;
        }
    }

    /* */
    sock->nl->nl_src_mc_groups  = name_nl->nl_groups;
    sock->nl->nl_src_pid        = mypid;

    /* Go reachable */
    ipnet_sock_set_reachable(sock);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_netlink_sock_getname
 *===========================================================================
 * Description: Returns the local name (or address) of the socket.
 * Parameters:  sock - The socket to return the name for.
 *              name - Buffer where the name will be stored.
 *              namelen - The length of the 'name' buffer.
 *              is_local_name - Is set to IP_TRUE of the local name of the
 *              socket should be returned, IP_FALSE means that the peer name
 *              should be returned.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_netlink_sock_getname(struct Ipnet_socket_struct *sock,
                           struct Ip_sockaddr *name,
                           Ip_socklen_t  *namelen,
                           Ip_bool is_local_name)
{
    struct Ip_sockaddr_nl *name_nl;

    if (*namelen < sizeof(struct Ip_sockaddr_nl))
        return -IP_ERRNO_ENOBUFS;

    if (is_local_name == IP_FALSE)
        return -IP_ERRNO_EPROTONOSUPPORT;

    *namelen = sizeof(struct Ip_sockaddr_nl);
    name_nl = (struct Ip_sockaddr_nl *)name;
    ipcom_memset(name_nl, 0, sizeof(struct Ip_sockaddr_nl));
    IPCOM_SA_LEN_SET(name_nl, sizeof(struct Ip_sockaddr_nl));
    name_nl->nl_family = IP_AF_NETLINK;
    name_nl->nl_groups = sock->nl->nl_src_mc_groups;
    name_nl->nl_pid    = sock->nl->nl_src_pid;
    return 0;
}

/*
 *===========================================================================
 *                    ipnet_netlink_sock_connect
 *===========================================================================
 * Description: The UDP transport layer connect.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_netlink_sock_connect(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    struct Ip_sockaddr_nl   *nl = (struct Ip_sockaddr_nl *)to;

    if (tolen < sizeof(struct Ip_sockaddr_nl))
        return -IP_ERRNO_EINVAL;

    if (nl->nl_groups == 0 && nl->nl_pid == 0)
    {
        IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);
    }
    else
    {
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);
    }

    sock->nl->nl_dst_pid        = nl->nl_pid;
    sock->nl->nl_dst_mc_groups  = nl->nl_groups;

    if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_BOUND))
    {
        struct Ip_sockaddr_nl bnl;
        ipcom_memset(&bnl, 0, sizeof(bnl));

        bnl.nl_family   = IP_AF_NETLINK;
        IPCOM_SA_LEN_SET(&bnl, sizeof(bnl));
        return sock->ops->bind(sock, (struct Ip_sockaddr *) &bnl, sizeof(bnl), IP_TRUE);
    }

    return 0;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_netlink_sock_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_netlink_sock_send(Ipnet_socket *sock, IP_CONST struct Ip_msghdr *msg, Ipcom_pkt *pkt)
{
    struct Ip_nlmsghdr          *nlmsg;
    int                         ret = 0;
    Ip_pid_t                    dst_pid     = 0;
    Ip_u32                      dst_groups  = 0;

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED))
    {
        dst_pid     = sock->nl->nl_dst_pid;
        dst_groups  = sock->nl->nl_dst_mc_groups;
    }
    else if (msg->msg_namelen >= sizeof(struct Ip_sockaddr_nl))
    {
        struct Ip_sockaddr_nl *nladdr = (struct Ip_sockaddr_nl *)msg->msg_name;

        dst_pid     = nladdr->nl_pid;
        dst_groups  = nladdr->nl_groups;
    }

    if (dst_pid || dst_groups)
    {
        /* Store the source pid */
        pkt->link_cookie = sock->nl->nl_src_pid;
        if (dst_groups != 0)
        {
            /* Should be broadcasted */
            Ipcom_pkt *npkt = ipnet_pkt_clone(pkt, IP_FALSE);
            if (npkt)
                ipnet_netlink_broadcast(npkt, sock->proto, ipcom_ffs(dst_groups));
        }

        /* Destined for a specific netlink socket? */
        if (dst_pid != 0)
        {
            Ipnet_socket *dsock = ipnet_netlink_sock_lookup_pid(sock->vr_index, sock->proto, dst_pid);
            if (dsock == IP_NULL)
            {
                ret = -IP_ERRNO_ECONNREFUSED;
                goto error_out;
            }

            ipnet_netlink_pkt_queue(&pkt, dsock);
            return 0;
        }
    }

    /* Kernel processing; find the requests in this packet buffer */
    while ((pkt->end - pkt->start) != 0)
    {
        /* Reset error parameter */
        ret = 0;

        /* Go kernel */
        if ((unsigned) (pkt->end - pkt->start) < sizeof(struct Ip_nlmsghdr))
            goto error_out;

        nlmsg = (struct Ip_nlmsghdr*)&pkt->data[pkt->start];

        /* Length sanity */
        if (nlmsg->nlmsg_len < IP_NLMSG_ALIGN(sizeof(struct Ip_nlmsghdr)) || (unsigned)(pkt->end - pkt->start) < nlmsg->nlmsg_len)
            goto error_out;

        /* What protocol */
        switch (sock->proto)
        {
        case IP_NETLINK_ROUTE:
            ret = ipnet_rtnetlink_sock_send(sock, IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING), nlmsg);
            break;
        default:
            ret = -IP_ERRNO_EAFNOSUPPORT;
            goto error_out;
        }

        /* Advance past this NLMSG */
        if (IP_NLMSG_ALIGN(nlmsg->nlmsg_len) > (unsigned)(pkt->end - pkt->start))
            pkt->start = pkt->end;
        else
            pkt->start += IP_NLMSG_ALIGN(nlmsg->nlmsg_len);

        /* Acknowledge or error out */
        if (ret < 0 || (nlmsg->nlmsg_flags & IP_NLM_F_ACK) != 0)
        {
            struct Ip_nlmsgerr      *err;
            Ipcom_pkt               *rpkt   = IP_NULL;
            Ipnet_netlink_mem_t     mem;
            struct Ip_nlmsghdr      *rnlmsg;

            if (ipnet_netlink_pkt_alloc(&rpkt, sock, IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING), &mem, 512))
            {
                rnlmsg = ipnet_netlink_nlmsg_sz_begin(&mem,
                                                      nlmsg->nlmsg_seq,
                                                      IP_NLMSG_ERROR,
                                                      0);
                ip_assert(rnlmsg != IP_NULL);

                err = ipnet_netlink_sz_put(&mem, IP_NULL, sizeof(struct Ip_nlmsgerr));

                ip_assert(err != IP_NULL);

                /* ERROR */
                err->error  = ret < 0? ret : 0;
                ipcom_memcpy(&err->msg, nlmsg, sizeof(*nlmsg));

                /* Finalize this NLMSG */
                rpkt->end += ipnet_netlink_nlmsg_sz_end(&mem, rnlmsg);
                ipnet_netlink_pkt_queue(&rpkt, sock);
            }
        }
    }

error_out:
    ipcom_pkt_free(pkt);
    return IP_MIN(ret, 0);
}


/*
 *===========================================================================
 *                      ipcom__netlink_post
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_netlink_respond(int proto, void *data, Ip_size_t len, int msg_group, Ip_u16 vr)
{
    Ipcom_pkt *pkt;

    if (ipnet->num_netlink_sock == 0)
        return;

    pkt = ipcom_pkt_malloc(len + 4, IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;

    pkt->start      = (Ip_u16)((int) pkt->data & 0x3);
    pkt->end        = (Ip_u16)(pkt->start + len);
    ipcom_memcpy(&pkt->data[pkt->start], data, len);
    pkt->vr_index   = vr;

    (void)ipnet_raw_input(pkt, IP_TRUE, proto, IP_NULL, 0, &msg_group, 0,
                          (Ipnet_sock_lookup_f) ipnet_netlink_sock_lookup);
}


/*
 *===========================================================================
 *                      ipcom__netlink_post
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_netlink_post(int unit, void *data, Ip_size_t len, int msg_group, Ip_u16 vr)
{
    (void)unit;

    ipcom_netlink_respond(IP_NETLINK_ROUTE, data, len, msg_group, vr);
}


/*
 *===========================================================================
 *                    ipnet_sock_netlink_register
 *===========================================================================
 * Description: Registers the UDP protocol for IPv4 and/or IPv6.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_netlink_register(void)
{
    int             types[] = { IP_SOCK_RAW, IP_SOCK_DGRAM };
    Ip_size_t       t;
    int             ret;

    for (t = 0; t < sizeof(types) / sizeof(types[0]); t++)
    {
        /* Ipnet_sock_ops *ops; */
        Ipnet_sock_inet_ops *ops;
        ops = ipcom_calloc(1, sizeof(*ops));
        if (ops == IP_NULL)
        {
            IPCOM_LOG0(CRIT, "Could not register the AF_NETLINK protocol, out of memory");
            IP_PANIC();
            return -IP_ERRNO_ENOMEM;
        }

        ipnet_usr_sock_get_ops(&ops->sock);
        ops->sock.domain       = IP_AF_NETLINK;
        ops->sock.type         = types[t];
        ops->sock.proto        = -1;
        ops->sock.bind         = ipnet_sock_bind;
        ops->sock.destroy      = ipnet_netlink_sock_destroy;
        ops->sock.init         = ipnet_netlink_sock_init;
        ops->sock.send         = ipnet_netlink_sock_send;
        ops->sock.getname      = ipnet_netlink_sock_getname;
        ops->sock.connect      = ipnet_netlink_sock_connect;
        ops->network_bind      = ipnet_netlink_sock_bind;
        ops->network_send      = ipnet_netlink_sock_send;

        ret = ipnet_sock_register_ops(&ops->sock);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/*
 *===========================================================================
 *                    ipnet_netlink_strdup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL char *
ipnet_netlink_strdup(struct Ip_nlattr *nla)
{
    char *ptr = IP_NULL;
    if (nla)
    {
        ptr = ipcom_calloc(nla->nla_len - IP_NLA_HDRLEN + 1, 1);
        if (ptr)
            ipcom_memcpy(ptr, (char *)nla + IP_NLA_HDRLEN, nla->nla_len - IP_NLA_HDRLEN);
    }
    return ptr;
}

/*
 *===========================================================================
 *                   ipnet_netlink_nlmsg_put
 *===========================================================================
 * Description: Compose a NETLINK message header.
 * Parameters:  data - Message buffer
 *              pid  - NETLINK caller pid.
 *              seq  - NETLINK sequence number.
 *              type - NETLINK message type.
 *              len  - messagel length
 *
 * Returns:     Pointer to message header.
 *
 */
IP_GLOBAL struct Ip_nlmsghdr*
ipnet_netlink_nlmsg_put(void *data, Ip_pid_t pid, Ip_u32 seq, int type, int len, Ip_u32 flags)
{
	struct Ip_nlmsghdr *nlh  = (struct Ip_nlmsghdr *) data;
	int                   size = IP_NLMSG_LENGTH(len);

	nlh->nlmsg_type  = (Ip_u16) type;
	nlh->nlmsg_len   = size;
	nlh->nlmsg_flags = flags;
	nlh->nlmsg_pid   = pid;
	nlh->nlmsg_seq   = seq;

    return (nlh);
}


/*
 *===========================================================================
 *                   ipnet_netlink_rta_put
 *===========================================================================
 * Description: Add a NETLINK route attribute to a NETLINK message.
 * Parameters:  buf      - Message buffer
 *              attrtype - Attribute type
 *              attrlen  - Lenght of attribute
 *              data     - Attribute data
 *
 * Returns: Length of attribute incuding header.
 *
 */
IP_GLOBAL int
ipnet_netlink_rta_put(void *buf, int attrtype, int attrlen, IP_CONST void *data)
{
    struct Ip_rtattr *rta = (struct Ip_rtattr *)buf;

    rta->rta_type = (Ip_u16) attrtype;
    rta->rta_len  = (Ip_u16) IP_RTA_LENGTH (attrlen);

    if (data)
        ipcom_memcpy(IP_RTA_DATA(rta), data, attrlen);

    return (rta->rta_len);
}


/*
 *===========================================================================
 *                   ipnet_netlink_pkt_alloc
 *===========================================================================
 * Description: Reserve space for a NLMSG
 * Parameters:  pktflags - Packet allocation flags
 *              prio     -  Priority
 *              hdr      - Head
 *              tail     - Tail
 *              len      - messagel length
 *
 * Returns:     IP_TRUE on success
 *
 */
IP_GLOBAL void
ipnet_netlink_pkt_queue(Ipcom_pkt       **pktp,
                        Ipnet_socket    *sock)
{
    (void)ipnet_queue_received_packet(*pktp, sock);
    *pktp = IP_NULL;
}



/*
 *===========================================================================
 *                    ipnet_netlink_dump
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_netlink_dump(Ipnet_netlink_dump_t         do_dump,
                   int                          family,
                   Ipnet_socket                 *sock,
                   struct Ip_nlmsghdr           *nlmsg,
                   Ip_bool                      non_blocking)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    struct Ip_nlmsghdr      *nlh    = IP_NULL;
    Ip_ptrdiff_t            args[8];
    Ipnet_netlink_mem_t     mem;

    ipcom_memset(&args[0], 0, sizeof(args));

    do
    {
        if (!ipnet_netlink_pkt_alloc(&pkt, sock, non_blocking, &mem, 512))
            return 0;

        /* Dump */
        pkt->end += (*do_dump) (&mem, nlmsg, family, args);

        if (!args[0])
            ipnet_netlink_pkt_queue(&pkt, sock);
    } while (!args[0]);


    if (pkt)
    {
        nlh = ipnet_netlink_nlmsg_sz_begin(&mem,
                                           nlmsg->nlmsg_seq,
                                           IP_NLMSG_DONE,
                                           IP_NLM_F_MULTI);
        if (!nlh)
            ipnet_netlink_pkt_queue(&pkt, sock);
    }



    if (!nlh)
    {
        if (!ipnet_netlink_pkt_alloc(&pkt, sock, non_blocking, &mem, 512))
            return -IP_ERRNO_ENOBUFS;

        nlh = ipnet_netlink_nlmsg_sz_begin(&mem,
                                           nlmsg->nlmsg_seq,
                                           IP_NLMSG_DONE,
                                           IP_NLM_F_MULTI);
        ip_assert(nlh != IP_NULL);
    }

    pkt->end += ipnet_netlink_nlmsg_sz_end(&mem, nlh);
    ipnet_netlink_pkt_queue(&pkt, sock);
    return 0;
}


#endif /* IPNET_USE_NETLINKSOCK */

#ifdef IPNET_USE_NETLINK
/*
 *===========================================================================
 *                   ipnet_netlink_nlmsg_pkt_end
 *===========================================================================
 * Description: Complete a NLMSG
 * Parameters:  pkt  - The packet
 *              nlh  - The header
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_netlink_nlmsg_sz_end(Ipnet_netlink_mem_t *mem, struct Ip_nlmsghdr* nlh)
{
    mem->used       += mem->offset;
    nlh->nlmsg_len  = mem->offset;
    mem->offset     = 0;
    return nlh->nlmsg_len;
}


/*
 *===========================================================================
 *                   ipnet_netlink_pkt_put
 *===========================================================================
 * Description: Add a NETLINK route attribute to a NETLINK message.
 * Parameters:  nlh      - Current NLMSG
 *              sz       - Size
 *              attrtype - Attribute type
 *              attrlen  - Lenght of attribute
 *              data     - Attribute data
 *
 * Returns:
 *
 */
IP_GLOBAL void *
ipnet_netlink_sz_put(Ipnet_netlink_mem_t    *mem,
                     void                  *data,
                     int                   len)
{
    void    *dptr = &mem->ptr[mem->used + mem->offset];

    /* Move past this particular element */
    mem->offset += IP_NLMSG_ALIGN(len);

    if ((mem->offset + mem->used) > mem->size)
        return IP_NULL;

    if (data)
        ipcom_memcpy(dptr, data, len);

    return dptr;
}


/*
 *===========================================================================
 *                   ipnet_netlink_nlmsg_put
 *===========================================================================
 * Description: Compose a NETLINK message header.
 * Parameters:  data - Message buffer
 *              pid  - NETLINK caller pid.
 *              seq  - NETLINK sequence number.
 *              type - NETLINK message type.
 *              len  - messagel length
 *
 * Returns:     Pointer to message header.
 *
 */
IP_GLOBAL struct Ip_nlmsghdr*
ipnet_netlink_nlmsg_sz_begin(Ipnet_netlink_mem_t *mem, Ip_u32 seq, int type, Ip_u32 flags)
{
    struct Ip_nlmsghdr  *nlh;
    int                 size = IP_NLMSG_LENGTH(0);

    if ((mem->size - mem->used) <= IP_NLMSG_LENGTH(0))
        return IP_NULL;

    /* Initialize the netlink header */
    nlh = (struct Ip_nlmsghdr *)&mem->ptr[mem->used];
    nlh->nlmsg_type  = (Ip_u16) type;
    nlh->nlmsg_len   = size;
    nlh->nlmsg_flags = flags;
    nlh->nlmsg_pid   = mem->pid;
    nlh->nlmsg_seq   = seq;

    mem->offset = size;

    return nlh;
}


/*
 *===========================================================================
 *                    ipnet_netlink_pkt_mem
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipnet_netlink_mem_t *
ipnet_netlink_pkt_mem(Ipcom_pkt* pkt, Ipnet_socket *sock, Ipnet_netlink_mem_t *mem)
{
    ipcom_memset(mem, 0, sizeof(*mem));

    if (pkt)
    {
        mem->ptr            = (char *)&pkt->data[pkt->end];
        mem->size           = pkt->maxlen - pkt->end;
    }

#ifdef IPNET_USE_NETLINKSOCK
    if (sock)
    {
        mem->vr             = sock->vr_index;
        mem->pid            = sock->nl->nl_src_pid;
    }
#else
    (void)sock;
#endif
    return mem;
}


/*
 *===========================================================================
 *                   ipnet_netlink_pkt_alloc
 *===========================================================================
 * Description: Reserve space for a NLMSG
 * Parameters:  pktflags - Packet allocation flags
 *              prio     -  Priority
 *              hdr      - Head
 *              tail     - Tail
 *              len      - messagel length
 *
 * Returns:     IP_TRUE on success
 *
 */
IP_GLOBAL Ip_bool
ipnet_netlink_pkt_alloc(Ipcom_pkt **pktp, Ipnet_socket *sock, Ip_bool non_blocking, Ipnet_netlink_mem_t *mem, Ip_size_t size)
{
    int                     pktflags    = 0;

    if (*pktp == IP_NULL)
    {
        if (!non_blocking)
            pktflags |= IP_FLAG_FC_BLOCKOK;

        *pktp = ipcom_pkt_malloc(size, pktflags);
        if (*pktp == IP_NULL)
            return IP_FALSE;

        (*pktp)->vr_index = sock? sock->vr_index : 0;
        (*pktp)->start          = 0;
        (*pktp)->end            = 0;
        (*pktp)->next           = IP_NULL;
        (*pktp)->link_cookie    = 0;
    }

    if (mem)
        ipnet_netlink_pkt_mem(*pktp, sock, mem);
    return IP_TRUE;
}

/*
 *===========================================================================
 *                      ipnet_netlink_broadcast
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_netlink_broadcast(Ipcom_pkt *pkt, int proto, int msg_group)
{
#ifndef IP_PORT_LKM
    (void)ipnet_raw_input(pkt, IP_TRUE, proto, IP_NULL, 0, &msg_group, 0,
                          (Ipnet_sock_lookup_f) ipnet_netlink_sock_lookup);
#else
    ipcom_netlink_post(pkt, msg_group);
#endif
}

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

