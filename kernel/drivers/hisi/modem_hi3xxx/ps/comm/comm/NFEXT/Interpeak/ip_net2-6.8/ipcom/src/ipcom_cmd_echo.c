/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_echo.c,v $ $Revision: 1.47 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_echo.c,v $
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
 */

/* Shell command to start/stop the echo server
 * Shell command (echoclient) to write, read back and verify data echoed from
 * the echoserver
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

#ifdef IP_PORT_INTEGRITY
#include "ipcom_ipcom2bsd.h"
#endif

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_time.h"
#include "ipcom_cmd.h"
#include "ipcom_os.h"
#include "ipcom_getopt.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_sock6.h"
#include "ipcom_inet.h"
#include "ipcom_err.h"
#include "ipcom_cmd.h"

#ifdef IP_PORT_INTEGRITY
#include "ipcom_ipcom2bsd.h"
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_CMD_ECHO_PORT             7301
#define IPCOM_CMD_SOCK_BUF_SIZE         8192
#define IPCOM_CMD_ECHO_BUF_SIZE         (IPCOM_CMD_SOCK_BUF_SIZE / 2)
#define IPCOM_CMD_ECHO_BUF_COUNT        1
#define IPCOM_CMD_ECHO_SERVER_NAME      "ipcom_echod"


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_cmd_test
 *===========================================================================
 */

typedef struct Ipcom_cmd_test_struct
{
    /* Address family IP_AF_INET or IP_AF_INET6 */
    int     domain;
    /* Socket type IP_SOCK_DGRAM or IP_SOCK_STREAM */
    int     type;
    /* Socket protocol */
    int     protocol;
    /* Should the deamon be stopped? */
    Ip_bool stop_daemon;
    /* Should the socket be connected? */
    Ip_bool connect;
    /* Should the send command read the server reply? */
    Ip_bool read_reply;
    /* Should the server close the socket after first echo */
    Ip_bool echo_once;
    /* Total number of bytes to send/maximum number to receive */
    int     buf_size;
    /* Size of send and recv buffers */
    int     sock_buf_size;
    /* No of buffers to send */
    int     buf_count;
    /* Reuse address */
    int     reuse_addr;
    /* Local port */
    int     local_port;
    /* An address */
    union Ip_sockaddr_union    addr;
    /* The interface to receive multicast packets */
    union
    {
        struct Ip_in_addr    in;
        int                     ifindex;
    } mcast_if;
    /* Print elapsed time*/
    Ip_bool verify_recv_data;
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
    /* Zero copy API */
    Ip_bool zerocopy;
#endif
}
Ipcom_cmd_test;


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

IP_PUBLIC int ipcom_cmd_echoserver(int argc, char **argv);
IP_PUBLIC int ipcom_cmd_echoclient(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_cmd_int_str_map ipcom_cmd_echo_type_map[] =
{
    { IP_SOCK_DGRAM,     "udp" },
    { IP_SOCK_STREAM,    "tcp" },
    { IP_SOCK_RAW,       "raw" },
    { IP_SOCK_STREAM,    "sctp"},
    { -1, IP_NULL }
};

IP_STATIC Ipcom_cmd_int_str_map ipcom_cmd_echo_proto_map[] =
{
    { IP_IPPROTO_UDP,    "udp" },
    { IP_IPPROTO_TCP,    "tcp" },
    { IP_IPPROTO_RAW,    "raw" },
    { IP_IPPROTO_SCTP,   "sctp"},
    { -1, IP_NULL }
};


IP_STATIC Ipcom_cmd_int_str_map ipcom_cmd_echo_domain_map[] =
{
    { IP_AF_INET,    "inet" },
    { IP_AF_INET6,   "inet6" },
    { -1, IP_NULL }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         get_millisec
 *===========================================================================
 */
static Ip_u32
get_millisec(void)
{
#ifdef IPCOM_USE_TIME
    struct Ip_tms tms;
    return ipcom_times(&tms);
#else
    return 0;
#endif
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_server_print_usage
 *===========================================================================
 * Description: Prints usage for the echoserver shell command.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_echo_server_print_usage(void)
{
    ipcom_printf(IP_LF
                 "NAME" IP_LF
                 "     echoserver - Echo daemon." IP_LF
                 IP_LF
                 "SYNOPSIS" IP_LF
                 "     echoserver [options] addr" IP_LF
                 IP_LF
                 "DESCRIPTION" IP_LF
                 "     echoserver will echo everything written to it" IP_LF
                 IP_LF
                 "     -b <size>" IP_LF
                 "             Size socket send/recv buffer (default is %d)." IP_LF, IPCOM_CMD_SOCK_BUF_SIZE);
    ipcom_printf("     -s <chunk_size>" IP_LF
                 "             Number of bytes written in each sendto() call" IP_LF
                 "             (default is %d)." IP_LF, IPCOM_CMD_ECHO_BUF_SIZE);
    ipcom_printf("     -d <domain>" IP_LF
                 "             Address domain, inet for IPv4 and inet6 for IPv6." IP_LF
                 "             The address domain will be guessed from the address format" IP_LF
                 "             if this option is not used." IP_LF
                 "     -t <proto>" IP_LF
                 "             Transport protocol, udp for UDP (default), tcp for TCP, raw for RAW, sctp for sctp(one to one)" IP_LF
                 "     -p <port>" IP_LF
                 "             Port to listen on (default is %d)." IP_LF, IPCOM_CMD_ECHO_PORT);
    ipcom_printf("     -i <ifname>" IP_LF
                 "             The interface to send and receive multicast packets." IP_LF
                 "     -c" IP_LF
                 "             Connect UDP socket before sending and disconnect after."IP_LF
                 "     -q" IP_LF
                 "             Stop the running echo server (if any)." IP_LF
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
                 "     -z" IP_LF
                 "             Use zero copy API" IP_LF
#endif
                 "     -x"IP_LF
                 "             Echo just once then close socket." IP_LF
                 "     -k"IP_LF
                 "             Set socket option IP_SO_REUSEADDR." IP_LF
                 IP_LF);
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_error_msg
 *===========================================================================
 * Description: Prints a description of the last error.
 * Parameters:  msg - The message to use as a prefix for the message.
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_echo_error_msg(const char *msg)
{
    ipcom_printf("%s: %s" IP_LF, msg, ipcom_strerror(ipcom_errno));
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_client_do
 *===========================================================================
 * Description: Implements the echoclient shell commmand.
 * Parameters:  param - The parameters specified when the shell command was
 *              started.
 * Returns:     0 = success, <0 = error
 *
 */
IP_STATIC int
ipcom_cmd_echo_client_do(Ipcom_cmd_test *param)
{
    Ip_fd sock = IP_SOCKERR;
    char *send_buf = IP_NULL;
    char *recv_buf = IP_NULL;
    int i;
    int x;
    int ret;
    int total_bytes_sent;
    int total_bytes_read;
    int bytes_sent;
    int bytes_read = 0;
    int offset = 0;
    char str[100];
    int err = -1;
#ifdef IP_SO_RCVTIMEO
    struct Ip_timeval tv;
#endif
    Ip_u32 msec_start;
    Ip_u32 msec;
    int read_io_calls = 0;
    int write_io_calls = 0;

#ifdef IP_PORT_INTEGRITY
    ipcom_init_libsocket();
#endif

    if (param->buf_size)
    {
        send_buf = ipcom_malloc(param->buf_size);
        if (send_buf == IP_NULL)
        {
            ipcom_printf("Out of memory");
            goto cleanup;
        }
    }
    else if (param->type == IP_SOCK_STREAM)
    {
        ipcom_printf("Invalid chunksize");
        goto cleanup;
    }
    for (i = 0; i < param->buf_size; i++)
        send_buf[i] = (char)('0' + i % ('z' - '0'));

    sock = ipcom_socket(param->domain, param->type, param->protocol);
    if (sock == IP_INVALID_SOCKET)
    {
        ipcom_cmd_echo_error_msg("Failed to open socket");
        goto cleanup;
    }
    ipcom_printf("\tNew socket: %d" IP_LF, sock);

    err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_SNDBUF, &param->sock_buf_size,
        sizeof(param->sock_buf_size));
    if (err < 0)
    {
        ipcom_printf("ipcom_setsockopt() IP_SO_SNDBUF ");
        goto cleanup;
    }

    err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_RCVBUF, &param->sock_buf_size,
        sizeof(param->sock_buf_size));
    if (err < 0)
    {
        ipcom_printf("ipcom_setsockopt() IP_SO_RCVBUF ");
        goto cleanup;
    }

#ifdef IP_SO_RCVTIMEO
    /* Set receive timeout */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_RCVTIMEO, &tv, sizeof(tv));
    if (err < 0)
    {
        ipcom_printf("ipcom_setsockopt() IP_SO_RCVTIMEO ");
        goto cleanup;
    }
#endif

#ifdef IPCOM_USE_INET
    if (param->domain == IP_AF_INET && IP_IN_CLASSD(param->addr.sin.sin_addr.s_addr))
    {
        /* Sent outgoing multicast interface for IPv4 */
        if (ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_MULTICAST_IF,
            &param->mcast_if.in, sizeof(param->mcast_if.in)) < 0)
        {
            ipcom_printf("Failed to set the outgoing multicast interface" IP_LF);
            goto cleanup;
        }
        ipcom_printf("\tMulticast packets to %s",
                     ipcom_cmd_sockaddr_to_str(&param->addr.sa, IP_TRUE, IP_TRUE, str, sizeof(str)));
        ipcom_printf(" will be sent on %s" IP_LF,
                     ipcom_inet_ntop(IP_AF_INET, &param->mcast_if.in, str, sizeof(str)));
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (param->domain == IP_AF_INET6 && IP_IN6_IS_ADDR_MULTICAST(&param->addr.sin6.sin6_addr))
    {
        /* Sent outgoing multicast interface for IPv6 */
        if (ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_MULTICAST_IF,
            &param->mcast_if.ifindex, sizeof(param->mcast_if.in)) < 0)
        {
            ipcom_printf("Failed to set the outgoing multicast interface" IP_LF);
            goto cleanup;
        }
        ipcom_printf("\tMulticast packets to %s",
            ipcom_cmd_sockaddr_to_str(&param->addr.sa, IP_TRUE, IP_TRUE, str, sizeof(str)));
        ipcom_printf(" will be sent on %s" IP_LF,
            ipcom_if_indextoname(param->mcast_if.ifindex, str));
    }
#endif /* IPCOM_USE_INET6 */

    if (param->read_reply)
    {
        union Ip_sockaddr_union  name;
        Ip_socklen_t             name_len;
        int                      opt_val = 1;

        ipcom_memset(&name, 0, sizeof(name));

        if (param->reuse_addr && param->type == IP_SOCK_STREAM)
            (void)ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_REUSEADDR, (void*)&opt_val, sizeof(opt_val));

        /* Copy domain (and length for new socket address format) */
        ipcom_memcpy(&name, &param->addr, sizeof(Ip_u16));
        *(Ip_u16 *)name.sa.sa_data = ip_htons(param->local_port);
        if (ipcom_bind(sock, &name.sa, IPCOM_SA_LEN_GET(&name.sa)) < 0)
        {
            ipcom_cmd_echo_error_msg("Failed to bind socket");
            goto cleanup;
        }
        ipcom_memset(&name, 0, sizeof(name));
        name_len = sizeof(name);
        (void)ipcom_getsockname(sock, &name.sa, &name_len);
        ip_assert(name_len <= sizeof(name));
        ipcom_printf("\tBound to %s" IP_LF,
            ipcom_cmd_sockaddr_to_str(&name.sa, IP_TRUE, IP_TRUE, str, sizeof(str)));
    }

    if (param->connect)
    {
        union Ip_sockaddr_union  peer;
        Ip_socklen_t             peer_len;

        if (ipcom_connect(sock, &param->addr.sa, IPCOM_SA_LEN_GET(&param->addr.sa)) < 0)
        {
            ipcom_cmd_echo_error_msg("Failed to connect socket");
            goto cleanup;
        }

        ipcom_memset(&peer, 0, sizeof(peer));
        peer_len = sizeof(peer);
        (void)ipcom_getpeername(sock, &peer.sa, &peer_len);
        ip_assert(peer_len <= sizeof(peer));
        ipcom_printf("\tConnected to %s" IP_LF,
            ipcom_cmd_sockaddr_to_str(&peer.sa, IP_TRUE, IP_TRUE, str, sizeof(str)));
    }

    /* Initialize send and receive counters */
    total_bytes_sent = 0;
    total_bytes_read = 0;
    if (param->read_reply)
    {
        recv_buf = ipcom_malloc(param->buf_size + IPCOM_IP_HEADER_LENGTH);
        if (recv_buf == IP_NULL)
        {
            ipcom_printf("Out of memory");
            goto cleanup;
        }
    }

    msec_start = get_millisec();
    for (i = 0; i < param->buf_count; i++)
    {
        int bytes_to_send;
        bytes_sent = 0;
        bytes_read = 0;

        do
        {
            bytes_to_send = IP_MIN(IP_MIN(param->buf_size - bytes_sent, param->sock_buf_size), param->buf_size);

            if (param->connect)
                ret = ipcom_send(sock, &send_buf[bytes_sent], bytes_to_send, 0);
            else
                ret = ipcom_sendto(sock, &send_buf[bytes_sent], bytes_to_send, 0,
                             &param->addr.sa, IPCOM_SA_LEN_GET(&param->addr.sa));

            if (ret < 0)
            {
                ipcom_cmd_echo_error_msg("Failed to send data");
                goto cleanup;
            }
            bytes_sent += ret;
            total_bytes_sent += ret;
            write_io_calls++;
        }
        while (bytes_sent < param->buf_size);

        if (param->read_reply)
        {
            do
            {
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
                void *zbuf;
                int zstart;
                Ipcom_pkt_freefunc freefunc;
                void *cookie;

                if(param->zerocopy && (param->type == IP_SOCK_DGRAM || param->type == IP_SOCK_RAW))
                {
                    ret = ipcom_pkt_recvfrom(sock, (void **)&zbuf, &zstart, &freefunc, &cookie, 0, IP_NULL, IP_NULL);
                    if(ret >= 0)
                        freefunc(zbuf, cookie);
                }
                else
#endif
                    ret = ipcom_recv(sock, &recv_buf[bytes_read], param->buf_size + IPCOM_IP_HEADER_LENGTH - bytes_read, 0);
                if (ret < 0)
                {
                    ipcom_cmd_echo_error_msg("Failed to recv data");
                    goto cleanup;
                }
                bytes_read += ret;
                total_bytes_read += ret;
                read_io_calls++;
            }
            while (bytes_read < bytes_sent);

            /* Verify that the data received is the same as the data sent */
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
            if(param->verify_recv_data && !param->zerocopy)
#else
            if(param->verify_recv_data)
#endif
            {
                if (param->type == IP_SOCK_RAW && param->domain == IP_AF_INET)
                {
                    /* Strip IP header (will not handle IP options) */
                    if (bytes_read >= IPCOM_IP_HEADER_LENGTH)
                    {
                        bytes_read -= IPCOM_IP_HEADER_LENGTH;
                        total_bytes_read -= IPCOM_IP_HEADER_LENGTH;
                        offset = IPCOM_IP_HEADER_LENGTH;
                    }
                }
                for (x = 0; x < bytes_read; x++)
                {
                    if (recv_buf[x+offset] != send_buf[x])
                    {
                        ipcom_printf("Data corruption detected at offset %d, send %d but %d was read" IP_LF,
                            x, send_buf[x], recv_buf[x]);
                        goto cleanup;
                    }
                }
            }
        }
    }

    ipcom_printf("\tSent %d bytes" IP_LF, total_bytes_sent);
    if (param->read_reply && total_bytes_read >= 0)
        ipcom_printf("\tReceived %d bytes" IP_LF, total_bytes_read);
    msec = get_millisec() - msec_start;
    if(msec == 0)
        ipcom_printf("\tElapsed time: 0 milliseconds, 0 kbytes/sec, 0 send i/o calls/sec, 0 read i/o calls/sec" IP_LF);
    else
        ipcom_printf("\tElapsed time: %ld milliseconds, %d kbytes/sec, %d send i/o calls/sec, %d read i/o calls/sec" IP_LF,
                     msec, total_bytes_sent/1024*1000/(int)msec, write_io_calls*1000/(int)msec, read_io_calls*1000/(int)msec);

    err = 0;

cleanup:
    if (send_buf)
        ipcom_free(send_buf);
    if (recv_buf)
        ipcom_free(recv_buf);
    if (sock != IP_SOCKERR)
        ipcom_socketclose(sock);

#ifdef IP_PORT_INTEGRITY
    ipcom_shutdown_libsocket();
#endif

    return err;
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_client_print_usage
 *===========================================================================
 * Description: Prints usage for the send shell command.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_echo_client_print_usage(void)
{
    ipcom_printf(""IP_LF);
    ipcom_printf("NAME"IP_LF);
    ipcom_printf("     echoclient - sends a string to a peer and possibly read back a response"IP_LF);
    ipcom_printf(""IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF);
    ipcom_printf("     echoclient [-cr -d <domain> -t <protocol> -p <port>] addr"IP_LF);
    ipcom_printf(""IP_LF);
    ipcom_printf("DESCRIPTION"IP_LF);
    ipcom_printf("     echoclient will send some data to an echoserver"IP_LF);
    ipcom_printf(""IP_LF);
    ipcom_printf("     -b <size>" IP_LF);
    ipcom_printf("             Size of socket send/recv buffer (default is %d)." IP_LF, IPCOM_CMD_SOCK_BUF_SIZE);
    ipcom_printf("     -s <chunk_size>" IP_LF);
    ipcom_printf("             Number of bytes written in each sendto() call" IP_LF);
    ipcom_printf("             (default is %d)." IP_LF, IPCOM_CMD_ECHO_BUF_SIZE);
    ipcom_printf("     -n <number>" IP_LF);
    ipcom_printf("             Number of buffers to write (default is %d)." IP_LF, IPCOM_CMD_ECHO_BUF_COUNT);
    ipcom_printf("     -c"IP_LF);
    ipcom_printf("             Connect the socket before sending."IP_LF);
    ipcom_printf("     -r"IP_LF);
    ipcom_printf("             Read reply from the peer before closing the socket."IP_LF);
    ipcom_printf("     -d <domain>"IP_LF);
    ipcom_printf("             Address domain, inet for IPv4 and inet6 for IPv6."IP_LF);
    ipcom_printf("             The address domain will be guessed from the address format"IP_LF);
    ipcom_printf("             if this option is not used."IP_LF);
    ipcom_printf("     -t <proto>"IP_LF);
    ipcom_printf("             Transport protocol, udp for UDP (default), tcp for TCP, raw for RAW, sctp for SCTP(one to one)"IP_LF);
    ipcom_printf("     -k"IP_LF);
    ipcom_printf("             Set socket option IP_SO_REUSEADDR when -r is given."IP_LF);
    ipcom_printf("     -j <port>"IP_LF);
    ipcom_printf("             Port to bind to when -r is given (default is 0 -> ephemeral)."IP_LF);
    ipcom_printf("     -p <port>"IP_LF);
    ipcom_printf("             Port to send to (default is %d)."IP_LF, IPCOM_CMD_ECHO_PORT);
    ipcom_printf("     -i <ifname>" IP_LF);
    ipcom_printf("             The interface to send and receive multicast packets." IP_LF);
    ipcom_printf("     -v"IP_LF);
    ipcom_printf("             Verify received data"IP_LF);
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
    ipcom_printf("     -z"IP_LF);
    ipcom_printf("             Use zero copy API"IP_LF);
#endif
    ipcom_printf(""IP_LF);
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_server_error
 *===========================================================================
 * Description: Sends an errorcode
 * Parameters:  ipc_handle - An open IPC handle.
 *              err - The error code to send.
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_echo_server_error(Ipcom_ipc *ipc_handle, int err)
{
    int *errno_ptr;

    ip_assert(ipcom_ipc_isopen(ipc_handle));

    errno_ptr = ipcom_ipc_malloc(sizeof(*errno_ptr));
    if (errno_ptr == IP_NULL)
        return;

    *errno_ptr = err;

    if (ipcom_ipc_send(ipc_handle, errno_ptr) != IPCOM_SUCCESS)
        ipcom_ipc_free(errno_ptr);
    (void)ipcom_ipc_close(ipc_handle);
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_server_daemon
 *===========================================================================
 * Description: The echoserver daemon process.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_echo_server_daemon(void)
{
    Ip_fd sock = IP_SOCKERR;
    Ip_fd lsock = IP_SOCKERR;
    int bytes_read;
    int ready_sockets;
    Ipcom_cmd_test *param = IP_NULL;
    Ipcom_cmd_test *param_stop;
    Ipcom_ipc ipc_handle;
    char *buf = IP_NULL;
    Ip_fd_set read_set;
    struct Ip_timeval tv;
    Ip_err err;
    union Ip_sockaddr_union  peer;
    Ip_socklen_t             peer_len;
    int                      opt_val = 1;

    ipcom_proc_init();

#ifdef IP_PORT_INTEGRITY
    ipcom_init_libsocket();
#endif

    if (ipcom_ipc_install() != IPCOM_SUCCESS)
        goto cleanup2;

    if (ipcom_ipc_receive(&ipc_handle, &param, IPC_TIMEOUT_INFINITE) != IPCOM_SUCCESS)
        goto cleanup;

    buf = ipcom_malloc(param->buf_size);
    if (buf == IP_NULL)
        goto cleanup;

    sock = ipcom_socket(param->domain, param->type, param->protocol);
    if (sock == IP_INVALID_SOCKET)
    {
        ipcom_cmd_echo_server_error(&ipc_handle, ipcom_errno);
        goto cleanup;
    }
    err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_SNDBUF, &param->sock_buf_size,
        sizeof(param->sock_buf_size));
    if (err < 0)
    {
        ipcom_cmd_echo_server_error(&ipc_handle, ipcom_errno);
        goto cleanup;
    }

    err = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_RCVBUF, &param->sock_buf_size,
        sizeof(param->sock_buf_size));
    if (err < 0)
    {
        ipcom_cmd_echo_server_error(&ipc_handle, ipcom_errno);
        goto cleanup;
    }

#ifdef IPCOM_USE_INET6
    if (param->domain == IP_AF_INET && IP_IN_CLASSD(param->addr.sin.sin_addr.s_addr))
    {
        /* Join the IPv4 multicast address */
        struct Ip_ip_mreq mreq;

        mreq.imr_interface = param->mcast_if.in;
        mreq.imr_multiaddr = param->addr.sin.sin_addr;
        if (ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_JOIN_GROUP,
            &mreq, sizeof(mreq)) < 0)
        {
            ipcom_cmd_echo_server_error(&ipc_handle, ipcom_errno);
            goto cleanup;
        }
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (param->domain == IP_AF_INET6 && IP_IN6_IS_ADDR_MULTICAST(&param->addr.sin6.sin6_addr))
    {
        /* Join the IPv4 multicast address */
        struct Ip_ipv6_mreq mreq6;

        mreq6.ipv6mr_interface = param->mcast_if.ifindex;
        mreq6.ipv6mr_multiaddr = param->addr.sin6.sin6_addr;
        if (ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_JOIN_GROUP,
            &mreq6, sizeof(mreq6)) < 0)
        {
            ipcom_cmd_echo_server_error(&ipc_handle, ipcom_errno);
            goto cleanup;
        }
    }
#endif /* IPCOM_USE_INET6 */

    if (param->reuse_addr && param->type == IP_SOCK_STREAM)
        (void)ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_REUSEADDR, (void*)&opt_val, sizeof(opt_val));

    /* Bind to the specified address and port */
    if (ipcom_bind(sock, &param->addr.sa, IPCOM_SA_LEN_GET(&param->addr.sa)) < 0)
    {
        ipcom_cmd_echo_server_error(&ipc_handle, ipcom_errno);
        goto cleanup;
    }

    if (param->type == IP_SOCK_STREAM)
    {
        if (ipcom_listen(sock, 0) < 0)
        {
            ipcom_cmd_echo_server_error(&ipc_handle, ipcom_errno);
            goto cleanup;
        }
    }

    /* Daemon started successfully */
    ipcom_cmd_echo_server_error(&ipc_handle, 0);

    for (;;)
    {
        IP_FD_ZERO(&read_set);

        for (;;)
        {
            IP_FD_SET(sock, &read_set);
            tv.tv_sec  = 0;
            tv.tv_usec = 500000;

            ready_sockets = ipcom_socketselect(sock + 1, &read_set, IP_NULL, IP_NULL, &tv);

            if (ready_sockets > 0)
                /* Data ready */
                break;

            if (ready_sockets < 0)
                goto cleanup;

            err = ipcom_ipc_receive(&ipc_handle, &param_stop, IPC_TIMEOUT_POLL);
            if (err == IPCOM_ERR_TIMEOUT)
                /* No new message */
                continue;
            /* The server should die */
            ipcom_ipc_free(param_stop);
            goto stopped;
        }
        if (param->type == IP_SOCK_STREAM)
        {
            lsock = sock;
            if ((sock = ipcom_accept(lsock, IP_NULL, IP_NULL)) < 0)
            {
                ipcom_debug_printf("echoserver: accept failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
                goto cleanup;
            }
        }

        peer_len = sizeof(peer);
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
        if(param->zerocopy && (param->type == IP_SOCK_DGRAM || param->type == IP_SOCK_RAW))
        {
            void *zbuf;
            int zstart;
            Ipcom_pkt_freefunc freefunc;
            void *cookie;

            while((bytes_read = ipcom_pkt_recvfrom(sock, (void **)&zbuf, &zstart, &freefunc, &cookie, 0, &peer.sa, &peer_len)) >= 0)
            {
                if(ipcom_pkt_sendto(sock, zbuf, zstart, bytes_read, freefunc, cookie, 0, &peer.sa, peer_len) < 0)
                {
                    ipcom_debug_printf("echoserver: ipcom_pkt_sendto failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
                    if (ipcom_errno != IP_ERRNO_ECONNRESET)
                        goto cleanup;
                    break;
                }
                bytes_read = 0;
                break;
            }
        }
        else
#endif
        {
            ipcom_memset(&peer, 0, sizeof(peer));
            while ((bytes_read = (param->type == IP_SOCK_STREAM ?
                                  ipcom_recv(sock, buf, param->buf_size, 0) :
                                  ipcom_recvfrom(sock, buf, param->buf_size, 0,
                                                 &peer.sa, &peer_len))) >= 0)
            {
                Ip_ssize_t  bytes_sent;
                Ip_bool     udp_connected = IP_FALSE;

                if (bytes_read == 0 && param->type == IP_SOCK_STREAM)
                    break;

                if (param->connect && (param->type == IP_SOCK_DGRAM || param->type == IP_SOCK_RAW))
                {
                    if (ipcom_connect(sock, &peer.sa, peer_len) < 0)
                    {
                        IP_PANIC();
                    }
                    else
                        udp_connected = IP_TRUE;
                }

                if (param->type == IP_SOCK_RAW && param->domain == IP_AF_INET)
                {
                    /* Strip IP header (will not handle IP options) */
                    if (bytes_read >= IPCOM_IP_HEADER_LENGTH)
                    {
                        bytes_read -= IPCOM_IP_HEADER_LENGTH;
                        ipcom_memmove(&buf[0], &buf[IPCOM_IP_HEADER_LENGTH], bytes_read);
                    }
                }

                if (param->type == IP_SOCK_STREAM || udp_connected)
                    bytes_sent = ipcom_send(sock, buf, bytes_read, 0);
                else
                    bytes_sent = ipcom_sendto(sock, buf, bytes_read, 0,
                                             &peer.sa, peer_len); /*lint !e645 Symbol 'peer' may not have been initialized */
                if (bytes_sent < 0)
                {
                    ipcom_debug_printf("echoserver: sendto failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
                    if (ipcom_errno != IP_ERRNO_ECONNRESET)
                        goto cleanup;
                    break;
                }
                if (udp_connected)
                {
                    /* disconnect */
                    union Ip_sockaddr_union  any;
                    ipcom_memset(&any, 0, sizeof(any));
                    any.sa.sa_family = (Ip_sa_family_t)param->domain;
                    IPCOM_SA_LEN_SET(&any.sa, param->domain == IP_AF_INET ? sizeof(struct Ip_sockaddr_in) : sizeof(struct Ip_sockaddr_in6));
                    (void)ipcom_connect(sock, &any.sa, IPCOM_SA_LEN_GET(&any.sa));
                }
                if (param->type == IP_SOCK_DGRAM || param->type == IP_SOCK_RAW || param->echo_once == IP_TRUE)
                {
                    bytes_read = 0;
                    break;
                }
            }
        }

        if (bytes_read != 0)
        {
            ipcom_debug_printf("echoserver: recvfrom failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
            if (ipcom_errno != IP_ERRNO_ECONNRESET)
                goto cleanup;
        }

        if (param->type == IP_SOCK_STREAM)
        {
            /* Close the socket returned from ipcom_accept() */
            ipcom_socketclose(sock);
            sock = lsock;
            lsock = IP_SOCKERR;
        }

        /* Reset any pending errors */
        if (ipcom_getsockopt(sock, IP_SOL_SOCKET, IP_SO_ERROR, &err, &peer_len) < 0)
        {
            ipcom_debug_printf("echoserver: getsockopt failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
            goto cleanup;
        }
    }

stopped:
    /* Daemon stopped successfully */
    ipcom_cmd_echo_server_error(&ipc_handle, 0);
cleanup:
    if (buf != IP_NULL)
        ipcom_free(buf);
    if (ipcom_ipc_isopen(&ipc_handle))
        (void)ipcom_ipc_close(&ipc_handle);
    if (param != IP_NULL)
        ipcom_ipc_free(param);
cleanup2:
    (void)ipcom_ipc_uninstall();
    if (lsock != IP_SOCKERR)
        ipcom_socketclose(lsock);
    if (sock != IP_SOCKERR)
        ipcom_socketclose(sock);

#ifdef IP_PORT_INTEGRITY
    ipcom_shutdown_libsocket();
#endif

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_server_do
 *===========================================================================
 * Description: Starts/stops the echoserver daemon process.
 * Parameters:  param - The parameters specified when the shell command
 *              was started.
 * Returns:     0 = success, <0 = error.
 *
 */
IP_STATIC int
ipcom_cmd_echo_server_do(Ipcom_cmd_test *param)
{
    Ip_err err;
    Ipcom_ipc ipc;
    void *ipc_buf;
    int *errno_ptr;
    int ret_errno = 0;
    char str[100];

    /* Check if the server is already running (stop it if it does) */
    err = ipcom_ipc_open(&ipc, IPCOM_CMD_ECHO_SERVER_NAME, IPC_TIMEOUT_POLL);
    if (err == IPCOM_SUCCESS)
    {
        ipc_buf = ipcom_ipc_malloc(1);
        if (ipc_buf == IP_NULL)
        {
            ipcom_printf("Out of memory" IP_LF);
            return err;
        }
        err = ipcom_ipc_send(&ipc, ipc_buf);
        if (err != IPCOM_SUCCESS)
            ipcom_ipc_free(ipc_buf);
        if (err != IPCOM_SUCCESS || ipcom_ipc_receive(IP_NULL, &errno_ptr, 10000) != IPCOM_SUCCESS)
        {
            ipcom_printf("Failed to stop the running echoserver process" IP_LF);
            return err;
        }
        ret_errno = *errno_ptr;
        ipcom_ipc_free(errno_ptr);
        (void)ipcom_ipc_close(&ipc);
    }

    if (param->stop_daemon)
    {
        ipcom_printf("Stopped echoserver process" IP_LF);
        return ret_errno;
    }

    /* Start the server */
    err = ipcom_proc_acreate(IPCOM_CMD_ECHO_SERVER_NAME, ipcom_cmd_echo_server_daemon, IP_NULL, IP_NULL);
    if (err != IPCOM_SUCCESS)
    {
        ipcom_printf("Failed to create the echoserver process (%d)" IP_LF, err);
        return err;
    }

    err = ipcom_ipc_open(&ipc, IPCOM_CMD_ECHO_SERVER_NAME, IPC_TIMEOUT_INFINITE);
    if (err != IPCOM_SUCCESS)
    {
        ipcom_printf("Failed to open IPC channel to echoserver (%d)" IP_LF, err);
        return err;
    }

    ipc_buf = ipcom_ipc_malloc(sizeof(*param));
    if (ipc_buf == IP_NULL)
    {
        ipcom_printf("Failed allocate ipc buffer (%d)" IP_LF, err);
        return err;
    }

    /* Send the parameters */
    ipcom_memcpy(ipc_buf, param, sizeof(*param));
    err = ipcom_ipc_send(&ipc, ipc_buf);
    if (err != IPCOM_SUCCESS)
    {
        ipcom_ipc_free(ipc_buf);
        ipcom_printf("Failed to send parameters to the echoserver process (%d)" IP_LF, err);
        return err;
    }

    /* Wait for the response */
    err = ipcom_ipc_receive(IP_NULL, &errno_ptr, IPC_TIMEOUT_INFINITE);
    if (err != IPCOM_SUCCESS)
    {
        ipcom_printf("Failed receive response from echoserver process (%d)" IP_LF, err);
        return err;
    }
    ret_errno = *errno_ptr;
    ipcom_ipc_free(errno_ptr);

    if (ret_errno != 0)
    {
        /* Something failed at the server side */
        ipcom_printf("The echoserver process failed to start (%d)" IP_LF, ret_errno);
        return ret_errno;
    }

    ipcom_printf("echoserver is now listening at %s" IP_LF,
                 ipcom_cmd_sockaddr_to_str(&param->addr.sa, IP_TRUE, IP_TRUE, str, sizeof(str)));

    (void)ipcom_ipc_close(&ipc);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_echo_parse_arguments
 *===========================================================================
 * Description: Parses the arguments specified at the shell prompt.
 * Parameters:  argc - Number of arguments.
 *              argv - The arguments.
 *              param - Will be updated with the information parsed from the
 *              shell arguments.
 * Returns:     0 = success, <0 = error.
 *
 */
IP_STATIC int
ipcom_cmd_echo_parse_arguments(int argc, char **argv, Ipcom_cmd_test *param)
{
    Ipcom_getopt  opt;
    int c;
    int ret;
    int port;
    int ifindex;
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(ifreq));
    port = IPCOM_CMD_ECHO_PORT;
    ipcom_memset(param, 0, sizeof(*param));
    param->type = IP_SOCK_DGRAM;
    param->buf_size = IPCOM_CMD_ECHO_BUF_SIZE;
    param->sock_buf_size = IPCOM_CMD_SOCK_BUF_SIZE;
    param->buf_count = IPCOM_CMD_ECHO_BUF_COUNT;

    ipcom_getopt_clear_r(&opt);
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
    while ((c = ipcom_getopt_r(argc, argv, "b:l:s:n:crd:t:j:kp:i:vqzx", &opt)) != -1)
#else
    while ((c = ipcom_getopt_r(argc, argv, "b:l:s:n:crd:t:j:kp:i:vqx", &opt)) != -1)
#endif
    {
        switch (c)
        {
        case 'd':
            param->domain = ipcom_cmd_str_to_key(ipcom_cmd_echo_domain_map, opt.optarg);
            break;
        case 't':
            param->type = ipcom_cmd_str_to_key(ipcom_cmd_echo_type_map, opt.optarg);
            if(param->type == -1)
            {
                ipcom_printf("%s is not a valid socket type" IP_LF, opt.optarg);
                return -1;
            }

            param->protocol = ipcom_cmd_str_to_key(ipcom_cmd_echo_proto_map, opt.optarg);
            if (param->protocol == -1)
            {
                ipcom_printf("%s is not a valid socket protocol" IP_LF, opt.optarg);
                return -1;
            }

            if (param->type == IP_SOCK_STREAM)
            {
                param->connect = IP_TRUE;
            }
            break;
        case 'j':
            param->local_port = ipcom_atoi(opt.optarg);
            break;
        case 'k':
            param->reuse_addr = 1;
            break;
        case 'p':
            port = ipcom_atoi(opt.optarg);
            break;
        case 'b':
            param->sock_buf_size = ipcom_atoi(opt.optarg);
            break;
        case 'l':
        case 's':
            param->buf_size = ipcom_atoi(opt.optarg);
            break;
        case 'c':
            param->connect = IP_TRUE;
            break;
        case 'r':
            param->read_reply = IP_TRUE;
            break;
        case 'i':
            ipcom_strncpy(ifreq.ifr_name, opt.optarg, sizeof(ifreq.ifr_name));
            break;
        case 'q':
            param->stop_daemon = IP_TRUE;
            break;
        case 'v':
            param->verify_recv_data = IP_TRUE;
            break;
        case 'n':
            param->buf_count = ipcom_atoi(opt.optarg);
            break;
#ifdef IPCOM_CMD_ECHO_USE_ZEROCOPY_API
        case 'z':
            param->zerocopy = IP_TRUE;
            break;
#endif
        case 'x':
            param->echo_once = IP_TRUE;
            break;
        default:
            break;
        }
    }

    if (param->stop_daemon)
        return 0;

    if (opt.optind + 1 != argc)
        return -1;

    if (param->domain != 0)
    {
        /* Domain was specified by the user */
        ret = ipcom_getsockaddrbyaddrname(param->domain, IP_FALSE, argv[opt.optind],
                &param->addr.sa);
    }
    else
    {
        /* Try to guess the domain from the address */
        ret = -1;
        for (c = 0; ret < 0 && ipcom_cmd_echo_domain_map[c].key != -1; c++)
        {
            param->domain = ipcom_cmd_echo_domain_map[c].key;
            ret = ipcom_getsockaddrbyaddrname(param->domain, IP_FALSE, argv[opt.optind],
                &param->addr.sa);
        }
    }

    if (ret < 0)
    {
        ipcom_printf("%s is not a valid address" IP_LF, argv[opt.optind]);
        return -1;
    }

#ifdef IPCOM_USE_INET
    if (param->domain == IP_AF_INET)
        param->addr.sin.sin_port = (Ip_u16)ip_htons(port);
#endif
#ifdef IPCOM_USE_INET6
    if (param->domain == IP_AF_INET6)
        param->addr.sin6.sin6_port = (Ip_u16)ip_htons(port);
#endif

    if (ifreq.ifr_name[0] != '\0')
    {
        ifindex = ipcom_if_nametoindex(ifreq.ifr_name);
        if (ifindex == 0)
        {
            ipcom_printf("%s is not a valid interface" IP_LF, ifreq.ifr_name);
            return -1;
        }

        /* Interface to use for multicast packets specified */
#ifdef IPCOM_USE_INET6
        if (param->domain == IP_AF_INET6)
            param->mcast_if.ifindex = ifindex;
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_USE_INET
        if (param->domain == IP_AF_INET)
        {
            /* Get one of the unicast addresses for this interface */
            c = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
            if (c == IP_INVALID_SOCKET)
            {
                ipcom_printf("Failed to create socket" IP_LF);
                return -1;
            }
            if (ipcom_socketioctl(c, IP_SIOCGIFADDR, &ifreq) < 0)
            {
                ipcom_socketclose(c);
                ipcom_printf("Failed to get primary address for %s" IP_LF, ifreq.ifr_name);
                return -1;
            }
            ipcom_memcpy(&param->mcast_if.in,
                         &((struct Ip_sockaddr_in*) &ifreq.ip_ifr_addr)->sin_addr,
                         sizeof(param->mcast_if.in));
            ipcom_socketclose(c);
        }
#endif /* IPCOM_USE_INET */
    }

    return 0;
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
 *                    ipcom_cmd_echoserver
 *===========================================================================
 * Description: Entry point for the echoserver shell command.
 * Parameters:  argc - Number of arguments.
 *              argv - The arguments.
 * Returns:     0 = success, <0 = error.
 *
 */
IP_PUBLIC int
ipcom_cmd_echoserver(int argc, char **argv)
{
    int ret;
    Ipcom_cmd_test param;

    ret = ipcom_cmd_echo_parse_arguments(argc, argv, &param);
    if (ret < 0)
    {
        ipcom_cmd_echo_server_print_usage();
        return ret;
    }

    return ipcom_cmd_echo_server_do(&param);
}


/*
 *===========================================================================
 *                    ipcom_cmd_echoclient
 *===========================================================================
 * Description: Entry point for the echoclient shell command.
 * Parameters:  argc - Number of arguments.
 *              argv - The arguments.
 * Returns:     0 = success, <0 = error.
 *
 */
IP_PUBLIC int
ipcom_cmd_echoclient(int argc, char **argv)
{
    int ret;
    Ipcom_cmd_test param;

    ret = ipcom_cmd_echo_parse_arguments(argc, argv, &param);
    if (ret < 0)
    {
        ipcom_cmd_echo_client_print_usage();
        return ret;
    }

    return ipcom_cmd_echo_client_do(&param);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

