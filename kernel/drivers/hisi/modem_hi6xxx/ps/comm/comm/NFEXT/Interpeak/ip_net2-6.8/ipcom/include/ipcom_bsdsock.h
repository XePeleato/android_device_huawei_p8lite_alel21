/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_bsdsock.h,v $ $Revision: 1.14 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_bsdsock.h,v $
 *   $Author: kenneth $ $Date: 2009-09-08 12:33:48 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_BSDSOCK_H
#define IPCOM_BSDSOCK_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * BSD socket to Interpeak IPCOM sockets wrapper header file.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ***********x*****************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_sock6.h>

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
 *                         misc
 *===========================================================================
 */
#define IFNAMSIZ      IP_IFNAMSIZ


/*
 *===========================================================================
 *                         domain
 *===========================================================================
 */
#define AF_UNSPEC     IP_AF_UNSPEC
#define AF_INET       IP_AF_INET
#define AF_NETLINK    IP_AF_NETLINK
#define AF_ROUTE      IP_AF_ROUTE
#define AF_LINK       IP_AF_LINK
#define AF_PACKET     IP_AF_PACKET
#define AF_INET6      IP_AF_INET6
#define AF_KEY        IP_AF_KEY
#define AF_MAX        IP_AF_MAX

#define PF_INET       IP_PF_INET
#define PF_NETLINK    IP_PF_NETLINK
#define PF_ROUTE      IP_PF_ROUTE
#define PF_LINK       IP_PF_LINK
#define PF_PACKET     IP_PF_PACKET
#define PF_INET6      IP_PF_INET6
#define PF_KEY        IP_PF_KEY


/*
 *===========================================================================
 *                         Socket types
 *===========================================================================
 */
#define SOCK_STREAM      IP_SOCK_STREAM
#define SOCK_DGRAM       IP_SOCK_DGRAM
#define SOCK_RAW         IP_SOCK_RAW
#define SOCK_PACKET      IP_SOCK_PACKET


/*
 *===========================================================================
 *                         IP protocol values
 *===========================================================================
 */
#define IPPROTO_IP          IP_IPPROTO_IP
#define IPPROTO_ICMP        IP_IPPROTO_ICMP
#define IPPROTO_IGMP        IP_IPPROTO_IGMP
#define IPPROTO_IPIP        IP_IPPROTO_IPIP
#define IPPROTO_IPV4        IP_IPPROTO_IPV4
#define IPPROTO_TCP         IP_IPPROTO_TCP
#define IPPROTO_UDP         IP_IPPROTO_UDP
#define IPPROTO_IPV6        IP_IPPROTO_IPV6
#define IPPROTO_RSVP        IP_IPPROTO_RSVP
#define IPPROTO_GRE         IP_IPPROTO_GRE
#define IPPROTO_ESP         IP_IPPROTO_ESP
#define IPPROTO_AH          IP_IPPROTO_AH
#define IPPROTO_ICMPV6      IP_IPPROTO_ICMPV6
#define IPPROTO_OSPFIGP     IP_IPPROTO_OSPFIGP
#define IPPROTO_PIM         IP_IPPROTO_PIM
#define IPPROTO_RAW         IP_IPPROTO_RAW
#define IPPROTO_MAX         IP_IPPROTO_MAX

/* IPv6 Non-IP Protocol Next Header field values */
#define IPPROTO_HOPOPTS     IP_IPPROTO_HOPOPTS
#define IPPROTO_ROUTING     IP_IPPROTO_ROUTING
#define IPPROTO_FRAGMENT    IP_IPPROTO_FRAGMENT
#define IPPROTO_NONE        IP_IPPROTO_NONE
#define IPPROTO_DSTOPTS     IP_IPPROTO_DSTOPTS


/*
 *===========================================================================
 *                  Standard IP address macros.
 *===========================================================================
 */
#define INADDR_ANY             IP_INADDR_ANY
#define INADDR_DEFAULT         IP_INADDR_DEFAULT
#define INADDR_LOOPBACK        IP_INADDR_LOOPBACK
#define INADDR_BROADCAST       IP_INADDR_BROADCAST
#define	INADDR_NONE            IP_INADDR_NONE

#define INADDR_UNSPEC_GROUP    IP_INADDR_UNSPEC_GROUP
#define INADDR_ALLHOSTS_GROUP  IP_INADDR_ALLHOSTS_GROUP


/*
 *===========================================================================
 *                   host <-> network endian
 *===========================================================================
 */
#ifndef htons
#define htons(x)          ip_htons(x)
#endif
#ifndef htonl
#define htonl(x)          ip_htonl(x)
#endif
#ifndef ntohs
#define ntohs(x)          ip_ntohs(x)
#endif
#ifndef ntohl
#define ntohl(x)          ip_ntohl(x)
#endif


/*
 *===========================================================================
 *                         NETMASK
 *===========================================================================
 */
#define NETMASK(xxip)          IP_NETMASK(xxip)
#define NETBITS(xxip)          IP_NETBITS(xxip)

#define	IN_CLASSA(xxip)	       IP_IN_CLASSA(xxip)
#define	IN_CLASSB(xxip)        IP_IN_CLASSB(xxip)
#define	IN_CLASSC(xxip)        IP_IN_CLASSC(xxip)
#define	IN_CLASSD(xxip)        IP_IN_CLASSD(xxip)
#define	IN_EXPERIMENTAL(xxip)  IP_IN_EXPERIMENTAL(xxip)


/*
 *===========================================================================
 *                      IPv6 address macros
 *===========================================================================
 */

#define IN6_ARE_ADDR_EQUAL(addr1)            IP_IN6_ARE_ADDR_EQUAL(addr1)
#define IN6_IS_ADDR_UNSPECIFIED(addr)        IP_IN6_IS_ADDR_UNSPECIFIED(addr)
#define IN6_IS_ADDR_LOOPBACK(addr)           IP_IN6_IS_ADDR_LOOPBACK(addr)
#define IN6_IS_ADDR_V4COMPAT(addr)           IP_IN6_IS_ADDR_V4COMPAT(addr)
#define IN6_IS_ADDR_V4MAPPED(addr)           IP_IN6_IS_ADDR_V4MAPPED(addr)
#define IN6_IS_ADDR_AGGR_GLOB_UNICAST(addr)  IP_IN6_IS_ADDR_AGGR_GLOB_UNICAST(addr)
#define IN6_IS_ADDR_SITE_LOCAL(addr)         IP_IN6_IS_ADDR_SITE_LOCAL(addr)
#define IN6_IS_ADDR_LINK_LOCAL(addr)         IP_IN6_IS_ADDR_LINK_LOCAL(addr)
#define IN6_IS_ADDR_MULTICAST(addr)          IP_IN6_IS_ADDR_MULTICAST(addr)


/*
 *===========================================================================
 *                         setsockopt/getsockopt
 *===========================================================================
 */

#define SOL_SOCKET         IP_SOL_SOCKET

/***** IP_SOL_SOCKET socket options: *****/
#define SO_REUSEADDR       IP_SO_REUSEADDR
#define SO_KEEPALIVE       IP_SO_KEEPALIVE
#define SO_DONTROUTE       IP_SO_DONTROUTE
#define SO_BROADCAST       IP_SO_BROADCAST
#define SO_REUSEPORT       IP_SO_REUSEPORT
#define SO_SNDBUF          IP_SO_SNDBUF
#define SO_RCVBUF          IP_SO_RCVBUF
#define SO_RCVTIMEO        IP_SO_RCVTIMEO
#define SO_ERROR           IP_SO_ERROR
#define SO_TYPE            IP_SO_TYPE
#define SO_BINDTODEVICE    IP_SO_BINDTODEVICE

/***** IP_IPPROTO_IP level socket options: *****/
#define IP_HDRINCL         IP_IP_HDRINCL
#define IP_TOS             IP_IP_TOS
#define IP_TTL             IP_IP_TTL
#define IP_PKTINFO         IP_IP_PKTINFO
#define IP_MULTICAST_IF    IP_IP_MULTICAST_IF
#define IP_MULTICAST_TTL   IP_IP_MULTICAST_TTL
#define IP_MULTICAST_LOOP  IP_IP_MULTICAST_LOOP
#define IP_ADD_MEMBERSHIP  IP_IP_ADD_MEMBERSHIP
#define	IP_JOIN_GROUP      IP_IP_JOIN_GROUP
#define IP_DROP_MEMBERSHIP IP_IP_DROP_MEMBERSHIP
#define	IP_LEAVE_GROUP     IP_IP_LEAVE_GROUP
#define IP_RECVIF          IP_IP_RECVIF
#define IP_ROUTER_ALERT    IP_IP_ROUTER_ALERT
#define IP_DONTFRAG        IP_IP_DONTFRAG

/***** IP_IPPROTO_IPV6 level socket options: *****/
#define IPV6_UNICAST_HOPS     IP_IPV6_UNICAST_HOPS
#define IPV6_MULTICAST_IF     IP_IPV6_MULTICAST_IF
#define IPV6_MULTICAST_HOPS   IP_IPV6_MULTICAST_HOPS
#define IPV6_MULTICAST_LOOP   IP_IPV6_MULTICAST_LOOP
#define	IPV6_JOIN_GROUP       IP_IPV6_JOIN_GROUP
#define IPV6_ADD_MEMBERSHIP   IP_IPV6_ADD_MEMBERSHIP
#define	IPV6_LEAVE_GROUP      IP_IPV6_LEAVE_GROUP
#define IPV6_DROP_MEMBERSHIP  IP_IPV6_DROP_MEMBERSHIP
#define IPV6_PKTINFO          IP_IPV6_PKTINFO
#define IPV6_TCLASS           IP_IPV6_TCLASS
#define IPV6_NEXTHOP          IP_IPV6_NEXTHOP
#define IPV6_RTHDR            IP_IPV6_RTHDR
#define IPV6_HOPOPTS          IP_IPV6_HOPOPTS
#define IPV6_DSTOPTS          IP_IPV6_DSTOPTS
#define IPV6_RTHDRDSTOPTS     IP_IPV6_RTHDRDSTOPTS
#define IPV6_RECVPKTINFO      IP_IPV6_RECVPKTINFO
#define IPV6_RECVHOPLIMIT     IP_IPV6_RECVHOPLIMIT
#define IPV6_RECVTCLASS       IP_IPV6_RECVTCLASS
#define IPV6_RECVRTHDR        IP_IPV6_RECVRTHDR
#define IPV6_RECVHOPOPTS      IP_IPV6_RECVHOPOPTS
#define IPV6_RECVDSTOPTS      IP_IPV6_RECVDSTOPTS
#define IPV6_CHECKSUM         IP_IPV6_CHECKSUM
#define IPV6_RECVIF           IP_IPV6_RECVIF

/***** IP_IPPROTO_ICMPV6 level socket options: *****/
#define ICMP6_FILTER          IP_ICMP6_FILTER

/***** IP_IPPROTO_TCP level socket options: *****/
#define TCP_NODELAY           IP_TCP_NODELAY

/* NOTE: This option can NOT be used with getsockopt/setsockopt!
 * Only sent/rcvd with ancillary data!
 */
#define IPV6_HOPLIMIT         IP_IPV6_HOPLIMIT


/*
 *===========================================================================
 *                         ioctl
 *===========================================================================
 */

/* socket ioctl */
#define SIOCSPGRP          IP_SIOCSPGRP
#define SIOCGPGRP          IP_SIOCGPGRP
#define FIONBIO            IP_FIONBIO

/* AF_INET ioctl. */
#define SIOCGIFADDR        IP_SIOCGIFADDR
#define SIOCSIFADDR        IP_SIOCSIFADDR
#define SIOCGIFBRDADDR     IP_SIOCGIFBRDADDR
#define SIOCSIFBRDADDR     IP_SIOCSIFBRDADDR
#define SIOCGIFNETMASK     IP_SIOCGIFNETMASK
#define SIOCSIFNETMASK     IP_SIOCSIFNETMASK
#define SIOCGIFDSTADDR     IP_SIOCGIFDSTADDR
#define SIOCSIFDSTADDR     IP_SIOCSIFDSTADDR
#define SIOCAIFADDR        IP_SIOCAIFADDR
#define SIOCDIFADDR        IP_SIOCDIFADDR
#define SIOCADDRT          IP_SIOCADDRT
#define SIOCDDDRT          IP_SIOCDDDRT

/* AF_INET6 ioctl. */
#define SIOCAIFADDR_IN6    IP_SIOCAIFADDR_IN6
#define SIOCDIFADDR_IN6    IP_SIOCDIFADDR_IN6
#define SIOCGIFDSTADDR_IN6 IP_SIOCGIFDSTADDR_IN6

/* interface ioctl. */
#define SIOCGIFFLAGS       IP_SIOCGIFFLAGS
#define SIOCSIFFLAGS       IP_SIOCSIFFLAGS
#define SIOCGIFMTU         IP_SIOCGIFMTU
#define SIOCSIFMTU         IP_SIOCSIFMTU
#define SIOCGIFMETRIC      IP_SIOCGIFMETRIC
#define SIOCSIFMETRIC      IP_SIOCSIFMETRIC
#define SIOCSIFRTAB        IP_SIOCSIFRTAB
#define SIOCGIFRTAB        IP_SIOCGIFRTAB
#define SIOCGETTUNNEL      IP_SIOCGETTUNNEL
#define SIOCADDTUNNEL      IP_SIOCADDTUNNEL
#define SIOCCHGTUNNEL      IP_SIOCCHGTUNNEL
#define SIOCDELTUNNEL      IP_SIOCDELTUNNEL

/* arp ioctl. */
#define SIOCSARP           IP_SIOCSARP
#define SIOCGARP           IP_SIOCGARP
#define SIOCDARP           IP_SIOCDARP


/*
 *===========================================================================
 *                         IFF_X
 *===========================================================================
 */
#define IFF_UP          IP_IFF_UP
#define IFF_BROADCAST   IP_IFF_BROADCAST
#define IFF_DEBUG       IP_IFF_DEBUG
#define IFF_LOOPBACK    IP_IFF_LOOPBACK
#define IFF_POINTOPOINT IP_IFF_POINTOPOINT
#define IFF_RUNNING     IP_IFF_RUNNING
#define IFF_NOARP       IP_IFF_NOARP
#define IFF_PROMISC     IP_IFF_PROMISC
#define IFF_ALLMULTI    IP_IFF_ALLMULTI
#define IFF_OACTIVE     IP_IFF_OACTIVE
#define IFF_SIMPLEX     IP_IFF_SIMPLEX
#define IFF_LINK0       IP_IFF_LINK0
#define IFF_LINK1       IP_IFF_LINK1
#define IFF_LINK2       IP_IFF_LINK2
#define IFF_MULTICAST   IP_IFF_MULTICAST


/*
 *===========================================================================
 *                    recvxx() argument flags
 *===========================================================================
 */
#define MSG_PEEK           IP_MSG_PEEK


/*
 *===========================================================================
 *                    shutdown() argument flags
 *===========================================================================
 */
#define SHUT_RD            IP_SHUT_RD
#define SHUT_WR            IP_SHUT_WR
#define SHUT_RDWR          IP_SHUT_RDWR


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    IP types
 *===========================================================================
 */

#define in_addr_t     Ip_in_addr_t
#define sa_family_t   Ip_sa_family_t
#define in_port_t     Ip_in_port_t
#define socklen_t     Ip_socklen_t

#define in6_addr      Ip_in6_addr
#define in6_addr_t    Ip_in6_addr_t
#define s6_addr       in6.addr8


/*
 *===========================================================================
 *                    iovec
 *===========================================================================
 */
#define iovec         Ip_iovec


/*
 *===========================================================================
 *                    in_addr
 *===========================================================================
 */
#ifdef s_addr
#undef s_addr
#endif

#define in_addr       Ip_in_addr


/*
 *===========================================================================
 *                    sockaddr
 *===========================================================================
 */
#define sockaddr_in     Ip_sockaddr_in
#define sockaddr_in6    Ip_sockaddr_in6
#define sockaddr_dl     Ip_sockaddr_dl
#define sockaddr_ll     Ip_sockaddr_ll
#define sockaddr        Ip_sockaddr

#define SOCKADDR_DL_LLADDR(sa)   IP_SOCKADDR_DL_LLADDR(sa)

/* Packet types 'sll_pkttype'.    Packet is addressed to: */
#define PACKET_HOST          IP_PACKET_HOST
#define PACKET_BROADCAST     IP_PACKET_BROADCAST
#define PACKET_MULTICAST     IP_PACKET_MULTICAST
#define PACKET_OTHERHOST     IP_PACKET_OTHERHOST
#define PACKET_OUTGOING      IP_PACKET_OUTGOING


/*
 *===========================================================================
 *                         IPv6 Extension Headers
 *===========================================================================
 */
#define pkt_ip6_ext_hdr        Ip_pkt_ip6_ext_hdr
#define pkt_ip6_hbh            Ip_pkt_ip6_hbh
#define pkt_ip6_dest           Ip_pkt_ip6_dest
#define pkt_ip6_rthdr          Ip_pkt_ip6_rthdr
#define IPV6_RTHDR_TYPE_0      IP_IPV6_RTHDR_TYPE_0
#define pkt_ip6_rthdr0         Ip_pkt_ip6_rthdr0
#define pkt_ip6_frag           Ip_pkt_ip6_frag

#define IP6F_OFF_MASK          IP_IP6F_OFF_MASK
#define IP6F_RESERVED_MASK     IP_IP6F_RESERVED_MASK
#define IP6F_MORE_FRAG         IP_IP6F_MORE_FRAG

#define IP6F_SIZE              IP_IP6F_SIZE
#define IP6F_GET_OFFSET(hdr)   IP_IP6F_GET_OFFSET(hdr)


/*
 *===========================================================================
 *                      Extension Header Options
 *===========================================================================
 */
#define pkt_ip6_opt              Ip_pkt_ip6_opt
#define IP6OPT_TYPE(o)           IP_IP6OPT_TYPE(o)
#define IP6OPT_TYPE_SKIP         IP_IP6OPT_TYPE_SKIP
#define IP6OPT_TYPE_DISCARD      IP_IP6OPT_TYPE_DISCARD
#define IP6OPT_TYPE_FORCEICMP    IP_IP6OPT_TYPE_FORCEICMP
#define IP6OPT_TYPE_ICMP         IP_IP6OPT_TYPE_ICMP
#define IP6OPT_MUTABLE           IP_IP6OPT_MUTABLE
#define IP6OPT_PAD1              IP_IP6OPT_PAD1
#define IP6OPT_PADN              IP_IP6OPT_PADN
#define IP6OPT_JUMBO             IP_IP6OPT_JUMBO
#define IP6OPT_NSAP_ADDR         IP_IP6OPT_NSAP_ADDR
#define IP6OPT_TUNNEL_LIMIT      IP_IP6OPT_TUNNEL_LIMIT
#define IP6OPT_ROUTER_ALERT      IP_IP6OPT_ROUTER_ALERT

#define ip6_opt_router           Ip_ip6_opt_router
#define IP6_ALERT_MLD            IP_IP6_ALERT_MLD
#define IP6_ALERT_RSVP           IP_IP6_ALERT_RSVP
#define IP6_ALERT_AN             IP_IP6_ALERT_AN


/*
 *===========================================================================
 *                    IPv6 icmp filter
 *===========================================================================
 */
#define ICMP6_FILTER_WILLPASS(type)         IP_ICMP6_FILTER_WILLPASS(type)
#define ICMP6_FILTER_WILLBLOCK(type)        IP_ICMP6_FILTER_WILLBLOCK(type)
#define ICMP6_FILTER_SETPASS(type)          IP_ICMP6_FILTER_SETPASS(type)
#define ICMP6_FILTER_SETBLOCK(type)         IP_ICMP6_FILTER_SETBLOCK(type)
#define ICMP6_FILTER_SETPASSALL(filterp)    IP_ICMP6_FILTER_SETPASSALL(filterp)
#define ICMP6_FILTER_SETBLOCKALL(filterp)   IP_ICMP6_FILTER_SETBLOCKALL(filterp)


/*
 *===========================================================================
 *                    timeval
 *===========================================================================
 */
#define timeval       Ip_timeval


/*
 *===========================================================================
 *                    msghdr
 *===========================================================================
 */
#define msghdr        Ip_msghdr

/* msg_flags values */
#define MSG_TRUNC     IP_MSG_TRUNC
#define MSG_CTRUNC    IP_MSG_CTRUNC


/*
 *===========================================================================
 *                    cmsghdr
 *===========================================================================
 */
#define cmsghdr        Ip_cmsghdr

#define CMSG_ALIGN(len)          IP_CMSG_ALIGN(len)
#define CMSG_FIRSTHDR(mhdrptr)   IP_CMSG_FIRSTHDR(mhdrptr)
#define CMSG_NXTHDR(mhdr,cmsg)   IP_CMSG_NXTHDR(mhdr,cmsg)
#define CMSG_DATA(cmsgptr)       IP_CMSG_DATA(cmsgptr)
#define CMSG_SPACE(len)          IP_CMSG_SPACE(len)
#define CMSG_LEN(len)            IP_CMSG_LEN(len)


/*
 *===========================================================================
 *                    if_nameindex
 *===========================================================================
 */
#ifdef if_nameindex
#undef if_nameindex
#endif
struct if_nameindex {
    struct Ip_if_nameindex ni;
};

#define if_index ni.if_index
#define if_name  ni.if_name

/*
 *===========================================================================
 *                    ioctl structures
 *===========================================================================
 */
#define ip_mreq             Ip_ip_mreq
#define arpreq              Ip_arpreq
#define ortentry            Ip_ortentry
#define aliasreq            Ip_ifaliasreq
#define ifra_broadaddr      ifra_dstaddr

#define ip6_mreq            Ip_ipv6_mreq
#define ipv6_mreq           Ip_ipv6_mreq
#define in6_pktinfo         Ip_in6_pktinfo
#define icmp6_filter        Ip_icmp6_filter
#define in6_addrlifetime    Ip_in6_addrlifetime
#define in6_aliasreq        Ip_in6_aliasreq


/*
 *===========================================================================
 *                        in_pktinfo
 *===========================================================================
 */
#define in_pktinfo      Ip_in_pktinfo


/*
 *===========================================================================
 *                        hostent
 *===========================================================================
 */
#define hostent         Ip_hostent


/*
 *===========================================================================
 *                        ifreq
 *===========================================================================
 */
#define ifreq           Ip_ifreq

#define ifr_addr        ifr_ifru.ifru_addr
#define ifr_dstaddr     ifr_ifru.ifru_dstaddr
#define ifr_broadaddr   ifr_ifru.ifru_broadaddr
#define ifr_flags       ifr_ifru.ifru_flags
#define ifr_metric      ifr_ifru.ifru_metric
#define ifr_mtu         ifr_ifru.ifru_mtu
#define ifr_data        ifr_ifru.ifru_data
#define ifr_rtab        ifr_ifru.ifru_rtab


/*
 *===========================================================================
 *                        in6_ifreq
 *===========================================================================
 */
#define in6_ifreq       Ip_in6_ifreq

#define IN6_IFF_TENTATIVE   IP_IN6_IFF_TENTATIVE


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *             socket & file functions/macros - SEPARATE FLIB
 *===========================================================================
 * Define IPCOM_USE_SEPARATE_FLIB if you use sockets and file IO with
 * system call conflicts in the same file.
 */
#ifdef IPCOM_USE_SEPARATE_FLIB
/* functions used by both sockets & files. */
#define socketioctl(fd,request,argp)       ipcom_socketioctl((Ip_fd)fd,request,argp)
#define socketclose(fd)                    ipcom_socketclose((Ip_fd)fd)
#define socketwrite(fd,buf,nbytes)         ipcom_socketwrite((Ip_fd)fd,(IP_CONST void *)buf,(Ip_size_t)nbytes)
#define socketwritev(fd,iov,iovlen)        ipcom_socketwritev((Ip_fd)fd,iov,iovlen)
#define socketread(fd,buf,nbytes)          ipcom_socketread((Ip_fd)fd,buf,(Ip_size_t)nbytes)
#define socketselect(nfds,rf,wf,ef,t)      ipcom_socketselect(nfds,rf,wf,ef,t)
/* errno */
#define socketerrno                        ipcom_errno
/* select stuff */
#define SOCKETFD_SETSIZE          IP_FD_SETSIZE
#define socketfd_set              Ip_fd_set
#define socketFD_ZERO(pfdset)     IP_FD_ZERO(pfdset)
#define socketFD_CLR(fd,pfdset)   IP_FD_CLR(fd,pfdset)
#define socketFD_SET(fd,pfdset)   IP_FD_SET(fd,pfdset)
#define socketFD_ISSET(fd,pfdset) IP_FD_ISSET(fd,pfdset)
#define	sockethowmany(x)          ip_howmany(x)

/*
 *===========================================================================
 *             socket & file functions/macros
 *===========================================================================
 */
#else
/* functions used by both sockets & files. */
#define ioctl(fd,request,argp)             ipcom_socketioctl((Ip_fd)fd,request,argp)
#define close(fd)                          ipcom_socketclose((Ip_fd)fd)
#define write(fd,buf,nbytes)               ipcom_socketwrite((Ip_fd)fd,(IP_CONST void *)buf,(Ip_size_t)nbytes)
#define writev(fd,iov,iovlen)              ipcom_socketwritev((Ip_fd)fd,iov,iovlen)
#define read(fd,buf,nbytes)                ipcom_socketread((Ip_fd)fd,buf,(Ip_size_t)nbytes)
#define select(nfds,rf,wf,ef,t)            ipcom_socketselect(nfds,rf,wf,IP_NULL,t)
#define errno                              ipcom_errno
/* select stuff */
#undef FD_SETSIZE
#define FD_SETSIZE          IP_FD_SETSIZE
#undef fd_set
#define fd_set              Ip_fd_set
#undef FD_ZERO
#define FD_ZERO(pfdset)     IP_FD_ZERO(pfdset)
#undef FD_CLR
#define FD_CLR(fd,pfdset)   IP_FD_CLR(fd,pfdset)
#undef FD_SET
#define FD_SET(fd,pfdset)   IP_FD_SET(fd,pfdset)
#undef FD_ISSET
#define FD_ISSET(fd,pfdset) IP_FD_ISSET(fd,pfdset)
#undef howmany
#define	howmany(x)          ip_howmany(x)
#endif /* IPCOM_USE_SHARED_FLIB */


/*
 *===========================================================================
 *                        socket only functions
 *===========================================================================
 */

#define socket(domain,type,protocol)       ipcom_socket(domain,type,protocol)
#define shutdown(fd,how)                   ipcom_shutdown((Ip_fd)fd,how)
#define bind(fd,addr,addrlen)              ipcom_bind((Ip_fd)fd,(IP_CONST struct Ip_sockaddr *)addr,(Ip_socklen_t)addrlen)
#define connect(fd,addr,addrlen)           ipcom_connect((Ip_fd)fd,(IP_CONST struct Ip_sockaddr *)addr,(Ip_socklen_t)addrlen)
#define accept(fd,addr,addrlenp)           ipcom_accept((Ip_fd)fd,(struct Ip_sockaddr *)addr,(Ip_socklen_t *)addrlenp)
#define listen(fd,backlog)                 ipcom_listen((Ip_fd)fd,backlog)

#define send(fd,msg,len,flags)             ipcom_send((Ip_fd)fd,(IP_CONST void *)msg,(Ip_size_t)len,flags)
#define sendto(fd,msg,len,flags,to,tolen)  ipcom_sendto((Ip_fd)fd,(IP_CONST void *)msg,(Ip_size_t)len,flags,(IP_CONST struct Ip_sockaddr *)to,(Ip_socklen_t)tolen)
#define sendmsg(fd,msg,flags)              ipcom_sendmsg((Ip_fd)fd,(IP_CONST struct Ip_msghdr *)msg,flags)

#define recv(fd,buf,len,flags)             ipcom_recv((Ip_fd)fd,(void *)buf,(Ip_size_t)len,flags)
#define recvfrom(fd,buf,len,flags,from,fp) ipcom_recvfrom((Ip_fd)fd,(void *)buf,(Ip_size_t)len,flags,(struct Ip_sockaddr *)from,(Ip_socklen_t *)fp)
#define recvmsg(fd,msg,flags)              ipcom_recvmsg((Ip_fd)fd,(struct Ip_msghdr *)msg,flags)

#define getsockopt(fd,level,optname,optval,optlenp)  ipcom_getsockopt((Ip_fd)fd,level,optname,optval,(Ip_socklen_t *)optlenp)
#define setsockopt(fd,level,optname,optval,optlen)   ipcom_setsockopt((Ip_fd)fd,level,optname,(IP_CONST void *)optval,(Ip_socklen_t)optlen)

#define getsockname(fd,name,namelenp)         ipcom_getsockname((Ip_fd)fd,(struct Ip_sockaddr *)name,(Ip_socklen_t *)namelenp)
#define getpeername(fd,name,namelenp)         ipcom_getpeername((Ip_fd)fd,(struct Ip_sockaddr *)name,(Ip_socklen_t *)namelenp)

#define inet_pton(family,strptr,addrptr)      ipcom_inet_pton(family,(IP_CONST char *)strptr,addrptr)
#define inet_ntop(family,addrptr,strptr,len)  ipcom_inet_ntop(family,(IP_CONST void *)addrptr,strptr,(Ip_size_t)len)

#define if_nametoindex(ifname)             ipcom_if_nametoindex((IP_CONST char *)ifname)
#define if_indextoname(ifindex,ifname)     ipcom_if_indextoname((Ip_u32)ifindex, (char *)ifname)
#define if_nameindex()                     ipcom_if_nameindex()
#define if_freenameindex(pif)              ipcom_if_freenameindex(pif)

#define inet6_rth_space(t,s)               ipcom_inet6_rth_space(t,s)
#define inet6_rth_init(b,l,t,s)            ipcom_inet6_rth_init(b,l,t,s)
#define inet6_rth_add(b,a)                 ipcom_inet6_rth_add(b,a)
#define inet6_rth_reverse(i,o)             ipcom_inet6_rth_reverse(i,o)
#define inet6_rth_segments(b)              ipcom_inet6_rth_segments(b)
#define inet6_rth_getaddr(b,i)             ipcom_inet6_rth_getaddr(b,i)

#define inet6_opt_init(b,l)                ipcom_inet6_opt_init(b,l)
#define inet6_opt_append(eb,el,o,t,l,a,db) ipcom_inet6_opt_append(eb,el,o,t,l,a,db)
#define inet6_opt_finish(eb,el,o)          ipcom_inet6_opt_finish(eb,el,o)
#define inet6_opt_set_val(db,o,v,l)        ipcom_inet6_opt_set_val(db,o,v,l)
#define inet6_opt_next(eb,el,o,t,l,db)     ipcom_inet6_opt_next(eb,el,o,t,l,db)
#define inet6_opt_find(eb,el,o,t,l,db)     ipcom_inet6_opt_find(eb,el,o,t,l,db)
#define inet6_opt_get_val(db,o,v,l)        ipcom_inet6_opt_get_val(db,o,v,l)

/*
 ****************************************************************************
 * 7                    SOCK2
 ****************************************************************************
 */

#define protoent   Ip_protoent
#define servent    Ip_servent
#define addrinfo   Ip_addrinfo

#define inet_ntoa(in)           ipcom_inet_ntoa(in)
#define inet_addr(cp)           ipcom_inet_addr(cp)
#define inet_aton(cp,addr)      ipcom_inet_aton(cp,addr)

/* port/src/ipcom_gethostby.c (use getipnodebyname/getipnodebyaddr for all new code!) */
#define gethostbyname(name)     ipcom_gethostbyname(name)
#define gethostbyaddr(a,l,t)    ipcom_gethostbyaddr(a,l,t)

/* src/ipcom_sock2.c */
#define getprotobyname(name)    ipcom_getprotobyname(name)
#define getprotobynumber(proto) ipcom_getprotobynumber(proto)

/* port/src/ipcom_getservby.c */
#define getservbyname(n,p)      ipcom_getservbyname(n,p)
#define getservbyport(p,p2)     ipcom_getservbyport(p,p2)


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
