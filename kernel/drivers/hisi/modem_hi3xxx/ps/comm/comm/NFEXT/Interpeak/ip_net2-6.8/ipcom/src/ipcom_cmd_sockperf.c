/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_sockperf.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_sockperf.c,v $
 *   $Author: rime $
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
 *
 * Interpeak network performance tool, supports:
 *   half- and full-duplex communication
 *   any number of active sockets
 *   IPv4 and IPv6
 *   TCP
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
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_getopt.h>
#include <ipcom_inet.h>
#include <ipcom_os.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_time.h>
#include <ipcom_type.h>

#ifdef IP_PORT_INTEGRITY
#include "ipcom_ipcom2bsd.h"
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

typedef struct Ipcom_cmd_sockperf_struct
{
    Ip_bool                 accept;
    Ip_bool                 transmit;
    Ip_bool                 receive;
    Ip_bool                 testpattern;
    Ip_bool                 echo;
    Ip_u32                  buf_len;
    Ip_u32                  num_buf;
    Ip_u32                  num_sock;
    struct Ip_addrinfo     *res;
    int                     width;
    Ip_fd_set               read_set;
    Ip_fd                  *sock_array;
    char                   *buf;
    struct Ip_linger        linger;
    int                     sendbuf_size;
    int                     recvbuf_size;
    Ip_u32                 *send_pattern;
    Ip_u32                 *recv_pattern;
} Ipcom_cmd_sockperf_t;


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

IP_PUBLIC int ipcom_cmd_sockperf(int argc, char **argv);


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
 *                    ipcom_cmd_sockperf_tv_to_msec
 *===========================================================================
 * Description: Returns the number of milliseconds between 2 absolute times.
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_s32
ipcom_cmd_sockperf_tv_to_msec(struct Ip_timeval *start, struct Ip_timeval *stop)
{
    Ip_s32 sec;
    Ip_s32 usec;

    usec = stop->tv_usec - start->tv_usec;
    sec = stop->tv_sec - start->tv_sec;
    if (usec < 0)
    {
        usec += 1000000;
        sec -= 1;
    }
    return IP_MAX(sec * 1000 + usec / 1000, 1);
}


/*
 *===========================================================================
 *                    ipcom_cmd_sockperf_echo_buf
 *===========================================================================
 * Description: Prints the contents of the buffer.
 * Parameters:  buf - The buffer to print.
 *              size - The size of 'buf'
 *              pattern_id - The pattern number the buffer should start at.
 * Returns:
 */
IP_STATIC void
ipcom_cmd_sockperf_echo_buf(char *buf, int size, int pattern_id)
{
    char pbuf[9];
    int  i;

    if (size == 0)
    {
        ipcom_printf(IP_LF);
        return;
    }

    pbuf[8] = '\0';
    for (i = 0; i < size; i += 8)
    {
        ipcom_memcpy(pbuf, buf + i, 8);
        if ((pattern_id % 16) == 0)
            ipcom_printf("%04d  ", i);
        if ((++pattern_id % 16) == 0)
            pbuf[7] = '\n';
        ipcom_printf(pbuf);
    }
}


/*
 *===========================================================================
 *                    ipcom_cmd_sockperf_run
 *===========================================================================
 * Description: Sends/receives data.
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_cmd_sockperf_run(Ipcom_cmd_sockperf_t *cmd)
{
    struct Ip_timeval  tmo = { 0, 0 };
    struct Ip_timeval *ptmo;
    struct Ip_timeval  start;
    struct Ip_timeval  stop;
    Ip_u32             i;
    Ip_u32             c;
    int                num_ready;
    Ip_fd             *s = cmd->sock_array;
    Ip_u32             total_bytes_to_send = cmd->num_buf * cmd->buf_len;
    Ip_u32             finished_sockets = 0;
    Ip_u32             total_bytes_read = 0;
    Ip_ssize_t         bytes;
    Ip_fd_set          read_set;
    int                send_flags = (cmd->transmit && cmd->receive) ? IP_MSG_DONTWAIT : 0;

    if (cmd->transmit)
        ipcom_printf("sockperf-t: send buffer is %u"IP_LF, cmd->sendbuf_size);
    if (cmd->receive)
        ipcom_printf("sockperf-r: receive buffer is %u"IP_LF, cmd->sendbuf_size);
    for (i = 0; i < cmd->num_sock; i++)
    {
        int on = 1;

        if (ipcom_setsockopt(s[i],
                             IP_SOL_SOCKET,
                             IP_SO_REUSEADDR,
                             &on,
                             sizeof(on)) < 0)
        {
            ipcom_printf("sockperf-c: setsockopt IP_SO_REUSEADDR failed : %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return;
        }

        if (ipcom_setsockopt(s[i],
                             IP_SOL_SOCKET,
                             IP_SO_SNDBUF,
                             &cmd->sendbuf_size,
                             sizeof(cmd->sendbuf_size)) < 0)
        {
            ipcom_printf("sockperf-c: setsockopt IP_SO_SNDBUF failed : %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return;
        }

        if (ipcom_setsockopt(s[i],
                             IP_SOL_SOCKET,
                             IP_SO_RCVBUF,
                             &cmd->recvbuf_size,
                             sizeof(cmd->recvbuf_size)) < 0)
        {
            ipcom_printf("sockperf-c: setsockopt IP_SO_SNDBUF failed : %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return;
        }
    }


    if (cmd->receive)
    {
        IP_FD_ZERO(&cmd->read_set);
        cmd->width = s[0];
        IP_FD_SET(s[0], &cmd->read_set);
        for (i = 1; i < cmd->num_sock; i++)
            if (s[i] != IP_INVALID_SOCKET)
            {
                cmd->width = IP_MAX(cmd->width, s[i]);
                IP_FD_SET(s[i], &cmd->read_set);
            }
    }

    ipcom_microtime(&start);
    while ((cmd->transmit && total_bytes_to_send)
           || (cmd->receive && finished_sockets < cmd->num_sock))
    {
        if (cmd->transmit && total_bytes_to_send)
        {
            for (i = 0; i < cmd->num_sock; i++)
            {
                if (cmd->testpattern)
                {
                    /* Test patter is "[B|E|D]xxxxxx ", B = first 8 byte in buffer, E = last 8 bytes, D = all other */
                    for (c = 0; c < cmd->buf_len; c += 8)
                        ipcom_sprintf(cmd->buf + c, "%c%06ld ",
                                      c == 0 ? '>' : (c >= cmd->buf_len - 8 ? '<' : '#'),
                                      cmd->send_pattern[i]++ % 1000000);
                }

                bytes = ipcom_send(s[i],
                                   cmd->buf,
                                   IP_MIN(cmd->buf_len, total_bytes_to_send),
                                   send_flags);
                if (bytes < 0 && ipcom_errno == IP_ERRNO_EWOULDBLOCK)
                    (void)ipcom_sleep(0);
                else
                {
                    if (bytes < 0)
                    {
                        ipcom_printf("sockperf-t: send failed : %s"IP_LF,
                                     ipcom_strerror(ipcom_errno));
                        return;
                    }
                    total_bytes_to_send -= bytes;
                }
            }

            if (cmd->receive && total_bytes_to_send == 0)
                for (i = 0; i < cmd->num_sock; i++)
                    if (ipcom_shutdown(s[i], IP_SHUT_WR) < 0)
                    {
                        ipcom_printf("sockperf-t: shutdown failed: %s"IP_LF,
                                     ipcom_strerror(ipcom_errno));
                        return;
                    }
        }

        if (cmd->receive)
        {
            ptmo = IP_NULL;
            while (finished_sockets < cmd->num_sock)
            {
                read_set = cmd->read_set;
                num_ready = ipcom_socketselect(cmd->width + 1, &read_set, IP_NULL, IP_NULL, ptmo);

                if (num_ready == 0)
                    break;

                if (num_ready < 0)
                {
                    ipcom_printf("sockperf-r: select failed: %s"IP_LF,
                                 ipcom_strerror(ipcom_errno));
                    return;
                }

                for (i = 0; i < cmd->num_sock; i++)
                {
                    if (IP_FD_ISSET(s[i], &read_set))
                    {
                        bytes = ipcom_recv(s[i], cmd->buf, cmd->buf_len, 0);
                        if (bytes < 0)
                        {
                            ipcom_printf("sockperf-r: recv failed: %s"IP_LF,
                                         ipcom_strerror(ipcom_errno));
                            return;
                        }

                        if (cmd->testpattern)
                        {
                            if (cmd->echo)
                                ipcom_cmd_sockperf_echo_buf(cmd->buf, bytes, cmd->recv_pattern[i]);
                            for (c = 0; c < (Ip_u32)bytes; c += 8)
                            {
                                if (cmd->buf[c] != '#' && cmd->buf[c] != '>' && cmd->buf[c] != '<')
                                {
                                    ipcom_printf("\nsockperf-r: test pattern error, expected B, D or E found %c(%d) offset %ld"IP_LF,
                                                 cmd->buf[c], (int)cmd->buf[c], c);
                                    ipcom_cmd_sockperf_echo_buf(cmd->buf, bytes, 0);
                                    return;
                                }

                                if (ipcom_atoi(cmd->buf + c + 1) != (int)(cmd->recv_pattern[i]++ % 1000000))
                                {
                                    ipcom_printf("\nsockperf-r: test pattern error, was %d should be %ld offset %ld"IP_LF,
                                                 ipcom_atoi(cmd->buf + c + 1), (cmd->recv_pattern[i] - 1) % 1000000, c);
                                    ipcom_cmd_sockperf_echo_buf(cmd->buf, bytes, 0);
                                    return;
                                }
                            }
                        }

                        if (bytes > 0)
                            total_bytes_read += bytes;
                        else
                        {
                            finished_sockets++;
                            IP_FD_CLR(s[i], &cmd->read_set);
                        }
                    }
                }

                if (cmd->transmit && total_bytes_to_send)
                    ptmo = &tmo;
            }
        }
    }
    ipcom_microtime(&stop);

    if (cmd->transmit)
        ipcom_printf("sockperf-t: %lu bytes sent in %ld ms (%lu kbyte/s)"IP_LF,
                     cmd->num_buf * cmd->buf_len,
                     ipcom_cmd_sockperf_tv_to_msec(&start, &stop),
                     cmd->num_buf * cmd->buf_len / (Ip_u32)ipcom_cmd_sockperf_tv_to_msec(&start, &stop) * 1000 / 1024);
    if (cmd->receive)
        ipcom_printf("sockperf-r: %lu bytes read in %ld ms (%lu kbyte/s)"IP_LF,
                     total_bytes_read,
                     ipcom_cmd_sockperf_tv_to_msec(&start, &stop),
                     total_bytes_read / (Ip_u32)ipcom_cmd_sockperf_tv_to_msec(&start, &stop) * 1000 / 1024);

}


/*
 *===========================================================================
 *                    ipcom_cmd_sockperf_connect
 *===========================================================================
 * Description: Active open of socket(s)
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_cmd_sockperf_connect(Ipcom_cmd_sockperf_t *cmd)
{
    Ip_fd            *s = cmd->sock_array;
    Ip_u32            created_sockets;
    struct Ip_timeval start;
    struct Ip_timeval stop;

    ipcom_printf("sockperf-c: connecting to %s[%d]"IP_LF,
                 cmd->res->ai_canonname,
                 ip_ntohs(((struct Ip_sockaddr_in *)cmd->res->ai_addr)->sin_port));
    ipcom_microtime(&start);
    for (created_sockets = 0; created_sockets < cmd->num_sock; created_sockets++)
    {
        s[created_sockets] = ipcom_socket(cmd->res->ai_family,
                                          cmd->res->ai_socktype,
                                          cmd->res->ai_protocol);
        if (s[created_sockets] == IP_SOCKERR)
        {
            ipcom_printf("sockperf-c: Failed to create socket: %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            created_sockets--;
            goto cleanup;
        }

        if (ipcom_setsockopt(s[created_sockets],
                             IP_SOL_SOCKET,
                             IP_SO_LINGER,
                             &cmd->linger,
                             sizeof(cmd->linger)) < 0)
        {
            ipcom_printf("sockperf-c: setsockopt SO_LINGER failed : %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            goto cleanup;
        }

        if (ipcom_connect(s[created_sockets], cmd->res->ai_addr, cmd->res->ai_addrlen) < 0)
        {
            ipcom_printf("sockperf-c: Failed to connect: %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            goto cleanup;
        }
    }
    ipcom_microtime(&stop);
    ipcom_printf("sockperf-t: %ld sockets connected in %ld ms"IP_LF,
                 created_sockets,
                 ipcom_cmd_sockperf_tv_to_msec(&start, &stop));

    ipcom_cmd_sockperf_run(cmd);

 cleanup:
    while (created_sockets > 0)
        (void)ipcom_socketclose(s[created_sockets--]);
}


/*
 *===========================================================================
 *                    ipcom_cmd_sockperf_accept
 *===========================================================================
 * Description: Passive open of socket(s).
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_cmd_sockperf_accept(Ipcom_cmd_sockperf_t *cmd)
{
    Ip_fd            *s = cmd->sock_array;
    int               listen_sock;
    Ip_u32            accepted_sockets = 0;
    struct Ip_timeval start;
    struct Ip_timeval stop;
    int               on = 1;

    listen_sock = ipcom_socket(cmd->res->ai_family,
                               cmd->res->ai_socktype,
                               cmd->res->ai_protocol);

    if (listen_sock == IP_SOCKERR)
    {
        ipcom_printf("sockperf-a: Failed to create socket: %s"IP_LF,
                     ipcom_strerror(ipcom_errno));
        goto cleanup;
    }


    if (ipcom_setsockopt(listen_sock,
                         IP_SOL_SOCKET,
                         IP_SO_REUSEADDR,
                         &on,
                         sizeof(on)) < 0)
    {
        ipcom_printf("sockperf-a: setsockopt IP_SO_REUSEADDR failed : %s"IP_LF,
                     ipcom_strerror(ipcom_errno));
        return;
    }

    if (ipcom_bind(listen_sock, cmd->res->ai_addr, cmd->res->ai_addrlen) < 0)
    {
        ipcom_printf("sockperf-a: Failed to bind: %s"IP_LF,
                     ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    if (ipcom_listen(listen_sock, cmd->num_sock) < 0)
    {
        ipcom_printf("sockperf-a: Failed to listen: %s"IP_LF,
                     ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    ipcom_printf("sockperf-a: listen on %s[%d]"IP_LF,
                 cmd->res->ai_canonname,
                 ip_ntohs(((struct Ip_sockaddr_in *)cmd->res->ai_addr)->sin_port));

    IP_FD_ZERO(&cmd->read_set);
    IP_FD_SET(listen_sock, &cmd->read_set);
    if (ipcom_socketselect(listen_sock + 1, &cmd->read_set, IP_NULL, IP_NULL, IP_NULL) != 1)
    {
        ipcom_printf("sockperf-a: Select failed for listen socket: %s"IP_LF,
                     ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    ipcom_microtime(&start);
    for (;accepted_sockets < cmd->num_sock; accepted_sockets++)
    {
        s[accepted_sockets] = ipcom_accept(listen_sock, IP_NULL, IP_NULL);
        if (s[accepted_sockets] == IP_SOCKERR)
        {
            ipcom_printf("sockperf-a: Failed to create socket: %s"IP_LF,
                     ipcom_strerror(ipcom_errno));
            goto cleanup;
        }
        if (ipcom_setsockopt(s[accepted_sockets],
                             IP_SOL_SOCKET,
                             IP_SO_LINGER,
                             &cmd->linger,
                             sizeof(cmd->linger)) < 0)
        {
            ipcom_printf("sockperf-a: setsockopt SO_LINGER failed : %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            goto cleanup;
        }
    }
    ipcom_microtime(&stop);
    ipcom_printf("sockperf-a: %ld sockets accepted in %ld ms"IP_LF,
                 accepted_sockets,
                 ipcom_cmd_sockperf_tv_to_msec(&start, &stop));

    ipcom_cmd_sockperf_run(cmd);

 cleanup:
    if (listen_sock >= 0)
        (void)ipcom_socketclose(listen_sock);
    while (accepted_sockets-- > 0)
        (void)ipcom_socketclose(s[accepted_sockets]);
}


/*
 *===========================================================================
 *                    ipcom_cmd_sockperf_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_cmd_sockperf_print_usage(void)
{
    ipcom_printf("Network performance tool"IP_LF
                 "usage:  "IP_LF
                 "   sockperf [options] [address]"IP_LF
                 "      -6         use IPv6 (default is IPv4)"IP_LF
                 "      -a         use passive open (default is active open)"IP_LF
                 "      -e         echo test pattern on shell"IP_LF
                 "      -L <sec>   linger time for all sockets (default is 3s)"IP_LF
                 "      -l <bytes> number of bytes to write/read in each call (default 8192)"IP_LF
                 "      -n <num>   number of buffers to write (default 2048)"IP_LF
                 "      -P         create(transmit) or verify(receive) test pattern"IP_LF
                 "      -p <port>  socket port (default 7373)"IP_LF
                 "      -R         socket receive buffer size (default 16384)"IP_LF
                 "      -r         receive data (not mutually exclusive with -t)"IP_LF
                 "      -s <num>   number of sockets to use, must be same on both ends (default 1)"IP_LF
                 "      -T         socket send buffer size (default 32767)"IP_LF
                 "      -t         transmit data (not mutually exclusive with -r)"IP_LF
                 "example:"IP_LF
                 "   \"sockperf -r -a\" and \"sockperf -t <addr>\" should give about the same"IP_LF
                 "   performance value as \"ttcp -r\" and \"ttcp <addr>\""IP_LF
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
 *                    ipcom_cmd_sockperf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_cmd_sockperf(int argc, char **argv)
{
    Ipcom_getopt         opt;
    Ipcom_cmd_sockperf_t cmd;
    int                  c;
    struct Ip_addrinfo   hints;
    char                *hostname = "0.0.0.0";
    char                *servname = "7373";
    int                  socktype = IP_SOCK_STREAM;
    int                  domain = IP_AF_INET;

    if (argc < 2)
    {
        ipcom_cmd_sockperf_print_usage();
        return 0;
    }

    ipcom_memset(&cmd, 0, sizeof(cmd));
    cmd.buf_len         = 8192;
    cmd.num_buf         = 2048;
    cmd.num_sock        = 1;
    cmd.linger.l_onoff  = 1;
    cmd.linger.l_linger = 3;
    cmd.sendbuf_size    = 32767;
    cmd.recvbuf_size    = 16384;

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "6aeL:l:n:Pp:R:rs:T:tu", &opt)) != -1)
    {
        switch(c)
        {
        case '6':
            domain = IP_AF_INET6;
            hostname = "::";
            break;
        case 'a':
            cmd.accept = IP_TRUE;
            break;
        case 'e':
            cmd.echo = IP_TRUE;
            break;
        case 'L':
            cmd.linger.l_linger = ipcom_atoi(opt.optarg);
            break;
        case 'l':
            cmd.buf_len = ipcom_atoi(opt.optarg);
            break;
        case 'n':
            cmd.num_buf = ipcom_atoi(opt.optarg);
            break;
        case 'P':
            cmd.testpattern = IP_TRUE;
            break;
        case 'p':
            servname = opt.optarg;
            break;
        case 'R':
            cmd.recvbuf_size = ipcom_atoi(opt.optarg);
            break;
        case 'r':
            cmd.receive = IP_TRUE;
            break;
        case 's':
            cmd.num_sock = ipcom_atoi(opt.optarg);
            break;
        case 'T':
            cmd.sendbuf_size = ipcom_atoi(opt.optarg);
            break;
        case 't':
            cmd.transmit = IP_TRUE;
            break;
        case 'u':
            socktype = IP_SOCK_DGRAM;
            break;
        default:
            ipcom_printf("sockperf: unknown option %c"IP_LF, (char)c);
            return -1;
        }
    }

    if (cmd.testpattern)
    {
        if (cmd.buf_len & 0x7)
        {
            ipcom_printf("sockperf: the buffert (set with -l) must be a multiple of 8 when "
                         "using a test pattern"IP_LF);
            goto cleanup;
        }

        if (cmd.transmit)
        {
            cmd.send_pattern = ipcom_calloc(cmd.num_sock, sizeof(Ip_u32));
            if (cmd.send_pattern == IP_NULL)
            {
                ipcom_printf("sockperf: out of memory when allocating send pattern"IP_LF);
                goto cleanup;
            }
        }
        if (cmd.receive)
        {
            cmd.recv_pattern = ipcom_calloc(cmd.num_sock, sizeof(Ip_u32));
            if (cmd.recv_pattern == IP_NULL)
            {
                ipcom_printf("sockperf: out of memory when allocating receive pattern"IP_LF);
                goto cleanup;
            }
        }
    }

    if (opt.optind < argc)
        hostname = argv[opt.optind];

    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = socktype;
    hints.ai_flags    = IP_AI_CANONNAME;
    hints.ai_family   = domain;
    if (ipcom_getaddrinfo(hostname, servname, &hints, &cmd.res) != 0)
    {
        ipcom_printf("sockperf: getaddrinfo() failed"IP_LF);
        goto cleanup;
    }

    cmd.buf = ipcom_malloc(cmd.buf_len + 1);
    if (cmd.buf == IP_NULL)
    {
        ipcom_printf("sockperf: Failed to allocate %lu bytes" IP_LF, cmd.buf_len);
        goto cleanup;
    }

    cmd.sock_array = ipcom_malloc(cmd.num_sock * sizeof(int));
    if (cmd.sock_array == IP_NULL)
    {
        ipcom_printf("sockperf: Failed to allocate %lu bytes for sockets" IP_LF,
                     cmd.num_sock * sizeof(int));
        goto cleanup;
    }

    if (cmd.accept)
        ipcom_cmd_sockperf_accept(&cmd);
    else
        ipcom_cmd_sockperf_connect(&cmd);

 cleanup:
    ipcom_free(cmd.send_pattern);
    ipcom_free(cmd.recv_pattern);
    ipcom_free(cmd.sock_array);
    ipcom_free(cmd.buf);
    ipcom_freeaddrinfo(cmd.res);
    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

