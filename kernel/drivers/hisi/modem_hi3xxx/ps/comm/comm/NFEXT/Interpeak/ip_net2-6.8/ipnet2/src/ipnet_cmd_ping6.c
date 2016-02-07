/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_ping6.c,v $ $Revision: 1.79 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_ping6.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *    Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Implements the "ping6" shell command. ping uses ICMPv6 echo
 * request/response messages to see if a specific node is currently
 * reachable.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef IPCOM_KERNEL
#undef IPCOM_KERNEL
#endif
#include "ipnet_config.h"

/* Maximum sequence number that will ever be sent, must fit within an
   unsigned 16-bit integer */
#define IPNET_CMD_PING6_MAX_SEQ   1000

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_INET6

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>
#include <ipcom_shell.h>
#include <ipcom_sock.h>
#include <ipcom_tmo2.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_cmd.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_CMD_PING6_DST_BUFLEN  32

#define IPNET_CMD_PING6_SEQ_RANGE (IPNET_CMD_PING6_MAX_SEQ + 1)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

enum Ipnet_cmd_ping6_state {
    IPNET_CMD_PING6_START,    /* No request has been sent */
    IPNET_CMD_PING6_PINGING,  /* At least one request has been sent */
    IPNET_CMD_PING6_DONE,     /* A response to the last ping request
                                 has been received */
    IPNET_CMD_PING6_FAILED,   /* Sending ping request failed */
};

struct Ipnet_cmd_ping6 {
    /* ICMPv6 raw socket used to send and receive ICMPv6 messages */
    int                        sock;

    /* Current state of this session */
    enum Ipnet_cmd_ping6_state state;

    /* Error code if sending failed */
    int                        error;

    /* Brief description of the error */
    const char                *err_desc;

    /* Egress interface for the request. 0 means that the stack should
       choose the egress interface. */
    int                        ifindex;

    /* Address of the node the requests are sent to */
    struct Ip_sockaddr_in6     to;

   /* Time when this session started */
    struct Ip_timeval          tv_start;

    /* Time when this session is done */
    unsigned                   msec_end;

    /* Time between echo/timestamp requests */
    unsigned                   msec_interval;

    /* Identifier of this session */
    Ip_u16                     id;

    /* Sequence number of next request */
    unsigned                   seq;

    /* Last sequence number to send, might be
       >IPNET_CMD_PING6_MAX_SEQ, which means that the same sequence
       number will be reused. */
    unsigned                   last_seq;

    /* Response map, bit B is set when/if a response with sequence
       number B is received. This bit map will keep track of all
       possible sequence numbers in a session. */
    char                       response_map[(IPNET_CMD_PING6_MAX_SEQ+8)/8];

    /* Length of the request, ICMPv6 header + payload. Minimum length
       is 8 since that it the size of the ICMPv6 header. */
    Ip_size_t                  request_len;

    /* Temporary buffer sending and receiving */
    void                      *data;

    /* Length of "data" */
    Ip_size_t                  data_len;

    /* Timeout queue, used to keep track of request transmissions */
    Ipcom_pqueue              *tmo_head;

    /* Timer for the next echo/timestamp request */
    Ipcom_tmo2                 tmo;

    /* Statistics of this session */
    Ipnet_cmd_ping_stat_t      stat;
};


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

IP_STATIC void
ipnet_ping6_schedule_echo_request(struct Ipnet_cmd_ping6 *session);

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
 *                        ipnet_ping6_error
 *===========================================================================
 * Description: Sets session error
 * Parameters:  session - a session
 *              desc - description of the error.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping6_error(struct Ipnet_cmd_ping6 *session,
                  const char *desc)
{
    /* Close the socket so the tmo2 machinery shuts down
       immediately */
    ipcom_socketclose(session->sock);
    session->sock     = IP_INVALID_SOCKET;
    session->state    = IPNET_CMD_PING6_FAILED;
    session->error    = ipcom_errno;
    session->err_desc = desc;
}


/*
 *===========================================================================
 *                      ipnet_ping6_get_response_time
 *===========================================================================
 * Description: Calculates the time since the message with the specified
 *              sequence number was sent.
 * Parameters:  session - a session
 *              seq - sequence number of the received response
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ping6_get_response_time(struct Ipnet_cmd_ping6 *session, Ip_u16 seq)
{
    unsigned full_seq; /* Most likely sequence number if it was not
                          trunkated to 16-bit */

    full_seq = (session->seq / IPNET_CMD_PING6_SEQ_RANGE)
        * IPNET_CMD_PING6_SEQ_RANGE;
    if ((session->seq % IPNET_CMD_PING6_SEQ_RANGE) < seq)
        /* Sent sequence number has wrapped */
        full_seq -= IPNET_CMD_PING6_SEQ_RANGE;
    full_seq += seq;

    return ipnet_cmd_msec_since(&session->tv_start)
        - full_seq * session->msec_interval;
}


/*
 *===========================================================================
 *                       ipnet_ping6_update_timeout
 *===========================================================================
 * Description: Calculates how much of this session that is left
 * Parameters:  session - a session
 *              tmo - will contain how much of this session that is left
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping6_update_timeout(struct Ipnet_cmd_ping6 *session,
                           struct Ip_timeval *tmo)
{
    unsigned msec_tmo;

    msec_tmo = ipnet_cmd_msec_since(&session->tv_start);
    if (msec_tmo < session->msec_end)
    {
        msec_tmo = session->msec_end - msec_tmo;
        tmo->tv_sec  = msec_tmo / 1000;
        tmo->tv_usec = (msec_tmo % 1000) * 1000;
    }
    else
    {
        /* We have passed the end time */
        tmo->tv_sec  = 0;
        tmo->tv_usec = 0;
    }
}


/*
 *===========================================================================
 *                    ipnet_ping6_send_echo_request
 *===========================================================================
 * Description: Creates a ICMPv6 echo request message and sends it to the
 *              specified address.
 * Parameters:  sock - The socket to use when sending the message.
 *              to - The recipient of the message.
 *              id - The id to use in the echo request.
 *              seq - The sequence number to use in the echo request.
 *              icmp_len - The total length of the ICMPv6 message,
 *              including header.
 * Returns:     IP_FALSE - do not reschedule the timeout
 *
 */
IP_STATIC Ip_bool
ipnet_ping6_send_echo_request(struct Ipnet_cmd_ping6 *session)
{
    Ipnet_pkt_icmp6 *echo_req;
    int              ret;
    Ip_size_t        i;
    Ip_size_t        msg_len = session->request_len;
    Ip_size_t        data_len = msg_len - 8;
    char            *ping_data;
    Ip_u16           seq;

    /* Calculate the next sequence number to use */
    seq = session->seq++ % IPNET_CMD_PING6_SEQ_RANGE;

    echo_req = session->data;

    echo_req->icmp6_type      = IPNET_ICMP6_ECHO_REQUEST;
    echo_req->icmp6_code      = 0;
    echo_req->ipnet_icmp6_id  = ip_htons(session->id);
    echo_req->ipnet_icmp6_seq = ip_htons(seq);
    echo_req->icmp6_cksum     = 0x5555;
    /* The checksum field can be left uninitialized since it will be
       calculated by the stack, it is set here to !=0 just to test
       that the stack still calculates the correct checksum */
    ping_data = (char *)(echo_req + 1);
    for (i = 0; i < data_len; i++)
        ping_data[i] = i % (127 - ' ') + ' ';

    ret = ipcom_sendto(session->sock,
                       echo_req,
                       msg_len,
                       IP_MSG_DONTWAIT,
                       (struct Ip_sockaddr*) &session->to,
                       IPCOM_SA_LEN_GET(&session->to));

    if (ret < 0
        && (ipcom_errno != IP_ERRNO_ENOBUFS
            && ipcom_errno != IP_ERRNO_EWOULDBLOCK))
        ipnet_ping6_error(session, "sendto()");
    else
    {
        session->state = IPNET_CMD_PING6_PINGING;
        ipnet_bit_array_clr(session->response_map, seq);
        ipnet_ping_stat_transmit(&session->stat);
        ipnet_ping6_schedule_echo_request(session);
    }

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_ping6_schedule_echo_request
 *===========================================================================
 * Description: Schedules a new ping request transmission
 * Parameters:  session - a session
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping6_schedule_echo_request(struct Ipnet_cmd_ping6 *session)
{
    unsigned timeout;
    unsigned now;

    if (session->seq > session->last_seq)
        return;

    timeout = session->seq * session->msec_interval;
    now = ipnet_cmd_msec_since(&session->tv_start);
    if (now <= timeout)
        timeout -= now;
    else
        timeout = 0;

    ipcom_tmo2_request(session->tmo_head,
                       &session->tmo,
                       (Ipcom_tmo2_func) ipnet_ping6_send_echo_request,
                       session,
                       timeout);
}


/*
 *===========================================================================
 *                      ipnet_ping6_recv_echo_reply
 *===========================================================================
 * Description: Receives a single ICMPv6 message.
 * Parameters:  session - a session
 *              from - will contain the address sender of the message
 *              hlim - will contain the TTL of the message
 *              seq - will contain the sequence number of the message
 *              receive_ts - will contain the receive timestamp of
 *                       the message if the message is a timestamp
 *              transmit_ts - will contain the transmit timestamp of
 *                       the message if the message is a timestamp
 *              dup - will be IP_TRUE if we received a duplicate.
 * Returns:     <0 = operation failed
 *               0 = message was ignored
 *              >0 = size of received response
 *
 */
IP_STATIC int
ipnet_ping6_recv_echo_reply(struct Ipnet_cmd_ping6 *session,
                            struct Ip_sockaddr_in6 *from,
                            int *hlim,
                            Ip_u16 *seq,
                            Ip_bool *dup)
{
    int                bytes_recv;
    struct Ip_msghdr   msg;
    struct Ip_iovec    iov;
    Ipnet_pkt_icmp6   *icmp6;
    char               cmsg_buf[IP_CMSG_SPACE(sizeof(int))];
    struct Ip_cmsghdr *cmsg;

    msg.msg_controllen = sizeof(cmsg_buf);
    msg.msg_control    = cmsg_buf;

    msg.msg_flags   = 0;
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;
    iov.iov_base    = session->data;
    iov.iov_len     = session->data_len;
    msg.msg_name    = from;
    msg.msg_namelen = sizeof(*from);

    bytes_recv = ipcom_recvmsg(session->sock, &msg, 0);
    if (bytes_recv < 0)
    {
        ipnet_ping6_error(session, "ipcom_recvmsg()");
        return bytes_recv;
    }

    icmp6 = (struct Ipnet_icmp6_hdr *) iov.iov_base;

    if (icmp6->icmp6_type != IPNET_ICMP6_ECHO_REPLY
        || ip_ntohs(icmp6->ipnet_icmp6_id) != session->id)
        return 0;

    for (cmsg = IP_CMSG_FIRSTHDR(&msg);
         cmsg != IP_NULL;
         cmsg = IP_CMSG_NXTHDR(&msg, cmsg))
    {
        if (cmsg->cmsg_level == IP_IPPROTO_IPV6 && cmsg->cmsg_type == IP_IPV6_HOPLIMIT)
            *hlim = *(int *) IP_CMSG_DATA(cmsg);
    }

    *seq = ip_ntohs(icmp6->ipnet_icmp6_seq);

#if IPNET_CMD_PING6_MAX_SEQ < 0xffff
    if (*seq > IPNET_CMD_PING6_MAX_SEQ)
        /* This cannot be a response to something this node sent */
        return 0;
#endif

    *dup = ipnet_bit_array_test_and_set(session->response_map, *seq);

    return bytes_recv;
}


/*
 *===========================================================================
 *                    ipnet_ping6_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping6_print_usage(void)
{
    ipcom_printf(IP_LF
                 "NAME" IP_LF
                 "     ping6 - send ICMPv6 ECHO_REQUEST packets to network hosts" IP_LF
                 IP_LF
                 "SYNOPSIS" IP_LF
                 "     ping6 [-Tnrt] [-c count] [-h hoplimit] [-m ifname] [-s packetsize] [-w timeout]"IP_LF
                 "           [-F label] [-S src_addr] [-Q tclass] [-V vr] [hop ...] host" IP_LF
                 IP_LF
                 "DESCRIPTION" IP_LF
                 "     ping6 uses the ICMPv6 protocol's mandatory ICMP6_ECHO_REQUEST datagram to" IP_LF
                 "     elicit an ICMP6_ECHO_REPLY from a host or gateway.  ICMP6_ECHO_REQUEST" IP_LF
                 "     datagrams (``pings'') have an IPv6 header, and ICMPv6 header formatted as" IP_LF
                 "     documented in RFC2463. It is possible to specify additional nodes that the" IP_LF
                 "     ping request must visit before reaching the final destination." IP_LF
                 "     This is accomplished by adding a routing header to the packet." IP_LF
                 IP_LF
                 "     -c count" IP_LF
                 "             Stop after sending count ECHO_RESPONSE packets." IP_LF
                 "     -F label" IP_LF
                 "             Specifcy the flow label in the IPv6 header." IP_LF
                 "     -h hoplimit" IP_LF
                 "             Set the IPv6 hoplimit." IP_LF
                 "     -m <ifname>" IP_LF
                 "             Name of outgoing interface for multicast addresses." IP_LF
                 "     -n      Numeric output only. No attempts will be made to look up" IP_LF
                 "             names for host addresses." IP_LF
                 "     -Q tclass" IP_LF
                 "             Specify the traffic class in the IPv6 header." IP_LF
                 "     -r      Bypass the normal routing tables and send directly to a host on" IP_LF
                 "             an attached network." IP_LF
                 "     -S src_addr  Specifies the address to use a source address when sending." IP_LF
                 "     -s packetsize" IP_LF
                 "             Specifies the number of data bytes to be sent. The default is" IP_LF
                 "             56, which translates into 64 ICMP data bytes when combined with" IP_LF
                 "             the 8 bytes of ICMP header data." IP_LF
                 "     -T Prefer temporary addresses as source." IP_LF
                 "     -V vr   Specify virtual router. vr = 0 if not specified."IP_LF
                 "     -w timeout  Timeout in milliseconds, default is 1000 ms."IP_LF
                 "     hop ... Zero or more nodes that the echo request must pass,"IP_LF
                 "             in order, before reaching the final host node"IP_LF
                 IP_LF);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_ping6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_ping6(int argc, char **argv)
{
    int                    var;
    Ip_socklen_t           var_len;
    int                    hlim;
    unsigned               count = 1;
    int                    i;
    int                    vr = 0;
    Ip_u32                 flow_label = 0;
    struct Ip_sockaddr_in6 from;
    char                   str_addr[IP_INET6_ADDRSTRLEN];
    char                   targetaddr[IP_INET6_ADDRSTRLEN];
    char                   targetname[256];
    struct Ip_timeval      tv;
    int                    err = 0;
    int                    no_dns = IP_FALSE;
    Ipcom_getopt           opt;
    struct Ip_icmp6_filter icmp6_filter;
    unsigned               mcast_ifindex;
    int                    exit_status = IPCOM_SHELL_ERR;
    int                    sock;
    Ip_fd_set              rd_set;
    Ip_bool                print_stat = IP_FALSE;
    Ip_size_t              extension_hdr_len = 0;
    Ip_pkt_ip6_rthdr0     *rthdr0 = IP_NULL;
#ifdef IPNET_DEBUG
    Ip_pkt_ip6_dest       *dst_ext = IP_NULL;
    int                    dst_offset;
    void                  *opt_data;
    Ip_u8                  padn_data[] = { 0, 0, 0, 0, 0, 0, 0 };
#endif /* IPNET_DEBUG */
    struct Ipnet_cmd_ping6 *session;

    session = ipcom_calloc(1, sizeof(*session));
    if (session)
    {
        session->tmo_head = ipcom_tmo2_new();
        session->sock = IP_SOCKERR;
    }

    if (session == IP_NULL || session->tmo_head == IP_NULL)
    {
        ipcom_printf("Out of memory" IP_LF);
        goto errout;
    }

    if (argc < 2)
    {
        ipnet_ping6_print_usage();
        goto errout;
    }

    /* Set default values */
    session->state         = IPNET_CMD_PING6_START;
    session->request_len   = 64;
    session->id            = (Ip_u16) ipcom_random();
    session->msec_interval = 1000;
    IPCOM_TMO2_INIT(&session->tmo);

    sock = ipcom_socket(IP_AF_INET6, IP_SOCK_RAW, IP_IPPROTO_ICMPV6);
    session->sock = sock;
    if (sock == IP_SOCKERR)
    {
        ipcom_printf("ipcom_socket, errno %d "IP_LF, ipcom_errno);
        goto errout;
    }

    /* Enable receive of hop-limit */
    var = 1;
    err = ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_RECVHOPLIMIT, &var, sizeof(var));
    if (err < 0)
    {
        ipcom_printf("ipcom_setsockopt IP_IPV6_RECVHOPLIMIT, errno %d "IP_LF, ipcom_errno);
        goto errout;
    }

    ipcom_getopt_clear_r(&opt);
    while ((i = ipcom_getopt_r(argc, argv, "124c:F:h:m:nQ:rS:s:TV:w:", &opt)) != -1)
    {
        switch (i)
        {
        case 'c':
            count = (unsigned) ipcom_strtoul(opt.optarg, IP_NULL, 10);
            break;
        case 'F':
            flow_label = ipcom_atoi(opt.optarg);
            break;
        case 'h':
            hlim = ipcom_atoi(opt.optarg);
            if (ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_UNICAST_HOPS, &hlim, sizeof(hlim)))
            {
                ipnet_ping6_error(session, "ipcom_setsockopt(IPV6_UNICAST_HOPS)");
                goto errout;
            }
            break;
        case 'Q':
            var = ipcom_atoi(opt.optarg);
            err = ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_TCLASS, &var, sizeof(var));
            if (err < 0)
            {
                ipnet_ping6_error(session, "ipcom_setsockopt(IPV6_TCLASS)");
                goto errout;
            }
            break;
        case 'm':
            mcast_ifindex = ipcom_if_nametoindex(opt.optarg);
            err = ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_MULTICAST_IF, &mcast_ifindex, sizeof(mcast_ifindex));
            if (err < 0)
            {
                ipnet_ping6_error(session, "ipcom_setsockopt(IPV6_MULTICAST_IF)");
                goto errout;
            }
            break;
        case 'n':
            no_dns = IP_TRUE;
            break;
        case 'r':
            var = 1;
            if (ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_DONTROUTE, &var, sizeof(var)) < 0)
            {
                ipnet_ping6_error(session, "ipcom_setsockopt(SO_DONTROUTE)");
                goto errout;
            }
            break;
        case 'S':
            ipcom_memset(&from, 0, sizeof(from));
            from.sin6_family = IP_AF_INET6;
            IPCOM_SA_LEN_SET(&from, sizeof(from));
            (void)ipcom_inet_pton(IP_AF_INET6, opt.optarg, &from.sin6_addr);
            err = ipcom_bind(sock, (struct Ip_sockaddr *) &from, sizeof(from));
            if (err < 0)
            {
                ipnet_ping6_error(session, session->data);
                ipcom_snprintf(session->data, session->data_len, "ipcom_bind(%s)", opt.optarg);
                goto errout;
            }
            break;
        case 's':
            session->request_len = ipcom_atoi(opt.optarg);
            if (session->request_len < (int)IPNET_ICMPV6_HDR_SIZE)
            {
                ipcom_printf("Data length must be %d or greater." IP_LF, IPNET_ICMPV6_HDR_SIZE);
                goto errout;
            }
            break;
        case 'T':
            var_len = sizeof(var);
            if (ipcom_getsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_ADDR_PREFERENCES, &var, &var_len) < 0)
            {
                ipnet_ping6_error(session, "ipcom_getsockopt(IPV6_ADDR_PREFERENCES)");
                goto errout;

            }
            IP_BIT_SET(var, IP_IPV6_PREFER_SRC_TMP);
            if (ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_ADDR_PREFERENCES, &var, sizeof(var)) < 0)
            {
                ipnet_ping6_error(session, "ipcom_setsockopt(IPV6_ADDR_PREFERENCES)");
                goto errout;

            }
            break;
        case 'V' :
            vr = ipcom_atoi(opt.optarg);
            err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_X_VR, &vr, sizeof(vr));
            if (err < 0)
            {
                ipnet_ping6_error(session, "ipcom_setsockopt(SO_X_VR)");
                goto errout;
            }
            break;
        case 'w':
            session->msec_interval = ipcom_atoi(opt.optarg);
            if (session->msec_interval == 0)
            {
                ipcom_printf("Interval between requests must be >0"IP_LF);
                goto errout;
            }
            break;
#ifdef IPNET_DEBUG
        case '1':
        case '2':
        case '4':
            if (dst_ext == IP_NULL)
            {
                extension_hdr_len += IPNET_CMD_PING6_DST_BUFLEN;
                dst_ext = ipcom_malloc(IPNET_CMD_PING6_DST_BUFLEN);
                if (dst_ext == IP_NULL)
                {
                    ipcom_printf("ipcom_malloc(IPNET_CMD_PING6_DST_BUFLEN=%d) failed"IP_LF,
                                 IPNET_CMD_PING6_DST_BUFLEN);
                    goto errout;
                }
            }

            dst_offset = ipcom_inet6_opt_init(dst_ext, IPNET_CMD_PING6_DST_BUFLEN);
            if (dst_offset < 0)
            {
                ipnet_ping6_error(session, "ipcom_inet6_opt_init IP_IPV6_DSTOPTS ");
                goto errout;
            }

            dst_offset = ipcom_inet6_opt_append(dst_ext, IPNET_CMD_PING6_DST_BUFLEN, dst_offset, IP_IP6OPT_PADN, sizeof(padn_data), 1, &opt_data);
            if (dst_offset < 0)
            {
                ipnet_ping6_error(session, "ipcom_inet6_opt_append IP_IPV6_DSTOPTS ");
                goto errout;
            }

            if (ipcom_inet6_opt_set_val(opt_data, 0, padn_data, sizeof(padn_data)) < 0)
            {
                ipnet_ping6_error(session, "ipcom_inet6_opt_set_val IP_IPV6_DSTOPTS ");
                goto errout;
            }

            dst_offset = ipcom_inet6_opt_finish(dst_ext, IPNET_CMD_PING6_DST_BUFLEN, dst_offset);
            if (dst_offset < 0)
            {
                ipnet_ping6_error(session, "ipcom_inet6_opt_finish IP_IPV6_DSTOPTS ");
                goto errout;
            }

            switch (i)
            {
            case '1':
                var = IP_IPV6_HOPOPTS;
                break;
            case '2':
                var = IP_IPV6_DSTOPTS;
                break;
            case '4':
                var = IP_IPV6_RTHDRDSTOPTS;
                break;
            default:
                IP_PANIC();
                var = 0;
                break;
            }
            if (ipcom_setsockopt(sock, IP_IPPROTO_IPV6, var, dst_ext, dst_offset))
            {
                ipnet_ping6_error(session, "ipcom_setsockopt IP_IPV6_DSTOPTS ");
                goto errout;
            }
            break;
#endif /* IPNET_DEBUG */
        default:
            ipcom_printf("Unknown switch '%c'"IP_LF, i);
            goto errout;
        }
    }

    if (argc - 1 > opt.optind)
    {
        /* Intermeadiate hops */
        Ip_socklen_t rhdr0_len;
        int segments;

        segments = argc - 1 - opt.optind;
        rhdr0_len = ipcom_inet6_rth_space(IP_IPV6_RTHDR_TYPE_0, segments);

        extension_hdr_len += rhdr0_len;
        rthdr0 = ipcom_malloc(rhdr0_len);
        if (rthdr0 == IP_NULL)
        {
            ipcom_printf("ipcom_malloc(%d) IP_IPV6_RTHDR_TYPE_0"IP_LF, rhdr0_len);
            err = -IP_ERRNO_ENOMEM;
            goto errout;
        }

        if (ipcom_inet6_rth_init(rthdr0, rhdr0_len, IP_IPV6_RTHDR_TYPE_0, segments) == IP_NULL)
        {
            ipnet_ping6_error(session, "ipcom_inet6_rth_init() IP_IPV6_RTHDR_TYPE_0 ");
            goto errout;
        }

        do
        {
            err = ipcom_getsockaddrbyaddrname(IP_AF_INET6,
                no_dns, argv[opt.optind], (struct Ip_sockaddr *) &session->to);
            if (err)
                goto errout;

            if (ipcom_inet6_rth_add(rthdr0, &session->to.sin6_addr) < 0)
            {
                ipnet_ping6_error(session, "ipcom_inet6_rth_add() IP_IPV6_RTHDR_TYPE_0 ");
                err = -IP_ERRNO_EINVAL;
                goto errout;
            }
        } while (argc - 1 > ++opt.optind);

        err = ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_RTHDR, rthdr0, rhdr0_len);
        if (err)
        {
            ipnet_ping6_error(session, "ipcom_setsockopt() IP_IPV6_RTHDR ");
            goto errout;
        }
    }

    err = ipcom_getsockaddrbyaddrname(IP_AF_INET6,
                                      no_dns,
                                      argv[argc-1],
                                      (struct Ip_sockaddr *) &session->to);
    if (err)
    {
        ipcom_printf("'%s' is not a valid IPv6 address"IP_LF, argv[argc-1]);
        goto errout;
    }
    session->to.sin6_flowinfo = ip_htonl(flow_label);

    /* Send/receive buffer must be large enough to receive IPv6+ICMPv6
       header and possibly an embedded IPv6+ICMPv6+payload datagram */
    session->data_len = (IPNET_ICMPV6_HDR_SIZE * 2
                         + IPNET_IP6_HDR_SIZE
                         + extension_hdr_len
                         + session->request_len);
    session->data = ipcom_malloc(session->data_len);
    if (session->data == IP_NULL)
    {
        ipcom_printf("Failed to allocate %u bytes"IP_LF, session->data_len);
        goto errout;
    }

    /* Set the size of the send buffer so that the ping message fit
      (ICMPv6 header + ICMPv6 payload) */
    var = 65535;
    err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_SNDBUF, &var, sizeof(var));
    if (err < 0)
    {
        ipnet_ping6_error(session, "ipcom_setsockopt() IP_SO_SNDBUF ");
        goto errout;
    }

    IP_ICMP6_FILTER_SETBLOCKALL(&icmp6_filter);
    IP_ICMP6_FILTER_SETPASS(IPNET_ICMP6_ECHO_REPLY, &icmp6_filter);
    err = ipcom_setsockopt(sock, IP_IPPROTO_ICMPV6, IP_ICMP6_FILTER, &icmp6_filter, sizeof(icmp6_filter));
    if (err < 0)
    {
        ipnet_ping6_error(session, "ipcom_setsockopt() IP_ICMP6_FILTER ");
        goto errout;
    }

    (void)ipcom_cmd_sockaddr_to_str((struct Ip_sockaddr *) &session->to,
                              IP_FALSE, IP_TRUE, targetaddr, sizeof(targetaddr));
    (void)ipcom_cmd_sockaddr_to_str((struct Ip_sockaddr *) &session->to,
                              IP_FALSE, no_dns, targetname, sizeof(targetname));

    ipcom_printf("\nPinging %s (%s) with %d bytes of data:" IP_LF,
                 targetname,
                 targetaddr,
                 session->request_len);

    ipnet_ping_stat_init(&session->stat);
    print_stat = IP_TRUE;
    session->last_seq = count - 1;

    /* Calculate the absolute time when this ping session must end */
    ipcom_microtime(&session->tv_start);
    session->msec_end = session->msec_interval * count;

    /* Send first request and start periodic timer */
    (void)ipnet_ping6_send_echo_request(session);
    ipnet_ping6_schedule_echo_request(session);

    IP_FD_ZERO(&rd_set);

    for (IP_FD_SET(sock, &rd_set), ipnet_ping6_update_timeout(session, &tv);
         session->state == IPNET_CMD_PING6_PINGING
             && ipcom_tmo2_select(session->tmo_head,
                                  sock + 1,
                                  &rd_set,
                                  IP_NULL,
                                  IP_NULL,
                                  &tv) > 0;
         IP_FD_SET(sock, &rd_set), ipnet_ping6_update_timeout(session, &tv))
    {
        unsigned msec;
        Ip_u16   seq;
        int      res_size;
        Ip_bool  dup;

        ip_assert(IP_FD_ISSET(sock, &rd_set));

        res_size = ipnet_ping6_recv_echo_reply(session,
                                               &from,
                                               &hlim,
                                               &seq,
                                               &dup);
        if (res_size < 0)
            break;
        if (res_size == 0)
            continue;

        /* Got at least one response */
        exit_status = IPCOM_SHELL_SUCCESS;

        if (session->last_seq == seq)
            /* Got a response on the last request we are going to
               send */
            session->state = IPNET_CMD_PING6_DONE;

        msec = ipnet_ping6_get_response_time(session, seq);

        ipcom_printf("Reply from %s bytes=%d hlim=%d seq=%d time=%dms%s" IP_LF,
                     ipcom_cmd_sockaddr_to_str((struct Ip_sockaddr *) &from,
                                               IP_FALSE,
                                               no_dns,
                                               str_addr,
                                               sizeof(str_addr)),
                     res_size,
                     hlim,
                     seq,
                     msec,
                     dup ? " (DUP!)" : "");

        ipnet_ping_stat_receive(&session->stat, msec, dup);
    }

errout:
    ipcom_free(rthdr0);
#ifdef IPNET_DEBUG
    ipcom_free(dst_ext);
#endif /* IPNET_DEBUG */

    if (session != IP_NULL && session->state == IPNET_CMD_PING6_FAILED)
    {
        err = session->error;
        if (err != IP_ERRNO_ENOBUFS)
            /* Packet dropped due to missing NDP response, can happens in v4->v6 tunnels */
            ipcom_printf("%s: operation failed: %s (%d)" IP_LF,
                         session->err_desc, ipcom_strerror(err), err);
    }
    else if (print_stat)
    {
        ipcom_printf(IP_LF);
        ipnet_ping_stat_report(&session->stat, targetname);
    }

    if (session)
    {
        if (session->sock != IP_SOCKERR)
            ipcom_socketclose(session->sock);

        if (session->tmo_head)
            ipcom_tmo2_delete(session->tmo_head);

        ipcom_free(session->data);
        ipcom_free(session);
    }

    return exit_status;
}

#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

