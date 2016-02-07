/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_ping.c,v $ $Revision: 1.95.16.1 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_ping.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Implements the "ping" shell command. ping uses ICMP echo
 * request/response messages to see if a specific node is currently
 * reachable.
 *
 * This version of ping also implements ICMP timestamp
 * request/response where each intermediate router, between this node
 * and the node the request is sent to, can insert the time the packet
 * passed through that router.
 *
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
#define IPNET_CMD_PING_MAX_SEQ   1000

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
  ****************************************************************************
 */

#ifdef IPCOM_USE_INET

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>
#include <ipcom_inet.h>
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

#define IPNET_ICMP4_TYPE_ECHO_REPLY        0
#define IPNET_ICMP4_TYPE_DST_UNREACHABLE   3
#define IPNET_ICMP4_TYPE_ECHO_REQUEST      8
#define IPNET_ICMP4_TYPE_TSTAMP_REQUEST    13
#define IPNET_ICMP4_TYPE_TSTAMP_REPLY      14

/* [3] Destination unreachable codes */
#define IPNET_ICMP4_CODE_DST_UNREACH_NET      0  /* Network Unreachable */
#define IPNET_ICMP4_CODE_DST_UNREACH_HOST     1  /* Host Unreachable */
#define IPNET_ICMP4_CODE_DST_NEEDFRAG         4  /* Fragmentation needed but no frag. bit set */
#define IPNET_ICMP4_CODE_DST_PROHIBITED_NET   9  /* Destination network administratively prohibited */
#define IPNET_ICMP4_CODE_DST_PROHIBITED_HOST 10  /* Destination host administratively prohibited */

#define IPNET_ICMP4_HDR_LEN                   8

#define IPNET_CMD_PING_SEQ_RANGE (IPNET_CMD_PING_MAX_SEQ + 1)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

enum Ipnet_cmd_ping_state {
    IPNET_CMD_PING_START,      /* No request has been sent */
    IPNET_CMD_PING_PINGING,    /* At least one request has been sent */
    IPNET_CMD_PING_DONE,       /* A response to the last ping request
                                  has been received */
    IPNET_CMD_PING_FAILED      /* Sending ping request failed */
};

struct Ipnet_cmd_ping {
    /* ICMP raw socket used to send and receive ICMP messages */
    int                       sock;

    /* IP_TRUE : send/receive timestamp request/responses
       IP_FALSE: send/receive echo request/response */
    Ip_bool                   tstamp;

    /* Current state of this session */
    enum Ipnet_cmd_ping_state state;

    /* Error code if sending failed */
    int                       error;

    /* Brief description of the error */
    const char               *err_desc;

    /* Egress interface for the request. 0 means that the stack should
       choose the egress interface. */
    int                       ifindex;

    /* Address of the node the requests are sent to */
    struct Ip_sockaddr_in     to;

   /* Time when this session started */
    struct Ip_timeval         tv_start;

    /* Time when this session is done */
    unsigned                  msec_end;

    /* Time between echo/timestamp requests */
    unsigned                  msec_interval;

    /* Identifier of this session */
    Ip_u16                    id;

    /* Last sequence number to send, might be
       >IPNET_CMD_PING_MAX_SEQ, which means that the same sequence
       number will be reused. */
    unsigned                  last_seq;

    /* Sequence number of next request */
    unsigned                  seq;

    /* Response map, bit B is set when/if a response with sequence
       number B is received. This bit map will keep track of all
       possible sequence numbers in a session. */
    char                      response_map[(IPNET_CMD_PING_MAX_SEQ+8)/8];

    /* Length of the request, ICMP header + payload. Minimum length is
       8 since that it the size of the ICMP header. */
    Ip_size_t                 request_len;

    /* Temporary buffer sending and receiving */
    void                     *data;

    /* Length of "data" */
    Ip_size_t                 data_len;

    /* IP options */
    char                      ip_opts[40];

    /* Timeout queue, used to keep track of request transmissions */
    Ipcom_pqueue             *tmo_head;

    /* Timer for the next echo/timestamp request */
    Ipcom_tmo2                tmo;

    /* Statistics of this session */
    Ipnet_cmd_ping_stat_t     stat;
};


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

Ipnet_cmd_ping_stat_t usrStat;

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC void
ipnet_ping_schedule_echo_request(struct Ipnet_cmd_ping *session);

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
 *                        ipnet_ping_error
 *===========================================================================
 * Description: Sets session error
 * Parameters:  session - a session
 *              desc - description of the error.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping_error(struct Ipnet_cmd_ping *session,
                 const char *desc)
{
    /* Close the socket so the tmo2 machinery shuts down
       immediately */
    ipcom_socketclose(session->sock);
    session->sock     = IP_INVALID_SOCKET;
    session->state    = IPNET_CMD_PING_FAILED;
    session->error    = ipcom_errno;
    session->err_desc = desc;
}


/*
 *===========================================================================
 *                      ipnet_ping_get_response_time
 *===========================================================================
 * Description: Calculates the time since the message with the specified
 *              sequence number was sent.
 * Parameters:  session - a session
 *              seq - sequence number of the received response
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_ping_get_response_time(struct Ipnet_cmd_ping *session, Ip_u16 seq)
{
    unsigned full_seq; /* Most likely sequence number if it was not
                          trunkated to 16-bit */


    full_seq = (session->seq / IPNET_CMD_PING_SEQ_RANGE)
        * IPNET_CMD_PING_SEQ_RANGE;
    if ((session->seq % IPNET_CMD_PING_SEQ_RANGE) < seq)
        /* Sent sequence number has wrapped */
        full_seq -= IPNET_CMD_PING_SEQ_RANGE;
    full_seq += seq;

    return ipnet_cmd_msec_since(&session->tv_start)
        - full_seq * session->msec_interval;
}


/*
 *===========================================================================
 *                       ipnet_ping_update_timeout
 *===========================================================================
 * Description: Calculates how much of this session that is left
 * Parameters:  session - a session
 *              tmo - will contain how much of this session that is left
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping_update_timeout(struct Ipnet_cmd_ping *session,
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
 *                    ipnet_ping_print_ip_opts
 *===========================================================================
 * Description: Prints the IP options
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping_print_ip_opts(char *ip_opts)
{
    int i, j, p;
    int flags_type;
    Ip_size_t ts_len;
    char addr[IP_INET_ADDRSTRLEN];

    for (i = 0; i < 40; )
    {
        switch (ip_opts[i])
        {
        case IP_IPOPT_END:
            return;
        case IP_IPOPT_NOOP:
            i++;
            break;
        case IP_IPOPT_TIMESTAMP:
            ipcom_printf("TS:");
            p = ip_opts[i + 2] - 1;
            flags_type = ip_opts[i + 3];
            if ((flags_type & 0xf) == IP_IPOPT_TS_TSONLY)
                ts_len = 4;
            else
                ts_len = 8;

            for (j = 4; j < p;)
            {
                if (ts_len == 8)
                    ipcom_printf("\t%s", ipcom_inet_ntop(IP_AF_INET,
                                                         &ip_opts[i + j],
                                                         addr,
                                                         sizeof(addr)));
                j += ts_len;
                ipcom_printf("\t%u"IP_LF, IP_GET_NTOHL(&ip_opts[i + j - 4]));
            }
            if (flags_type >> 4)
                ipcom_printf("\tUnrecorded hops: %u"IP_LF, flags_type >> 4);

            /* Fall through */
        default:
            i += ip_opts[i + 1];
            break;
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_ping_send_echo_request
 *===========================================================================
 * Description: Sends one echo or timestamp request.
 * Parameters:  session - state of current session.
 * Returns:     IP_FALSE - don't reschedule this timeout. Rescheduling is
 *                         handled manually.
 *
 */
IP_STATIC Ip_bool
ipnet_ping_send_echo_request(struct Ipnet_cmd_ping *session)
{
    Ipnet_pkt_icmp       *echo_req;
    int                   ret;
    Ip_size_t             msg_len = session->request_len;
    Ip_size_t             data_len = msg_len - 8;
    char                 *ping_data;
    struct Ip_msghdr      m;
    struct Ip_iovec       iov;
    Ip_u8                 cmsg_buf[IP_CMSG_SPACE(sizeof(struct Ip_in_pktinfo))];
    struct Ip_cmsghdr    *c;
    struct Ip_in_pktinfo *pktinfo;
    Ip_u16           seq;

    /* Calculate the next sequence number to use */
    seq = session->seq++ % IPNET_CMD_PING_SEQ_RANGE;

    echo_req = session->data;

    ping_data = (char*) echo_req->data.echo.data;
    echo_req->code          = 0;
    echo_req->data.echo.id  = ip_htons(session->id);
    echo_req->data.echo.seq = ip_htons(seq);
    echo_req->cksum         = 0;

    if (session->tstamp)
    {
        struct Ip_timeval tv_now;
        Ip_u32            msec;

        echo_req->type = IPNET_ICMP4_TYPE_TSTAMP_REQUEST;
        ipcom_microtime(&tv_now);
        msec = IPNET_TIMEVAL_TO_MSEC(tv_now);
        IP_SET_HTONL(echo_req->data.timestamp.originate, msec);
    }
    else
    {
        Ip_size_t i;

        echo_req->type = IPNET_ICMP4_TYPE_ECHO_REQUEST;
        for (i = 0; i < data_len; i++)
            ping_data[i] = i % (127 - ' ') + ' ';
    }

    echo_req->cksum = ipcom_in_checksum(echo_req, (Ip_size_t)msg_len);

    c = (struct Ip_cmsghdr *) cmsg_buf;
    c->cmsg_len   = IP_CMSG_LEN(sizeof(struct Ip_in_pktinfo));
    c->cmsg_level = IP_IPPROTO_IP;
    c->cmsg_type  = IP_IP_PKTINFO;
    pktinfo = IP_CMSG_DATA(c);
    ipcom_memset(pktinfo, 0, sizeof(*pktinfo));
    pktinfo->ipi_ifindex = session->ifindex;

    iov.iov_base = echo_req;
    iov.iov_len  = msg_len;
    m.msg_name       = &session->to;
    m.msg_namelen    = IPCOM_SA_LEN_GET(&session->to);
    m.msg_iov        = &iov;
    m.msg_iovlen     = 1;
    m.msg_control    = c;
    m.msg_controllen = sizeof(cmsg_buf);
    m.msg_flags      = 0;

    ret = ipcom_sendmsg(session->sock, &m, IP_MSG_DONTWAIT);

    if (ret < 0 && ipcom_errno != IP_ERRNO_EWOULDBLOCK)
        ipnet_ping_error(session, "sendmsg()");
    else
    {
        session->state = IPNET_CMD_PING_PINGING;
        ipnet_bit_array_clr(session->response_map, seq);
        ipnet_ping_stat_transmit(&session->stat);
        ipnet_ping_stat_transmit(&usrStat);
        ipnet_ping_schedule_echo_request(session);
    }

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_ping_schedule_echo_request
 *===========================================================================
 * Description: Schedules a new ping request transmission
 * Parameters:  session - a session
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping_schedule_echo_request(struct Ipnet_cmd_ping *session)
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
                       (Ipcom_tmo2_func) ipnet_ping_send_echo_request,
                       session,
                       timeout);
}


/*
 *===========================================================================
 *                         ipnet_ping_process_msg
 *===========================================================================
 * Description: Checks if this ICMP message should be processed
 * Parameters:  session - current session
 *              icmp - ICMP header
 *              icmp_len - total length of this ICMP message
 * Returns:     IP_TRUE - this message should be processed
 *              IP_FALSE - this message should be ignored
 *
 */
IP_STATIC Ip_bool
ipnet_ping_process_msg(struct Ipnet_cmd_ping *session,
                       Ipnet_pkt_icmp *icmp,
                       Ip_size_t icmp_len)
{
    if (icmp->type == (session->tstamp
                       ? IPNET_ICMP4_TYPE_TSTAMP_REPLY
                       : IPNET_ICMP4_TYPE_ECHO_REPLY)
        && ip_ntohs(icmp->data.echo.id) == session->id)
        /* Response to one of our requests */
        return IP_TRUE;

    if (icmp->type == IPNET_ICMP4_TYPE_DST_UNREACHABLE
        && icmp_len > IPNET_ICMP4_HDR_LEN + sizeof(Ipnet_pkt_ip))
    {
        /* Check if the embedded IP datagram contains something that
           belongs to this session */
        Ipnet_pkt_ip *e_ip;
        Ip_size_t     e_ip_len;

        e_ip = (Ipnet_pkt_ip *) icmp->data.failing_pkt.ip;
        e_ip_len = (e_ip->v_hl & 0x0F) * 4;
        if (ipcom_memcmp(e_ip->dst, &session->to.sin_addr, 4) == 0
            && icmp_len >= IPNET_ICMP4_HDR_LEN * 2 + e_ip_len)
        {
            Ipnet_pkt_icmp *e_icmp = (Ipnet_pkt_icmp *) ((char *) e_ip + e_ip_len);
            return session->id == ip_htons(e_icmp->data.echo.id)
                && session->seq > ip_htons(e_icmp->data.echo.seq);
        }
    }

    return IP_FALSE;
}


/*
 *===========================================================================
 *                       ipnet_ping_recv_echo_reply
 *===========================================================================
 * Description: Receives a single ICMP message.
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
ipnet_ping_recv_echo_reply(struct Ipnet_cmd_ping *session,
                           struct Ip_sockaddr_in *from,
                           int *hlim,
                           Ip_u16 *seq,
                           Ip_u32 *receive_ts,
                           Ip_u32 *transmit_ts,
                           Ip_bool *dup,
                           Ip_bool silent)
{
    int             totlen;
    Ip_socklen_t    fromlen = sizeof(*from);
    Ipnet_pkt_ip   *ip;
    Ipnet_pkt_icmp *icmp;
    char           *data = session->data;

    *dup = IP_FALSE;

    totlen = ipcom_recvfrom(session->sock,
                            data,
                            session->data_len,
                            0,
                            (struct Ip_sockaddr*) from,
                            &fromlen);
    if (totlen < 0)
    {
        ipnet_ping_error(session, "recvfrom()");
        return -1;
    }

    ip = (Ipnet_pkt_ip*) data;
    icmp = (Ipnet_pkt_icmp*) (data + (ip->v_hl & 0x0F) * 4);

    if (!ipnet_ping_process_msg(session, icmp, totlen - sizeof(*ip)))
        /* Received ICMP message is not part of this session,
           ignore */
        return 0;

    if (icmp->type == IPNET_ICMP4_TYPE_DST_UNREACHABLE)
    {
        char        straddr[IP_INET_ADDRSTRLEN];
        const char *format = IP_NULL;

        switch (icmp->code)
        {
        case IPNET_ICMP4_CODE_DST_UNREACH_NET:
            format = "Network unreachable received from %s"IP_LF;
            break;
        case IPNET_ICMP4_CODE_DST_UNREACH_HOST:
            format = "Host unreachable received from %s"IP_LF;
            break;
        case IPNET_ICMP4_CODE_DST_NEEDFRAG:
            format = "Need frag received from %s"IP_LF;
            break;
        case IPNET_ICMP4_CODE_DST_PROHIBITED_NET:
            format = "Prohibited network received from %s"IP_LF;
            break;
        case IPNET_ICMP4_CODE_DST_PROHIBITED_HOST:
            format = "Prohibited host received from %s"IP_LF;
            break;
        default:
            /* Ignore other unreachable codes. */
            return 0;
        }
        totlen = 0;
        if (!silent && format != IP_NULL)
            ipcom_printf(format, ipcom_inet_ntop(IP_AF_INET,
                                                 ip->src,
                                                 straddr,
                                                 sizeof(straddr)));
        return 0;
    }

    /* Received respose to our echo/timestamp request */
    if ((ip->v_hl & 0x0F) == 5)
        session->ip_opts[0] = IP_IPOPT_END;
    else
        ipcom_memcpy(session->ip_opts, ip + 1, ((ip->v_hl & 0x0F) - 5) * 4);

    *hlim = ip->ttl;
    if (session->tstamp)
    {
        ipcom_memcpy(receive_ts, icmp->data.timestamp.receive, sizeof(Ip_u32));
        *receive_ts = ip_htonl(*receive_ts);
        ipcom_memcpy(transmit_ts, icmp->data.timestamp.transmit, sizeof(Ip_u32));
        *transmit_ts = ip_htonl(*transmit_ts);
    }

    *seq = ip_ntohs(icmp->data.echo.seq);

#if IPNET_CMD_PING_MAX_SEQ < 0xffff
    if (*seq > IPNET_CMD_PING_MAX_SEQ)
        /* This cannot be a response to something this node sent */
        return 0;
#endif

    *dup = ipnet_bit_array_test_and_set(session->response_map, *seq);

    return totlen - sizeof(Ipnet_pkt_ip);
}

/*
 *===========================================================================
 *                    ipnet_ping_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ping_print_usage(void)
{
    ipcom_printf(IP_LF
                 "NAME" IP_LF
                 "     ping - send ICMPv4 ECHO_REQUEST packets to network hosts" IP_LF
                 IP_LF
                 "SYNOPSIS" IP_LF
                 "     ping [-silent] [-ADbnrx] [-I ifname] [-S src_addr] [-T tsonly|tsandaddr|tsprespec]"IP_LF
                 "          [-Q tos] [-V vr] [-c count] [-s packetsize] [-t ttl] [-w ms] host" IP_LF
                 IP_LF
                 "DESCRIPTION" IP_LF
                 "     ping uses the ICMPv4 protocol's mandatory ECHO_REQUEST datagram to" IP_LF
                 "     elicit an ECHO_REPLY from a host or gateway." IP_LF
                 IP_LF
                 "     -A      Add router alert option to each sent ping request." IP_LF
                 "     -b      Allow pinging a broadcast address." IP_LF
                 "     -c count" IP_LF
                 "             Stop after sending count ECHO_RESPONSE packets." IP_LF);
    ipcom_printf("     -D      Set the DF (don't fragment) flag on each sent ping request." IP_LF
                 "     -I ifname" IP_LF
                 "             Specify outgoing interface." IP_LF
                 "     -n      Numeric output only. No attempts will be made to look up" IP_LF
                 "             names for host addresses." IP_LF
                 "     -Q tos  Specify the Type of Service field in the IPv4 header." IP_LF
                 "     -r      Bypass the normal routing tables and send directly to a host on" IP_LF
                 "             an attached network." IP_LF
                 "     -R      Record route. Includes the RECORD_ROUTE option in the ECHO_REQUEST"IP_LF
                 "             packet. IPNET will discard ECHO_REQUEST that contains RECORD_ROUTE"IP_LF
                 "             Note2: the record route IP option cannot hold more than 9 hops."IP_LF
                 "             Many nodes ignore or discard this option."IP_LF
                 "     -s packetsize" IP_LF
                 "             Specifies the number of data bytes to be sent. The default is" IP_LF
                 "             56, which translates into 64 ICMP data bytes when combined with" IP_LF
                 "             the 8 bytes of ICMP header data." IP_LF
                 "     -S src_addr  Specifies the address to use a source address when sending." IP_LF
                 "     -t ttl  Use the specified time-to-live." IP_LF
                 "     -T timestamp options" IP_LF
                 "             Include IP timestamp option. timestamp can be tsonly (only timestamp)" IP_LF
                 "             tsandaddr (address and timestamp) or" IP_LF
                 "             tsprespec host1 [host2 [host3 [host4]]] (timestamp specific hops)" IP_LF
                 "     -x      Send a ICMP timestamp request instead of a ECHO request." IP_LF
                 "     -V vr   Specify virtual router. 0 if not specified."IP_LF
                 "     -w timeout in milliseconds, default is 1000 ms."IP_LF
                 "     -silent no output, returns success or failure."IP_LF
                 IP_LF);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_ping
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_ping_internal(int argc, char **argv, int *stop)
{
    Ip_fd                 sock = IP_SOCKERR;
    int                   val;
    Ip_u8                 val_u8;
    int                   i;
    int                   ttl = 0;
    unsigned              count = 1;
    struct Ip_sockaddr_in from;
    struct Ip_timeval     tv;
    char                  str_addr[IP_INET_ADDRSTRLEN];
    char                  targetaddr[IP_INET_ADDRSTRLEN];
    char                  targetname[256];
    int                   err = 0;
    int                   exit_status = IPCOM_SHELL_ERR;
    int                   no_dns = IP_FALSE;
    Ipcom_getopt          opt;
    Ip_bool               silent = IP_FALSE;
    struct Ipnet_cmd_ping *session;
    Ip_fd_set             rd_set;
    char                 *ip_opts;
    Ip_bool               print_stat = IP_FALSE;

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
        ipnet_ping_print_usage();
        exit_status = IPCOM_SHELL_ERR_USAGE;
        goto errout;
    }

    /* Set default values */
    session->state         = IPNET_CMD_PING_START;
    session->request_len   = 64;
    session->id            = (Ip_u16) ipcom_random();
    session->msec_interval = 1000;
    IPCOM_TMO2_INIT(&session->tmo);

    sock = ipcom_socket(IP_AF_INET, IP_SOCK_RAW, IP_IPPROTO_ICMP);
    session->sock = sock;
    if (sock == IP_SOCKERR)
    {
        ipnet_ping_error(session, "ipcom_socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)");
        goto errout;
    }

    /* Check for silent mode */
    if(ipcom_strcmp(argv[1], "-silent") == 0)
    {
        argc--;
        argv++;
        silent = IP_TRUE;
        if(argc < 2)
            goto errout;
    }

    ipcom_getopt_clear_r(&opt);
    while ((i = ipcom_getopt_r(argc, argv, "Abc:DI:nQ:rRs:S:t:T:xV:w:", &opt)) != -1)
    {
        switch (i)
        {
        case 'A':
            val_u8 = 1;
            err = ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_ROUTER_ALERT, &val_u8, sizeof(val_u8));
            if (err < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(IP_ROUTER_ALERT)");
                goto errout;
            }
            break;
        case 'b':
            err = 1;
            err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_BROADCAST, &err, sizeof(err));
            if (err < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(SO_BROADCAST)");
                goto errout;
            }
            break;
        case 'c':
            count = ipcom_atoi(opt.optarg);
            if (count < 1)
            {
                ipcom_printf("count must be >0"IP_LF);
                goto errout;
            }
            break;
        case 'D':
            val_u8 = 1;
            err = ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_DONTFRAG, &val_u8, sizeof(val_u8));
            if (err < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(IP_DONTFRAG)");
                goto errout;
            }
            break;
        case 'I':
            session->ifindex = ipcom_if_nametoindex(opt.optarg);
            if (session->ifindex == 0)
            {
                ipcom_snprintf(session->data, session->data_len, "%s is an unknown interface.", opt.optarg);
                ipnet_ping_error(session, session->data);
                goto errout;
            }
            break;
        case 'n':
            no_dns = IP_TRUE;
            break;
        case 'Q':
            val = ipcom_atoi(opt.optarg);
            err = ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_TOS, &val, sizeof(val));
            if (err < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(IP_TOS)");
                goto errout;
            }
            break;
        case 'r':
            val = 1;
            if (ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_DONTROUTE, &val, sizeof(val)) < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(SO_DONTROUTE)");
                goto errout;
            }
            break;
        case 'R':
            ip_opts = session->ip_opts;
            ipcom_memset(ip_opts, IP_IPOPT_END, sizeof(session->ip_opts));
            /* Align the IP addresses on even 4 bytes */
            ip_opts[0] = IP_IPOPT_NOOP;
            ip_opts[1] = IP_IPOPT_RR;
            ip_opts[2] = 39; /* Option length */
            ip_opts[3] = 4;  /* Next router address (offset from option start) */
            err = ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_OPTIONS, ip_opts, ip_opts[2]+1);
            if (err < 0)
            {
                ipcom_printf("ipcom_setsockopt(IP_OPTIONS) IPOPT_RR");
                goto errout;
            }
            break;
        case 's':
            session->request_len = ipcom_atoi(opt.optarg);
            if (session->request_len < IPNET_ICMP4_HDR_LEN)
            {
                ipcom_printf("Data length must be %d or greater." IP_LF, IPNET_ICMP4_HDR_LEN);
                goto errout;
            }
            if (session->request_len > 0xffff - IPNET_IP_HDR_SIZE)
            {
                ipcom_printf("Data length must be %d or less." IP_LF, 0xffff - IPNET_IP_HDR_SIZE);
                goto errout;
            }
            break;
        case 'S':
            from.sin_family = IP_AF_INET;
            IPCOM_SA_LEN_SET(&from, sizeof(from));
            from.sin_port   = 0;
            (void)ipcom_inet_pton(IP_AF_INET, opt.optarg, &from.sin_addr);
            err = ipcom_bind(sock, (struct Ip_sockaddr *) &from, sizeof(from));
            if (err < 0)
            {
                ipnet_ping_error(session, session->data);
                ipcom_snprintf(session->data, session->data_len, "ipcom_bind(%s)", opt.optarg);
                goto errout;
            }
            break;
        case 'T':
            ip_opts = session->ip_opts;
            ipcom_memset(ip_opts, IP_IPOPT_END, sizeof(session->ip_opts));
            if (ipcom_strcmp("tsonly", opt.optarg) == 0)
            {
                ip_opts[1] = 40;
                ip_opts[3] = IP_IPOPT_TS_TSONLY;
            }
            else if (ipcom_strcmp("tsandaddr", opt.optarg) == 0)
            {
                ip_opts[1] = 36;
                ip_opts[3] = IP_IPOPT_TS_TSANDADDR;
            }
            else if (ipcom_strcmp("tsprespec", opt.optarg) == 0)
            {
                ip_opts[1] = 4;
                ip_opts[3] = IP_IPOPT_TS_PRESPEC;
                for (i = opt.optind; i < argc - 1; i++)
                {
                    if (ipcom_inet_pton(IP_AF_INET, argv[i], &ip_opts[4 + (i - opt.optind) * 8]) <= 0)
                    {
                        ipcom_printf("'%s' is not a valid IPv4 address"IP_LF, argv[i]);
                        goto errout;
                    }
                    ip_opts[1] += 8;
                }
                opt.optind = i;
            }
            else
            {
                ipcom_printf("'%s' is not a valid timestamp format"IP_LF, opt.optarg);
                goto errout;
            }
            ip_opts[0] = IP_IPOPT_TIMESTAMP;
            ip_opts[2] = 5;
            err = ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_OPTIONS, ip_opts, ip_opts[1]);
            if (err < 0)
            {
                ipcom_printf("ipcom_setsockopt(IP_OPTIONS) IPOPT_TIMESTAMP");
                goto errout;
            }
            break;
        case 't':
            val = ipcom_atoi(opt.optarg);
            err = ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_TTL, &val, sizeof(val));
            if (err < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(IP_TTL)");
                goto errout;
            }
            val_u8 = (Ip_u8)val;
            err = ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_MULTICAST_TTL, &val_u8, sizeof(val_u8));
            if (err < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(IP_MULTICAST_TTL)");
                goto errout;
            }
            break;
        case 'x':
            session->tstamp = IP_TRUE;
            break;
        case 'V' :
            val = ipcom_atoi(opt.optarg);
            err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_X_VR, &val, sizeof(val));
            if (err < 0)
            {
                ipnet_ping_error(session, "ipcom_setsockopt(SO_X_VR)");
                goto errout;
            }
            break;
        case 'w':
            session->msec_interval = ipcom_atoi(opt.optarg);
            if (session->msec_interval == 0)
            {
                ipcom_printf("Interval between requests must be >0");
                goto errout;
            }
            break;
        default:
            ipcom_printf("'%c' is an unknown switch" IP_LF, i);
            exit_status = IPCOM_SHELL_ERR_USAGE;
            goto errout;
        }
    }

    if (session->tstamp)
        /* Timestamp request cannot contain payload */
        session->request_len = 5 * sizeof(Ip_u32);

    /* Send/receive buffer must be large enough to receive IP+ICMP
       header and possibly an embedded IP+ICMP+payload datagram */
    session->data_len = (IPNET_ICMP4_HDR_LEN * 2
                         + sizeof(Ipnet_pkt_ip)
                         + sizeof(session->ip_opts)
                         + session->request_len);
    session->data = ipcom_malloc(session->data_len);
    if (session->data == IP_NULL)
    {
        ipcom_printf("Failed to allocate %u bytes", session->data_len);
        goto errout;
    }

    /* Set the send buffer large enough to send any valid ICMP packet */
    val = 65535;
    err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_SNDBUF, &val, sizeof(val));
    if (err < 0)
    {
        ipnet_ping_error(session, "ipcom_setsockopt(SO_SNDBUF)");
        goto errout;
    }

    err = ipcom_getsockaddrbyaddrname(IP_AF_INET,
                                      no_dns,
                                      argv[argc-1],
                                      (struct Ip_sockaddr *) &session->to);
    if (err)
    {
        ipcom_printf("'%s' is not a valid IPv4 address"IP_LF, argv[argc-1]);
        goto errout;
    }

    (void)ipcom_cmd_sockaddr_to_str((struct Ip_sockaddr *) &session->to,
                                    IP_FALSE, IP_TRUE, targetaddr, sizeof(targetaddr));
    (void)ipcom_cmd_sockaddr_to_str((struct Ip_sockaddr *) &session->to,
                                    IP_FALSE, no_dns, targetname, sizeof(targetname));
    if (!silent)
        ipcom_printf("\nPinging %s (%s) with %d bytes of data:" IP_LF,
                     targetname,
                     targetaddr,
                     session->request_len);

    ipnet_ping_stat_init(&session->stat);
    ipnet_ping_stat_init(&usrStat);
    print_stat = IP_TRUE;

    session->last_seq = (Ip_u16) (count - 1);

    /* Calculate the absolute time when this ping session must end */
    ipcom_microtime(&session->tv_start);
    session->msec_end = session->msec_interval * count;

    /* Send first request and start periodic timer */
    (void)ipnet_ping_send_echo_request(session);
    ipnet_ping_schedule_echo_request(session);

    IP_FD_ZERO(&rd_set);

    for (IP_FD_SET(sock, &rd_set), ipnet_ping_update_timeout(session, &tv);
         session->state == IPNET_CMD_PING_PINGING
             && ipcom_tmo2_select(session->tmo_head,
                                  sock + 1,
                                  &rd_set,
                                  IP_NULL,
                                  IP_NULL,
                                  &tv) > 0;
         IP_FD_SET(sock, &rd_set), ipnet_ping_update_timeout(session, &tv))
    {
        unsigned msec;
        Ip_u16   seq = 0;
        Ip_u32   receive_ts;
        Ip_u32   transmit_ts;
        int      res_size;
        Ip_bool  dup;

        ip_assert(IP_FD_ISSET(sock, &rd_set));
        if (stop && (*stop == IPNET_PING_CMD_REQUEST_STOP))
        {
            *stop = IPNET_PING_CMD_STOPPED;
            goto errout;
        }

        res_size = ipnet_ping_recv_echo_reply(session,
                                              &from,
                                              &ttl,
                                              &seq,
                                              &receive_ts,
                                              &transmit_ts,
                                              &dup,
                                              silent);
        if (res_size < 0)
            break;
        if (res_size == 0)
            continue;

        /* Got at least one response */
        exit_status = IPCOM_SHELL_SUCCESS;

        if (session->last_seq == seq)
            /* Got a response on the last request we are going to
               send */
            session->state = IPNET_CMD_PING_DONE;

        msec = ipnet_ping_get_response_time(session, seq);

        if (!silent)
        {
            ipcom_printf("Reply from %s ",
                         ipcom_inet_ntop(IP_AF_INET,
                                         &from.sin_addr,
                                         str_addr,
                                         sizeof(str_addr)));

            if (!session->tstamp)
                ipcom_printf("bytes=%d ", res_size);

            ipcom_printf("ttl=%d seq=%d",
                         ttl,
                         seq);

            if (session->tstamp)
                ipcom_printf(" receive TS=%lu transmit TS=%lu",
                             (unsigned)receive_ts,
                             (unsigned)transmit_ts);
            ipcom_printf(" time=%dms%s" IP_LF,
                         msec,
                         dup ? " (DUP!)" : "");
            ipnet_ping_print_ip_opts(session->ip_opts);
        }

        ipnet_ping_stat_receive(&session->stat, msec, dup);
        ipnet_ping_stat_receive(&usrStat, msec, dup);
    }

 errout:
    if (session != IP_NULL && session->state == IPNET_CMD_PING_FAILED)
    {
        err = session->error;
        if (err != IP_ERRNO_ENOBUFS && !silent)
            /* Packet dropped due to missing NDP response, can happens in v4->v6 tunnels */
            ipcom_printf("%s: operation failed: %s (%d)" IP_LF,
                         session->err_desc, ipcom_strerror(err), err);
    }
    else if (!silent && print_stat)
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

IP_PUBLIC int
ipnet_cmd_ping_stoppable(int argc, char **argv, int *stop)
{
    return ipnet_cmd_ping_internal(argc, argv, stop);
}

IP_PUBLIC int
ipnet_cmd_ping(int argc, char **argv)
{
    return ipnet_cmd_ping_internal(argc, argv, IP_NULL);
}

#endif /* IPCOM_USE_INET */


/*
****************************************************************************
*                      END OF FILE
****************************************************************************
*/

