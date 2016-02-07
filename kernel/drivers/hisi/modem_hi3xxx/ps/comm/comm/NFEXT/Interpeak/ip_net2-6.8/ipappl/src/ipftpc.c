/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $ipftpc.c$ $RCSfile: ipftpc.c,v $ $Revision: 1.11 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipftpc.c,v $
 *   $Author: jyang $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
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

#include "ipftpc_api_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_os.h>
#include <ipcom_clib.h>
#include <ipcom_time.h>

#define IPFTPC_API_C
#include "ipftpc_api.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Telnet */
#define IPFTPC_SE       240
#define IPFTPC_NOP      241
#define IPFTPC_DM       242
#define IPFTPC_BRK      243
#define IPFTPC_IP       244
#define IPFTPC_AO       245
#define IPFTPC_AYT      246
#define IPFTPC_EC       247
#define IPFTPC_EL       248
#define IPFTPC_GA       249
#define IPFTPC_SB       250
#define IPFTPC_WILL     251
#define IPFTPC_WONT     252
#define IPFTPC_DO       253
#define IPFTPC_DONT     254
#define IPFTPC_IAC      255

/* Sockwait return codes */
#define IPFTPC_SOCKWAIT_ERROR   -1
#define IPFTPC_SOCKWAIT_TIMEOUT 0
#define IPFTPC_SOCKWAIT_READ    1
#define IPFTPC_SOCKWAIT_WRITE   2
#define IPFTPC_SOCKWAIT_ASYNC   3

/* Line terminator */
#define CL "\r\n"

/* Source code line string */
#define __LINESTR(line) #line
#define __LINESTR_(line) __LINESTR(line)
#define __LINESTR__ __LINESTR_(__LINE__)

/* Syslog facility code */
#define IPCOM_SYSLOG_FACILITY IPCOM_LOG_IPFTP

/* Create log messages */
#undef IPCOM_SYSLOG_BODY
#ifdef IP_DEBUG
#define IPCOM_SYSLOG_BODY(x) "ipftpc_%d at " __FILE__ "(" __LINESTR__ "): " x, \
                             (session == IP_NULL ? -1 : session->id)
#else
#define IPCOM_SYSLOG_BODY(x) "ipftpc_%d: " x, \
                             (session == IP_NULL ? -1 : session->id)
#endif

/* Max length of IP address string */
#define IPFTPC_IPV4_MAXLEN \
        sizeof("255.255.255.255")
#define IPFTPC_IPV6_MAXLEN \
        sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* IPv4/IPv6 macros */
#ifdef IPCOM_USE_INET6

#define IPFTPC_IP_MAXLEN IPFTPC_IPV6_MAXLEN
#define IPFTPC_PORT(s) *((s)->sa.sa_family == IP_AF_INET ? \
                       &(s)->sin.sin_port : &(s)->sin6.sin6_port)
#define IPFTPC_SOCKADDRLEN(s) ((s)->sa.sa_family == IP_AF_INET ? \
                              sizeof((s)->sin) : sizeof((s)->sin6))
#define IPFTPC_ADDRCMP(a1,a2) ((a1)->sa.sa_family != (a2)->sa.sa_family || \
                              ((a1)->sa.sa_family == IP_AF_INET ? \
                              ipcom_memcmp(&(a1)->sin.sin_addr, \
                                  &(a2)->sin.sin_addr, \
                                  sizeof((a1)->sin.sin_addr)) : \
                              ipcom_memcmp(&(a1)->sin6.sin6_addr, \
                                  &(a2)->sin6.sin6_addr, \
                                  sizeof((a1)->sin6.sin6_addr))) != 0)
#else

#define IPFTPC_IP_MAXLEN IPFTPC_IPV4_MAXLEN
#define IPFTPC_PORT(s) ((s)->sin.sin_port)
#define IPFTPC_SOCKADDRLEN(s) (sizeof((s)->sin))
#define IPFTPC_ADDRCMP(a1,a2) ((a1)->sa.sa_family != (a2)->sa.sa_family || \
                              ipcom_memcmp(&(a1)->sin.sin_addr, \
                                  &(a2)->sin.sin_addr, \
                                  sizeof((a1)->sin.sin_addr)) != 0)
#endif

/* Liberal free */
#define IPFTPC_FREE(x) \
    if ((x) != IP_NULL) do {ipcom_free(x); (x) = IP_NULL; (void)(x);} while (0)

/* Get session pointer from ID */
#define IPFTPC_ID2SESSION(ses,id,act) \
    do {if ((id) > IPFTPC_MAX_SESSIONS || \
            ((ses) = ipftpc_api.session[id]) == IP_NULL) act;} while (0)

/*!! to work with ipcom410. */
#ifndef IP_ERRNO_EPERM
#define IP_ERRNO_EPERM 1
#endif

/* IPLITE/IPNET does not support write select */
#if !defined(IPCOM_SOCK_IPCOM)
#define IPFTPC_USE_WRITESELECT
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Socket control block */
typedef struct
{
    Ipftpc_addr paddr;                  /* Peer address */
    Ipftpc_addr laddr;                  /* Local address */
    Ip_fd fd;                           /* File descriptor */
}
Ipftpc_sockcb;

/* Session descriptor */
typedef struct Ipftpc_session
{
    Ipftpc_attr attr;                   /* Options */
    Ipftpc_sockcb csock;                /* Control socket */
    Ipftpc_sockcb dsock;                /* Data socket */
    Ipftpc_id id;                       /* Session ID */
    Ip_u32 eno;                         /* Error number */
    Ip_bool extended;                   /* Use extended addressing */
    Ip_bool epsvall;                    /* Only extended passive addressing */
    Ip_bool binary;                     /* Binary transfer */
    Ip_bool quit;                       /* QUIT in progress */
    Ip_bool flagoldstate;               /* Old state of flag in progress */
    Ip_bool *flaginprogress;            /* Pointer to flag in progress */
    int sinlen;                         /* Length of current command */
    int sinbuflen;                      /* Length of all commands */
    char sinbuf[IPFTPC_SINBUFSIZE];     /* Input buffer */
}
Ipftpc_session;

/* Public data */
typedef struct
{
    Ipftpc_session *session[IPFTPC_MAX_SESSIONS + 1]; /* Session descriptors */
    Ipcom_mutex     mutex;              /* Session descriptor mutex */
}
Ipftpc_api_data;


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

IP_PUBLIC Ip_err ipftpc_start(void);
IP_PUBLIC const char *ipftpc_version(void);

#include "ipftpc_api_proto.h"


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* Global data */
IP_STATIC Ipftpc_api_data ipftpc_api;


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftpc_ntop
 *===========================================================================
 *
 * Description:
 *      Print IP address using ntop.
 *
 * Parameters:
 *      addr    IP address.
 *      dest    Result buffer.
 *      len     Size of result buffer.
 *
 * Returns:
 *      Pointer to buffer if success, 0 otherwise.
 */
IP_STATIC const char *
ipftpc_ntop(Ipftpc_addr *addr, char *dest, int len)
{
    switch (addr->sa.sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipcom_inet_ntop(addr->sa.sa_family,
                               &addr->sin.sin_addr, dest, len);
#endif

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipcom_inet_ntop(addr->sa.sa_family,
                               &addr->sin6.sin6_addr, dest, len);
#endif

    default:
        ipcom_strcpy(dest, "?");
        break;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_flush
 *===========================================================================
 *
 * Description:
 *      Flush control socket input queue.
 *
 * Parameters:
 *      session Session descriptor.
 *      fdrd    Read file descriptor.
 *      fdwr    Write file descriptor.
 *      timeout Timeout in seconds.
 *
 * Returns:
 *      -1 if select error.
 *      0 if timeout.
 *      1 if read descriptor ready.
 *      2 if write descriptor ready.
 *      4 if control channel is ready for read,
 *        and no read descriptor is supplied.
 */
IP_STATIC int
ipftpc_flush(Ipftpc_session *session)
{
    /*
     ** Flush possible input from old commands.
     ** Abort if client error (timeout, socket close, etc.).
     */
    for (session->eno = IPFTPC_ESUCCESS;;)
    {
        int eno = ipftpc_sinpoll(session);

        if (eno == (IPFTPC_CFTPC + IPFTPC_ETIMEOUT))
        {
            /* Queue is empty */
            break;
        }
        else if (IPFTPC_GET_C(eno) == IPFTPC_CFTPS)
        {
            /* Server output, abort if x21 (disconnect) */
            if (eno % 100 == 21)
            {
                /* Connection closed */
                return 1;
            }
        }
        else
        {
            /* An error, abort */
            return 1;
        }

        /* Prepare for next lap */
        session->eno = IPFTPC_ESUCCESS;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_sockwait
 *===========================================================================
 *
 * Description:
 *      Wait until socket ready for I/O.
 *
 * Parameters:
 *      session Session descriptor.
 *      fdrd    Read file descriptor.
 *      fdwr    Write file descriptor.
 *      timeout Timeout in seconds.
 *
 * Returns:
 *      -1 if select error.
 *      0 if timeout.
 *      1 if read descriptor ready.
 *      2 if write descriptor ready.
 *      4 if control channel is ready for read,
 *        and no read descriptor is supplied.
 */
IP_STATIC int
ipftpc_sockwait(Ipftpc_session *session, Ip_fd fdrd, Ip_fd fdwr, int timeout)
{
    struct Ip_timeval timeval;
    Ip_fd_set setrd;
    Ip_fd_set setwr;
    Ip_fd_set *setwrp = IP_NULL;
    int num, max = IP_MAX(fdrd, fdwr);

    max = IP_MAX(max, session->csock.fd);

    timeval.tv_sec = timeout;
    timeval.tv_usec = 0;

    IP_FD_ZERO(&setrd);
    IP_FD_SET(session->csock.fd, &setrd);
    if (fdrd != -1)
    {
        IP_FD_SET(fdrd, &setrd);
    }

    IP_FD_ZERO(&setwr);
    if (fdwr != -1)
    {
#ifdef IPFTPC_USE_WRITESELECT
        IP_FD_SET(fdwr, &setwr);
        setwrp = &setwr;
#else
        timeval.tv_sec = 0;
#endif
    }

    num = ipcom_socketselect(max + 1, &setrd, setwrp, IP_NULL, &timeval);
    if (num < 0)
    {
        /* Error */
        return IPFTPC_SOCKWAIT_ERROR;
    }
    else if (num > 0)
    {
        if (fdrd != -1 && IP_FD_ISSET(fdrd, &setrd))
        {
            /* Ready for read */
            return IPFTPC_SOCKWAIT_READ;
        }
        else if (fdwr != -1 && IP_FD_ISSET(fdwr, &setwr))
        {
            /* Ready for write */
            return IPFTPC_SOCKWAIT_WRITE;
        }
        else if (IP_FD_ISSET(session->csock.fd, &setrd))
        {
            /* Control socket ready for read */
            return IPFTPC_SOCKWAIT_ASYNC;
        }
    }

    return IPFTPC_SOCKWAIT_TIMEOUT;
}


/*
 *===========================================================================
 *      ipftpc_dsock_active
 *===========================================================================
 *
 * Description:
 *      Prepare socket for active connection (by server).
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_active(Ipftpc_session *session)
{
    Ipftpc_sockcb *dsock = &session->dsock;

    session->eno = IPFTPC_ESUCCESS;

    /* Use extended addressing? */
    if (session->extended != 0)
    {
        char buf[IPFTPC_IP_MAXLEN];
        Ip_u16 port = IPFTPC_PORT(&dsock->laddr);

        (void)ipftpc_ntop(&session->dsock.laddr, buf, sizeof(buf));

        (void)ipftpc_sout(session, 1, "EPRT |%d|%s|%u|",
                          (dsock->paddr.sa.sa_family == IP_AF_INET ? 1 : 2),
                          buf, ip_ntohs(port));

        if (ipftpc_sin(session) / 100 != 2 &&
            session->csock.paddr.sa.sa_family == IP_AF_INET &&
            session->epsvall == 0 && session->attr.fb_extended)
        {
            /*
            ** Extended addressing not supported, disable.
            ** IPv4 connections will get a second chance
            ** when using traditional PORT below.
            */
            session->attr.extended = session->extended = 0;
        }
    }

    /* Use traditional active mode */
    if (session->extended == 0)
    {
        Ip_u32 addr = ip_ntohl(dsock->laddr.sin.sin_addr.s_addr);
        Ip_u16 port = ip_ntohs(dsock->laddr.sin.sin_port);

        session->eno = IPFTPC_ESUCCESS;

        (void)ipftpc_sout(session, 1, "PORT %d,%d,%d,%d,%d,%d",
                          (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
                          (addr >> 8) & 0xFF, addr & 0xFF,
                          (port >> 8) & 0xFF, port & 0xFF);

        (void)ipftpc_sin(session);
    }

    /* Adjust return code */
    if (session->eno / 100 == 2)
    {
        session->eno = IPFTPC_ESUCCESS;
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_dsock_passive
 *===========================================================================
 *
 * Description:
 *      Prepare socket for passive connection (by client).
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_passive(Ipftpc_session *session)
{
    Ipftpc_sockcb *dsock = &session->dsock;
    Ip_u32 port;
    int i;

    session->eno = IPFTPC_ESUCCESS;

    /* Use extended addressing? */
    if (session->extended != 0)
    {
        (void)ipftpc_sout(session, 1, "EPSV");
        if (ipftpc_sin(session) == 229)
        {
            char *cp = ipcom_strchr(session->sinbuf, '(');
            if (cp++ != IP_NULL)
            {
                char delim[2] = "|";
                char *stop;

                /* Port number is third element */
                for (i = 0, delim[0] = *cp; i < 3; i++)
                {
                    cp += ipcom_strcspn(cp, delim);
                    cp++;
                }

                port = ipcom_strtol(cp, &stop, 10);

                if (*stop != delim[0] || port > 65535 ||
                    port <= session->attr.pportbase)
                {
                    session->eno = IPFTPC_CFTPC + IPFTPC_EPORT;
                    IPCOM_LOG1(WARNING, "Invalid port number: %lu",
                               (Ip_u32)port);
                    goto leave;
                }

                IPFTPC_PORT(&dsock->paddr) = (Ip_u16)ip_htons(port);
            }
        }
        else if (session->csock.paddr.sa.sa_family == IP_AF_INET &&
                 session->epsvall == 0 && session->attr.fb_extended)
        {
            /*
            ** Extended addressing not supported, disable.
            ** IPv4 connections will get a second chance
            ** when using traditional passive mode below.
            */
            session->attr.extended = session->extended = 0;
        }
    }

    /* Use traditional passive mode */
    if (session->extended == 0)
    {
        session->eno = IPFTPC_ESUCCESS;

        (void)ipftpc_sout(session, 1, "PASV");
        if (ipftpc_sin(session) == 227)
        {
            char *cp = ipcom_strchr(session->sinbuf, '(');
            if (cp != IP_NULL)
            {
                char *stop;
                Ip_u32 d[6];
                Ip_u32 addr;

                /* Parse all six elements */
                for (i = 0; i < 6; i++)
                {
                    cp += ipcom_strcspn(cp, "(,");
                    cp += ipcom_strspn(cp, "(,");
                    d[i] = (Ip_u16)ipcom_strtol(cp, &stop, 10);

                    if (*stop != (i < 5 ? ',' : ')') || d[i] > 255)
                    {
                        session->eno = IPFTPC_CFTPC + IPFTPC_EPORT;
                        IPCOM_LOG1(WARNING,
                                   "Invalid IP address or port: %u",
                                   (Ip_u32)d[i]);
                        goto leave;
                    }
                }

                /* Assemble address and port */
                addr = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3];
                port = (d[4] << 8) | d[5];

                /* Check that addresses match */
                if (session->attr.ipcheck &&
                    (session->csock.paddr.sa.sa_family != IP_AF_INET ||
                     addr != ip_ntohl(session->csock.paddr.sin.sin_addr.s_addr)))
                {
                    char buf[IPFTPC_IP_MAXLEN];
                    Ip_u32 u = ip_htonl(addr);

                    (void)ipcom_inet_ntop(IP_AF_INET, &u, buf, sizeof(buf));
                    session->eno = IPFTPC_CFTPC + IPFTPC_EPORT;
                    IPCOM_LOG1(WARNING, "IP address %s denied", buf);
                    goto leave;
                }

                /* Check port */
                if (port > 65535 || port <= session->attr.pportbase)
                {
                    session->eno = IPFTPC_CFTPC + IPFTPC_EPORT;
                    IPCOM_LOG1(WARNING, "Invalid port number: %lu",
                              (Ip_u32)port);
                    goto leave;
                }

                /* Save address */
                IPCOM_SA_LEN_SET(&dsock->paddr.sin, sizeof(struct Ip_sockaddr_in));
                dsock->paddr.sin.sin_family = IP_AF_INET;
                dsock->paddr.sin.sin_addr.s_addr = ip_htonl(addr);
                dsock->paddr.sin.sin_port = (Ip_u16)ip_htons(port);
            }
        }
    }

leave:
    /* Adjust return code */
    if (session->eno / 100 == 2)
    {
        session->eno = IPFTPC_ESUCCESS;
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_dsock_vopen
 *===========================================================================
 *
 * Description:
 *      Open data socket and send FTP command to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      fmt     Format string for FTP command.
 *      argptr  Additional parms for FTP command.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_vopen(Ipftpc_session *session, const char *fmt, va_list argptr)
{
    Ipftpc_sockcb *dsock = &session->dsock;
    int attempts = IPFTPC_BIND_ATTEMPTS;
    int server_async = 0;
    int port = 0;
    Ip_fd fd;

    session->eno = IPFTPC_ESUCCESS;

    /* Start with control port as template */
    ipcom_memcpy(&dsock->paddr, &session->csock.paddr, sizeof(dsock->paddr));
    session->extended = session->attr.extended ||
                        dsock->paddr.sa.sa_family != IP_AF_INET;

    /* Get peer info for passive mode */
    if (session->attr.passive &&
        ipftpc_dsock_passive(session) != IPFTPC_ESUCCESS)
    {
        /* Fallback to active mode? */
        if (session->attr.fb_passive)
        {
            session->attr.passive = 0;
        }
        else
        {
            goto leave2;
        }
    }

    /* Close possibly open socket */
    if (dsock->fd != -1)
    {
        if (ipcom_socketclose(dsock->fd) != 0)
        {
            session->eno = IPFTPC_CSCLOSE + ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot close socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            goto leave2;
        }

        dsock->fd = -1;
    }

    /* Create new one */
    if ((dsock->fd = ipcom_socket(dsock->paddr.sa.sa_family,
                                  IP_SOCK_STREAM, IPFTPC_PROTO)) < 0)
    {
        session->eno = IPFTPC_CSOCKET + ipcom_errno;
        IPCOM_LOG1(ERR, "Cannot allocate data socket: %s",
                   ipcom_strerror(IPFTPC_GET_E(session->eno)));
        goto leave2;
    }

    /* Set local address to bind to */
    ipcom_memcpy(&dsock->laddr, &session->csock.laddr, sizeof(dsock->laddr));

again:
    if (session->attr.lportbase != 0)
    {
        /* Select random port above lportbase */
        port = ((unsigned long)ipcom_random() %
                (65535U - session->attr.lportbase)) +
               session->attr.lportbase + 1;
    }

    IPFTPC_PORT(&dsock->laddr) = (Ip_u16)ip_htons(port);

    if (ipcom_bind(dsock->fd, &dsock->laddr.sa,
                   IPFTPC_SOCKADDRLEN(&dsock->laddr)) < 0)
    {
        if (--attempts > 0)
        {
            goto again;
        }

        session->eno = IPFTPC_CBIND + ipcom_errno;
        IPCOM_LOG1(ERR, "Cannot bind data socket: %s",
                   ipcom_strerror(IPFTPC_GET_E(session->eno)));
        goto leave1;
    }

    if (session->attr.lportbase == 0)
    {
        Ip_socklen_t len = sizeof(dsock->laddr);

        /* Get the port number our OS gave us */
        if (ipcom_getsockname(dsock->fd, &dsock->laddr.sa, &len) != 0)
        {
            session->eno = IPFTPC_CBIND + ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot read address of data socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            goto leave1;
        }
    }

#ifdef IPIPTOS_THROUGHPUT
    {
        int option = IPIPTOS_THROUGHPUT;

        /* Setup for max throughput service */
        if (ipcom_setsockopt(dsock->fd, IP_IPPROTO_IP, IPIP_TOS,
                             &option, sizeof(option)) < 0)
        {
            IPCOM_LOG1(WARNING, "Cannot setup for max throughput service: %s",
                      ipcom_strerror(ipcom_errno));
        }
    }
#endif

    /* Active or passive open */
    if (session->attr.passive)
    {
        /* Connect to peer */
        if (ipcom_connect(dsock->fd, &dsock->paddr.sa,
                          IPFTPC_SOCKADDRLEN(&dsock->paddr)) < 0)
        {
            session->eno = IPFTPC_CCONNECT + ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot connect to data socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            goto leave1;
        }

        /* Send command */
        (void)ipftpc_vsout(session, 1, fmt, argptr);
    }
    else
    {
        Ip_socklen_t len = sizeof(dsock->paddr);

        /* Inform peer of our end */
        if (ipftpc_dsock_active(session) != IPFTPC_ESUCCESS)
        {
            goto leave1;
        }

        /* Allow peer to connect */
        if (ipcom_listen(dsock->fd, 1) != 0)
        {
            session->eno = IPFTPC_CLISTEN + ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot listen on data socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            goto leave1;
        }

        /* Send command */
        (void)ipftpc_vsout(session, 1, fmt, argptr);

wconnect:
        /* Wait for peer to connect */
        switch (ipftpc_sockwait(session, session->dsock.fd, -1,
                                IPFTPC_RECEIVE_TIMEOUT))
        {
        case IPFTPC_SOCKWAIT_ERROR:
            session->eno = IPFTPC_CSELECT + ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot select on data socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            goto leave1;
        case IPFTPC_SOCKWAIT_TIMEOUT:
            session->eno = IPFTPC_CFTPC + IPFTPC_ETIMEOUT;
            IPCOM_LOG0(WARNING, "No connect from server in active mode");
            goto leave1;
        case IPFTPC_SOCKWAIT_ASYNC:
            session->eno = IPFTPC_ESUCCESS;
            if ((server_async = ipftpc_sinpoll(session)) == 150)
            {
                /* The server sent us the 150 first, continue to wait */
                session->eno = IPFTPC_ESUCCESS;
                goto wconnect;
            }
            else if (server_async == (IPFTPC_CFTPC + IPFTPC_ETIMEOUT))
            {
                /* We got only a partial message, continue to wait */
                session->eno = IPFTPC_ESUCCESS;
                server_async = 0;
                goto wconnect;
            }
            goto leave1;
        }

        /* Accept connection */
        if ((fd = ipcom_accept(dsock->fd, &dsock->paddr.sa, &len)) < 0)
        {
            session->eno = IPFTPC_CACCEPT + ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot accept on data socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            goto leave1;
        }

        /* Close original socket and replace with new */
        if (ipcom_socketclose(dsock->fd) != 0)
        {
            session->eno = IPFTPC_CSCLOSE + ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot close socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            goto leave1;
        }
        dsock->fd = fd;

        /* Check if allowed IP address */
        if (session->attr.ipcheck &&
            IPFTPC_ADDRCMP(&dsock->paddr, &session->csock.paddr) != 0)
        {
            char buf[IPFTPC_IP_MAXLEN];

            (void)ipftpc_ntop(&dsock->paddr, buf, sizeof(buf));
            session->eno = IPFTPC_CFTPC + IPFTPC_EINVHOST;
            IPCOM_LOG1(WARNING, "Peer IP address %s denied", buf);
            goto leave1;
        }
    }

leave1:
    if (session->eno != IPFTPC_ESUCCESS)
    {
        (void)ipftpc_sock_close(session, dsock);
    }

leave2:
    if (session->eno == IPFTPC_ESUCCESS && server_async != 0)
    {
        session->eno = server_async;
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_dsock_open
 *===========================================================================
 *
 * Description:
 *      Open data socket and send FTP command to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      fmt     Format string for FTP command.
 *      ...     Additional parms for FTP command.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_open(Ipftpc_session *session, const char *fmt, ...)
{
    va_list argptr;
    int ret;

    va_start(argptr, fmt);
    ret = ipftpc_dsock_vopen(session, fmt, argptr);
    va_end(argptr);

    return ret;
}


/*
 *===========================================================================
 *      ipftpc_sock_close
 *===========================================================================
 *
 * Description:
 *      Close socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      sock    Socket descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sock_close(Ipftpc_session *session, Ipftpc_sockcb *sock)
{
    int eno = IPFTPC_ESUCCESS;

    (void)session;

    if (sock->fd != IP_INVALID_SOCKET && ipcom_socketclose(sock->fd) != 0)
    {
        eno = IPFTPC_CSCLOSE + ipcom_errno;
        IPCOM_LOG1(ERR, "Cannot close socket: %s",
                   ipcom_strerror(IPFTPC_GET_E(eno)));
    }

    sock->fd = IP_INVALID_SOCKET;

    return eno;
}


/*
 *===========================================================================
 *      ipftpc_telnet
 *===========================================================================
 *
 * Description:
 *      Handle telnet stuff.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftpc_telnet(Ipftpc_session *session)
{
    Ip_u8 *cp = (Ip_u8 *)(session->sinbuf);
    int i = 0;

    /* Loop through entire command buffer */
    while (i < session->sinbuflen)
    {
        int left = session->sinbuflen - i;

        do
        {
            int skip = 0;
            int reply = 0;
            int j;

            /* Check for telnet */
            if (cp[0] == IPFTPC_IAC && left >= 2)
            {
                switch (cp[1])
                {
                case IPFTPC_WILL:
                    if (left >= 3)
                    {
                        skip = 3;
                        reply = 1;
                        cp[1] = IPFTPC_DONT;
                    }
                    break;
                case IPFTPC_DO:
                    if (left >= 3)
                    {
                        skip = 3;
                        reply = 1;
                        cp[1] = IPFTPC_WONT;
                    }
                    break;
                case IPFTPC_WONT:
                case IPFTPC_DONT:
                    if (left >= 3)
                    {
                        skip = 3;
                    }
                    break;
                default:
                    skip = 2;
                    break;
                }

                /* Reply if negotiation detected */
                if (reply)
                {
                    (void)ipftpc_swrite(session, cp, skip);
                }
            }

            /* Remove telnet stuff from command buffer */
            if (skip > 0)
            {
                for (j = 0; j < (left - skip); j++)
                {
                    cp[j] = cp[j + skip];
                }

                session->sinbuflen -= skip;

                /* Continue without advancing indices */
                break;
            }

            i++;
            cp++;
        }
        while (0);
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_sread
 *===========================================================================
 *
 * Description:
 *      Get input line from control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      timeout Timeout in seconds.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sread(Ipftpc_session *session, int timeout)
{
    int len;
    int i;

    /* Ignore if previous error */
    if (session->eno != IPFTPC_ESUCCESS)
    {
        return session->eno;
    }

    /* Put possible last input first in buffer */
    for (i = 0; i < (session->sinbuflen - session->sinlen); i++)
    {
        session->sinbuf[i] = session->sinbuf[session->sinlen + i];
    }
    session->sinbuflen -= session->sinlen;
    session->sinlen = 0;

    /* Loop until input line available */
    for (;;)
    {
        int ret;

        /* Handle telnet stuff */
        (void)ipftpc_telnet(session);

        /* Check if there already is a pending input line */
        for (i = 0; i < session->sinbuflen; i++)
        {
            if (session->sinbuf[i] == '\r')
            {
                if (i < (session->sinbuflen - 1) &&
                    session->sinbuf[i + 1] == '\n')
                {
                    session->sinbuf[i] = '\0';
                    session->sinlen = i + 2;
                    if ((session->attr.verbose & 1) && session->attr.ctrlin)
                    {
                        session->attr.ctrlin(&session->attr, session->sinbuf, i);
                    }

                    session->eno = IPFTPC_ESUCCESS;
                    goto leave;
                }
            }
        }

        /* Wait until socket ready */
        if ((ret = ipftpc_sockwait(session, session->csock.fd, -1, timeout)) ==
            IPFTPC_SOCKWAIT_ERROR)
        {
            session->eno = IPFTPC_CSELECT + ipcom_errno;
            IPCOM_LOG1(WARNING, "Cannot select on control socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            break;
        }
        else if (ret == IPFTPC_SOCKWAIT_TIMEOUT)
        {
            session->eno = IPFTPC_CFTPC + IPFTPC_ETIMEOUT;
            if (timeout != 0)
            {
                IPCOM_LOG0(WARNING, "Read timeout");
            }
            break;
        }

        /* Get more data */
        if ((len = ipcom_socketread(session->csock.fd,
                                    session->sinbuf + session->sinbuflen,
                                    IPFTPC_SINBUFSIZE - session->sinbuflen)) <= 0)
        {
            session->eno = IPFTPC_CREAD +
                           (len == 0 ? IP_ERRNO_ECONNRESET : ipcom_errno);

            IPCOM_LOG1(INFO, "Cannot read from control socket: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
            break;
        }

        session->sinbuflen += len;
    }

leave:
    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_sin
 *===========================================================================
 *
 * Description:
 *      Get terminating input line from control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sin(Ipftpc_session *session)
{
    while (ipftpc_sread(session, IPFTPC_RECEIVE_TIMEOUT) == IPFTPC_ESUCCESS)
    {
        /* A valid terminating line has three digits and a space */
        if (ipcom_strspn(session->sinbuf, "0123456789") == 3 &&
            session->sinbuf[3] == ' ')
        {
            return session->eno = ipcom_strtol(session->sinbuf, IP_NULL, 10);
        }
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_sinpoll
 *===========================================================================
 *
 * Description:
 *      Poll for terminating input line from control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sinpoll(Ipftpc_session *session)
{
    while (ipftpc_sread(session, 0) == IPFTPC_ESUCCESS)
    {
        /* A valid terminating line has three digits and a space */
        if (ipcom_strspn(session->sinbuf, "0123456789") == 3 &&
            session->sinbuf[3] == ' ')
        {
            return session->eno = ipcom_strtol(session->sinbuf, IP_NULL, 10);
        }
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_swrite
 *===========================================================================
 *
 * Description:
 *      Write to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      buf     Buffer.
 *      len     Length of buffer.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_swrite(Ipftpc_session *session, void *buf, int len)
{
    int eno = IPFTPC_ESUCCESS;

    /* Ignore if previous error */
    if (session->eno != IPFTPC_ESUCCESS)
    {
        return session->eno;
    }

    /* Write to socket */
    if (ipcom_socketwrite(session->csock.fd, buf, len) != len)
    {
        eno = IPFTPC_CWRITE + ipcom_errno;
        IPCOM_LOG1(WARNING, "Socket write error: %s",
                   ipcom_strerror(ipcom_errno));
    }

    return eno;
}


/*
 *===========================================================================
 *      ipftpc_vsout
 *===========================================================================
 *
 * Description:
 *      Write formatted string to control socket,
 *      interpret FTP command and set local flags accordingly.
 *
 * Parameters:
 *      session Session descriptor.
 *      cl      Append CR,LF to output.
 *      fmt     Format string.
 *      argptr  Additional arguments.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_vsout(Ipftpc_session *session, int cl, const char *fmt, va_list argptr)
{
    char buf[256];
    int cllen = (cl ? 2 : 0);
    int len = ipcom_vsnprintf(buf, sizeof(buf) - cllen, fmt, argptr);

    if (len < 0 || len >= (int)(sizeof(buf) - cllen))
    {
        IPCOM_LOG0(WARNING, "Output buffer overflow");
        len = sizeof(buf) - cllen;
    }

    if ((session->attr.verbose & 2) && session->attr.ctrlout)
    {
        session->attr.ctrlout(&session->attr, buf, len);
    }

    if (cl)
    {
        ipcom_strcat(buf, CL);
        len += cllen;
    }

    /* Interpret commands and set flags accordingly */
    if (ipcom_strncasecmp(buf, "QUIT", 4) == 0)
    {
        session->flagoldstate = session->quit;
        session->flaginprogress = &session->quit;
        session->quit = 1;
    }
    else if (ipcom_strncasecmp(buf, "TYPE", 4) == 0)
    {
        session->flagoldstate = session->binary;
        session->flaginprogress = &session->binary;
        session->binary = (ipcom_strncasecmp(buf + 5, "A", 1) != 0);
    }
    else if (ipcom_strncasecmp(buf, "EPSV ALL", 8) == 0)
    {
        session->flagoldstate = session->epsvall;
        session->flaginprogress = &session->epsvall;
        session->epsvall = 1;
    }

    return ipftpc_swrite(session, buf, len);
}


/*
 *===========================================================================
 *      ipftpc_sout
 *===========================================================================
 *
 * Description:
 *      Write formatted string to control socket,
 *
 * Parameters:
 *      session Session descriptor.
 *      cl      Append CR,LF to output.
 *      fmt     Format string.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sout(Ipftpc_session *session, int cl, const char *fmt, ...)
{
    va_list argptr;
    int ret;

    va_start(argptr, fmt);
    ret = ipftpc_vsout(session, cl, fmt, argptr);
    va_end(argptr);

    return ret;
}


/*
 *===========================================================================
 *      ipftpc_cout
 *===========================================================================
 *
 * Description:
 *      Send buffer to console.
 *
 * Parameters:
 *      session Session descriptor.
 *      buf     Buffer.
 *      bufsize Buffer size.
 *      orphant True if last buffer was terminated with '\r'.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_cout(Ipftpc_session *session, char *buf, int bufsize, Ip_bool *orphant)
{
    /* Don´t bother if no handler function */
    if (session->attr.conout == IP_NULL)
    {
        return 0;
    }

#ifdef IPFTPC_USE_VERBATIM_CONOUT
    /* No conversion */
    buf[bufsize] = '\0';
    return session->attr.conout(&session->attr, buf, bufsize);
#else

    {
        int begin;
        int end;
        int no;

        /* Output '\r' if not followed by '\n' */
        if (*orphant && buf[0] != '\n')
        {
            /* Output missing '\r' */
            if (session->attr.conout(&session->attr, "\r", 1) != 0)
            {
                return 1;
            }
        }

        *orphant = 0;

        /* Loop for entire buffer */
        for (begin = end = 0; end < bufsize; end++)
        {
            if (buf[end] == '\r')
            {
                if (end == (bufsize - 1))
                {
                    /* '\r' at end of buffer, remember */
                    *orphant = 1;
                }
                else if (buf[end + 1] != '\n')
                {
                    /* Not followed by '\n' - ignore */
                    continue;
                }

                /* Write up to '\r' */
                no = end - begin;
                buf[end] = '\0';
                if (session->attr.conout(&session->attr, &buf[begin], no) != 0)
                {
                    return 1;
                }

                /* Continue at '\n' */
                begin = end + 1;
            }
        }

        /* Write last part */
        no = end - begin;
        buf[end] = '\0';
        if (session->attr.conout(&session->attr, &buf[begin], no) != 0)
        {
            return 1;
        }

        return 0;
    }
#endif
}


/*
 *===========================================================================
 *      ipftpc_filewrite_ascii
 *===========================================================================
 *
 * Description:
 *      Write ASCII buffer to file.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *      orphant True if last buffer was terminated with '\r'.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftpc_filewrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize,
                       Ip_bool *orphant)
{
    char *cp = nbuf;
    int begin;
    int end;
    int no;

    /* Enter '\r' if not followed by '\n' */
    if (*orphant && buf[0] != '\n')
    {
        /* Output missing '\r' */
        *cp++ = '\r';
    }

    *orphant = 0;

    /* Loop for entire buffer */
    for (begin = end = 0; end < bufsize; end++)
    {
        if (buf[end] == '\r')
        {
            if (end == (bufsize - 1))
            {
                /* '\r' at end of buffer, remember */
                *orphant = 1;
            }
            else if (buf[end + 1] != '\n')
            {
                /* Not followed by '\n' - ignore */
                continue;
            }

            /* Enter everything up to '\r' */
            no = end - begin;
            ipcom_memcpy(cp, &buf[begin], no);
            cp += no;

            /* Continue at '\n' */
            begin = end + 1;
        }
    }

    /* Enter last part */
    no = end - begin;
    ipcom_memcpy(cp, &buf[begin], no);
    cp += no;

    /* Write to file */
    no = cp - nbuf;
    ip_assert(no <= IPFTPC_DATABUFSIZE+1);
    if (ipcom_filewrite(fd, nbuf, no) != no)
    {
        return 1;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_socketwrite_ascii
 *===========================================================================
 *
 * Description:
 *      Write ASCII buffer to socket.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftpc_socketwrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize)
{
    char *cp = nbuf;
    int begin;
    int end;
    int no;

    /* Loop for entire buffer */
    for (begin = end = 0; end < bufsize; end++)
    {
        if (buf[end] == '\n')
        {
            /* Enter everything up to '\n' */
            no = end - begin;
            ipcom_memcpy(cp, &buf[begin], no);
            cp += no;

            /* Enter newline */
            *cp++ = '\r';
            *cp++ = '\n';

            /* Continue after '\n' */
            begin = end + 1;
        }
    }

    /* Enter last part */
    no = end - begin;
    ipcom_memcpy(cp, &buf[begin], no);
    cp += no;

    /* Write to socket */
    no = cp - nbuf;
    if (ipcom_socketwrite(fd, nbuf, no) != no)
    {
        return 1;
    }

    return 0;
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftpc_exit
 *===========================================================================
 *
 * Description:
 *      Deinitialize FTP service and free resources. This
 *      requires that all FTP sessions are closed.
 *
 * Parameters:
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_exit(void)
{
    int i;

    ipcom_mutex_lock(ipftpc_api.mutex);

    /* Check for active sessions */
    for (i = 1;
         i <= IPFTPC_MAX_SESSIONS && ipftpc_api.session[i] == IP_NULL;
         i++)
    {
    }

    ipcom_mutex_unlock(ipftpc_api.mutex);

    if (i <= IPFTPC_MAX_SESSIONS)
    {
        /* Still active sessions */
        return IPFTPC_CFTPC + IPFTPC_EBUSY;
    }

    (void)ipcom_mutex_delete(&ipftpc_api.mutex);

    return IPFTPC_ESUCCESS;
}


/*
 *===========================================================================
 *      ipftpc_open
 *===========================================================================
 *
 * Description:
 *      Open FTP session.
 *
 * Parameters:
 *      idp     Pointer to where the session ID will be stored.
 *      addr    IP address of FTP server.
 *      attr    Session attributes.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_open(Ipftpc_id *idp, Ipftpc_addr *addr, Ipftpc_attr *attr)
{
    Ipftpc_session *session = IP_NULL;
    Ipftpc_sockcb *csock;
    int eno;
    int i;
    Ip_socklen_t addrlen;

    ipcom_mutex_lock(ipftpc_api.mutex);

    /* Get a free entry */
    for (i = 1; i <= IPFTPC_MAX_SESSIONS; i++)
    {
        if (ipftpc_api.session[i] == IP_NULL)
        {
            ipftpc_api.session[i] = session = ipcom_malloc(sizeof(*session));
            break;
        }
    }

    ipcom_mutex_unlock(ipftpc_api.mutex);

    if (session == IP_NULL)
    {
        if (i <= IPFTPC_MAX_SESSIONS)
        {
            /* Got ID, but failed to malloc */
            eno = IPFTPC_CFTPC + IPFTPC_ENOMEM;
            IPCOM_LOG0(ERR, "Out of memory");
        }
        else
        {
            /* No ID available */
            eno = IPFTPC_CFTPC + IPFTPC_ESESSIONMAX;
        }
        goto leave3;
    }

    /* Init session control block */
    ipcom_memset(session, 0, sizeof(*session));
    session->id = (Ip_u16)i;
    session->csock.paddr = *addr;
    session->attr = *attr;
    session->dsock.fd = -1;
    csock = &session->csock;

    /* Open control connection */
    if ((csock->fd = ipcom_socket(csock->paddr.sa.sa_family,
                                  IP_SOCK_STREAM, IPFTPC_PROTO)) < 0)
    {
        eno = IPFTPC_CSOCKET + ipcom_errno;
        IPCOM_LOG1(ERR, "Cannot allocate control socket: %s",
                   ipcom_strerror(IPFTPC_GET_E(eno)));
        goto leave2;
    }

#ifdef IPIPTOS_LOWDELAY
    {
        int option = IPIPTOS_LOWDELAY;

        /* Setup for low delay service */
        if (ipcom_setsockopt(csock->fd, IP_IPPROTO_IP, IPIP_TOS,
                             &option, sizeof(option)) < 0)
        {
            IPCOM_LOG1(WARNING, "Cannot setup for low delay service: %s",
                       ipcom_strerror(ipcom_errno));
        }
    }
#endif

    /* Connect to server */
    if (ipcom_connect(csock->fd, &csock->paddr.sa,
                      IPFTPC_SOCKADDRLEN(&csock->paddr)) < 0)
    {
        eno = IPFTPC_CCONNECT + ipcom_errno;
        IPCOM_LOG1(ERR, "Cannot connect to control socket: %s",
                   ipcom_strerror(IPFTPC_GET_E(eno)));
        goto leave1;
    }

    /* Get local address */
    addrlen = sizeof(csock->laddr);
    if (ipcom_getsockname(csock->fd, &csock->laddr.sa, &addrlen) != 0)
    {
        eno = IPFTPC_CCONNECT + ipcom_errno;
        IPCOM_LOG1(ERR, "Cannot get own IP address: %s",
                   ipcom_strerror(IPFTPC_GET_E(eno)));
        goto leave1;
    }

    session->eno = IPFTPC_ESUCCESS;

    /* Get the first greeting */
    for (;;)
    {
        switch (eno = ipftpc_sin(session) / 100)
        {
        case 1:
            /* More to come... */
            break;
        case 2:
            /* OK */
            goto leave0;
        default:
            /* Error */
            goto leave1;
        }
    }

leave0:
    *idp = session->id;
    eno = IPFTPC_ESUCCESS;
    goto leave3;

leave1:
    /* Close control socket */
    if (ipcom_socketclose(session->csock.fd) != 0)
    {
        IPCOM_LOG1(ERR, "Cannot close socket: %s",
                   ipcom_strerror(IPFTPC_GET_E(eno)));
    }

leave2:
    /* Unlink and free session descriptor */
    ipcom_mutex_lock(ipftpc_api.mutex);
    ipftpc_api.session[session->id] = IP_NULL;
    ipcom_mutex_unlock(ipftpc_api.mutex);

    IPFTPC_FREE(session);

leave3:
    return eno;
}


/*
 *===========================================================================
 *      ipftpc_close
 *===========================================================================
 *
 * Description:
 *      Close FTP session.
 *
 * Parameters:
 *      id      Session ID.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_close(Ipftpc_id id)
{
    Ipftpc_session *session;

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    (void)ipftpc_sock_close(session, &session->dsock);
    (void)ipftpc_sock_close(session, &session->csock);

    /* Unlink and free session descriptor */
    ipcom_mutex_lock(ipftpc_api.mutex);
    ipftpc_api.session[id] = IP_NULL;
    ipcom_mutex_unlock(ipftpc_api.mutex);

    IPFTPC_FREE(session);

    return IPFTPC_ESUCCESS;
}


/*
 *===========================================================================
 *      ipftpc_getattr
 *===========================================================================
 *
 * Description:
 *      Get FTP session attributes.
 *
 * Parameters:
 *      id      Session id.
 *      attr    Pointer to attributes.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_getattr(Ipftpc_id id, Ipftpc_attr *attr)
{
    Ipftpc_session *session;

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    session->sinbuf[0] = '\0';
    *attr = session->attr;

    return IPFTPC_ESUCCESS;
}


/*
 *===========================================================================
 *      ipftpc_setattr
 *===========================================================================
 *
 * Description:
 *      Set FTP session parameters.
 *
 * Parameters:
 *      id      Session id.
 *      attr    Pointer to attributes.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_setattr(Ipftpc_id id, Ipftpc_attr *attr)
{
    Ipftpc_session *session;

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    session->sinbuf[0] = '\0';
    session->attr = *attr;

    return IPFTPC_ESUCCESS;
}


/*
 *===========================================================================
 *      ipftpc_cmd
 *===========================================================================
 *
 * Description:
 *      Perform simple FTP command.
 *
 * Parameters:
 *      id      Session ID.
 *      fmt     Format string.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_cmd(Ipftpc_id id, const char *fmt, ...)
{
    Ipftpc_session *session;
    va_list argptr;

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    session->flaginprogress = IP_NULL;
    session->sinbuf[0] = '\0';

    /* Flush possible input from old commands */
    if (ipftpc_flush(session))
    {
        return session->eno;
    }

    session->eno = IPFTPC_ESUCCESS;

    /* Send command */
    va_start(argptr, fmt);
    (void)ipftpc_vsout(session, 1, fmt, argptr);
    va_end(argptr);

    /* Get reply */
    (void)ipftpc_sin(session);

    /* Restore flag value if unsuccessful command */
    if (session->eno / 100 != 2 && session->flaginprogress != IP_NULL)
    {
        *(session->flaginprogress) = session->flagoldstate;
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_login
 *===========================================================================
 *
 * Description:
 *      FTP server login.
 *
 * Parameters:
 *      id       Session ID.
 *      username User name.
 *      password Password.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_login(Ipftpc_id id, const char *username, const char *password)
{
    Ipftpc_session *session;

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    session->sinbuf[0] = '\0';

    /* Flush possible input from old commands */
    if (ipftpc_flush(session))
    {
        return session->eno;
    }

    session->eno = IPFTPC_ESUCCESS;

    (void)ipftpc_sout(session, 1, "USER %s", username);
    if (ipftpc_sin(session) == 331)
    {
        session->eno = IPFTPC_ESUCCESS;
        (void)ipftpc_sout(session, 1, "PASS %s", password);
        (void)ipftpc_sin(session);
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_list
 *===========================================================================
 *
 * Description:
 *      List files on remote server.
 *
 * Parameters:
 *      id      Session ID.
 *      fmt     Format string.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_list(Ipftpc_id id, const char *fmt, ...)
{
    Ipftpc_session *session;
    va_list argptr;
    char *buf;
    int server_async = 0;
    int orphant = 0;

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    session->sinbuf[0] = '\0';

    /* Flush possible input from old commands */
    if (ipftpc_flush(session))
    {
        return session->eno;
    }

    session->eno = IPFTPC_ESUCCESS;

    if ((buf = ipcom_malloc(IPFTPC_DATABUFSIZE+1)) == IP_NULL)
    {
        session->eno = IPFTPC_CFTPC + IPFTPC_ENOMEM;
    }
    else
    {
        /* Open data socket */
        va_start(argptr, fmt);
        (void)ipftpc_dsock_vopen(session, fmt, argptr);
        va_end(argptr);

        /* Await reply */
        if (session->eno / 100 == 1 ||
            (session->eno == IPFTPC_ESUCCESS &&
             ipftpc_sin(session) / 100 == 1))
        {
            session->eno = IPFTPC_ESUCCESS;

            for (;;)
            {
                int no;

                /* Wait for input */
                if ((no = ipftpc_sockwait(session, session->dsock.fd, -1,
                                          IPFTPC_RECEIVE_TIMEOUT)) == IPFTPC_SOCKWAIT_ERROR)
                {
                    session->eno = IPFTPC_CSELECT + ipcom_errno;
                    IPCOM_LOG1(ERR, "Socket select error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }
                else if (no == IPFTPC_SOCKWAIT_TIMEOUT)
                {
                    session->eno = IPFTPC_CFTPC + IPFTPC_ETIMEOUT;
                    break;
                }
                else if (no == IPFTPC_SOCKWAIT_ASYNC)
                {
                    /* Got stuff on control socket, read and abort */
                    session->eno = IPFTPC_ESUCCESS;
                    if ((server_async = ipftpc_sinpoll(session)) / 100 == 2)
                    {
                        /*
                        ** This should be the termination message
                        ** from the server that has arrived before
                        ** end of data. Continue transfer and use
                        ** this as our return code.
                        */
                        session->eno = IPFTPC_ESUCCESS;
                        continue;
                    }
                    else if (server_async == (IPFTPC_CFTPC + IPFTPC_ETIMEOUT))
                    {
                        /*
                        ** An incomplete async message is received,
                        ** keep going and wait for the terminator.
                        */
                        session->eno = IPFTPC_ESUCCESS;
                        server_async = 0;
                        continue;
                    }

                    /* Abort transfer if it wasn't the termination message */
                    break;
                }

                /* Read */
                if ((no = ipcom_socketread(session->dsock.fd, buf,
                                           IPFTPC_DATABUFSIZE)) == 0)
                {
                    session->eno = IPFTPC_ESUCCESS;
                    break;
                }
                else if (no < 0)
                {
                    session->eno = IPFTPC_CREAD + ipcom_errno;
                    IPCOM_LOG1(ERR, "Socket read error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }

                /* Write to console */
                if (ipftpc_cout(session, buf, no, &orphant) != 0)
                {
                    session->eno = IPFTPC_CFTPC + IPFTPC_ECONSOLE;
                    IPCOM_LOG0(ERR, "Console output error");
                    break;
                }
            }
        }

        (void)ipftpc_sock_close(session, &session->dsock);
    }

    IPFTPC_FREE(buf);

    if (server_async == 0)
    {
        /* Wait for server termination message */
        (void)ipftpc_sin(session);
    }
    else
    {
        /* Already got a termination message */
        session->eno = server_async;
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_get
 *===========================================================================
 *
 * Description:
 *      Get file from remote server.
 *
 * Parameters:
 *      id      Session ID.
 *      lpath   Local path.
 *      rpath   Remote path.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_get(Ipftpc_id id, const char *lpath, const char *rpath)
{
    Ipftpc_session  *session;
    char *nbuf = IP_NULL;
    char *buf;
    int  server_async = 0;
    int  orphant = 0;
    struct Ip_stat st;
    Ip_fd fd = -1;
    int mode;
    Ip_u32 bytes = 0;
#ifdef IPFTPC_DEBUG
    Ip_bool isreg = IP_TRUE;
#endif

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    /*
     * Remove file before opening it to simulate O_TRUNC,
     * which isn't supported by IPCOM.
     */
    (void)ipcom_unlink(lpath);

    /* Set file mode */
    mode = IP_O_RDWR | IP_O_CREAT
#ifdef IP_O_BINARY
               | (session->binary ? IP_O_BINARY : 0)
#endif
               ;

    session->sinbuf[0] = '\0';

    /* Flush possible input from old commands */
    if (ipftpc_flush(session))
    {
        return session->eno;
    }

    session->eno = IPFTPC_ESUCCESS;

    buf = ipcom_malloc(IPFTPC_DATABUFSIZE + 1);
    if (buf == IP_NULL)
    {
        session->eno = IPFTPC_CFTPC + IPFTPC_ENOMEM;
    }
    else if ((nbuf = ipcom_malloc(IPFTPC_DATABUFSIZE+1)) == IP_NULL)
    {
        session->eno = IPFTPC_CFTPC + IPFTPC_ENOMEM;
    }
    else if ((fd = ipcom_fileopen(lpath, mode, IP_S_IRUSR | IP_S_IWUSR |
                                  IP_S_IRGRP | IP_S_IROTH)) < 0)
    {
        session->eno = IPFTPC_COPEN + ipcom_errno;
    }
    else if (ipcom_fstat(fd, &st) != 0)
    {
        session->eno = IPFTPC_COPEN + ipcom_errno;
    }
    else
    {
        Ip_u32 chunks = 0;
#ifdef IPFTPC_DEBUG
        char            cbuf[64];
        Ip_bool         corphant    = IP_FALSE;   

        /* Regular file we're writing to? */
        isreg = IP_S_ISREG(st.st_mode);
#endif
        /* Open data socket */
        (void)ipftpc_dsock_open(session, "RETR %s", rpath);

        /* Await reply */
        if (session->eno / 100 == 1 ||
            (session->eno == IPFTPC_ESUCCESS &&
             ipftpc_sin(session) / 100 == 1))
        {
            session->eno = IPFTPC_ESUCCESS;

            for (;;)
            {
                int no;

                /* Wait for input */
                if ((no = ipftpc_sockwait(session, session->dsock.fd, -1,
                                          IPFTPC_RECEIVE_TIMEOUT)) == IPFTPC_SOCKWAIT_ERROR)
                {
                    session->eno = IPFTPC_CSELECT + ipcom_errno;
#ifdef IPFTPC_DEBUG
                    ipcom_snprintf(cbuf, 
                                   sizeof(cbuf), 
                                   "Socket select error: %s\r\n", 
                                   ipcom_strerror(IPFTPC_GET_E(session->eno)));

                    ipftpc_cout(session, cbuf, ipcom_strlen(cbuf), &corphant);
#endif
                    IPCOM_LOG1(ERR, "Socket select error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }
                else if (no == IPFTPC_SOCKWAIT_TIMEOUT)
                {
                    session->eno = IPFTPC_CFTPC + IPFTPC_ETIMEOUT;

#ifdef IPFTPC_DEBUG
                    ipcom_snprintf(cbuf, 
                                   sizeof(cbuf), 
                                   "Connection timed out: %s\r\n", 
                                   ipcom_strerror(IPFTPC_GET_E(session->eno)));

                    ipftpc_cout(session, cbuf, ipcom_strlen(cbuf), &corphant);
#endif
                    break;
                }
                else if (no == IPFTPC_SOCKWAIT_ASYNC)
                {
                    /* Got stuff on control socket */
                    session->eno = IPFTPC_ESUCCESS;
                    if ((server_async = ipftpc_sinpoll(session)) / 100 == 2)
                    {
                        /*
                        ** This should be the termination message
                        ** from the server that has arrived before
                        ** end of data. Continue transfer and use
                        ** this as our return code.
                        */
                        session->eno = IPFTPC_ESUCCESS;
                        continue;
                    }
                    else if (server_async == (IPFTPC_CFTPC + IPFTPC_ETIMEOUT))
                    {
                        /*
                        ** An incomplete async message is received,
                        ** keep going and wait for the terminator.
                        */
                        session->eno = IPFTPC_ESUCCESS;
                        server_async = 0;
                        continue;
                    }

#ifdef IPFTPC_DEBUG
                    ipcom_snprintf(cbuf, 
                                   sizeof(cbuf), 
                                   "File transfer aborted\r\n");

                    ipftpc_cout(session, cbuf, ipcom_strlen(cbuf), &corphant);
#endif
                    /* Abort transfer if it wasn't the termination message */
                    break;
                }

                /* Read */
                if ((no = ipcom_socketread(session->dsock.fd, buf,
                                           IPFTPC_DATABUFSIZE)) == 0)
                {
                    session->eno = IPFTPC_ESUCCESS;
                    break;
                }
                else if (no < 0)
                {
                    session->eno = IPFTPC_CREAD + ipcom_errno;

#ifdef IPFTPC_DEBUG
                    ipcom_snprintf(cbuf, 
                                   sizeof(cbuf), 
                                   "Socket read error: %s\r\n", 
                                   ipcom_strerror(IPFTPC_GET_E(session->eno)));

                    ipftpc_cout(session, cbuf, ipcom_strlen(cbuf), &corphant);
#endif

                    IPCOM_LOG1(ERR, "Socket read error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }

                /* Write according to mode */
                if (session->binary == 0)
                {
                    if (ipftpc_filewrite_ascii(fd, buf, nbuf, no,
                                               &orphant) != 0)
                    {
                        session->eno = IPFTPC_CFTPC + ipcom_errno;

#ifdef IPFTPC_DEBUG
                        ipcom_snprintf(cbuf, 
                                       sizeof(cbuf), 
                                       "File write error: %s\r\n", 
                                       ipcom_strerror(IPFTPC_GET_E(session->eno)));

                        ipftpc_cout(session, cbuf, ipcom_strlen(cbuf), &corphant);
#endif

                        IPCOM_LOG1(ERR, "File write error: %s",
                                   ipcom_strerror(IPFTPC_GET_E(session->eno))
                          );
                        break;
                    }
                }
                else if (ipcom_filewrite(fd, buf, no) != no)
                {
                    session->eno = IPFTPC_CFTPC + ipcom_errno;

#ifdef IPFTPC_DEBUG
                    ipcom_snprintf(cbuf, 
                                   sizeof(cbuf), 
                                   "File write error: %s\r\n", 
                                   ipcom_strerror(IPFTPC_GET_E(session->eno)));

                    ipftpc_cout(session, cbuf, ipcom_strlen(cbuf), &corphant);
#endif

                    IPCOM_LOG1(ERR, "File write error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }

#ifdef IPFTPC_DEBUG
                ipcom_snprintf(cbuf, sizeof(cbuf), "Received: %u\r", bytes);
                ipftpc_cout(session, cbuf, ipcom_strlen(cbuf), &corphant);
#endif
                bytes += no;
                chunks++;
            }
        }

        /* Write orphant '\r' */
        if (orphant && session->eno == IPFTPC_ESUCCESS &&
            ipcom_filewrite(fd, "\r", 1) != 1)
        {
            session->eno = IPFTPC_CFTPC + ipcom_errno;
            IPCOM_LOG1(ERR, "File write error: %s",
                       ipcom_strerror(IPFTPC_GET_E(session->eno)));
        }

        IPCOM_LOG2(DEBUG2, "Received %lu bytes in %lu chunks",
                   (Ip_u32)bytes, (Ip_u32)chunks);
        (void)chunks;
        (void)ipftpc_sock_close(session, &session->dsock);
    }

    if (fd >= 0)
    {
        (void)ipcom_fileclose(fd);
    }

    IPFTPC_FREE(nbuf);
    IPFTPC_FREE(buf);

    if (server_async == 0)
    {
        /* Wait for server termination message */
        (void)ipftpc_sin(session);
    }
    else
    {
        /* Already got a termination message */
        session->eno = server_async;
    }

#ifdef IPFTPC_USE_REMOVE_FILE_ON_ERROR
    /* Remove the file if error */
    if (session->eno / 100 != 2)
#else
    /* Remove empty file if error */
    if (bytes == 0 && session->eno / 100 != 2)
#endif
    {
#ifdef IPFTPC_DEBUG
        /* If it's a regular file, unlink it */
        if (isreg)
#endif
            (void)ipcom_unlink(lpath);
    }

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_put
 *===========================================================================
 *
 * Description:
 *      Put file to remote server.
 *
 * Parameters:
 *      id      Session ID.
 *      lpath   Local path.
 *      rpath   Remote path.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_put(Ipftpc_id id, const char *lpath, const char *rpath, int append)
{
    Ipftpc_session *session;
    char *nbuf = IP_NULL;
    char *buf;
    struct Ip_stat st;
    Ip_fd fd = -1;
    int mode;

    IPFTPC_ID2SESSION(session, id, return IPFTPC_CFTPC + IPFTPC_EID);

    /* Set file mode */
    mode = IP_O_RDONLY
#ifdef IP_O_BINARY
           | (session->binary ? IP_O_BINARY : 0)
#endif
           ;

    session->sinbuf[0] = '\0';

    /* Flush possible input from old commands */
    if (ipftpc_flush(session))
    {
        return session->eno;
    }

    session->eno = IPFTPC_ESUCCESS;

    buf = ipcom_malloc(IPFTPC_DATABUFSIZE + 1);
    if (buf == IP_NULL)
    {
        session->eno = IPFTPC_CFTPC + IPFTPC_ENOMEM;
    }
    else if ((nbuf = ipcom_malloc((2 * IPFTPC_DATABUFSIZE)+1)) == IP_NULL)
    {
        session->eno = IPFTPC_CFTPC + IPFTPC_ENOMEM;
    }
    else if ((fd = ipcom_fileopen(lpath, mode, 0)) < 0)
    {
        session->eno = IPFTPC_COPEN + ipcom_errno;
    }
    else if (ipcom_fstat(fd, &st) != 0)
    {
        session->eno = IPFTPC_COPEN + ipcom_errno;
    }
    else if (IP_S_ISREG(st.st_mode) == 0)
    {
        session->eno = IPFTPC_COPEN + IP_ERRNO_EPERM;
    }
    else
    {
        Ip_u32 chunks = 0;
        Ip_u32 bytes = 0;

        /* Open data socket */
        (void)ipftpc_dsock_open(session,
                                (append ? "APPE %s" : "STOR %s"), rpath);

        /* Await reply */
        if (session->eno / 100 == 1 ||
            (session->eno == IPFTPC_ESUCCESS &&
             ipftpc_sin(session) / 100 == 1))
        {
            session->eno = IPFTPC_ESUCCESS;

            for (;;)
            {
                int eno;
                int no;

                /* Get data */
                if ((no = ipcom_fileread(fd, buf, IPFTPC_DATABUFSIZE)) == 0)
                {
                    break;
                }
                else if (no < 0)
                {
                    session->eno = IPFTPC_CREAD + ipcom_errno;
                    IPCOM_LOG1(ERR, "File read error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }

                /* Wait until socket ready */
                if ((eno = ipftpc_sockwait(session, -1, session->dsock.fd,
                                           IPFTPC_SEND_TIMEOUT)) == IPFTPC_SOCKWAIT_ERROR)
                {
                    session->eno = IPFTPC_CSELECT + ipcom_errno;
                    IPCOM_LOG1(ERR, "Socket select error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }
#ifdef IPFTPC_USE_WRITESELECT
                else if (eno == IPFTPC_SOCKWAIT_TIMEOUT)
                {
                    session->eno = IPFTPC_CFTPC + IPFTPC_ETIMEOUT;
                    break;
                }
#endif
                else if (eno == IPFTPC_SOCKWAIT_ASYNC)
                {
                    /* Got stuff on control socket, check it out */
                    session->eno = IPFTPC_ESUCCESS;
                    if (ipftpc_sinpoll(session) == (IPFTPC_CFTPC + IPFTPC_ETIMEOUT))
                    {
                        /*
                        ** An incomplete async message is received,
                        ** keep going and wait for the terminator.
                        */
                        session->eno = IPFTPC_ESUCCESS;
                    }
                    else
                    {
                        /* A complete input message aborts the transfer */
                        break;
                    }
                }

                /* Write according to mode */
                if (session->binary == 0)
                {
                    if (ipftpc_socketwrite_ascii(session->dsock.fd,
                                                 buf, nbuf, no) != 0)
                    {
                        session->eno = IPFTPC_CFTPC + ipcom_errno;
                        IPCOM_LOG1(ERR, "Socket write error: %s",
                                   ipcom_strerror(IPFTPC_GET_E(session->eno)));
                        break;
                    }
                }
                else if (ipcom_socketwrite(session->dsock.fd, buf, no) != no)
                {
                    session->eno = IPFTPC_CFTPC + ipcom_errno;
                    IPCOM_LOG1(ERR, "Socket write error: %s",
                               ipcom_strerror(IPFTPC_GET_E(session->eno)));
                    break;
                }

                bytes += no;
                chunks++;
            }
        }

        IPCOM_LOG2(DEBUG2, "Sent %lu bytes in %lu chunks",
                   (Ip_u32)bytes, (Ip_u32)chunks);
        (void)bytes;
        (void)chunks;
        (void)ipftpc_sock_close(session, &session->dsock);
    }

    if (fd >= 0)
    {
        (void)ipcom_fileclose(fd);
    }

    IPFTPC_FREE(nbuf);
    IPFTPC_FREE(buf);

    (void)ipftpc_sin(session);

    return session->eno;
}


/*
 *===========================================================================
 *      ipftpc_lastreply
 *===========================================================================
 *
 * Description:
 *      Return last reply from server.
 *
 * Parameters:
 *      id      Session ID.
 *
 * Returns:
 *      Error class string.
 */
IP_PUBLIC char *
ipftpc_lastreply(Ipftpc_id id)
{
    Ipftpc_session *session;

    IPFTPC_ID2SESSION(session, id, return IP_NULL);

    return session->sinbuf;
}


/*
 *===========================================================================
 *      ipftpc_strclass
 *===========================================================================
 *
 * Description:
 *      Return error class.
 *
 * Parameters:
 *      err     Error number.
 *
 * Returns:
 *      Error class string.
 */
IP_PUBLIC char *
ipftpc_strclass(int err)
{
    if (err == IPFTPC_ESUCCESS)
    {
        return "General";
    }

    switch (IPFTPC_GET_C(err))
    {
    case IPFTPC_CFTPS:    return "FTP server";
    case IPFTPC_CFTPC:    return "FTP client";
    case IPFTPC_CACCEPT:  return "accept";
    case IPFTPC_CBIND:    return "bind";
    case IPFTPC_CCONNECT: return "connect";
    case IPFTPC_CLISTEN:  return "listen";
    case IPFTPC_CREAD:    return "read";
    case IPFTPC_CMUTEX:   return "mutex";
    case IPFTPC_COPEN:    return "open";
    case IPFTPC_CSCLOSE:  return "socket close";
    case IPFTPC_CSELECT:  return "select";
    case IPFTPC_CSOCKET:  return "socket";
    case IPFTPC_CWRITE:   return "write";
    default:              break;
    }

    return "<unknown>";
}


/*
 *===========================================================================
 *      ipftpc_strerror
 *===========================================================================
 *
 * Description:
 *      Return error description.
 *
 * Parameters:
 *      err     Error number.
 *
 * Returns:
 *      Error description string.
 */
IP_PUBLIC char *
ipftpc_strerror(int err)
{
    int c = IPFTPC_GET_C(err);
    int e = IPFTPC_GET_E(err);

    if (e == IPFTPC_ESUCCESS)
    {
        return "Success";
    }

    switch (c)
    {
    case IPFTPC_CFTPS:
        switch (e)
        {
        case 120: return "Service ready in nnn minutes";
        case 125: return "Data connection already open; transfer starting";
        case 150: return "File status okay; about to open data connection";
        case 200: return "Command okay";
        case 202: return "Command not implemented, superfluous at this site";
        case 211: return "System status, or system help reply";
        case 212: return "Directory status";
        case 213: return "File status";
        case 214: return "Help message";
        case 215: return "System type";
        case 220: return "Service ready for new user";
        case 221: return "Service closing control connection";
        case 225: return "Data connection open; no transfer in progress";
        case 226: return "Success, closing data connection";
        case 227: return "Entering passive mode (h1,h2,h3,h4,p1,p2)";
        case 229: return "Entering extended passive mode (|||port|)";
        case 230: return "User logged in, proceed";
        case 250: return "Requested file action okay, completed";
        case 257: return "File or directory created";
        case 331: return "User name okay, need password";
        case 332: return "Need account for login";
        case 350: return "Requested file action pending further information";
        case 421: return "Service not available, closing control connection";
        case 425: return "Can't open data connection";
        case 426: return "Connection closed; transfer aborted";
        case 450: return "Requested file action not taken, file unavailable";
        case 451: return "Requested action aborted: local error in processing";
        case 452: return "Requested action not taken, insufficient storage";
        case 500: return "Syntax error, command unrecognized";
        case 501: return "Syntax error in parameters or arguments";
        case 502: return "Command not implemented";
        case 503: return "Bad sequence of commands";
        case 504: return "Command not implemented for that parameter";
        case 522: return "Bad address family, use (1,2)";
        case 530: return "Not logged in";
        case 532: return "Need account for storing files";
        case 550: return "Requested action not taken, file unavailable";
        case 551: return "Requested action aborted, page type unknown";
        case 552: return "Requested file action aborted, exceeded storage allocation";
        case 553: return "Requested action not taken, file name not allowed";
        default:  break;
        }
        break;

    case IPFTPC_CFTPC:
        switch (e)
        {
        case IPFTPC_EBUSY:       return "Session is busy";
        case IPFTPC_ECONSOLE:    return "Console write error";
        case IPFTPC_EID:         return "Invalid ID";
        case IPFTPC_EINVHOST:    return "Invalid host";
        case IPFTPC_ENOMEM:      return "Out of memory";
        case IPFTPC_EPORT:       return "Invalid port";
        case IPFTPC_ESESSIONMAX: return "No more sessions allowed";
        case IPFTPC_ETIMEOUT:    return "Timeout";
        default: return ipcom_strerror(e);
        }

    default:
        return ipcom_strerror(e);
    }

    return "<unknown>";
}


/*
 *===========================================================================
 *                         ipftpc_create
 *===========================================================================
 */
IP_PUBLIC Ip_err
ipftpc_create(void)
{
    Ip_err  err;
    Ipftpc_session *session = IP_NULL;

    (void)session;

    ipcom_memset(&ipftpc_api, 0, sizeof(ipftpc_api));

    err = ipcom_mutex_create(&ipftpc_api.mutex);
    if (err != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Cannot create IPFTPC list mutex");
    }

    return err;
}


/*
 *===========================================================================
 *                    ipftpc_start
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipftpc_start(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipftpc_version
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char *
ipftpc_version(void)
{
    return "@(#) IPFTPC $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
