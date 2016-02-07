/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_smptest.c,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_smptest.c,v $
 *   $Author: gulgi $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Adam Skoglund <adam.skoglund@windriver.com>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * SMP stresstest.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#ifdef IPCOM_USE_SMPTEST_CMD

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_cstyle.h"
#include "ipcom_type.h"
#include "ipcom_getopt.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"
#include "ipcom_atomic.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define MAX_TASKS       40
#define SECONDS_CLIENT  5
#define SECONDS_SERVER  45

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_cmd_smptest_struct
{
    Ip_bool                 server;
    Ip_bool                 ip4;
    Ip_bool                 tcp;
    Ip_u32                  num_bytes;
    Ip_u32                  num_sock;
    Ip_u32                  port;

    struct Ip_addrinfo      *res;
} Ipcom_cmd_smptest_t;


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

IP_PUBLIC int ipcom_cmd_smptest(int argc, char **argv);
IP_STATIC IPCOM_PROCESS( ipcom_cmd_smptest_server_spawn );
IP_STATIC IPCOM_PROCESS( ipcom_cmd_smptest_client_spawn );


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

static Ipcom_sem        sem_wait_client;
static Ipcom_sem        sem_wait_server;
static Ipcom_atomic_t   num_wait_client;
static Ipcom_atomic_t   num_wait_server;
static Ipcom_atomic_t   listen_wait;
static int              return_client       = 0;
static int              return_server       = 0;
static Ipcom_cmd_smptest_t  smp_opt_client;
static Ipcom_cmd_smptest_t  smp_opt_server;
static volatile Ip_u32           spawn_number_client = 0;
static volatile Ip_u32           spawn_number_server = 0;
static Ip_bool          verbose             = 0;

static Ip_fd            server_out          = 0;

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_cmd_smptest_print_usage
 *===========================================================================
 * Description: Print usage information for the SMP test shell command.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipcom_cmd_smptest_print_usage(void)
{
   ipcom_printf( "SMP stress test"IP_LF
                 "usage:  "IP_LF
                 "   smptest [options] [address]"IP_LF
                 "      -c         client mode (default is server mode)"IP_LF
                 "      -6         use IPv6 (default is IPv4)"IP_LF
                 "      -u         use UDP (default is TCP)"IP_LF
                 "      -l <bytes> number of bytes to write/read in each call (default 1024)"IP_LF
                 "      -p <port>  socket port (default 8484)"IP_LF
                 "      -s <num>   number of sockets to use, must be same on both ends (default 1)"IP_LF
                 "      -v         verbose"IP_LF
                 "      -h         this help"IP_LF
                 IP_LF
                 );
}


/*
 *===========================================================================
 *                    ipcom_cmd_smptest_server
 *===========================================================================
 * Description: This is the server part of the smptest command
 * Parameters:  The address we should listen to, and info about how many sockets
 *              we should use.
 * Returns:     0 on success. -1 on failure.
 */
IP_STATIC int
ipcom_cmd_smptest_server()
{
    Ipcom_proc_attr    attr;
    unsigned int i;
    Ipcom_proc *proc;
    int retval;

    spawn_number_server     = 0;

    if ( 0 != ipcom_sem_create( &sem_wait_server, 0 ) )
    {
        ipcom_printf("ipcom_sem_create failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -1;
    }
    ipcom_atomic_set( &num_wait_server, smp_opt_server.num_sock );
    ipcom_atomic_set( &listen_wait, smp_opt_server.num_sock );

    ipcom_proc_attr_init(&attr);
    attr.priority   = IPCOM_PRIORITY_DEFAULT;


    /* Hacky hacky way to write from a task.. copied from ipcom_mshell.c */
    proc = ipcom_proc_self();
    ip_assert( proc != 0 );
    server_out = proc->shell_fd;

    for ( i = 0;i < smp_opt_server.num_sock;++i )
    {
        retval = ipcom_proc_acreate("ipcom_cmd_smptest_server_spawn",
                                        (Ipcom_proc_func)ipcom_cmd_smptest_server_spawn, &attr, 0 );
        if (retval != 0)
        {
            ipcom_printf( "Error spawning smptest server"IP_LF );
            return retval;
        }
    }

    ipcom_sem_wait( sem_wait_server );

    ipcom_sem_delete( &sem_wait_server );

    return 0;
}



/*
 *===========================================================================
 *                    ipcom_cmd_smptest_server_spawn
 *===========================================================================
 * Description: This is the server spawn part of the smptest command
 * Parameters:  None
 * Returns:     Nothing.
 */

IP_STATIC
IPCOM_PROCESS( ipcom_cmd_smptest_server_spawn )
{
    union
    {
        struct Ip_sockaddr          sa;
#ifdef IPCOM_USE_INET6
        struct Ip_sockaddr_in6      sin6;
#endif
        struct Ip_sockaddr_storage  ss;
        struct Ip_sockaddr_in       sin;
    } addr;

    Ip_fd  listen_socket;
    Ip_fd  connect_sock = 0;
    int    opt_val = 1;
    int    portadd = spawn_number_server++;
    int    bytes = 0;
    int    sec = SECONDS_CLIENT + SECONDS_SERVER;
    unsigned char *buf = 0;
    int    num_recives = 0;
    unsigned long num_bytes = 0;
    struct Ip_sockaddr from;
    struct Ip_linger   linger;
    Ip_socklen_t       from_length = 0;
    int     retry_count = 0;

    ipcom_proc_init();

    ipcom_memset( &addr.ss, 0, sizeof( addr.ss ) );
    ipcom_memcpy( &addr.sa, smp_opt_server.res->ai_addr, smp_opt_server.res->ai_addrlen );

    linger.l_onoff  = 1;
    linger.l_linger = 2;

    listen_socket = ipcom_socket( smp_opt_server.res->ai_family, smp_opt_server.res->ai_socktype, smp_opt_server.res->ai_protocol );
    if (listen_socket == IP_SOCKERR)
    {
        ipcom_printf("Failed to create socket: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return_server = 1;
        ipcom_sem_post( sem_wait_server );
        return;
    }

    if ( 0 != ipcom_setsockopt(listen_socket, IP_SOL_SOCKET, IP_SO_REUSEPORT, &opt_val, sizeof (opt_val)) )
    {
        ipcom_printf("ipcom_setsockopt failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return_server = 1;
        ipcom_sem_post( sem_wait_server );
        return;
    }

    addr.sin.sin_port = ip_htons( smp_opt_server.port + portadd );   /* port is in the same place for IPv4 and IPv6 */
    if ( 0 != ipcom_bind( listen_socket, &addr.sa, sizeof( addr.ss ) ) )
    {
        ipcom_printf("Failed to bind: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return_server = 1;
        ipcom_sem_post( sem_wait_server );
        return;
    }

    buf = ipcom_malloc( smp_opt_server.num_bytes );

    if ( smp_opt_server.tcp )
    {
        if ( -1 == ipcom_listen( listen_socket, 0 ) )
        {
            ipcom_printf("Listen failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
            return_server = 1;
            ipcom_sem_post( sem_wait_server );
            return;
        }
        if ( verbose )
            ipcom_printf("Thread %d listens to %s:%d"IP_LF, portadd, smp_opt_server.res->ai_canonname, ip_ntohs( addr.sin.sin_port ) );
    }

    if ( 0 == ipcom_atomic_sub_and_return( &listen_wait, 1 ) )
    {
        /* Send to the test-server or write to stdout? */
        if ( server_out >= 0 )
        {
            char ready[] = "Ready.";
            ipcom_socketwrite(server_out, ready, 8 );
        }
        else
            ipcom_printf("Ready."IP_LF );
    }

    if ( 0 != ipcom_socketioctl( listen_socket, IP_X_SIOCSINTR, &sec ) )
    {
        ipcom_printf("ipcom_socketioctl failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return_server = 1;
        ipcom_sem_post( sem_wait_server );
        return;
    }

    if ( smp_opt_server.tcp )
    {
retry:
        connect_sock = ipcom_accept( listen_socket, IP_NULL, 0 );
        if ( -1 == connect_sock )
        {
            if ( ipcom_errno == IP_ERRNO_EINTR )
            {
                if ( verbose )
                {
                    if ( ++retry_count < 5 )
                    {
                        ipcom_printf("Accept failed for thread %d: %s.. Retrying."IP_LF, portadd, ipcom_strerror(ipcom_errno));
                        goto retry;
                    }
                }
            }
            ipcom_printf("Accept failed for thread %d: %s"IP_LF, portadd, ipcom_strerror(ipcom_errno));
            return_server = 1;
            ipcom_socketclose( listen_socket );
            ipcom_sem_post( sem_wait_server );
            return;
        }

        if ( 0 != ipcom_socketioctl( connect_sock, IP_X_SIOCSINTR, &sec ) )
        {
            ipcom_printf("ipcom_socketioctl failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
            return_server = 1;
            ipcom_sem_post( sem_wait_server );
            return;
        }

        if ( 0 != ipcom_setsockopt( connect_sock, IP_SOL_SOCKET, IP_SO_LINGER, &linger, sizeof (linger)) )
        {
            ipcom_printf("ipcom_setsockopt failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
            return_server = 1;
            ipcom_sem_post( sem_wait_server );
            return;
        }
    }

    while ( 1 )
    {
        num_recives++;
        if ( smp_opt_server.tcp )
            bytes = ipcom_recv( connect_sock, buf, smp_opt_server.num_bytes, 0);
        else
        {
            bytes = ipcom_recvfrom( listen_socket, buf, smp_opt_server.num_bytes, 0, &from, &from_length );
            if ( num_recives == 1 && 0 == smp_opt_server.tcp )
            {
                sec = SECONDS_CLIENT+2+portadd;
                if ( 0 != ipcom_socketioctl( listen_socket, IP_X_SIOCSINTR, &sec ) )
                {
                    ipcom_printf("ipcom_socketioctl failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
                    return_server = 1;
                    ipcom_sem_post( sem_wait_server );
                    return;
                }
            }
        }

        if ( bytes > 0 )
            num_bytes += bytes;
        if (bytes == 0)
        {
            if ( verbose )
            {
                ipcom_printf("Thread %d done."IP_LF, portadd );
                ipcom_printf("  Recives: %d"IP_LF, num_recives );
                ipcom_printf("  MB/s:    %f"IP_LF, ((float)(num_bytes)/(1024.0f*1024.0f) )/5.0f );
            }
            break;
        }
        else if (bytes < 0)
        {
            if ( ipcom_errno == IP_ERRNO_EINTR )
            {
                if ( smp_opt_server.tcp )
                {
                    return_server = 1;
                    ipcom_printf("Error! Out of time!"IP_LF );
                    break;
                }
                else
                {
                    if ( verbose )
                    {
                        ipcom_printf("Thread %d done."IP_LF, portadd );
                        ipcom_printf("  Recives: %d"IP_LF, num_recives );
                        ipcom_printf("  MB/s:    %f"IP_LF, ((float)(num_bytes)/(1024.0f*1024.0f) )/5.0f );
                    }
                    break;
                }
            }
            else if ( bytes == IP_SOCKERR )    /* Connection reset by peer */
            {
            }

            return_server = 1;
            ipcom_printf("recv failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
            return;
        }
    }

    if ( verbose )
    {
        if ( spawn_number_server != smp_opt_server.num_sock )
        {
            ipcom_printf("Error. Only %d server-sockets seemed to work."IP_LF, spawn_number_server );
        }
    }

    if ( smp_opt_server.tcp )
        ipcom_shutdown( connect_sock, IP_SHUT_RDWR );

    ipcom_free( buf );
    ipcom_socketclose( listen_socket );

    if ( 0 == ipcom_atomic_sub_and_return( &num_wait_server, 1 ) )
        ipcom_sem_post( sem_wait_server );

    if ( smp_opt_server.tcp )
        ipcom_socketclose( connect_sock );

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_cmd_smptest_client
 *===========================================================================
 * Description: This is the client part of the smptest command
 * Parameters:  The address we should connect to, and info about how many sockets
 *              we should use.
 * Returns:     0 on success. -1 on failure.
 */
IP_STATIC int
ipcom_cmd_smptest_client()
{
    Ipcom_proc_attr    attr;
    unsigned int i;
    int retval;

    spawn_number_client     = 0;

    if ( 0 != ipcom_sem_create( &sem_wait_client, 0 ) )
    {
        ipcom_printf("ipcom_sem_create failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -1;
    }
    ipcom_atomic_set( &num_wait_client, smp_opt_client.num_sock );

    ipcom_proc_attr_init(&attr);
    attr.priority   = IPCOM_PRIORITY_DEFAULT;

    for ( i = 0;i < smp_opt_client.num_sock;++i )
    {
        retval = ipcom_proc_acreate("ipcom_cmd_smptest_client_spawn",
                                        (Ipcom_proc_func)ipcom_cmd_smptest_client_spawn, &attr, 0 );
        if (retval != 0)
        {
            ipcom_printf( "Error spawning smptest client"IP_LF );
            return retval;
        }
    }

    ipcom_sem_wait( sem_wait_client ); /* wait for tasks to complete */

    ipcom_sem_delete( &sem_wait_client );

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_smptest_client_spawn
 *===========================================================================
 * Description: This is the server spawn part of the smptest command
 * Parameters:  None
 * Returns:     Nothing.
 */

IP_STATIC
IPCOM_PROCESS( ipcom_cmd_smptest_client_spawn )
{
    union
    {
        struct Ip_sockaddr          sa;
#ifdef IPCOM_USE_INET6
        struct Ip_sockaddr_in6      sin6;
#endif
        struct Ip_sockaddr_storage  ss;
        struct Ip_sockaddr_in       sin;
    } addr;
    Ip_fd socket;
    unsigned char *buf = 0;
    int portadd = spawn_number_client++;
    int opt_val = 1;
    int sec = SECONDS_CLIENT;
    int send = 0;
    int num_sends = 0;
    unsigned long num_bytes = 0;

    ipcom_proc_init();

    ipcom_memset( &addr.ss, 0, sizeof( addr.ss ) );
    ipcom_memcpy( &addr.sa, smp_opt_client.res->ai_addr, smp_opt_client.res->ai_addrlen );

    socket = ipcom_socket( smp_opt_client.res->ai_family, smp_opt_client.res->ai_socktype, smp_opt_client.res->ai_protocol );
    if (socket == IP_SOCKERR)
    {
        ipcom_printf("Failed to create socket for thread %d: %s"IP_LF, portadd, ipcom_strerror(ipcom_errno));
        return_client = 1;
        ipcom_sem_post( sem_wait_client );
        return;
    }

    if ( 0 != ipcom_setsockopt(socket, IP_SOL_SOCKET, IP_SO_REUSEPORT, &opt_val, sizeof (opt_val)) )
    {
        ipcom_printf("ipcom_setsockopt failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return_client = 1;
        ipcom_sem_post( sem_wait_client );
        return;
    }

    if ( 0 != ipcom_socketioctl( socket, IP_X_SIOCSINTR, &sec ) )
    {
        ipcom_printf("ipcom_socketioctl failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return_client = 1;
        ipcom_sem_post( sem_wait_client );
        return;
    }

    addr.sin.sin_port = ip_htons( smp_opt_client.port + portadd );   /* port is in the same place for IPv4 and IPv6 */
    if ( smp_opt_client.tcp )
    {
        if ( 0 != ipcom_connect( socket, &addr.sa, smp_opt_client.res->ai_addrlen ) )
        {
            ipcom_printf("Thread %d failed to connect: %s"IP_LF, portadd, ipcom_strerror(ipcom_errno));
            return_client = 1;
            ipcom_sem_post( sem_wait_client );
            return;
        }
        if ( verbose )
            ipcom_printf("Thread %d connected to port %d"IP_LF, portadd, smp_opt_client.port + portadd );
    }

    buf = ipcom_malloc( smp_opt_client.num_bytes );
    while ( 1 )
    {
        if ( smp_opt_client.tcp )
            send = ipcom_send( socket, buf, smp_opt_client.num_bytes, 0);
        else
            send = ipcom_sendto( socket, buf, smp_opt_client.num_bytes, 0, &addr.sa, smp_opt_client.res->ai_addrlen );

        if ( send > 0 )
           num_bytes += send;
        num_sends++;
        if ( send == 0 )
        {
           ipcom_printf( "Error: Disconnected"IP_LF );
           return_client = 1;
           break;
        }
        else if ( send < 0 )
        {
            if ( ipcom_errno == IP_ERRNO_EINTR )
            {
                if ( verbose )
                {
                    ipcom_printf("Thread %d done."IP_LF, portadd );
                    ipcom_printf("  Sends: %d"IP_LF, num_sends );
                    ipcom_printf("  MB/s:  %f"IP_LF, ((float)(num_bytes)/(1024.0f*1024.0f) )/5.0f );
                }
                break;
            }
            return_client = 1;
            ipcom_printf("Error on thread %d: %s"IP_LF, portadd, ipcom_strerror(ipcom_errno));
            break;
        }
    }

    if ( verbose )
    {
        if ( spawn_number_client != smp_opt_client.num_sock )
        {
            ipcom_printf("Error. Only %d client-sockets seemed to work."IP_LF, spawn_number_client );
        }
    }

    ipcom_socketclose( socket );

    ipcom_free( buf );
    if ( 0 == ipcom_atomic_sub_and_return( &num_wait_client, 1 ) )
        ipcom_sem_post( sem_wait_client );

    ipcom_proc_exit();
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

IP_PUBLIC int
ipcom_cmd_smptest(int argc, char **argv)
{
    Ipcom_getopt         opt;
    int                  c;
    struct Ip_addrinfo   hints;
    struct Ip_addrinfo  *res;
    char                *hostname = "0.0.0.0";
    char                *servname = "8484";
    int                  socktype = IP_SOCK_STREAM;
    int                  domain = IP_AF_INET;
    Ipcom_cmd_smptest_t  smp_opt;

    return_client           = 0;
    return_server           = 0;
    verbose                 = 0;
    ipcom_memset(&smp_opt, 0, sizeof(smp_opt));
    smp_opt.server          = 1;
    smp_opt.ip4             = 1;
    smp_opt.tcp             = 1;
    smp_opt.num_bytes       = 1024;
    smp_opt.num_sock        = 1;
    smp_opt.port            = 8484;

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "l:p:s:c6uhv", &opt)) != -1)
    {
        switch(c)
        {
        case 'c':
            smp_opt.server = 0;
            if ( verbose )
                ipcom_printf( "client"IP_LF );
            break;
        case '6':
            smp_opt.ip4 = 0;
            domain = IP_AF_INET6;
            hostname = "::";
            if ( verbose )
                ipcom_printf( "IPv6"IP_LF );
            break;
        case 'u':
            smp_opt.tcp = 0;
            socktype = IP_SOCK_DGRAM;
            if ( verbose )
                ipcom_printf( "UDP"IP_LF );
            break;
        case 'l':
            smp_opt.num_bytes = ipcom_atoi(opt.optarg);
            if ( verbose )
                ipcom_printf( "num bytes: %d"IP_LF, smp_opt.num_bytes );
            break;
        case 'p':
            servname = opt.optarg;
            smp_opt.port = ipcom_atoi(opt.optarg);
            if ( verbose )
                ipcom_printf( "port: %s"IP_LF, servname );
            break;
        case 's':
            smp_opt.num_sock = ipcom_atoi(opt.optarg);
            if ( smp_opt.num_sock > MAX_TASKS )
                smp_opt.num_sock = MAX_TASKS;
            if ( verbose )
                ipcom_printf( "sockets: %d"IP_LF, smp_opt.num_sock );
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
            ipcom_cmd_smptest_print_usage();
            return -1;
        default:
            ipcom_printf("smptest: unknown option %c"IP_LF, (char)c);
            ipcom_cmd_smptest_print_usage();
            return -1;
        }
    }

   if (opt.optind < argc)
        hostname = argv[opt.optind];
    else
    {
        if ( 0 == smp_opt.server )
        {
            if ( smp_opt.ip4 )
                hostname = "127.0.0.1";
            else
                hostname = "::1";
           }
    }

    if ( verbose )
        ipcom_printf( "%s : %s"IP_LF, hostname, servname );

    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = socktype;
    hints.ai_flags    = IP_AI_CANONNAME;
    hints.ai_family   = domain;
    if (ipcom_getaddrinfo(hostname, servname, &hints, &res) != 0)
    {
        ipcom_printf("sockperf: getaddrinfo() failed"IP_LF);
        goto cleanup;
    }

    smp_opt.res = res;

    if ( smp_opt.server )
        smp_opt_server = smp_opt;
    else
        smp_opt_client = smp_opt;

    if ( smp_opt.server == 1 )
    {
        if ( 0 != ipcom_cmd_smptest_server() || 0 != return_server )
        {
            ipcom_printf( "Failure."IP_LF );
            goto cleanup;
        }
    }
    else
    {
        if ( 0 != ipcom_cmd_smptest_client() || 0 != return_client )
        {
            ipcom_printf( "Failure"IP_LF );
            goto cleanup;
        }
    }

    ipcom_printf( "Success." );    /* Success */

cleanup:
    ipcom_freeaddrinfo( res );
    return 0;
}

#else
int ipcom_cmd_smptest_empty_file;
#endif /* #ifdef IPCOM_USE_SMPTEST_CMD */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

