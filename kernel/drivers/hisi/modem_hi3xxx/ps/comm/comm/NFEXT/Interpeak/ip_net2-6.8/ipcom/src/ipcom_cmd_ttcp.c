/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_ttcp.c,v $ $Revision: 1.104 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_ttcp.c,v $
 *     $Author: fguo $ $Date: 2009-07-22 09:38:56 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Firstname Lastname <email@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1  DESCRIPTION.
 *
 * Test TCP connection.  Makes a connection on default port 5001
 * and transfers fabricated buffers or data copied from stdin.
 *
 * Machines using System V with BSD sockets should define SYSV.
 *
 * HISTORY OF DEVELOPMENT.
 *
 * Modified for operation under 4.2BSD, 18 Dec 84
 *      T.C. Slattery, USNA
 * Minor improvements, Mike Muuss and Terry Slattery, 16-Oct-85.
 * Modified in 1989 at Silicon Graphics, Inc.
 *      catch SIGPIPE to be able to print stats when receiver has died
 *      for tcp, don't look for sentinel during reads to allow small transfers
 *      increased default buffer size to 8K, nbuf to 2K to transfer 16MB
 *      moved default port to 5001, beyond IPPORT_USERRESERVED
 *      make sinkmode default because it is more popular,
 *              -s now means don't sink/source
 *      count number of read/write system calls to see effects of
 *              blocking from full socket buffers
 *      for tcp, -D option turns off buffered writes (sets TCP_NODELAY sockopt)
 *      buffer alignment options, -A and -O
 *      print stats in a format that's a bit easier to use with grep & awk
 *      for SYSV, mimic BSD routines to use most of the existing timing code
 * Modified by Steve Miller of the University of Maryland, College Park
 *      -b sets the socket buffer size (SO_SNDBUF/SO_RCVBUF)
 * Modified Sept. 1989 at Silicon Graphics, Inc.
 *      restored -s sense at request of tcs@brl
 * Modified Oct. 1991 at Silicon Graphics, Inc.
 *      use getopt(3) for option processing, add -f and -T options.
 *      SGI IRIX 3.3 and 4.0 releases don't need #define SYSV.
 * Rewritten (being careful not to affect test loop performance) to use Interpeak
 *      IPCOM APIs. Added IPv6 support. October 2001, Lennart Bång.
 *
 * Distribution Status -
 *      Public Domain.  Distribution Unlimited.
 */


/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#define DEFAULT_PORT       5001

#ifdef IPCOM_SOCK
#define DEFAULT_UDP_RCVBUF     10000
#define DEFAULT_UDP_SNDBUF     10000
#define DEFAULT_TCP_RCVBUF     16384
#define DEFAULT_TCP_SNDBUF     32767
#else
#define DEFAULT_UDP_RCVBUF     16384
#define DEFAULT_UDP_SNDBUF     32767
#define DEFAULT_TCP_RCVBUF     16384
#define DEFAULT_TCP_SNDBUF     32767
#endif

/* Includes ipcom_fwrite and ipcom_fread:
#define IPCOM_TTCP_USE_STDIOMODE
*/

/* Do not set if want exact ttcp comparison numbers. */
#define IPCOM_TTCP_REENTRANT


/*
 ****************************************************************************
 * 3  INCLUDE FILES.
 ****************************************************************************
 */

#if defined(IP_PORT_INTEGRITY)
#include "ipcom_ipcom2bsd.h"
#endif

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_getopt.h"
#include "ipcom_os.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_time.h"
#include "ipcom_type.h"
#include "ipcom_err.h"

#ifdef IP_PORT_INTEGRITY
#include "ipcom_ipcom2bsd.h"
#endif

#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
/*#define IPCOM_CMD_SOCKTEST_USE_BSD_API*/
#endif

#if defined(IPCOM_CMD_SOCKTEST_USE_BSD_API)
#include "ipcom_ipcom2bsd.h"
#endif

#ifdef IPSCTP
#include "ipsctp_sock.h"
#endif
/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define UDPMINLEN    1
#define ERR         -1

#ifdef IPCOM_TTCP_REENTRANT
#define NWRITE(fd,buf,len,ctx)    Nwrite(fd,buf,len,&addrhim,ctx)
#define ZERONWRITE(fd,len,ctx) ZeroNwrite(fd,len,&addrhim,ctx)
#else
#define NWRITE(fd,buf,len,ctx)    Nwrite(fd,buf,len,ctx)
#define ZERONWRITE(fd,len,ctx) ZeroNwrite(fd,len,ctx)
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */
struct Ipcom_cmd_ttcp_data;

/* Local functions. */
int ipcom_cmd_ttcp(int argc, char **argv);

static void   mes(int trans, char *s);
static void   pattern(char *cp, int cnt);
static int    Nread(int fd, void *ibuf, int count, struct Ipcom_cmd_ttcp_data *context);
#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
static int    ZeroNread(int fd, struct Ipcom_cmd_ttcp_data *context);
#endif
#ifdef IPCOM_TTCP_REENTRANT
static int    Nwrite(int fd, void *obuf, int count, union Ip_sockaddr_union *addrhimp, struct Ipcom_cmd_ttcp_data *context);
#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
static int    ZeroNwrite(int fd, int count, union Ip_sockaddr_union *addrhimp, struct Ipcom_cmd_ttcp_data *context);
#endif
#else
static int    Nwrite(int fd, void *obuf, int count, struct Ipcom_cmd_ttcp_data *context);
#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
static int    ZeroNwrite(int fd, int count, struct Ipcom_cmd_ttcp_data *context);
#endif
#endif

#ifdef IPCOM_USE_TCP
static int    mread(int fd, char *bufp, unsigned n, struct Ipcom_cmd_ttcp_data *context);
#endif

IP_PUBLIC int ipcom_getsockaddrbyaddrname(int domain, Ip_bool no_dns, IP_CONST char *addr_or_name, struct Ip_sockaddr *sa);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         USAGE
 *===========================================================================
 */

/*
 *===========================================================================
 *                         STATIC DATA
 *===========================================================================
 */

struct Ipcom_cmd_ttcp_data
{
    int   udp;		   /* 0 = tcp, !0 = udp */
    int   b_flag;		   /* use mread() */
    int   touchdata;		   /* access data after reading */
    int   err_no;
    Ip_u32   numCalls;	   /* # of I/O system calls */

    int family;
    Ip_socklen_t addrsize;
#ifndef IPCOM_TTCP_REENTRANT
    union Ip_sockaddr_union  addrhim;
#endif
};



/*
 ****************************************************************************
 * 8                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ttcperror
 *===========================================================================
 */
#define ttcperror(ctx,expr) do { \
  (ctx)->err_no = ipcom_errno; \
  ipcom_fprintf(ip_stderr, "ttcp error: %s (line %d, errno %d)."IP_LF, expr, __LINE__, (ctx)->err_no); \
  goto errorout; \
  } while((0))


/*
 *===========================================================================
 *                         terror
 *===========================================================================
 */
#define terror(ctx, expr)     softbreak(ctx, expr, __LINE__)

static void
softbreak(struct Ipcom_cmd_ttcp_data *context, char *expr, unsigned int line)
{
    char buf[256];
    context->err_no = ipcom_errno;

    ipcom_sprintf(buf, "TERROR: %s [line %d]  [errno %d]", expr, line, context->err_no);
    ipcom_perror(buf);
}


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
 *                         ipcom_cmd_ttcp
 *===========================================================================
 */
int
ipcom_cmd_ttcp(int argc, char **argv)
{
    /* Uninitialized variables. */
    union Ip_sockaddr_union     addrme;
    char                        *host = IP_NULL;	/* ptr to name of host */
    int                         c;
    Ip_u32                      msec0, msec1, msecd, bpermsec, kbpersec;
    Ip_fd                       sockfd = -1;
    char                        *buf;		/* ptr to dynamic buffer */
    char                        *orgbuf = 0;     /* the buffer to free. */
    struct Ipcom_cmd_ttcp_data  context;

    /* misc initialized variables. */
#ifdef IPCOM_USE_TCP
    union Ip_sockaddr_union frominet;
    Ip_socklen_t           fromlen;
    int                   one = 1; /* for 4.3 BSD style setsockopt() */
    Ip_fd                  oldfd;	  /* fd of network socket */
#endif
    Ip_u32    nbytes = 0;		  /* bytes on net */
    int    trans = 1;	          /* 0=receive, !0=transmit mode. Default transmit! */

    /* Configuration variables. */
#if defined(IP_SO_X_VR)
    int    vr = 0;
#endif
    int    buflen = 8 * 1024;	/* length of buffer */
    int    nbuf   = 2 * 1024;	/* number of buffers to send in sinkmode */
    int    bufoffset = 0;	/* align buffer to this */
    int    bufalign = 16;	/* modulo this */
    int    options = 0;		/* socket options */
    unsigned short  port = DEFAULT_PORT;	/* TCP port number */
    unsigned short  meport = 0;       	/* local port, default 0. */
    int    sinkmode = 1;		/* 0=normal I/O, !0=sink/source mode */
    int    verbose = 0;		/* 0=print basic info, 1=print cpu rate, proc resource usage. */
    int    sockbufsize = -1;	/* Default socket buffer size to use. */
#ifdef IP_TCP_NODELAY
    int    nodelay = 0;		/* set TCP_NODELAY socket option. */
#endif
    int    enable = 1;
#ifdef TCP_OSERFC2385
    char   *md5opt = IP_NULL;
#endif
#ifdef IP_SO_REUSEPORT
    int     reuse_port = 0;
#endif
#ifdef IPCOM_TTCP_REENTRANT
    union Ip_sockaddr_union  addrhim;
#endif
    int     sock_error;
    int     mev = 0;                /* Use event logging */
    int     nend = 5;               /* Number of UDP end packets */
#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
    int     zerocopy = 0;
#endif
    int     cpu = -1;
#ifdef IPSCTP
    Ip_bool sctp_type_conflict = IP_FALSE;
#endif

    /* Initialize some static data, default configuration. */
#ifdef IPCOM_USE_TCP
    int     proto = IP_IPPROTO_TCP;
    int     sock_type = IP_SOCK_STREAM;
    const char *proto_as_str = "TCP";
    context.udp         = 0;    /* Default is TCP. */
#else
    int     proto = IP_IPPROTO_UDP;
    int     sock_type = IP_SOCK_DGRAM;
    const char *proto_as_str = "UDP";
    context.udp         = 1;    /* Only UDP. */
#endif
    context.b_flag      = 0;
    context.touchdata   = 0;
    context.numCalls    = 0;
    context.err_no       = 0;
#ifdef IPCOM_USE_INET
    context.family      = IP_AF_INET;  /* default IPv4 */
#else
    context.family      = IP_AF_INET6;  /* default IPv6 */
#endif
    context.addrsize    = sizeof(struct Ip_sockaddr_in);

    (void)ipcom_proc_self();	/* when called from vxworks shell, this prevents a crash */

    (void)options;

    /* Init some more. */
    ipcom_memset(&addrhim, 0, sizeof (addrhim));
    IPCOM_SA_LEN_SET(&addrhim.sin, sizeof(struct Ip_sockaddr_in));
    addrhim.sin.sin_family = IP_AF_INET;

    ipcom_memset(&addrme, 0, sizeof(addrme));
    IPCOM_SA_LEN_SET(&addrme.sin, sizeof(struct Ip_sockaddr_in));
    addrme.sin.sin_family = IP_AF_INET;

#ifdef IP_PORT_INTEGRITY
    ipcom_init_libsocket();
#endif

    if (argc < 2)
        goto usage;
    ipcom_getopt_clear();
    while ((c = ipcom_getopt(argc, argv, "M:xdrstUuvBDTb:f:l:n:p:A:O:V:RP:me:a:cS")) != -1)
    {
        switch (c)
        {
#ifdef TCP_OSERFC2385
        case 'M':			/* MD5 signature option */
            md5opt = ip_optarg;
            break;
#endif
#ifdef IPCOM_USE_INET6
        case 'x':
            context.family = IP_AF_INET6;
            break;
#endif
        case 'm':
            mev = 1;
            break;
        case 'e':
            nend = ipcom_atoi(ip_optarg);
            break;
        case 'B':
            context.b_flag = 1;
            break;
        case 't':
            trans = 1;
            break;
        case 'r':
            trans = 0;
            break;
#ifdef IP_SO_DEBUG
        case 'd':
            options |= IP_SO_DEBUG;
            break;
#endif
        case 'D':
#ifdef IP_TCP_NODELAY
            nodelay = 1;
#else
            ipcom_fprintf(ip_stderr, "ttcp: -D option ignored: IP_TCP_NODELAY socket option not supported"IP_LF);
#endif
            break;
        case 'n':
            nbuf = ipcom_atoi(ip_optarg);
            break;
        case 'l':
            buflen = ipcom_atoi(ip_optarg);
            break;
        case 's':
#ifdef IPCOM_TTCP_USE_STDIOMODE
            sinkmode = !sinkmode;
#endif
            break;
        case 'p':
            port = (unsigned short)ipcom_atoi(ip_optarg);
            break;
        case 'P':
            meport = (unsigned short)ipcom_atoi(ip_optarg);
            break;
#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
        case 'U':
            zerocopy = 1;
            /* fall through */
#endif
        case 'u':
            proto = IP_IPPROTO_UDP;
            sock_type = IP_SOCK_DGRAM;
            proto_as_str = "UDP";
            context.udp = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'A':
            bufalign = ipcom_atoi(ip_optarg);
            break;
        case 'O':
            bufoffset = ipcom_atoi(ip_optarg);
            break;
        case 'b':
#if defined(IP_SO_SNDBUF) || defined(IP_SO_RCVBUF)
            sockbufsize = ipcom_atoi(ip_optarg);
#else
            ipcom_fprintf(ip_stderr, "ttcp: -b option ignored: IP_SO_SNDBUF/IP_SO_RCVBUF socket options not supported"IP_LF);
#endif
            break;
#ifdef IP_SO_REUSEPORT
        case 'R':
            reuse_port = 1;
            break;
#endif
        case 'T':
            context.touchdata = 1;
            break;
#if defined(IP_SO_X_VR)
        case 'V':
            vr = ipcom_atoi(ip_optarg);
            break;
#endif
        case 'a':
            cpu = ipcom_atoi(ip_optarg);
            break;
#ifdef IPSCTP
        case 'c':
            proto = IP_IPPROTO_SCTP;
            sock_type = IP_SOCK_STREAM;
            proto_as_str = "SCTP";
            if (IP_TRUE == sctp_type_conflict)
            {
                ipcom_fprintf(ip_stderr, "ttcp: c/S conflict in command."IP_LF);
                goto errorout;
            }
            sctp_type_conflict = IP_TRUE;
            break;

        case 'S':
            proto = IP_IPPROTO_SCTP;
            sock_type = IP_SOCK_SEQPACKET;
            proto_as_str = "SCTP";
            if (IP_TRUE == sctp_type_conflict)
            {
                ipcom_fprintf(ip_stderr, "ttcp: c/S conflict in command."IP_LF);
                goto errorout;
            }
            sctp_type_conflict = IP_TRUE;
            break;
#endif
        default:
            goto usage;
        }
    }

#ifndef IP_PORT_LAS
    if (cpu >= 0)
    {
        Ip_cpu_set_t cpuset;

        if (cpu < 0 || cpu >= IP_CPU_SETSIZE)
        {
            ipcom_fprintf(ip_stderr, "ttcp: CPU must be between #0 and #%d"IP_LF, IP_CPU_SETSIZE-1);
            return -1;
        }
        IP_CPU_ZERO(&cpuset);
        IP_CPU_SET(cpu, &cpuset);
        if (ipcom_proc_cpu_affinity_set(0, &cpuset) != IPCOM_SUCCESS)
        {
            ipcom_fprintf(ip_stderr, "ttcp: Failed to set affinity to CPU #%d"IP_LF, cpu);
            return -1;
        }
    }
#endif /* IP_PORT_LAS */

    if(trans)
    {
        host = argv[ip_optind];

        if (host == 0)
            goto usage;
        sock_error = ipcom_getsockaddrbyaddrname(context.family, IP_FALSE, host, (struct Ip_sockaddr *)&addrhim);
        if(sock_error != 0)
        {
            ipcom_fprintf(ip_stderr, "ttcp error: ipcom_getsockaddrbyaddrname failed, errno = %d."IP_LF, sock_error);
            ttcperror(&context, "unknown host");
        }

        addrme.sin.sin_port  = ip_htons(meport);
        addrhim.sin.sin_port = ip_htons(port);   /* same port offset for IPv4 and IPv6 */
    }
    else
    {
        /* rcvr */
        addrme.sin.sin_port = ip_htons(port);
    }

#ifdef IPCOM_USE_INET6
    if(context.family == IP_AF_INET6)
    {
        context.addrsize = sizeof(struct Ip_sockaddr_in6);

        IPCOM_SA_LEN_SET(&addrme.sin6, sizeof(struct Ip_sockaddr_in6));
        addrme.sin6.sin6_family  = IP_AF_INET6;

        IPCOM_SA_LEN_SET(&addrhim.sin6, sizeof(struct Ip_sockaddr_in6));
        addrhim.sin6.sin6_family = IP_AF_INET6;
    }
#endif

    /* Send more than the sentinel size - UDPMINLEN. */
    if(context.udp && buflen <= UDPMINLEN)
        buflen = UDPMINLEN + 1;

    /* Align buffer. */
    if ((buf = (char *) ipcom_malloc(buflen + bufalign)) == (char *) IP_NULL)
        ttcperror(&context, "malloc");
    else
        orgbuf = buf;
    if (bufalign != 0)
        buf += (bufalign - ((Ip_ptrdiff_t) buf % bufalign) + bufoffset) % bufalign;

    /* Get a socket. */
    if ((sockfd = ipcom_socket(context.family, sock_type, proto)) == ERR)
        ttcperror(&context,"socket");

    if (trans)
    {
        if(sockbufsize < 0)
        {
            if(context.udp)
                sockbufsize = DEFAULT_UDP_SNDBUF;
            else
                sockbufsize = DEFAULT_TCP_SNDBUF;
        }
        ipcom_fprintf(ip_stdout, "ttcp-t: fd=%d, buflen=%d, nbuf=%d, align=%d/%d, port=%d",
                      sockfd, buflen, nbuf, bufalign, bufoffset, (int)port);
        if (sockbufsize)
            ipcom_fprintf(ip_stdout, ", sockbufsize=%d", sockbufsize);
        ipcom_fprintf(ip_stdout, "  %s  -> %s"IP_LF, proto_as_str, host);
    }
    else
    {
        if(sockbufsize < 0)
        {
            if(context.udp)
                sockbufsize = DEFAULT_UDP_RCVBUF;
            else
                sockbufsize = DEFAULT_TCP_RCVBUF;
        }
        ipcom_fprintf(ip_stdout, "ttcp-r: fd=%d, buflen=%d, nbuf=%d, align=%d/%d, port=%d",
                      sockfd, buflen, nbuf, bufalign, bufoffset, (int)port);
        if (sockbufsize)
            ipcom_fprintf(ip_stdout, ", sockbufsize=%d", sockbufsize);
        ipcom_fprintf(ip_stdout, "  %s"IP_LF, proto_as_str);
    }

    /* Code to open socket is moved to before the printf to get 'sockfd' right. */
    mes(trans, "socket");

    /* Set REUSE addr */
    if(ipcom_setsockopt(sockfd, IP_SOL_SOCKET, IP_SO_REUSEADDR, (char *)&enable, sizeof (int)) == ERR)
        ttcperror(&context, "setsockopt(IP_SO_REUSEADDR)");
    else
        mes(trans, "setsockopt(IP_SO_REUSEADDR)");

#ifdef IP_SO_REUSEPORT
    /* Set REUSE port */
    if(reuse_port)
    {
        if(ipcom_setsockopt(sockfd, IP_SOL_SOCKET, IP_SO_REUSEPORT, (char *)&reuse_port, sizeof (int)) == ERR)
            ttcperror(&context, "setsockopt(IP_SO_REUSEPORT)");
        else
            mes(trans, "setsockopt(IP_SO_REUSEPORT)");
    }
#endif

#ifdef IPCOM_USE_TCP
#ifdef TCP_OSERFC2385
    if (!udp && md5opt)
    {
        if (ipcom_setsockopt(sockfd, IPPROTO_TCP, TCP_OSERFC2385,
                             md5opt, strlen(md5opt)) == ERR)
        {
            ttcperror(&context, "setsockopt(IP_SO_REUSEADDR)");
        }
        else
        {
            mes(trans, "setsockopt(IP_SO_REUSEADDR)");
        }
    }
#endif
#endif

	/* Set routing table index in socket. */
#if defined(IP_SO_X_VR)
	if(vr != 0)
    {
	    if (ipcom_setsockopt(sockfd, IP_SOL_SOCKET, IP_SO_X_VR, (char *)&vr, 4) == ERR)
            ttcperror(&context, "setsockopt: vr");
	    else
            mes(trans, "setsockopt(vr)");
    }
#endif

    /* Bind the socket. */
    if(ipcom_bind(sockfd, (struct Ip_sockaddr *)&addrme, context.addrsize) == ERR)
        ttcperror(&context, "bind");
    else
#ifdef IPCOM_USE_INET6
        ipcom_fprintf(ip_stderr, "ttcp%s: bind %s %d"IP_LF, trans ? "-t" : "-r",
                      context.family == IP_AF_INET ? "IPv4" : "IPv6",
                      (int)ip_ntohs(addrme.sin.sin_port));
#else
    {
        char   tmpbuf[32];
        ipcom_sprintf(tmpbuf, "bind %d", ip_ntohs(addrme.sin.sin_port));
        mes(trans, tmpbuf);
    }
#endif

    /* Adjust socker buffer size. */
#if defined(IP_SO_SNDBUF) || defined(IP_SO_RCVBUF)
    if(sockbufsize)
    {
        if (trans)
        {
            if (ipcom_setsockopt(sockfd, IP_SOL_SOCKET, IP_SO_SNDBUF, (char *)&sockbufsize, sizeof sockbufsize) == ERR)
                ttcperror(&context,"setsockopt: sndbuf");
            else
                mes(trans, "setsockopt(sndbuf)");
        }
        else
        {
            if (ipcom_setsockopt(sockfd, IP_SOL_SOCKET, IP_SO_RCVBUF, (char *)&sockbufsize, sizeof sockbufsize) == ERR)
                ttcperror(&context,"setsockopt: rcvbuf");
            else
                mes(trans, "setsockopt(rcvbuf)");
        }
    }
#endif

    /* If TCP we need to connect else accept to remote side. */
#ifdef IPCOM_USE_TCP
    if (context.udp)
    {
        mes(trans, "opened");
    }
    else
    {
        /* We are the client if transmitting */
        if(trans)
        {
            if(options)
            {
                if(ipcom_setsockopt(sockfd, IP_SOL_SOCKET, options, (char *)&one, sizeof (one)) == ERR)
                    ttcperror(&context,"setsockopt");
            }
#ifdef IP_TCP_NODELAY
            if(nodelay)
            {
                if(ipcom_setsockopt(sockfd, IP_IPPROTO_TCP, IP_TCP_NODELAY, (char *)&one, sizeof (one)) == ERR)
                    ttcperror(&context,"setsockopt: nodelay");
                mes(trans, "nodelay");
            }
#endif

            if(ipcom_connect(sockfd, (struct Ip_sockaddr *)&addrhim, context.addrsize) == ERR)
                ttcperror(&context,"connect");
            else
                mes(trans, "connect");
        }

        /* Otherwise, we are the TCP server and should listen for the connections.  */
        else
        {
            if(ipcom_listen(sockfd, 0) == ERR)
                ttcperror(&context,"listen");
            else
                mes(trans, "listen");

            if(options)
            {
                if(ipcom_setsockopt(sockfd, IP_SOL_SOCKET, options, (char *)&one, sizeof (one)) == ERR)
                    ttcperror(&context,"setsockopt");
            }

            fromlen = context.addrsize;
            oldfd = sockfd;
#ifdef IPSCTP
            if ((IP_SOCK_SEQPACKET == sock_type)
                && (IP_IPPROTO_SCTP == proto))
            {
                #define TTCP_SCTP_RCV_BUF_LEN 128
                struct Ipsctp_event_subscribe events;
                union Ip_sockaddr_union  local_addr;
                struct Ipsctp_sndrcvinfo sinfo;
                Ip_u32                   len   = 0;
                char                     pbuf[TTCP_SCTP_RCV_BUF_LEN];
                int                      flags = 0;
                int                      ret   = 0;

                /* set data I/O event flag */
                ipcom_memset(&events, 0, sizeof(events));
                events.Ipsctp_data_io_event = 1;
                ret = ipcom_setsockopt(sockfd,
                                       IP_IPPROTO_SCTP,
                                       IPSCTP_EVENTS,
                                       (void *)&events,
                                       sizeof(events));
                if (ERR == ret)
                    ttcperror(&context, "setsockopt events");

                /* get the association identifier */
                ipcom_memset(pbuf, 0, TTCP_SCTP_RCV_BUF_LEN);
                len = sizeof(local_addr);
                /* Wait for connections */
                ret = ipsctp_recvmsg(sockfd,
                                     pbuf,
                                     TTCP_SCTP_RCV_BUF_LEN,
                                     &local_addr.sa,
                                     &len,
                                     &sinfo,
                                     &flags);
                sockfd = ipsctp_peeloff(sockfd, sinfo.sinfo_assoc_id);
                if (IP_SOCKERR == sockfd)
                    ttcperror(&context,"peeloff");
            }
            else
#endif
            {
                if((sockfd = ipcom_accept(sockfd, (struct Ip_sockaddr *)&frominet, &fromlen)) == ERR)
                    ttcperror(&context,"accept");
            }

            {
                union Ip_sockaddr_union peer;
                Ip_socklen_t            peerlen = context.addrsize;

                /* Close the mother socket. */
                if(ipcom_socketclose(oldfd) == ERR)
                    ttcperror(&context,"close of oldfd.");

#if defined(IP_SO_RCVBUF)
                /* Had to add this code in addition to the above because some stacks don't inherit
                 * the receive buffer size from the server socket. /Lennart Bang Enea Ose Systems 980116.
                 */
                if(sockbufsize)
                {
                    if(ipcom_setsockopt(sockfd, IP_SOL_SOCKET, IP_SO_RCVBUF, (char *)&sockbufsize, sizeof sockbufsize) == ERR)
                        ttcperror(&context,"setsockopt: rcvbuf");
                    else
                        mes(trans, "setsockopt(rcvbuf)");
                }
#endif

                if(ipcom_getpeername(sockfd, (struct Ip_sockaddr *) &peer, &peerlen) == ERR)
                    ttcperror(&context, "getpeername");

#ifdef IPCOM_USE_INET
                if(context.family == IP_AF_INET)
                {
                    char addr[16];
                    ipcom_fprintf(ip_stderr, "ttcp-r: accept from %s"IP_LF,
                                  ipcom_inet_ntop(IP_AF_INET, &peer.sin.sin_addr, addr, sizeof(addr)));
                }
#endif
#ifdef IPCOM_USE_INET6
                if(context.family == IP_AF_INET6)
                {
                    char addr[40];
                    ipcom_fprintf(ip_stderr, "ttcp-r: accept from %s"IP_LF,
                                  ipcom_inet_ntop(IP_AF_INET6, &peer.sin6.sin6_addr, addr, sizeof(addr)));
                }
#endif
            }
        }
    }
#endif

    /* Begin the TTCP performance test. */
    msec0 = get_millisec();

    if (mev)
    {
#if defined (IP_PORT_INTEGRITY)
#include "ipcom_integrity.h"
        extern void EventLogOn(void);
#if IP_VER < 500
        extern void EventLogMask(unsigned int newmask);
        EventLogMask(~0);
#endif
        EventLogOn();
#endif
    }

    if(sinkmode)
    {
        register int cnt;

        /* Transmit side. */
        if(trans)
        {
            pattern(buf, buflen);
            if(context.udp)
            {
                (void) NWRITE(sockfd, buf, UDPMINLEN, &context);	/* rcvr start */
                ipcom_millisleep(500);                  /* arp time */
                /* Renew the start time to not include the 500 msec sleep */
                msec0 = get_millisec();
            }
#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
            if (zerocopy)
            {
                while (nbuf-- && ZERONWRITE(sockfd, buflen, &context) == buflen)
                    nbytes += buflen;
            }
            else
#endif
            {
                while (nbuf-- && NWRITE(sockfd, buf, buflen, &context) == buflen)
                    nbytes += buflen;
            }
            if(context.udp)
                (void) NWRITE(sockfd, buf, UDPMINLEN, &context);	/* rcvr end */
        }

        /* Receive side. */
        else
        {
            if(context.udp)
            {
                int going = 0;

#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
                if (zerocopy)
                {
                    while ((cnt = ZeroNread(sockfd, &context)) > 0)
                    {
                        if(cnt <= UDPMINLEN)
                        {
                            if(going)
                                break;	/* "EOF" */
                            going = 1;
                            msec0 = get_millisec();
                        }
                        else
                        {
                            nbytes += cnt;
                        }
                    }
                }
                else
#endif
                {
                    while ((cnt = Nread(sockfd, buf, buflen, &context)) > 0)
                    {
                        if(cnt <= UDPMINLEN)
                        {
                            if(going)
                                break;	/* "EOF" */
                            going = 1;
                            msec0 = get_millisec();
                        }
                        else
                        {
                            nbytes += cnt;
                        }
                    }
                }
            }
#ifdef IPCOM_USE_TCP
            else
            {
                while ((cnt = Nread(sockfd, buf, buflen, &context)) > 0)
                {
                    nbytes += cnt;
                }
            }
#endif
        }
    }

#ifdef IPCOM_TTCP_USE_STDIOMODE
    /* non sinkmode. */
    else
    {
        register int cnt;

        if(trans)
        {
            /* Read from standard input and send to other side. */
            while ((cnt = ipcom_fread(buf, buflen, 1, ip_stdin)) > 0 &&
                   NWRITE(sockfd, buf, cnt) == cnt)
                nbytes += cnt;
        }
        else
        {
            /* Read from network and print on stdout. */
            while ((cnt = Nread(sockfd, buf, buflen, &context)) > 0)
            {
                ipcom_fwrite(buf, cnt, 1, ip_stdout);
                nbytes += cnt;
            }
        }
    }
#endif

    if (mev)
    {
#if defined (IP_PORT_INTEGRITY)
        extern void EventLogOff(void);
        EventLogOff();
#elif defined (IP_PORT_RTCORE)
        extern void ip_trace(char *what, char *file, int line, int id, void *data);
        extern void ip_tracedump(void);
        ip_trace("[ASSERT]", __FILE__, __LINE__, ipcom_getpid(), IP_NULL);
        ip_tracedump();
#endif
    }

    /* Read the final time and calculate some statistics. */
    msec1 = get_millisec();
    msecd = msec1 - msec0;
    if (msecd == 0)
        msecd = 1;
    bpermsec = nbytes / msecd;   /* Bytes per millisecond. */
    kbpersec = nbytes / 128;
    kbpersec = kbpersec * 125 / msecd; /* kilobyte per sec. */

    /* End of test assemble statistice. */
    if(context.err_no)
    {
        ipcom_fprintf(ip_stdout, "ttcp%s: socket errno: %d."IP_LF, trans ? "-t" : "-r", context.err_no);
    }

    if(context.udp && trans)
    {
        int j;

        for (j = 0; j < nend; j++)
        {
            ipcom_millisleep(100); /* Let the reading side catch up. */
            (void) NWRITE(sockfd, buf, UDPMINLEN, &context);	/* rcvr end */
        }
    }

    /* Print the statistics. */
    ipcom_fprintf(ip_stdout, "ttcp%s: %lu bytes in %lu milliseconds = %lu KB/sec, %lu B/msec +++"IP_LF,
                  trans ? "-t" : "-r", nbytes, msecd, kbpersec, bpermsec);

    ipcom_fprintf(ip_stdout, "ttcp%s: %lu I/O calls, msec/call = %lu, calls/sec = %lu"IP_LF,
                  trans ? "-t" : "-r",
                  context.numCalls,
                  msecd / context.numCalls,
                  1000 * context.numCalls / msecd);

    if (verbose)
        ipcom_fprintf(ip_stdout, "ttcp%s: buffer address %p"IP_LF, trans ? "-t" : "-r", buf);


    /* Test succeeded, goto errorout to cleanup. */
    goto errorout;

    /* Usage. */
 usage:
    ipcom_fprintf(ip_stderr,
                  "Usage: ttcp -t [-options] host [ < in ]"IP_LF
                  "ttcp -r [-options > out]"IP_LF
                  "Common options:"IP_LF
                  "-x      use IPv6 instead of IPv4"IP_LF
                  "-l ##	length of bufs read from or written to network (default 8192)"IP_LF
#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
                  "-U	use UDP and interpeak zero copy API instead of TCP"IP_LF
#endif
                  "-u	use UDP (default is TCP)"IP_LF
#ifdef IPSCTP
                  "-c	use SCTP STREAM type(default is TCP)"IP_LF
                  "-S	use SCTP SEQPACKET type(default is TCP)"IP_LF
#endif
                  "-p ##	port number to send to or listen at (default DEFAULT_PORT)"IP_LF);
    ipcom_fprintf(ip_stderr,
                  "-s	-t: source a pattern to network"IP_LF
                  "-r	sink (discard) all data from network"IP_LF
                  "-e ##	number of packets to end UDP transmission"IP_LF
                  "-m	use event logging (not supported on all platforms)"IP_LF
                  "-A	align the start of buffers to this modulus (default 16384)"IP_LF
                  "-O	start buffers at this offset from the modulus (default 0)"IP_LF
                  "-v	verbose: print more statistics"IP_LF
                  "-d	set SO_DEBUG socket option (if supported)"IP_LF);
    ipcom_fprintf(ip_stderr,
                  "-b ##	set socket buffer size (if supported)"IP_LF
                  "Options specific to -t:"IP_LF
                  "-n##	number of source bufs written to network (default 2048)"IP_LF
                  "-D	don't buffer TCP writes (sets TCP_NODELAY socket option)"IP_LF
                  "Options specific to -r:"IP_LF
                  "-B	for -s, only output full blocks as specified by -l (for TAR)"IP_LF
                  "-T	\"touch\": access each byte as it's read"IP_LF
#if defined(IP_SO_X_VR)
                  "-V      virtual router index (if supported)"IP_LF);
#endif
    /* fall through to cleanup...*/

    /* Free buffer and close socket if any error. */
 errorout:
#ifndef IP_PORT_LAS
    if (cpu >= 0)
        ipcom_proc_cpu_affinity_clr(0);
#endif /* IP_PORT_LAS */
    if(orgbuf)
        ipcom_free(orgbuf);
    if(sockfd != -1)
    {
        if(ipcom_socketclose(sockfd) == ERR)
            ipcom_fprintf(ip_stderr, "ttcp error: close of sockfd. (line %d, errno %d)."IP_LF, __LINE__, ipcom_errno);
    }

#ifdef IP_PORT_INTEGRITY
    ipcom_shutdown_libsocket();
#endif

    return 0;
}


/*
 *===========================================================================
 *                         mes
 *===========================================================================
 */
static void
mes(int trans, char *s)
{
    ipcom_fprintf(ip_stdout, "ttcp%s: %s"IP_LF, trans ? "-t" : "-r", s);
}


/*
 *===========================================================================
 *                         pattern
 *===========================================================================
 */
static void
pattern(char *cp, int cnt)
{
    register char c;

    c = 0;
    while (cnt-- > 0)
    {
        while (!ipcom_isprint((c & 0x7F)))
            c++;
        *cp++ = (c++ & 0x7F);
    }
}


/*
 *===========================================================================
 *                         Nread
 *===========================================================================
 */
static int
Nread(int fd, void *ibuf, int count, struct Ipcom_cmd_ttcp_data *context)
{
    union Ip_sockaddr_union from;
    Ip_socklen_t            len = context->addrsize;
    register int         cnt;

    if(context->udp)
    {
        if ((cnt = ipcom_recvfrom(fd, ibuf, count, 0, (struct Ip_sockaddr *)&from, &len)) < 0)
            context->err_no = ipcom_errno;
        context->numCalls++;
    }
    else
#ifdef IPCOM_USE_TCP
    {
        if(context->b_flag)
            cnt = mread(fd, ibuf, count, context);		/* fill buf */
        else
        {
            if ((cnt = ipcom_socketread(fd, ibuf, count)) < 0)
                context->err_no = ipcom_errno;
            context->numCalls++;
        }
        if(context->touchdata && cnt > 0)
        {
            register int c = cnt;
            volatile int sum = 0;
            register char *b = ibuf;

            while (c--)
                sum += *b++;

            (void)sum;				  /* lint */
        }
    }
#else
    {
        (void)b_flag;
        (void)touchdata;
        cnt = 0;
    }
#endif /* IPCOM_USE_TCP */

    return (cnt);
}


/*
 *===========================================================================
 *                         Nwrite
 *===========================================================================
 */
#ifdef IPCOM_TTCP_REENTRANT
static int
Nwrite(int fd, void *obuf, int count, union Ip_sockaddr_union *addrhimp, struct Ipcom_cmd_ttcp_data *context)
#else
static int
Nwrite(int fd, void *obuf, int count, struct Ipcom_cmd_ttcp_data *context)
#endif
{
    register int cnt;

    ipcom_errno = 0;
    if(context->udp)
    {
    again:

#ifdef IPCOM_TTCP_REENTRANT
        cnt = ipcom_sendto(fd, obuf, count, 0, (struct Ip_sockaddr *)addrhimp, context->addrsize);
#else
        cnt = ipcom_sendto(fd, obuf, count, 0, (struct Ip_sockaddr *)&addrhim, context->addrsize);
#endif
        context->numCalls++;
        if(cnt == ERR)
        {
            terror(context, "Nwrite");
            /* The vxWorks native stack returns ENOBUFS. Could be because the driver
             * schedules netTask to free buffers once they are sent. Forcing a context
             * switch and retrying seems to help */
            if(ipcom_errno == IP_ERRNO_ENOBUFS)
            {
                (void)ipcom_proc_yield();
                goto again;
            }
            return -1;
        }
    }
    else
#ifdef IPCOM_USE_TCP
    {
        if ((cnt = ipcom_socketwrite(fd, obuf, count)) < 0)
            context->err_no = ipcom_errno;
        context->numCalls++;
    }
#else
        cnt = 0;
#endif /* IPCOM_USE_TCP */

    return (cnt);
}


/*
 *===========================================================================
 *                         mread
 *===========================================================================
 * This function performs the function of a read(II) but will
 * call read(II) multiple times in order to get the requested
 * number of characters.  This can be necessary because
 * network connections don't deliver data with the same
 * grouping as it is written with.  Written by Robert S. Miles, BRL.
 */
#ifdef IPCOM_USE_TCP
static int
mread(int fd, char *bufp, unsigned n, struct Ipcom_cmd_ttcp_data *context)
{
    register unsigned count = 0;
    register int nread;

    do
    {
        nread = ipcom_socketread(fd, bufp, n - count);
        context->numCalls++;
        if(nread == ERR)
        {
            terror(context, "ttcp_mread");
            return (-1);
        }
        if(nread == 0)
            return ((int) count);
        count += (unsigned) nread;
        bufp += nread;
    }
    while (count < n);

    return ((int) count);
}
#endif


#ifdef IPCOM_CMD_TTCP_USE_ZEROCOPY_API
/*
 *===========================================================================
 *                         ZeroNread
 *===========================================================================
 */
static int
ZeroNread(int fd, struct Ipcom_cmd_ttcp_data *context)
{
    /* Only sink mode */
    union Ip_sockaddr_union from;
    Ip_socklen_t            len = context->addrsize;
    register int            cnt;
    void                   *buf;
    int                     start;
    Ipcom_pkt_freefunc      freefunc;
    void                   *cookie;

    cnt = ipcom_pkt_recvfrom(fd, &buf, &start, &freefunc, &cookie, 0, &from.sa, &len);
    if (cnt < 0)
        context->err_no = ipcom_errno;
    context->numCalls++;
    freefunc(buf, cookie);

    return (cnt);
}



/*
 *===========================================================================
 *                         ZeroNwriteFreeFunc
 *===========================================================================
 */
static void ZeroNwriteFreeFunc(void *aligned_buf, void *buf)
{
    (void)aligned_buf;
    ipcom_pkt_buffer_free(buf);
}

/*
 *===========================================================================
 *                         Nwrite
 *===========================================================================
 */
#ifdef IPCOM_TTCP_REENTRANT
static int
ZeroNwrite(int fd, int count, union Ip_sockaddr_union *addrhimp, struct Ipcom_cmd_ttcp_data *context)
#else
static int
ZeroNwrite(int fd, int count, struct Ipcom_cmd_ttcp_data *context)
#endif
{
    register int cnt;
    int start;
    void *buf;
    void *aligned_buf;

    start = ipcom_pkt_hdrspace(fd);
    buf = ipcom_pkt_buffer_malloc(start + count + 15);
    aligned_buf = (void *) (((unsigned long) buf + 15u) & ~0xf);

#ifdef IPCOM_TTCP_REENTRANT
    cnt = ipcom_pkt_sendto(fd, aligned_buf, start, count, ZeroNwriteFreeFunc, buf, 0, (struct Ip_sockaddr *)addrhimp, context->addrsize);
#else
    cnt = ipcom_pkt_sendto(fd, aligned_buf, start, count, ZeroNwriteFreeFunc, buf, 0, (struct Ip_sockaddr *)&addrhim, context->addrsize);
#endif
    context->numCalls++;
    if (cnt == ERR)
    {
        terror(context, "Nwrite");
        return -1;
    }

    return (cnt);
}
#endif /* IPCOM_CMD_TTCP_USE_ZEROCOPY_API */


#if defined(IPCOM_CMD_SOCKTEST_USE_BSD_API)
#include "ipcom_ipcom2bsd_finish.h"
#endif


/*
 *===========================================================================
 *                         END OF FILE
 *===========================================================================
 */

