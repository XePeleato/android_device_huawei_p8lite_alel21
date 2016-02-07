/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_tracert.c,v $ $Revision: 1.15.20.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_tracert.c,v $
 *   $Author: ulf $
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
 * Interpeak trace route command for IPv4 and IPv6.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/*
 * UNIX traceroute uses 33434, tracepath uses 44444. Any port should in theory
 * work equally well.
 */
#define IPCOM_CMD_TRACERT_BASE_PORT "33434"

/*
 * Number of bytes to include as payload in each probe.
 * UNIX traceroute includes 40 bytes
 */
#define IPCOM_CMD_TRACERT_DATA_LEN 40

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_getopt.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_time.h>
#include <ipcom_type.h>
#ifdef IPCOM_USE_INET6
#include <ipcom_sock6.h>
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_CMD_TRACERT_RESPONSE_ROUTER     0
#define IPCOM_CMD_TRACERT_RESPONSE_END_NODE   1
#define IPCOM_CMD_TRACERT_NO_RESPONSE         2
#define IPCOM_CMD_TRACERT_WRONG_RESPONSE      3


#define IPCOM_CMD_TRACERT_ICMP_TYPE_DST_UNREACH    3
#define IPCOM_CMD_TRACERT_ICMP_TYPE_TIME_EXCEEDED  11

#define IPCOM_CMD_TRACERT_ICMP_CODE_UNREACH_PORT   3


#define IPCOM_CMD_TRACERT_ICMP6_TYPE_DST_UNREACH    1
#define IPCOM_CMD_TRACERT_ICMP6_TYPE_TIME_EXCEEDED  3

#define IPCOM_CMD_TRACERT_ICMP6_CODE_UNREACH_PORT   4


#ifdef IPCOM_USE_INET6
#define IPCOM_CMD_TRACERT_ADDSTRLEN  IP_INET6_ADDRSTRLEN
#else
#define IPCOM_CMD_TRACERT_ADDSTRLEN  IP_INET_ADDRSTRLEN
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef int (*Ipcom_cmd_tracert_icmp_func)(Ip_fd isock, union Ip_sockaddr_union *remote_addr);


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipcom_cmd_tracert(int argc, char **argv);

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
 *                    ipcom_cmd_tracert_tv_to_msec
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC int
ipcom_cmd_tracert_tv_to_msec(struct Ip_timeval *start, struct Ip_timeval *stop)
{
    int sec;
    int usec;

    usec = stop->tv_usec - start->tv_usec;
    sec = stop->tv_sec - start->tv_sec;
    if (usec < 0)
    {
        usec += 1000000;
        sec -= 1;
    }
    return sec * 1000 + usec / 1000;
}


/*
 *===========================================================================
 *                     ipcom_cmd_tracert_icmp4
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET
IP_STATIC int
ipcom_cmd_tracert_icmp4(Ip_fd isock, union Ip_sockaddr_union *remote_addr)
{
    Ip_u8   buf[100];
    Ip_u8  *v_hl;
    Ip_u8   icmp_type = 0;
    Ip_u8   icmp_code = 0;
    int     icmp_hdr_offset;
    int     bytes_recv;

    bytes_recv = ipcom_recv(isock, buf, sizeof(buf), 0);

    v_hl = (Ip_u8 *) buf;
    icmp_hdr_offset = (*v_hl & 0x0F) * 4;

    if (bytes_recv >= 20 && bytes_recv >= icmp_hdr_offset + 8)
    {
        /* Got both IPv4 and ICMP header */
        remote_addr->sin.sin_family = IP_AF_INET;
        IPCOM_SA_LEN_SET(&remote_addr->sin, sizeof(*remote_addr));
        ipcom_memcpy(&remote_addr->sin.sin_addr, &buf[12], sizeof(Ip_u32));
        icmp_type = buf[icmp_hdr_offset];
        icmp_code = buf[icmp_hdr_offset + 1];
    }

    switch (icmp_type)
    {
    case IPCOM_CMD_TRACERT_ICMP_TYPE_DST_UNREACH:
        if (icmp_code != IPCOM_CMD_TRACERT_ICMP_CODE_UNREACH_PORT)
            return IPCOM_CMD_TRACERT_WRONG_RESPONSE;
        /* Got port unreachable */
        return IPCOM_CMD_TRACERT_RESPONSE_END_NODE;
    case IPCOM_CMD_TRACERT_ICMP_TYPE_TIME_EXCEEDED:
        /* Time exceeded */
        return IPCOM_CMD_TRACERT_RESPONSE_ROUTER;
    default:
        return IPCOM_CMD_TRACERT_WRONG_RESPONSE;
    }
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                     ipcom_cmd_tracert_icmp6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipcom_cmd_tracert_icmp6(Ip_fd isock, union Ip_sockaddr_union *remote_addr)
{
    Ip_u8              buf[20];
    Ip_socklen_t       from_len = sizeof(union Ip_sockaddr_union);
    Ip_u8              icmp_type = 0;
    Ip_u8              icmp_code = 0;
    int                bytes_recv;

    bytes_recv = ipcom_recvfrom(isock, buf, sizeof(buf), 0, &remote_addr->sa, &from_len);

    if (bytes_recv >= 8)
    {
        icmp_type = buf[0];
        icmp_code = buf[1];
    }

    switch (icmp_type)
    {
    case IPCOM_CMD_TRACERT_ICMP6_TYPE_DST_UNREACH:
        if (icmp_code != IPCOM_CMD_TRACERT_ICMP6_CODE_UNREACH_PORT)
            return IPCOM_CMD_TRACERT_WRONG_RESPONSE;
        /* Got port unreachable */
        return IPCOM_CMD_TRACERT_RESPONSE_END_NODE;
    case IPCOM_CMD_TRACERT_ICMP6_TYPE_TIME_EXCEEDED:
        /* Time exceeded */
        return IPCOM_CMD_TRACERT_RESPONSE_ROUTER;
    default:
        return IPCOM_CMD_TRACERT_WRONG_RESPONSE;
    }
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                      ipcom_cmd_tracert_ntop
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC const char *
ipcom_cmd_tracert_ntop(struct Ip_sockaddr *sa_addr, char *str, int str_len)
{
    union Ip_sockaddr_union *addr = (union Ip_sockaddr_union *) sa_addr;

    switch (addr->sa.sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipcom_inet_ntop(IP_AF_INET, &addr->sin.sin_addr, str, str_len);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipcom_inet_ntop(IP_AF_INET6, &addr->sin6.sin6_addr, str, str_len);
#endif
    default:
        IP_PANIC();
        return IP_NULL;
    }
}


/*
 *===========================================================================
 *                    ipcom_cmd_tracert_send_recv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC int
ipcom_cmd_tracert_send_recv(Ip_fd usock, Ip_fd isock,
                            IP_CONST struct Ip_sockaddr *peer_addr,
                            int hops, int reps, long timeout_ms,
                            Ip_bool no_dns,
                            int level, int optname,
                            Ipcom_cmd_tracert_icmp_func icmp_handler,
                            const char *payload)
{
    struct Ip_timeval       start;
    struct Ip_timeval       stop;
    int                     bytes_recv;
    Ip_fd_set               read_set;
    int                     ready_socks;
    struct Ip_timeval       tv;
    int                     response = IPCOM_CMD_TRACERT_NO_RESPONSE;
    union Ip_sockaddr_union remote_addr;
    Ip_bool                 print_addr = IP_TRUE;
    Ip_in_port_t            initial_port_n;
    Ip_in_port_t           *port_n;

    if (ipcom_setsockopt(usock, level, optname, &hops, sizeof(hops)) < 0)
        return -1;

    /*
     * The sin_port and sin6_port is located at the same offset, so
     * this do work.
     */
    port_n = &((struct Ip_sockaddr_in *) peer_addr)->sin_port;
    initial_port_n = *port_n;

    IP_FD_ZERO(&read_set);
    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ipcom_printf(" %2d: ", hops);
    while (reps--)
    {
        ipcom_microtime(&start);

        if (ipcom_sendto(usock, payload,
                         IPCOM_CMD_TRACERT_DATA_LEN,
                         0,
                         peer_addr,
                         IPCOM_SA_LEN_GET(peer_addr)) < 0)
            return -1;

        *port_n = ip_htons(ip_ntohs(*port_n) + 1);

    wait_for_next:
        IP_FD_SET(usock, &read_set);
        IP_FD_SET(isock, &read_set);
        ready_socks = ipcom_socketselect(IP_MAX(usock, isock) + 1, &read_set, IP_NULL, IP_NULL, &tv);

        if (ready_socks <= 0)
        {
            /* No response */
            ipcom_printf("     *     ");
            continue;
        }

        ipcom_memset(&remote_addr, 0, sizeof(remote_addr));
        if (IP_FD_ISSET(isock, &read_set))
        {
            int this_response;
            this_response = icmp_handler(isock, &remote_addr);
            if (this_response == IPCOM_CMD_TRACERT_WRONG_RESPONSE)
                goto wait_for_next;
            if (response != IPCOM_CMD_TRACERT_RESPONSE_END_NODE)
                response = this_response;
        }

        if (IP_FD_ISSET(usock, &read_set))
        {
            bytes_recv = ipcom_recv(usock, IP_NULL, 0, 0);

            if (bytes_recv == 0 || ipcom_errno == IP_ERRNO_ECONNREFUSED)
            {
                response = IPCOM_CMD_TRACERT_RESPONSE_END_NODE;
                ipcom_memcpy(&remote_addr, peer_addr, IPCOM_SA_LEN_GET(peer_addr));
            }
        }

        ipcom_microtime(&stop);
        if (print_addr)
        {
            char host[200];
            char host_addr[IPCOM_CMD_TRACERT_ADDSTRLEN];

            print_addr = IP_FALSE;
            if (ipcom_getnameinfo(&remote_addr.sa, sizeof(remote_addr),
                                  host, sizeof(host), IP_NULL, 0,
                                  no_dns ? IP_NI_NUMERICHOST : 0) < 0)
                return -1;
            ipcom_strcat(host, " (");
            ipcom_strcat(host, ipcom_cmd_tracert_ntop(&remote_addr.sa, host_addr, sizeof(host_addr)));
            ipcom_strcat(host, ")");
            ipcom_printf("%-60s ", host);
        }
        ipcom_printf("%6d ms ", ipcom_cmd_tracert_tv_to_msec(&start, &stop));
    }
    ipcom_printf(IP_LF);
    *port_n = initial_port_n;
    return response;
}


/*
 *===========================================================================
 *                    ipcom_cmd_rtsock_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_cmd_rtsock_print_usage(void)
{
    ipcom_printf("Trace route shell command for IPv4 and IPv6"IP_LF
                 "usage:  "IP_LF
                 "   traceroute [options] <node>"IP_LF
                 "      -c <num>  set number of probes per hop limit (default 3)"IP_LF
                 "      -m <hops> set max hops (default 16)"IP_LF
                 "      -n        do not perform DNS lookup on addresses"IP_LF
                 "      -w <sec>  set the max time to wait for answer (default 5 sec)"IP_LF
                 IP_LF
                 );
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_tracert
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_cmd_tracert(int argc, char **argv)
{
    union Ip_sockaddr_union  local;
    Ip_socklen_t             local_len = sizeof(local);
    struct Ip_addrinfo       hints;
    struct Ip_addrinfo      *res = IP_NULL;
    Ip_fd                    usock = IP_SOCKERR;
    Ip_fd                    isock = IP_SOCKERR;
    Ip_bool                  done = IP_FALSE;
    int                      hops = 1;
    int                      max_hops = 16;
    long                     timeout_ms = 5000;
    int                      probes_per_host = 3;
    int                      no_response;
    Ipcom_getopt             opt;
    int                      c;
    int                      icmp_proto;
    Ip_bool                  no_dns = IP_FALSE;
    char                     str_addr[IPCOM_CMD_TRACERT_ADDSTRLEN];
    int                      level;
    int                      optname;
    int                      vr = 0;
    Ipcom_cmd_tracert_icmp_func f;
    int                      i;
    char                     payload[IPCOM_CMD_TRACERT_DATA_LEN];
    static char             *data =
        "1234567890"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    if (argc < 2)
    {
        ipcom_cmd_rtsock_print_usage();
        return 1;
    }

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "c:m:nV:w:", &opt)) != -1)
    {
        switch (c)
        {
        case 'c':
            probes_per_host = ipcom_atoi(opt.optarg);
            break;
        case 'm':
            max_hops = ipcom_atoi(opt.optarg);
            break;
        case 'n':
            no_dns = IP_TRUE;
            break;
        case 'V':
            vr = ipcom_atoi(opt.optarg);
            break;
        case 'w':
            timeout_ms = (long)ipcom_atoi(opt.optarg) * 1000;
            break;
        default:
            ipcom_printf("%s: unknown option %c"IP_LF, argv[0], (char)c);
            return -1;
        }
    }

    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = IP_SOCK_DGRAM;
    if (no_dns)
        hints.ai_flags = IP_AI_NUMERICHOST;
    else
        hints.ai_flags = IP_AI_CANONNAME;
#ifdef IPCOM_USE_INET
    hints.ai_family   = IP_AF_INET;
    icmp_proto = IP_IPPROTO_ICMP;
    level = IP_IPPROTO_IP;
    optname = IP_IP_TTL;
    f = ipcom_cmd_tracert_icmp4;
    if (ipcom_getaddrinfo(argv[argc - 1], IPCOM_CMD_TRACERT_BASE_PORT, &hints, &res) != 0)
#endif
    {
#ifdef IPCOM_USE_INET6
        hints.ai_family = IP_AF_INET6;
        icmp_proto = IP_IPPROTO_ICMPV6;
        level = IP_IPPROTO_IPV6;
        optname = IP_IPV6_UNICAST_HOPS;
        f = ipcom_cmd_tracert_icmp6;
        if (ipcom_getaddrinfo(argv[argc - 1], IPCOM_CMD_TRACERT_BASE_PORT, &hints, &res) != 0)
#endif
        {
            ipcom_printf("getaddrinfo() failed"IP_LF);
            goto cleanup;
        }
    }

    usock = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (usock == IP_SOCKERR)
        goto cleanup;
    if (ipcom_setsockopt(usock, IP_SOL_SOCKET, IP_SO_X_VR, &vr, sizeof(vr)) < 0)
        goto cleanup;

    /* Connect to let the stack select local address to bind to */
    if (ipcom_connect(usock, res->ai_addr, res->ai_addrlen) < 0)
        goto cleanup;

    if (ipcom_getsockname(usock, &local.sa, &local_len) < 0)
        goto cleanup;

    ipcom_socketclose(usock);

    usock = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (usock == IP_SOCKERR)
        goto cleanup;
    if (ipcom_setsockopt(usock, IP_SOL_SOCKET, IP_SO_X_VR, &vr, sizeof(vr)) < 0)
        goto cleanup;

    if (ipcom_bind(usock, &local.sa, local_len) < 0)
        goto cleanup;

    isock = ipcom_socket(res->ai_family, IP_SOCK_RAW, icmp_proto);
    if (isock == IP_SOCKERR)
        goto cleanup;
    if (ipcom_setsockopt(isock, IP_SOL_SOCKET, IP_SO_X_VR, &vr, sizeof(vr)) < 0)
        goto cleanup;

    if (ipcom_bind(isock, &local.sa, local_len) < 0)
        goto cleanup;

    /*
     * Initialize the payload that will be included in each probe
     */
    for (i = 0; i < sizeof(payload); i++)
        payload[i] = data[i % ipcom_strlen(data)];

    ipcom_cmd_tracert_ntop(res->ai_addr, str_addr, sizeof(str_addr));
    ipcom_printf("traceroute to %s (%s) from ",
                 res->ai_canonname ? res->ai_canonname : str_addr,
                 str_addr);
    ipcom_printf("%s, %d hops max" IP_LF,
                 ipcom_cmd_tracert_ntop(&local.sa, str_addr, sizeof(str_addr)),
                 max_hops);

    for (hops = 1, no_response = 0; hops < max_hops && !done; hops++)
    {
        int ret;

        ret = ipcom_cmd_tracert_send_recv(usock,
                                          isock,
                                          res->ai_addr,
                                          hops,
                                          probes_per_host,
                                          timeout_ms,
                                          no_dns,
                                          level,
                                          optname,
                                          f,
                                          payload);
        switch (ret)
        {
        case IPCOM_CMD_TRACERT_RESPONSE_END_NODE:
            /* Response from destination node */
            done = IP_TRUE;
            no_response = 0;
            break;
        case IPCOM_CMD_TRACERT_RESPONSE_ROUTER:
            no_response = 0;
            break;
        case IPCOM_CMD_TRACERT_NO_RESPONSE:
            if (++no_response == 3)
            {
                ipcom_printf("No response from %d nodes, aborting trace..." IP_LF, no_response);
                goto cleanup;
            }
            break;
        default:
            goto cleanup;
        }
    }

 cleanup:
    if (hops < max_hops && !done)
        ipcom_printf("tracert failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
    if (usock != IP_SOCKERR)
        (void)ipcom_socketclose(usock);
    if (isock != IP_SOCKERR)
        (void)ipcom_socketclose(isock);
    ipcom_freeaddrinfo(res);

    return !done;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

