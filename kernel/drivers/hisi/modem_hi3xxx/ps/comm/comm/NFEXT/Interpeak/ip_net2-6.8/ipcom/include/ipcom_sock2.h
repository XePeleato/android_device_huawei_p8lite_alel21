/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sock2.h,v $ $Revision: 1.36 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_sock2.h,v $
*     $Author: kenneth $ $Date: 2009-06-04 19:50:50 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*     Design and implementation by Lennart Bang <lob@interpeak.se>
******************************************************************************
*/
#ifndef IPCOM_SOCK2_H
#define IPCOM_SOCK2_H


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Extended (try to avoid!) Interpeak BSD socket API.
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

#include "ipcom_type.h"
#include "ipcom_clibmap.h"
#include "ipcom_cstyle.h"
#include "ipcom_sock.h"
#include "ipcom_pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *            must define IP_LITTLE_ENDIAN or IP_BIG_ENDIAN
 *===========================================================================
 */

#ifndef _shroud
#if !defined(IP_LITTLE_ENDIAN) && !defined(IP_BIG_ENDIAN)
#error must_define_IP_LITTLE_ENDIAN_or_IP_BIG_ENDIAN
#endif
#endif

/*
 *===========================================================================
 *            flags for ipcom_getnameinfo()
 *===========================================================================
 */

#define	IP_NI_NOFQDN	    0x00000001
#define	IP_NI_NUMERICHOST	0x00000002
#define	IP_NI_NAMEREQD	    0x00000004
#define	IP_NI_NUMERICSERV	0x00000008
#define	IP_NI_DGRAM	        0x00000010

/*
 *===========================================================================
 *            flags for ipcom_getaddrinfo()
 *===========================================================================
 */

#define	IP_AI_PASSIVE	    0x00000001 /* get address to use bind() */
#define	IP_AI_CANONNAME	    0x00000002 /* fill ai_canonname */
#define	IP_AI_NUMERICHOST	0x00000004 /* prevent name resolution */
#define	IP_AI_NUMERICSERV   0x00000008 /* prevent service name resolution */
#define	IP_AI_ALL		    0x00000100 /* IPv6 and IPv4-mapped (with AI_V4MAPPED) */
#define	IP_AI_ADDRCONFIG	0x00000400 /* only if any address is assigned */
#define	IP_AI_V4MAPPED	    0x00000800 /* accept IPv4-mapped IPv6 address */


/*
 *===========================================================================
 *        error codes for ipcom_getnameinfo()/ipcom_getaddrinfo()
 *===========================================================================
 */

#define	IP_EAI_AGAIN	    2
#define	IP_EAI_BADFLAGS     3
#define	IP_EAI_FAIL	        4
#define	IP_EAI_FAMILY	    5
#define	IP_EAI_MEMORY	    6
#define	IP_EAI_NONAME	    8
#define	IP_EAI_SERVICE	    9
#define	IP_EAI_SOCKTYPE	    10
#define	IP_EAI_SYSTEM	    11


/*
 *===========================================================================
 *     ipcom_h_errno values for ipcom_gethostbyname()/ipcom_gethostbyaddr()
 *===========================================================================
 */
#define IP_HOST_NOT_FOUND       1
#define IP_TRY_AGAIN            2
#define IP_NO_RECOVERY          3
#define IP_NO_DATA              4
#define IP_NO_ADDRESS           IP_NO_DATA


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ip_protoent
 *===========================================================================
 */
struct Ip_protoent
{
    char    *p_name;       /* official name of protocol */
    char   **p_aliases;    /* alias list */
    int      p_proto;      /* protocol number */
};


/*
 *===========================================================================
 *                         Ip_servent
 *===========================================================================
 */
struct Ip_servent
{
    char    *s_name;        /* official name of service */
    char   **s_aliases;     /* alias list */
    int      s_port;        /* port service resides at */
    char    *s_proto;       /* protocol to use */
};


/*
 *===========================================================================
 *                         Ip_addrinfo
 *===========================================================================
 */
struct Ip_addrinfo
{
	int	ai_flags;	                /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
	int	ai_family;	                /* PF_xxx */
	int	ai_socktype;	            /* SOCK_xxx */
	int	ai_protocol;	            /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	Ip_size_t ai_addrlen;	        /* length of ai_addr */
	char	*ai_canonname;	        /* canonical name for hostname */
	struct	Ip_sockaddr *ai_addr;	/* binary address */
	struct	Ip_addrinfo *ai_next;	/* next structure in linked list */
};


/*
 *===========================================================================
 *                         Ip_ifaddrs
 *===========================================================================
 * port/src/ipcom_ifaddrs.c function structure.
 */
struct Ip_ifaddrs
{
    struct Ip_ifaddrs   *ifa_next;
    char                *ifa_name;
    unsigned int         ifa_flags;
    struct Ip_sockaddr  *ifa_addr;
    struct Ip_sockaddr  *ifa_netmask;
    union
    {
        struct Ip_sockaddr *ifu_broadaddr;
        struct Ip_sockaddr *ifu_dstaddr;
    } ifa_ifu;
# ifndef ifa_broadaddr
#  define ifa_broadaddr	ifa_ifu.ifu_broadaddr
# endif
# ifndef ifa_dstaddr
#  define ifa_dstaddr	ifa_ifu.ifu_dstaddr
# endif
    void *ifa_data;
};


/*
 *===========================================================================
 *                         Ip_sockaddr_mpls
 *===========================================================================
 * Used for IP->MPLS psuedo-wire routes
 */
#include "ipcom_align16.h"
IP_PACKED1 struct Ip_sockaddr_mpls
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8             smpls_len;      /* Length of this structure */
#endif
    Ip_sa_family_t    smpls_family;   /* Always IP_AF_MPLS */
    Ip_u32            smpls_key;      /* Key to an NHLFE entry */
    char              smpls_zero[10]; /* Unused space */
}
IP_PACKED2;
#include "ipcom_align16.h"


/*
 ****************************************************************************
 * 6                    SOCKET EXTENSION FUNCTIONS
 ****************************************************************************
 */

/* src/ipcom_sock2.c - address conversions (use pton/ntop for all new code!) */
IP_PUBLIC char         *ipcom_inet_ntoa(const struct Ip_in_addr in);
IP_PUBLIC unsigned long ipcom_inet_addr(const char *cp);
IP_PUBLIC int           ipcom_inet_aton(const char *cp, struct Ip_in_addr *addr);

/* port/src/ipcom_gethostby.c (use getipnodebyname/getipnodebyaddr for all new code!) */
IP_EXTERN int ipcom_h_errno;
IP_PUBLIC struct Ip_hostent *ipcom_gethostbyname(const char *name);
IP_PUBLIC struct Ip_hostent *ipcom_gethostbyname_r(const char *name,
                                                   struct Ip_hostent *result,
                                                   char *buffer, int buflen, int *h_errnop);
IP_PUBLIC struct Ip_hostent *ipcom_gethostbyaddr(const char *addr, int len, int type);
IP_PUBLIC struct Ip_hostent *ipcom_gethostbyaddr_r(const char *addr, int length, int type,
                                                   struct Ip_hostent *result,
                                                   char *buffer, int buflen, int *h_errnop);

/* src/ipcom_sock2.c (dummy functions, avoid!) */
IP_PUBLIC struct Ip_protoent *ipcom_getprotobyname(const char *name);
IP_PUBLIC struct Ip_protoent *ipcom_getprotobynumber(int proto);

/* port/src/ipcom_getservby.c */
IP_PUBLIC struct Ip_servent  *ipcom_getservbyname(const char *name, const char *proto);
IP_PUBLIC struct Ip_servent  *ipcom_getservbyport(int port, char *proto);

/* port/src/ipcom_getinfo.c */
IP_PUBLIC int ipcom_getnameinfo(IP_CONST struct Ip_sockaddr *sa,
                                Ip_socklen_t salen,
                                char *host, Ip_size_t hostlen,
                                char *serv, Ip_size_t servlen,
                                int flags);
IP_PUBLIC int ipcom_getaddrinfo(IP_CONST char *hostname,
                                IP_CONST char *servname,
                                IP_CONST struct Ip_addrinfo *hints,
                                struct Ip_addrinfo **res);
IP_PUBLIC void ipcom_freeaddrinfo(struct Ip_addrinfo *ai);

/* port/src/ipcom_ifaddrs.c */
IP_PUBLIC int ipcom_getifaddrs(struct Ip_ifaddrs **ifap);
IP_PUBLIC void ipcom_freeifaddrs(struct Ip_ifaddrs *ifp);


/*
 *===========================================================================
 *                         hostname
 *===========================================================================
 * port/src/ipcom_hostname.c - ipcom_gethostname() and ipcom_sethostname()
 */

#define IP_MAXHOSTNAMELEN 64  /* Maximum length of hostname */

#if defined(IPCOM_GETHOSTNAME) && IPCOM_GETHOSTNAME != 1
#define ipcom_gethostname IPCOM_GETHOSTNAME
#elif !defined(IPCOM_GETHOSTNAME)
#define ipcom_gethostname gethostname
#endif

#if (defined(IPCOM_GETHOSTNAME) && IPCOM_GETHOSTNAME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_gethostname(char *name, Ip_size_t len);
#endif


#if defined(IPCOM_SETHOSTNAME) && IPCOM_SETHOSTNAME != 1
#define ipcom_sethostname IPCOM_SETHOSTNAME
#elif !defined(IPCOM_SETHOSTNAME)
#define ipcom_sethostname sethostname
#endif

#if (defined(IPCOM_SETHOSTNAME) && IPCOM_SETHOSTNAME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_sethostname(const char *name, Ip_size_t len);
#endif


/*
 *===========================================================================
 *                          ipcom_socketpair
 *===========================================================================
 * Description: Creates a pair of connected sockets in the specified domain.
 * Parameters:  domain - Address domain to use, support domains are
 *                       IP_AF_INET and IP_AF_INET6.
 *              type - Socket type to use, supported types are
 *                     IP_SOCK_STREAM and IP_SOCK_DGRAM.
 *              protocol - Transport protocol to use, can be set to 0 which
 *                         means that the kernel will pick the protocol.
 *              sv - The 2 sockets are returned into this array.
 * Returns:     0 = success, -1 = error (code in ipcom_errno).
 *
 */
IP_PUBLIC int ipcom_socketpair(int domain, int type, int protocol, int sv[2]);


/*
 *===========================================================================
 *                    ipcom_pkt_add_hdr_to_pool
 *===========================================================================
 * Description: Allocates packet headers and put them in the pool.
 * Parameters:  num - Number of packet headers to add.
 * Returns:     0 = success, < 0 = negative errno code.
 *
 */
IP_PUBLIC int ipcom_pkt_add_hdr_to_pool(int num);


/*
 *===========================================================================
 *                    ipcom_pkt_hdrspace
 *===========================================================================
 * Description: Returns the number of octets that must be reserved before
 *              the application data when calling ipcom_pkt_sendto() for
 *              this socket.
 * Parameters:  fd - The socket descriptor to use when sending this packet.
 * Returns:     >=0 = the number of octets that must be reserved for
 *              transport, network and link header.
 *              =IP_SOCKERR = error, get the error from ipcom_errno.
 *
 */
IP_PUBLIC int ipcom_pkt_hdrspace(Ip_fd fd);


/*
 *===========================================================================
 *                    ipcom_pkt_sendto
 *===========================================================================
 * Description: Zero copy version of ipcom_sendto().
 * Parameters:  fd - The socket descriptor to use when sending this packet.
 *              buf - The data to send.
 *              start - Offset into 'buf' where the data begins, must
 *              be large enough to allow the stack to add transport, network
 *              and link header before of the data (use ipcom_pkt_hdrspace()
 *              to get the minimum value), this value must be even.
 *              len - The length of the data, 'buf' must be at least start+len
 *              octets long.
 *              freefunc - A function that will free the 'buf' pointer.
 *              freefunc_cookie - Will be passed as the 2:nd argument to
 *              'freefunc' when the 'buf' is freed.
 *              flags - IP_MSG_xxx flags.
 *              to - The destination address. Unused if the socket has been
 *              connected by a call to ipcom_connect().
 *              tolen - The length of the 'to' buffer.
 * Returns:     >=0 = the number of bytes sent (success), =IP_SOCKERR =
 *              error, get the error code from ipcom_errno.
 *
 */
IP_PUBLIC int ipcom_pkt_sendto(Ip_fd fd,
                               void *buf, int start, Ip_size_t len,
                               Ipcom_pkt_freefunc freefunc, void *freefunc_cookie,
                               int flags, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen);


/*
 *===========================================================================
 *                    ipcom_pkt_recvfrom
 *===========================================================================
 * Description: Receives a packet (zero-copy).
 * Parameters:  fd - A socket descriptor.
 *              pbuf - Pointer to where the buffer pointer should be stored.
 *              pstart - Will contain the offset into 'buf' where the
 *              application data starts.
 *              pfreefunc - Pointer to the function pointer that must
 *              be called to free the buffer will be stored.
 *              pfreefunc_cookie - Must be passed as argument 2 when calling
 *              'freefunc'.
 *              flags - IP_MSG_xxx flags.
 *              from - The sender of the packet.
 *              fromlen - The length of the from buffer.
 * Returns:     The length of the returned buffer or IP_SOCKERR if the call
 *              failed, get the error code from ipcom_errno.
 *
 */
IP_PUBLIC int ipcom_pkt_recvfrom(Ip_fd fd,
                                 void **pbuf, int *pstart,
                                 Ipcom_pkt_freefunc *pfreefunc, void **pfreefunc_cookie,
                                 int flags, struct Ip_sockaddr *from, Ip_socklen_t *fromlen);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

