/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sock.h,v $ $Revision: 1.286.20.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_sock.h,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:20:55 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

#ifndef IPCOM_SOCK_H
#define IPCOM_SOCK_H


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak socket API.
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
#include "ipcom_cstyle.h"
#include "ipcom_errno.h"
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
 *            User library support
 *===========================================================================
 */
#if (!defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)) || defined(IPCOM_USE_REMOTE_STACK) || (defined(IPCOM_USE_NATIVE_SOCK_API) && !defined(IPCOM_SKIP_NATIVE_SOCK_API))
#define ipcom_socket           ipcom_socket_usr
#define ipcom_socketclose      ipcom_socketclose_usr
#define ipcom_shutdown         ipcom_shutdown_usr
#define ipcom_bind             ipcom_bind_usr
#define ipcom_connect          ipcom_connect_usr
#define ipcom_accept           ipcom_accept_usr
#define ipcom_listen           ipcom_listen_usr
#define ipcom_socketwrite      ipcom_socketwrite_usr
#define ipcom_socketwritev     ipcom_socketwritev_usr
#define ipcom_send             ipcom_send_usr
#define ipcom_sendto           ipcom_sendto_usr
#define ipcom_sendmsg          ipcom_sendmsg_usr
#define ipcom_socketread       ipcom_socketread_usr
#define ipcom_recv             ipcom_recv_usr
#define ipcom_recvfrom         ipcom_recvfrom_usr
#define ipcom_recvmsg          ipcom_recvmsg_usr
#define ipcom_getsockopt       ipcom_getsockopt_usr
#define ipcom_setsockopt       ipcom_setsockopt_usr
#define ipcom_getsockname      ipcom_getsockname_usr
#define ipcom_getpeername      ipcom_getpeername_usr
#define ipcom_socketioctl      ipcom_socketioctl_usr
#ifndef IPCOM_USE_NATIVE_SOCK_API
#define ipcom_if_nametoindex   ipcom_if_nametoindex_usr
#define ipcom_if_indextoname   ipcom_if_indextoname_usr
#define ipcom_if_nameindex     ipcom_if_nameindex_usr
#define ipcom_if_freenameindex ipcom_if_freenameindex_usr
#endif
#define ipcom_socketselect     ipcom_socketselect_usr
#endif


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
 *            misc
 *===========================================================================
 */
/* The maximum length of a network interface name */
#define IP_IFNAMSIZ       16
/* The number of sockets in the system */
#define IP_FD_SETSIZE     1024

#define IP_INVALID_SOCKET (Ip_fd) -1
#define IP_SOCKERR        (-1)

#define IPCOM_ADDR_INFINITE    0xffffffff  /* Infinite lifetime */

/* Max length of an address created by ipcom_inet_ntop(IP_AF_INET, ...) */
#define IP_INET_ADDRSTRLEN    16

/* The maximum length of route table name */
#define IPCOM_RTNAMSIZ        16


/*
 *===========================================================================
 *                         domain
 *===========================================================================
 */

#define IP_AF_UNSPEC    0    /* Unspecified domain */
#define IP_AF_INET      2    /* IPv4 */
#define IP_AF_IPX       4    /* IPX */
#define IP_AF_NETLINK  16    /* Linux Netlink, RFC 3549 */
#define IP_AF_ROUTE    17    /* Internal routing Protocol */
#define IP_AF_LINK     18    /* Link layer address (used in route entries and ancillary data) */
#define IP_AF_PACKET   19    /* Linux packet family */
#if defined(IP_PORT_LINUX) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
#define IP_AF_INET6    10    /* Default family value of IP version 6 */
#elif defined(IP_PORT_VXWORKS)
#define IP_AF_INET6    28    /* Default family value of IP version 6 */
#else
#define IP_AF_INET6    24    /* Default family value of IP version 6 */
#endif
#define IP_AF_KEY      27    /* PFKEYv2 key-management socket API */
#define IP_AF_MPLS     30    /* Used by the IPMPLS module */
#define IP_AF_SOCKDEV  31    /* Socket connected to a network interface (type IP_IFF_SOCKDEV) */
#ifdef IP_PORT_VXWORKS
#define IP_AF_WINDNET  32    /* Interoperability with windnet */
#endif
#define IP_AF_TIPC     33    /* Transparent Inter-Process Communic. */
#define IP_AF_MAX      35

#define IP_PF_INET    IP_AF_INET
#define IP_PF_NETLINK IP_AF_NETLINK
#define IP_PF_ROUTE   IP_AF_ROUTE
#define IP_PF_LINK    IP_AF_LINK
#define IP_PF_PACKET  IP_AF_PACKET
#define IP_PF_INET6   IP_AF_INET6
#define IP_PF_KEY     IP_AF_KEY
#define IP_PF_MPLS    IP_AF_MPLS
#define IP_PF_TIPC    IP_AF_TIPC
#define IP_PF_IPX     IP_AF_IPX


/*
 *===========================================================================
 *                         Socket types
 *===========================================================================
 */
#define IP_SOCK_STREAM        1
#define IP_SOCK_DGRAM         2
#define IP_SOCK_RAW           3
#define IP_SOCK_SEQPACKET     5
#define IP_SOCK_PACKET        10    /* linux packet socket API. */


/*
 *===========================================================================
 *                         IP protocol values
 *===========================================================================
 */
#define IP_IPPROTO_IP           0   /* Dummy for IP. */
#define IP_IPPROTO_ICMP         1   /* Internet Control Message Protocol. */
#define IP_IPPROTO_IGMP         2   /* Internet Group Management Protocol. */
#define IP_IPPROTO_IPIP         4   /* IPv4 inside IPv4. */
#define IP_IPPROTO_IPV4         4
#define IP_IPPROTO_TCP          6   /* Transmission Control Protocol */
#define IP_IPPROTO_UDP          17  /* User Datagram Protocol */
#define IP_IPPROTO_IPV6         41  /* IPv6 inside IPv4 */
#define IP_IPPROTO_RSVP         46  /* Reservation Protocol */
#define IP_IPPROTO_GRE          47  /* Cisco GRE tunnels (RFC 1701, 1702) */
#define IP_IPPROTO_ESP          50  /* Encap. Security Payload. */
#define IP_IPPROTO_AH           51  /* Authentication header. */
#define IP_IPPROTO_MOBILE       55  /* IPv4 in IPv4 Minimal Encapsulation (RFC 2002) */
#define IP_IPPROTO_ICMPV6       58  /* Internet Control Message Protocol version 6 */
#define IP_IPPROTO_NONEXTHDR    59  /* IANA no next header used in IPv6 and ESP */
#define IP_IPPROTO_OSPFIGP      89  /* Open Shortest Path First protocol */
#define IP_IPPROTO_PIM          103 /* Protocol Independent Multicast */
#define IP_IPPROTO_L2TP         115 /* Mobile Header for IPv6   */
#define IP_IPPROTO_SCTP         132 /* Stream Control Transmisstion Protocol */
#define IP_IPPROTO_MH           135 /* Mobile Header for IPv6   */
#define IP_IPPROTO_RAW          255 /* Raw IP datagram. */
#define IP_IPPROTO_MAX          256


/*
 *===========================================================================
 *              Flags to ipcom_send/ipcom_sendto/ipcom_sendmsg/
 *                  ipcom_recv/ipcom_recvfrom/ipcom_recvmsg
 *===========================================================================
 */
#define IP_MSG_OOB                0x1    /* Specify to send/receive out-of-band data */
#define IP_MSG_PEEK               0x2    /* Leave the data on the receive buffer (receive only) */
#define IP_MSG_DONTROUTE          0x4    /* Bypass normal routing, the packet will be sent
                                            on the interface that matches the network part
                                            of the destination address */
#define IP_MSG_WAITALL            0x40   /* Wait for full request or error (receive only) */
#define IP_MSG_DONTWAIT           0x80   /* Enables non-blocking operation */

#define IP_MSG_NOTIFICATION       0x800 /* SCTP Notification */
#define IP_MSG_EOR                0x8

#define IP_MSG_MORE               0x8000 /* Disable Nagle algorithm (send only) */
#ifdef IPCOM_ZEROCOPY
#define IP_MSG_ZBUF               0x80000/* zbuf */
#endif

/*
 *===========================================================================
 *                  Standard IP address macros.
 *===========================================================================
 */
#define IP_INADDR_ANY             0x00000000
#define IP_INADDR_DEFAULT         0x00000000
#define IP_INADDR_LOOPBACK        0x7F000001
#define IP_INADDR_BROADCAST       0xffffffff
#define	IP_INADDR_NONE	          0xffffffff   /* ipcom_inet_addr() error. */

#define IP_INADDR_UNSPEC_GROUP    0xe0000000
#define IP_INADDR_ALLHOSTS_GROUP  0xe0000001
#define IP_INADDR_ALLRTRS_GROUP   0xe0000002
#define IP_INADDR_ALLMA_GROUP     0xe000000b
#define IP_INADDR_MAX_LOCAL_GROUP 0xe00000ff


/*
 *===========================================================================
 *                   host <-> network endian
 *===========================================================================
 */
#define ip_htons(x)       IP_SWAP16(x)
#define ip_htonl(x)       IP_SWAP32(x)

#define ip_ntohs(x)       IP_SWAP16(x)
#define ip_ntohl(x)       IP_SWAP32(x)

/* For use with IPXINC and code which assigns variables in the macros */
IP_PUBLIC Ip_u16 ipcom_htons(Ip_u16 host);
IP_PUBLIC Ip_u16 ipcom_ntohs(Ip_u16 net);
IP_PUBLIC Ip_u32 ipcom_htonl(Ip_u32 host);
IP_PUBLIC Ip_u32 ipcom_ntohl(Ip_u32 net);


/*
 *===========================================================================
 *                         IP_NETMASK
 *===========================================================================
 */

/* Macro to get the network mask from an IP address. */
#define IP_NETMASK(xxip)         (((xxip) == 0UL)   ? 0UL : \
				 IP_IN_CLASSA((xxip)) ? IP_IN_CLASSA_NET : \
				 IP_IN_CLASSB((xxip)) ? IP_IN_CLASSB_NET : \
				 IP_IN_CLASSC((xxip)) ? IP_IN_CLASSC_NET : \
				 IP_IN_CLASSD((xxip)) ? IP_IN_CLASSD_NET : \
				 0UL) /*lint !e778 */

#define IP_NETBITS(xxip)         ((Ip_s16) (((xxip) == 0UL)  ? 0 : \
                 IP_IN_CLASSA((xxip)) ?  8 : \
                 IP_IN_CLASSB((xxip)) ? 16 : \
                 IP_IN_CLASSC((xxip)) ? 24 : \
                 IP_IN_CLASSD((xxip)) ?  4 : \
                 0))  /*lint !e778 */

/* submacros used by IP_NETMASK. */
#define	IP_IN_CLASSA_NET		ip_htonl(0xff000000UL) /*lint !e778 */
#define	IP_IN_CLASSB_NET		ip_htonl(0xffff0000UL) /*lint !e778 */
#define	IP_IN_CLASSC_NET		ip_htonl(0xffffff00UL) /*lint !e778 */
#define IP_IN_CLASSD_NET        ip_htonl(0xf0000000UL) /*lint !e778 */

#ifdef IP_LITTLE_ENDIAN
#define	IP_IN_CLASSA(xxip)	((((Ip_u32)(xxip)) & 0x00000080UL) == 0UL)
#define	IP_IN_CLASSB(xxip)	((((Ip_u32)(xxip)) & 0x000000c0UL) == 0x00000080UL)
#define	IP_IN_CLASSC(xxip)	((((Ip_u32)(xxip)) & 0x000000e0UL) == 0x000000c0UL)
#define	IP_IN_CLASSD(xxip)	((((Ip_u32)(xxip)) & 0x000000f0UL) == 0x000000e0UL)
#define	IP_IN_EXPERIMENTAL(xxip) ((((Ip_u32)(xxip)) & 0x000000f0UL) == 0x000000f0UL)
#elif defined(IP_BIG_ENDIAN)
#define	IP_IN_CLASSA(xxip)	((((Ip_u32)(xxip)) & 0x80000000UL) == 0UL)
#define	IP_IN_CLASSB(xxip)	((((Ip_u32)(xxip)) & 0xc0000000UL) == 0x80000000UL)
#define	IP_IN_CLASSC(xxip)	((((Ip_u32)(xxip)) & 0xe0000000UL) == 0xc0000000UL)
#define	IP_IN_CLASSD(xxip) 	((((Ip_u32)(xxip)) & 0xf0000000UL) == 0xe0000000UL)
#define	IP_IN_EXPERIMENTAL(xxip) ((((Ip_u32)(xxip)) & 0xf0000000UL) == 0xf0000000UL)
#endif

/*
 * Define IPCOM_ALLOW_EXPERIMENTAL_CLASS if the node should be allowed
 * to use the unicast addresses in the experimental class.
 */
#ifdef IPCOM_ALLOW_EXPERIMENTAL_CLASS
#define IP_IN_BADCLASS(xxip) ((xxip) == IP_INADDR_BROADCAST)
#else
#define IP_IN_BADCLASS(xxip) IP_IN_EXPERIMENTAL(xxip)
#endif

/*
 *===========================================================================
 *                         setsockopt/getsockopt
 *===========================================================================
 */

#define IP_SOL_SOCKET       0xffff          /* options for socket level */

/***** IP_SOL_SOCKET socket options: *****/
#define IP_SO_REUSEADDR     0x0004  /* int; allow a socket to bind to an address/port that
                                     * is the same as some other local socket in TIME_WAIT state */
#define IP_SO_KEEPALIVE     0x0008  /* int; protocol probes idle connection */
#define IP_SO_DONTROUTE     0x0010  /* int; enable/disable routing bypass for outgoing messages */
#define IP_SO_RCVLOWAT      0x0012  /* int; minimum number of bytes that can be read without
                                       blocking when select report the socket as readable */
#define IP_SO_SNDLOWAT      0x0013  /* int; minimum number of bytes that can be sent without
                                       blocking when select report the socket as writable */
#define IP_SO_ACCEPTCONN    0x001e  /* int; get only: 0 if the socket is listening, 1 if it is */
#define IP_SO_BROADCAST     0x0020  /* int; Allow/disallow sending of packet to the broadcast address (default is enabled) */
#define IP_SO_USELOOPBACK   0x0040  /* int; Disable routing socket receive() (default is enabled) */
#define IP_SO_LINGER        0x0080  /* struct Ip_linger; Set get the linger time */
#define IP_SO_REUSEPORT     0x0200  /* int; allow local address & port reuse */

#define IP_SO_SNDBUF        0x1001  /* int; send buffer size */
#define IP_SO_RCVBUF        0x1002  /* int; receive buffer size */
#define IP_SO_RCVTIMEO      0x1006  /* struct Ip_timeval; receive timeout */
#define IP_SO_ERROR         0x1007  /* int; get error status and clear */
#define IP_SO_TYPE          0x1008  /* int; get socket type */
#define IP_SO_BINDTODEVICE  0x1010  /* char[IP_IFNAMSIZ]; bind to specified device */
#define IP_SO_OOBINLINE     0x1011  /* int; controls if out-of-band data should be mixed with normal
                                       data, default is off -> not mixed (i.e. must use IP_MSG_OOB) */


/***** IP_IPPROTO_IP level socket options: *****/
#define IP_IP_OPTIONS           1   /* char[0..40]; set/get IP options */
#define IP_IP_HDRINCL           2   /* int; IP header is included before user data. */
#define IP_IP_TOS               3   /* int; Type of Service field value in the IP header */
#define IP_IP_TTL               4   /* int; IP time to live */
#define IP_IP_RECVDSTADDR       7   /* int; turn "get IP dst address with dgram" on/off */
#define IP_IP_PKTINFO           8   /* int; turn "get packet information ancillary data" on/off */
#define IP_IP_MULTICAST_IF      9   /* struct Ip_in_addr or struct Ip_ip_mreq or struct Ip_ip_mreqn.
                                       Which one is used is determined by size of argument.
                                       Controls which interface multicast packets
                                       sent from this socket should use */
#define IP_IP_MULTICAST_TTL     10  /* Ip_u8; Time to live for multicast packets (0 = defalt value) */
#define IP_IP_MULTICAST_LOOP    11  /* Ip_u8; Controls if a copy of multicast packets should be sent
                                       to the loopback device */
#define IP_IP_ADD_MEMBERSHIP    12  /* struct Ip_ip_mreq or struct Ip_ip_mreqn;
                                       Which one that is used is determiend by size of argument.
                                       add an IPv4 group membership */
#define	IP_IP_JOIN_GROUP        12
#define IP_IP_DROP_MEMBERSHIP   13  /* struct Ip_ip_mreq or struct Ip_ip_mreqn.
                                       Which one that is used is determiend by size of argument.
                                       drop an IPv4 group membership */
#define	IP_IP_LEAVE_GROUP       13
#define IP_IP_RECVIF            20  /* int; ipcom_recvmsg() returns a struct Ip_sockaddr_dl
                                       corresponding to the interface on which the packet was
                                       received for IP_SOCK_RAW and IP_SOCK_DGRAM. */
#define IP_IP_ROUTER_ALERT      21  /* Ip_u8; != 0 to add router alert to each packet sent from this socket */
#define IP_IP_DONTFRAG          22  /* Ip_u8; != 0 set the DF bit on each packet sent from this socket */
#define IP_IP_RECVTTL           31  /* int; turn "get the time-to-live ancillary data" on/off */
#define IP_IP_RECVTOS           32  /* int; turn "get the type-of-service ancillary data" on/off */
#define IP_UDP_X_CHKSUM         40  /* int; Turn on/off checksum for outgoing UDP packages (default on) */
/* NOTE: 42-47 is used by MCAST_xxx macros */
#define IP_IP_NAT               50  /* Ipnet_nat_ctrl; Network address translation control */
#define IP_IP_FW                51  /* Ipfirewall_ctrl; Firewall control */
#define IP_IP_NEXTHOP           52  /* struct Ip_sockaddr_in; specify the next hop router as ancillary data */
#define IP_IP_X_SENDERLINKADDR  100 /* int; ipcom_recvmsg() returns a struct Ip_sockaddr_dl
                                       that will contain the link address of the sender */
#define IP_IP_X_VRID            101 /* Ip_u8; specify the virtual router ID as ancillary data */


/***** IP_IPPROTO_IP level multicast socket options: *****/
#define IP_MRT_BASE      0x40000000

#define IP_MRT_INIT      (IP_MRT_BASE + 0)    /* int; mrouted is starting */
#define IP_MRT_DONE      (IP_MRT_BASE + 1)    /* int; mrouted is shutting down */
#define IP_MRT_ADD_VIF   (IP_MRT_BASE + 2)    /* struct Ip_vifctl; add virtual interface */
#define IP_MRT_DEL_VIF   (IP_MRT_BASE + 3)    /* struct Ip_vifctl; delete virtual interface */
#define IP_MRT_ADD_MFC   (IP_MRT_BASE + 4)    /* struct Ip_mfcctl; add a multicast forwarding entry */
#define IP_MRT_DEL_MFC   (IP_MRT_BASE + 5)    /* struct Ip_mfcctl; delete a multicast forwarding entry */
#define IP_MRT_VERSION   (IP_MRT_BASE + 6)    /* int; returns the version of 'mrouted' needed */
#define IP_MRT_ASSERT    (IP_MRT_BASE + 7)    /* int; set/get if PIM assert processing is enabled */


/***** IP_IPPROTO_IP & IP_IPPROTO_IPV6 level multicast socket options from RFC3678 *****/
/* Any-Source Multicast API; cannot be mixed with Source-Specific Multicast API */
#define IP_MCAST_JOIN_GROUP         42 /* struct Ip_group_req; join an any-source group */
#define IP_MCAST_BLOCK_SOURCE       43 /* struct Ip_group_source_req; block a given source to a given group */
#define IP_MCAST_UNBLOCK_SOURCE     44 /* struct Ip_group_source_req; unmute a given source to a given group */
#define IP_MCAST_LEAVE_GROUP        45 /* struct Ip_group_req; leave an any-source group */

/* Source-Specific Multicast API; cannot be mixed with Any-Source Multicast API */
#define IP_MCAST_JOIN_SOURCE_GROUP  46 /* struct Ip_group_source_req; join a source-specific group */
#define IP_MCAST_LEAVE_SOURCE_GROUP 47 /* struct Ip_group_source_req; leave a source-specific group */
#define IP_MCAST_LEAVE_GROUP        45 /* struct Ip_group_req; drop all sources which have been
                                          joined for a particular group and interface. */

#define IP_MCAST_EXCLUDE            0  /* Exclude filter mode */
#define IP_MCAST_INCLUDE            1  /* Include filter mode */


/***** IP_IPPROTO_TCP level socket options: *****/
#define IP_TCP_NODELAY              1  /* int; Enable/disable the Nagle algorithm
                                          (default is enabled) */
#define IP_TCP_MAXSEG               2  /* int; Limit MSS */
#define IP_TCP_CORK                 3  /* int; Never send partially complete segments if turned on */
#define IP_TCP_NOPUSH               IP_TCP_CORK
#define IP_TCP_KEEPIDLE             4  /* int; Send first keepalive probe when the connections
                                          been isdl this time (in seconds) */
#define IP_TCP_KEEPINTVL            5  /* int; Interval (in seconds) between keepalives */
#define IP_TCP_KEEPCNT              6  /* int; Maximum number of keepalives before dropping
                                          the connection */
#define IP_TCP_PERSIST_TIMEOUT      7  /* int; Maximum time a TCP session can be kept in
                                               persist state if the peer announces a
                                               zero size receive window. A timeout value
                                               of zero means infinite persist timeout */

#define IP_TCP_X_MD5_SIGNATURE_KEY  16 /* char *; Set MD5 key (length 0 to disable) */
#define IP_TCP_RFC1122_URG          17 /* int; Select between RFC 1122 (!= 0)
                                          and RFC 793 (== 0) urgent data implementation */
#define IP_TCP_TSO_FACTOR           18 /* int; The multiplier that will be used when creating
                                          outgoing TCP segments when the outgoing interface
                                          supports TCP Segmentation Offload. The segments
                                          may contain up to (TSO_FACTOR * MSS) of TCP payload */
#define IP_TCP_X_SEQ                19 /* Ip_u32; Set/get the next sequence number.
                                          Set operation may only be used before the socket
                                          is connected to a peer, i.e. it is only possible
                                          to set the initial sequence number */


/***** IP_IPPROTO_UDP level socket options: *****/
/* Note that UDP socket options and SOCKET options are treated at the same
 * level internally, i.e. they must not have the same values.
 */
#define IP_UDP_NAT_T        0x1012  /* int: Nat traversal (one of IP_UDP_NAT_T_*) */

/* Values for the IP_UDP_NAT_T */
#define IP_UDP_NAT_T_DISABLED               (0)
#define IP_UDP_NAT_T_RFC3519                (1)             /* Mobile IP UDP encapsulation */
#define IP_UDP_NAT_T_RFC3948                (2)             /* IKE/IPSEC UDP encapsulation */
#define IP_UDP_NAT_T_DSMIPV6                (3)             /* Mobile IPv6 UDP encap            */
#define IP_UDP_NAT_T_DSMIPV6_TLV            (4)             /* Mobile IPv6 UDP encap - Internal */

/***** IP_SOL_PACKET level options: *****/
#define IP_SOL_PACKET              263

#define IP_PACKET_ADD_MEMBERSHIP   1  /* struct Ip_packet_mreq; Join a link layer multicast group */
#define IP_PACKET_DROP_MEMBERSHIP  2  /* struct Ip_packet_mreq; Leave a link layer multicast group */


/***** IP_SOL_RAW level socket options *****/
#define IP_SOL_RAW            255

#define IP_ICMP_FILTER         1   /* struct Ip_icmp_filter; set/get the ICMP message filter */


/*
 *===========================================================================
 *                         socket option extensions
 *===========================================================================
 * Interpeak proprietary TCP/IP stack socket option extensions.
 */

/***** IP_SOL_SOCKET socket options: *****/
#define IP_SO_X_VR                0x1020  /* int; Set virtual router */
#define IP_SO_X_PROBEDELAY        0x1021  /* int; number of seconds an connection (with keep-alive
                                           * active) must be idle before the first probe is sent */
#define IP_SO_X_COOKIE            0x1022  /* void *; socket IPCOM port specific cookie */
#define IP_SO_X_EVENTCB_RO        0x1024  /* Ipcom_socket_eventcb(); Read-only socket event callback.
                                           * CB_INPKT event must not modify or consume the packet! */
#define IP_SO_X_PKT_MPRIO         0x1025  /* int; ipcom_pkt_malloc() priority */
#define IP_SO_X_BYPASS_FLOW       0x1026  /* int; Turn on/off bypass flow */
#define IP_SO_X_DRAINABLE         0x1027  /* int; ==0 packets in the receive queue must not be
                                                      freed even in low memory situations (default)
                                                  !=0 the stack might discard already queued packets
                                                      in low memory situations */
#define IP_SO_X_GRE_KEY           0x1028  /* struct Ip_gre_key; set the GRE flow key for traffic
                                                                sent from this socket */
#define IP_SO_X_WINDNET_SO        0x1029  /* void *; WINDNET socket pointer */
#define IP_SO_VLAN                0x8000  /* struct Ip_sovlan; Per socket priority-tag or VLAN-tag */

/*
 *===========================================================================
 *                         ioctl values
 *===========================================================================
 * IOCTL value: <3-bit type> <13-bit size> <4-bit flag> <4-bit group> <8-bit id>
 */

/* IOCTL types: */
#define IP_IOC_VOID  0x20000000
#define IP_IOC_OUT   0x40000000
#define IP_IOC_IN    0x80000000
#define IP_IOC_INOUT (IP_IOC_IN | IP_IOC_OUT)

#define IP_IOC_READ(a) (((0x40 << 24) & (a)) == IP_IOC_OUT)
#define IP_IOC_WRITE(a) (((0x80 << 24) & (a)) == IP_IOC_IN)
#define IP_IOC_RW(a) (((0xc0 << 24) & (a)) == IP_IOC_INOUT)

/* IOCTL size: */
#define IP_IOCPARM_MASK    0x1fff      /* Parameter length 13 bits */
#define IP_IOCPARM_LEN(r)  (IP_IOCPARM_MASK & ((r) >> 16))

/* IOCTL flags. */
#define IP_IOCF_R       0x00000000   /* read. */
#define IP_IOCF_W       0x00001000   /* write bit. */

/* IOCTL groups: */
#define IP_IOC_GROUP(c)  (((c) & 0x00000f00) >> 8)
#define IP_IOCG_BASE        1
#define IP_IOCG_SOCK        2
#define IP_IOCG_NETIF       3
#define IP_IOCG_ARP         4
#define IP_IOCG_INET        5
#define IP_IOCG_INET6       6
#define IP_IOCG_ETH         7
#define IP_IOCG_PPP         8
#define IP_IOCG_WLAN        9
#define IP_IOCG_MCAST       10
#define IP_IOCG_MCAST_IN6   11
#define IP_IOCG_MPLS        12
#define IP_IOCG_RTAB        13
#define IP_IOCG_DS          14
#define IP_IOCG_POLICY_RT   15

#define IP_IOX(g,i,f,t)   (((sizeof(t) & IP_IOCPARM_MASK) << 16) | (IP_IOCF_ ## f) | ((IP_IOCG_ ## g) << 8) | ((i) & 0xff))
#define IP_IO(g,i,f,t)    (IP_IOC_VOID  | /* no parameter */ \
        (((sizeof(t) & IP_IOCPARM_MASK) << 16) | (IP_IOCF_ ## f) | ((IP_IOCG_ ## g) << 8) | ((i) & 0xff)))
#define IP_IOR(g,i,f,t)   (IP_IOC_OUT   | /* out-parameter */ \
        (((sizeof(t) & IP_IOCPARM_MASK) << 16) | (IP_IOCF_ ## f) | ((IP_IOCG_ ## g) << 8) | ((i) & 0xff)))
#define IP_IOW(g,i,f,t)   (IP_IOC_IN    | /* in-parameter */ \
        (((sizeof(t) & IP_IOCPARM_MASK) << 16) | (IP_IOCF_ ## f) | ((IP_IOCG_ ## g) << 8) | ((i) & 0xff)))
#define IP_IOWR(g,i,f,t)  (IP_IOC_INOUT | /* in- and out-parameter */ \
        (((sizeof(t) & IP_IOCPARM_MASK) << 16) | (IP_IOCF_ ## f) | ((IP_IOCG_ ## g) << 8) | ((i) & 0xff)))


/*
 *===========================================================================
 *                         base ioctls
 *===========================================================================
 * IO controls supported by both IPLITE and IPNET.
 */

/* socket ioctls: */
#define IP_SIOCSPGRP        IP_IOW(SOCK, 1, W, Ip_pid_t) /* Set process ID of socket */
#define IP_SIOCGPGRP        IP_IOR(SOCK, 2, R, Ip_pid_t) /* Get process ID of socket */
#define IP_FIONBIO          IP_IOW(SOCK, 3, W, int) /* Enable/disable non-blocking I/O */
#define IP_X_FIOGNBIO       IP_IOR(SOCK, 4, R, int) /* Get non-blocking I/O, 1=non-blocking, 0=blocking */
#define IP_FIONREAD         IP_IOR(SOCK, 5, R, int) /* Get the number of bytes waiting to be read. */
#define IP_SIOCATMARK       IP_IOR(SOCK, 6, R, int) /* Returns 1 when the all urgent data has been already
                                                       received by the user program */
#define IP_SIOCINQ          IP_IOR(SOCK, 7, R, int) /* Returns the number of bytes currently stored in the
                                                       receive queue */
#define IP_SIOCOUTQ         IP_IOR(SOCK, 8, R, int) /* Returns the number of bytes currently stored in the
                                                       send queue */
#define IP_X_SIOCSINTR      IP_IOW(SOCK, 10, W, int) /* Simulation of SIGALARM, all sockets owned by
                                                        the calling process will receive EINTR after
                                                        the specified amount of seconds has passed */
#define IP_X_SIOCGINTR      IP_IOW(SOCK, 11, R, int) /* Clear all pending EINTR on sockets owned by
                                                        the caller */
#define IP_X_SIOCINURG      IP_IOR(SOCK, 12, R, int) /* Returns 1 if the socket is in urgent mode */
#ifdef IP_DEBUG
/* The debug value has no logical effect at all, it is just an integer
   that is stored in the global data segment of the stack. */
#define IP_X_SIOCSDEBUG     IP_IOW(SOCK, 13, W, int) /* Set the current stack debug value */

#define IP_X_SIOCGDEBUG     IP_IOR(SOCK, 13, R, int) /* Get the current stack debug value */
#endif /* IP_DEBUG */

/* AF_INET ioctls: */
#define IP_SIOCGIFADDR      IP_IOWR(INET,  1, R, struct Ip_ifreq) /* Get interface primary address. */
#define IP_SIOCSIFADDR      IP_IOWR(INET,  2, W, struct Ip_ifreq) /* Set interface primary address. */
#define IP_SIOCGIFBRDADDR   IP_IOWR(INET,  3, R, struct Ip_ifreq) /* Get broadcast address. */
#define IP_SIOCSIFBRDADDR   IP_IOWR(INET,  4, R, struct Ip_ifreq) /* Set broadcast address. */
#define IP_SIOCGIFNETMASK   IP_IOWR(INET,  5, R, struct Ip_ifreq) /* Get (sub)net address mask. */
#define IP_SIOCSIFNETMASK   IP_IOWR(INET,  6, W, struct Ip_ifreq) /* Set (sub)net address mask. */
#define IP_SIOCGIFDSTADDR   IP_IOWR(INET,  7, R, struct Ip_ifreq) /* Get point-to-point IPv4 address. */
#define IP_SIOCSIFDSTADDR   IP_IOWR(INET,  8, W, struct Ip_ifreq) /* Set point-to-point IPv4 address. */
#define IP_SIOCAIFADDR      IP_IOWR(INET,  9, W, struct Ip_in_aliasreq) /* Add interface alias/address. */
#define IP_SIOCDIFADDR      IP_IOWR(INET, 10, W, struct Ip_ifreq) /* Delete interface address. */
#define IP_SIOCADDRT        IP_IOWR(INET, 11, W, struct Ip_ortentry) /* Deprecated (BSD 4.3 compat): add route. */
#define IP_SIOCDELRT        IP_IOWR(INET, 12, W, struct Ip_ortentry) /* Deprecated (BSD 4.3 compat): remove route. */
#define IP_SIOCXGETRT       IP_IOWR(INET, 13, R, struct Ip_ortentry) /* Proprietary (BSD 4.3 compat): get route. */

#define IP_SIOCXADDRT       IP_IOWR(BASE, 14, W, struct Ip_rtxmsg)
#define IP_SIOCXDELRT       IP_IOWR(BASE, 15, W, struct Ip_rtxmsg)


/* Proprietary AF_INET MIP ioctls */
#define IP_SIOCAHOMEADDR    IP_IOWR(INET,  14, W, struct Ip_in_aliasreq) /* Add a home address */
#define IP_SIOCMIP4         IP_IOWR(INET,  15, W, struct Ip_iovec)       /* MIP4  */

/* AF_INET6 ioctls: */
#define IP_SIOCAIFADDR_IN6    IP_IOWR(INET6, 1, W, struct Ip_in6_aliasreq) /* Add interface IPv6 address '' */
#define IP_SIOCDIFADDR_IN6    IP_IOWR(INET6, 2, W, struct Ip_in6_ifreq) /* Delete interface IPv6 address. */
#define IP_SIOCGIFDSTADDR_IN6 IP_IOWR(INET6, 3, R, struct Ip_in6_ifreq) /* Get destination IPv6 address. */

/* proprietary AF_INET6 MIP6 ioctl */
#define IP_SIOCGIFPREFIX_IN6  IP_IOWR(INET6, 5, R, struct Ip_in6_ifreq) /* Get interface IPv6 address prefix  */
#define IP_SIOCXGIFADDR_IN6   IP_IOWR(INET6, 6, W, struct Ip_in6_aliasreq) /* Get interface IPv6 address '' */

/* ARP ioctls: */
#define IP_SIOCSARP         IP_IOWR(ARP, 1, W, struct Ip_arpreq) /* Create/modify ARP entry. */
#define IP_SIOCGARP         IP_IOWR(ARP, 2, R, struct Ip_arpreq) /* Get ARP entry. */
#define IP_SIOCDARP         IP_IOWR(ARP, 3, W, struct Ip_arpreq) /* Delete ARP entry */
#define IP_SIOCPARP         IP_IOWR(ARP, 4, W, struct Ip_arpreq) /* Send ARP request to a node */

#define IP_SIOCSARPN        IP_IOWR(ARP, 5, W, struct Ip_arpreqn) /* Create/modify ARP entry. */
#define IP_SIOCGARPN        IP_IOWR(ARP, 6, R, struct Ip_arpreqn) /* Get ARP entry. */
#define IP_SIOCDARPN        IP_IOWR(ARP, 7, W, struct Ip_arpreqn) /* Delete ARP entry */

/* interface ioctls: */
#define IP_SIOCGIFFLAGS     IP_IOWR(NETIF,  1, R, struct Ip_ifreq)  /* Get interface flags. */
#define IP_SIOCSIFFLAGS     IP_IOWR(NETIF,  2, W, struct Ip_ifreq)  /* Set interface flags. */
#define IP_SIOCGIFMTU       IP_IOWR(NETIF,  3, R, struct Ip_ifreq)  /* Get interface MTU. */
#define IP_SIOCSIFMTU       IP_IOWR(NETIF,  4, W, struct Ip_ifreq)  /* Set interface MTU. */
#define IP_SIOCGIFLLADDR    IP_IOWR(NETIF, 13, R, struct Ip_ifreq)  /* Get link level address.  */
#define IP_SIOCSIFLLADDR    IP_IOWR(NETIF, 14, W, struct Ip_ifreq)  /* Set link level address.  */
#define IP_SIOCIFDESTROY    IP_IOWR(NETIF, 18, W, struct Ip_ifreq)  /* Destroy a pseudo interface (e.g. vlan) */
#define IP_SIOCGIFCONF      IP_IOWR(NETIF, 21, W, struct Ip_ifconf) /* Get interface list. */
#define IP_SIOCGIFINDEX     IP_IOWR(NETIF, 22, R, struct Ip_ifreq)  /* Get interface index */
#define IP_SIOCGIFCAP       IP_IOWR(NETIF, 23, R, struct Ip_ifreq)  /* Get interface capabilities */
#define IP_SIOCGIFNAME      IP_IOWR(NETIF, 24, R, struct Ip_ifreq)  /* Get interface name */
#define IP_SIOCGIFTXQLEN    IP_IOWR(NETIF, 25, R, struct Ip_ifreq)  /* Get TX queue length */
#define IP_SIOCSIFTXQLEN    IP_IOWR(NETIF, 26, R, struct Ip_ifreq)  /* Set TX queue length */
#define IP_SIOCSIFNAME      IP_IOWR(NETIF, 27, R, struct Ip_ifreq)  /* Set interface name */
#define IP_SIOCSIFHWBROADCAST   IP_IOWR(NETIF, 28, R, struct Ip_ifreq)  /* Set HW broadcast */
#define IP_SIOCADDMULTI     IP_IOWR(NETIF, 29, R, struct Ip_sockaddr)
#define IP_SIOCDELMULTI     IP_IOWR(NETIF, 30, R, struct Ip_sockaddr)
/* Linux style HW ioctl's */
#define IP_SIOCGIFHWADDR    IP_IOWR(NETIF, 31, R, struct Ip_ifreq)  /* Set interface name */

/* Linux style tunnel ioctl's */
#define IP_X_SIOCDELTUNNEL  IP_IOWR(NETIF, 40, R, struct Ip_ifreq)  /* Del tunnel */
#define IP_X_SIOCADDTUNNEL  IP_IOWR(NETIF, 41, R, struct Ip_ifreq)  /* Add tunnel */
#define IP_X_SIOCGETTUNNEL  IP_IOWR(NETIF, 42, R, struct Ip_ifreq)  /* Get tunnel */
#define IP_X_SIOCCHGTUNNEL  IP_IOWR(NETIF, 43, R, struct Ip_ifreq)  /* CHG tunnel */

/* proprietary ioctls: */
#define IP_SIOCXDETACH       IP_IOWR(NETIF, 50, W, struct Ip_ifreq) /* Detach interface. */
#define IP_SIOCXGDHCPRUNNING IP_IOWR(NETIF, 52, W, struct Ip_ifreq) /* Get DHCP status. */
#define IP_SIOCXSDHCPRUNNING IP_IOWR(NETIF, 53, W, struct Ip_ifreq) /* Enable(1)/Disable(0) DHCP. */
#define IP_SIOCGIFPRIVATE    IP_IOWR(NETIF, 54, W, struct Ip_ifreq) /* Get private interface data */
#define IP_SIOCSIFPRIVATE    IP_IOWR(NETIF, 55, W, struct Ip_ifreq) /* Set private interface data */

/* IP Forwarding */
#define IP_SIOCGHWSTATS         IP_IOR(NETIF, 56, R, struct Ip_fwreq)
#define IP_SIOCSRESETHWSTATS    IP_IOW(NETIF, 57, W, struct Ip_fwreq)


typedef int (*Ipcom_if_eventcb)(char *ifname, Ip_u32 ifindex, Ip_u32 ifflags, int event, void *data);
#define IP_SIOCXSIFFEVENTCB  IP_IOW(BASE,   74, W, Ipcom_if_eventcb) /* Set interface callback. */
#define IP_SIOCXDIFFEVENTCB  IP_IOW(BASE,   75, W, Ipcom_if_eventcb) /* Delete interface callback. */
#define IP_SIOCXGETGW_IN6    IP_IOWR(BASE,  53, R, struct Ip_sockaddr_in6) /* Proprietary : get IPv6 gateway. */
#define IP_SIOCXSETGW_IN6    IP_IOWR(BASE,  54, W, struct Ip_sockaddr_in6) /* Proprietaty : set IPv6 gateway. */

/* IPSEC ioctls (NOTE: see ipipsec.h for additional ioctls (BASE #80-86)! */
#define IP_SIOCXIPSEC_CTL       IP_IOWR(BASE, 55, W, void *)
#define IP_SIOCXIPSEC_SA_CTL    IP_IOWR(BASE, 56, W, void *[2])
#define IP_SIOCXIPSEC_CONF_CTL  IP_IOWR(BASE, 57, W, void *)


/*
 *===========================================================================
 *                         IPNET only ioctls
 *===========================================================================
 * IO controls supported by IPNET only.
 */

#if IPCOM_USE_SOCK == IPCOM_SOCK_IPNET || IPCOM_USE_SOCK == IPCOM_SOCK_IPCOM || defined(IP_PORT_LKM)  || defined(IP_PORT_LAS)

/* socket ioctls: */

/* Multicast ioctls: */
#define IP_SIOCGETVIFCNT    IP_IOWR(MCAST,  0, R, struct Ip_sioc_vif_req) /* get VIF statistics */
#define IP_SIOCGETSGCNT     IP_IOWR(MCAST,  1, R, struct Ip_sioc_sg_req)  /* get mcast route statistics */

/* AF_INET6 multicast ioctls: */
#define IP_SIOCGETMIFCNT_IN6 IP_IOWR(MCAST_IN6,  0, R, struct Ip_sioc_mif_req6) /* get MIF statistics */
#define IP_SIOCGETSGCNT_IN6  IP_IOWR(MCAST_IN6,  1, R, struct Ip_sioc_sg_req6)  /* get mcast6 route statistics */

/* route table ioctls */
#define IP_SIOCADDVR        IP_IOWR(RTAB, 0, W, Ip_u16) /* Add a new virtual router and creates a route table with table ID == 0 */
#define IP_SIOCDELVR        IP_IOWR(RTAB, 1, W, Ip_u16) /* Delete a virtual router and all tables owned by it */
#define IP_SIOCADDROUTETAB  IP_IOWR(RTAB, 2, W, struct Ip_sioc_route_table) /* Add a route table to a (virtual) router */
#define IP_SIOCDELROUTETAB  IP_IOWR(RTAB, 3, W, struct Ip_sioc_route_table) /* Delete a route table to a (virtual) router */
#define IP_SIOCGETROUTETAB  IP_IOWR(RTAB, 4, W, struct Ip_sioc_route_table) /* Get/create a route table by name */
#define IP_SIOCSROUTETABNAME IP_IOWR(RTAB, 5, W, struct Ip_sioc_route_table) /* Set a name for a route table. */
#define IP_SIOCGROUTETABNAME IP_IOWR(RTAB, 6, W, struct Ip_sioc_route_table) /* Mapps a route table name to VR and table ID */

/* interface ioctls: */
#define IP_SIOCGIFMETRIC    IP_IOWR(NETIF,  5, R, struct Ip_ifreq)  /* Get interface metric. */
#define IP_SIOCSIFMETRIC    IP_IOWR(NETIF,  6, W, struct Ip_ifreq)  /* Set interface metric. */
#define IP_SIOCGIFVR        IP_IOWR(NETIF,  7, R, struct Ip_ifreq)  /* Get the route table index for an interface. */
#define IP_SIOCSIFVR        IP_IOWR(NETIF,  8, W, struct Ip_ifreq)  /* Set an interface to a specific route table. */
#define IP_SIOCGETTUNNEL    IP_IOWR(NETIF, 10, R, struct Ip_ip_tunnel_param) /* Get tunnel parameter. */
#define IP_SIOCCHGTUNNEL    IP_IOWR(NETIF, 12, W, struct Ip_ip_tunnel_param) /* Change tunnel parameters. */
#define IP_SIOCGETVLAN      IP_IOWR(NETIF, 15, R, struct Ip_ifreq)  /* Get information about a virtual LAN interface,
                                                                     * 'ifru_data' must point to a struct Ip_vlanreq */
#define IP_SIOCSETVLAN      IP_IOWR(NETIF, 16, W, struct Ip_ifreq)  /* Set information about a virtual LAN interface,*
                                                                     'ifru_data' must point to a struct Ip_vlanreq */
#define IP_SIOCIFCREATE     IP_IOWR(NETIF, 17, W, struct Ip_ifreq)   /* Create a pseudo interface (like vlan) */
#define IP_SIOCSETVLANPRI   IP_IOWR(NETIF, 19, W, struct Ip_ifreq)  /* Set vlan priority for a virtual LAN interface,*
                                                                     'ifru_data' must point to a struct Ip_vlanreq */


/* proprietary ioctls: */
#define IP_SIOCXRESETSTAT   IP_IOWR(NETIF, 60, W, struct Ip_ifreq)   /* Reset interface statistics counter */
#define IP_SIOCXPCAP        IP_IOWR(NETIF, 61, W, struct Ip_ifreq)   /* Packet capture control */

/* proprietary ROHC ioctls */
#define IP_SIOCSROHCSTATUS  IP_IOWR(BASE, 58, W, struct Ip_rohcreq) /* Open/close ROHC */
#define IP_SIOCROHCADDIF    IP_IOWR(BASE, 59, R, struct Ip_rohcreq)  /* Add a monitoring interface */
#define IP_SIOCROHCDELIF    IP_IOWR(BASE, 60, R, struct Ip_rohcreq)  /* Delete a monitoring interface */
#define IP_SIOCROHCADDIFPROFILE    IP_IOWR(BASE, 61, R, struct Ip_rohcreq)  /* Add a profile on a monitoring interface */
#define IP_SIOCROHCDELIFPROFILE    IP_IOWR(BASE, 62, R, struct Ip_rohcreq)  /* Delete a profile on a monitoring interface */
#define IP_SIOCSROHCCID     IP_IOWR(BASE, 63, W, struct Ip_rohcreq) /* Set CID */

#endif /* IPCOM_USE_SOCK == IPCOM_SOCK_IPNET || defined(IPCOM_SOCK_IPCOM) || defined(IP_PORT_LKM) */


/*
 *===========================================================================
 *                         IP_IFF_X
 *===========================================================================
 * Supported interface flags used with IP_SIOC<S|G>IFFLAGS ioctl().
 */
#define IP_IFF_UP            0x0001  /* Interface is up. */
#define IP_IFF_BROADCAST     0x0002  /* Broadcast address valid <read only>. */
#define IP_IFF_DEBUG         0x0004  /* Unsupported! */
#define IP_IFF_LOOPBACK      0x0008  /* Loopback net <read only>. */
#define IP_IFF_POINTOPOINT   0x0010  /* Interface is point-to-point link <read only>. */
#define IP_IFF_X_DHCPRUNNING 0x0020  /* Internal IFF flag */
#define IP_IFF_RUNNING       0x0040  /* Driver is opened/running <read only>. */
#define IP_IFF_NOARP         0x0080  /* No ARP or NDP will be performed by this interface */
#define IP_IFF_PROMISC       0x0100  /* Receive all packets. */
#define IP_IFF_ALLMULTI      0x0200  /* Unsupported! */
#define IP_IFF_OACTIVE       0x0400  /* Unsupported! */
#define IP_IFF_SIMPLEX       0x0800  /* Interface cannot hear own transmissions <read only>. */
#define IP_IFF_LINK0         0x1000  /* Link layer specific flag 0 */
#define IP_IFF_LINK1         0x2000  /* Link layer specific flag 1 */
#define IP_IFF_LINK2         0x4000  /* Link layer specific flag 2 */
#define IP_IFF_MULTICAST     0x8000  /* Supports multicast <read only>. */


/*
 *===========================================================================
 *                    ipcom_shutdown() argument flags
 *===========================================================================
 */
#define IP_SHUT_RD      0x0     /* further receives will be disallowed */
#define IP_SHUT_WR      0x1     /* further sends will be disallowed */
#define IP_SHUT_RDWR    0x2     /* further sends and receives will be disallowed */


/*
 *===========================================================================
 *                    IPv4 options
 *===========================================================================
 * used when communicating with the MIP subsystem
 */
#define IP_IPOPT_COPY        0x80 /* Set if the options must be copied to every fragment */
#define IP_IPOPT_CLASS_MASK  0x60 /* Mask for the IP option class */
#define IP_IPOPT_NUMBER_MASK 0x1f /* Mask for the IP option number */

#define IP_IPOPT_COPIED(opt) ((opt) & IP_IPOPT_COPY)        /* Option copy bit */
#define IP_IPOPT_CLASS(opt)  ((opt) & IP_IPOPT_CLASS_MASK)  /* Option class */
#define IP_IPOPT_NUMBER(opt) ((opt) & IP_IPOPT_NUMBER_MASK) /* Option number */

/* Option classes */
#define IP_IPOPT_CONTROL     0x00
#define IP_IPOPT_MEASUREMENT 0x40

/* Option types */
#define IP_IPOPT_END       (0  | IP_IPOPT_CONTROL)     /* End of options */
#define IP_IPOPT_NOOP      (1  | IP_IPOPT_CONTROL)     /* Skip 1 (this) byte */
#define IP_IPOPT_LSRR      (3  | IP_IPOPT_CONTROL)     /* Loose source route */
#define IP_IPOPT_TIMESTAMP (4  | IP_IPOPT_MEASUREMENT) /* Timestamp */
#define IP_IPOPT_RR        (7  | IP_IPOPT_CONTROL)     /* Record route */
#define IP_IPOPT_SSRR      (9  | IP_IPOPT_CONTROL)     /* Strict source route */
#define IP_IPOPT_RA        (20 | IP_IPOPT_CONTROL | IP_IPOPT_COPY) /* Router alert */

/* Timestamp option flags */
#define IP_IPOPT_TS_TSONLY    0  /* Timestamps only */
#define IP_IPOPT_TS_TSANDADDR 1  /* IP address and timestamps */
#define IP_IPOPT_TS_PRESPEC   3  /* Timestamp for prespecified IP addresses */



/* Ethernet types */

#define IP_ETH_P_ALL            0
#define IP_ETH_P_IP             0x0800
#define IP_ETH_P_ARP            0x0806
#define IP_ETH_P_RARP           0x8035
#define IP_ETH_P_IPV6           0x86DD
#define IP_ETH_P_PPPOE_DIS      0x8863
#define IP_ETH_P_PPPOE_SES      0x8864
#define IP_ETH_P_8021Q_VLAN     0x8100
#define IP_ETH_P_MPLS_UNICAST   0x8847
#define IP_ETH_P_MPLS_MULTICAST 0x8848

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    Ip_sioc_route_table
 *===========================================================================
 * used when adding new route tables to an existing (virtual) router.
 */
struct Ip_sioc_route_table
{
    Ip_u16   vr;    /* (Virtual) router index, 0 == default, IPCOM_VR_ANY
                       can be used to let the stack choose a virtual router ID */
    Ip_u32   table; /* The ID of this table. Table 0 is always present and
                       cannot be deleted. Setting table to IPCOM_ROUTE_TABLE_ANY
                       in an add operation will result in the stack choosing a
                       free ID */
    char     name[IPCOM_RTNAMSIZ]; /* The (optional) name of the route table */
};

/* This VR ID tells the stack to choose an unused ID and also choose the
   default route table ID */
#define IPCOM_VR_ANY               0xffff
/* This table ID tells the stack to choose an unused ID on the specified VR */
#define IPCOM_ROUTE_TABLE_ANY      0xffffffff

/* Unspecified table ID */
#define IPCOM_ROUTE_TABLE_UNSPEC   0

/* The ID of the table (also called FIB) that is created automatically
   on each VR, is always preset and is selected by default */
#define IPCOM_ROUTE_TABLE_DEFAULT  254

/* Deal with route table comparisons */
#define IPCOM_ROUTE_TABLE_GET(t)            ((t) == IPCOM_ROUTE_TABLE_UNSPEC? IPCOM_ROUTE_TABLE_DEFAULT : (t))
#define IPCOM_ROUTE_TABLE_IS_EQUAL(t1,t2)   (IPCOM_ROUTE_TABLE_GET(t1) == IPCOM_ROUTE_TABLE_GET(t2))
#define IPCOM_ROUTE_TABLE_IS_DEFAULT(t)     (IPCOM_ROUTE_TABLE_IS_EQUAL(t, IPCOM_ROUTE_TABLE_DEFAULT))
#define IPCOM_ROUTE_TABLE_IS_ANY(t)         (IPCOM_ROUTE_TABLE_IS_EQUAL(t, IPCOM_ROUTE_TABLE_ANY))


/*
 *===========================================================================
 *                    IP types
 *===========================================================================
 */

typedef Ip_u32    Ip_in_addr_t;      /* base type for internet address */
typedef Ip_u16    Ip_in_port_t;      /* IP port type */
typedef Ip_u32    Ip_socklen_t;      /* length type for network syscalls */

#ifdef IPCOM_USE_SA_LEN
typedef Ip_u8     Ip_sa_family_t;    /* sockaddr address family type */
#else
typedef Ip_u16    Ip_sa_family_t;    /* sockaddr address family type */
#endif

#ifdef IPCOM_USE_SA_LEN
#define IPCOM_SA_LEN_GET(sa)        (((struct Ip_sockaddr *)(sa))->sa_len)
#define IPCOM_SA_LEN_SET(sa, len)   (((struct Ip_sockaddr *)(sa))->sa_len = (Ip_u8)(len))
#else
#define IPCOM_SA_LEN_SET(sa, len)   IP_NOOP
#define IPCOM_SA_LEN_GET(sa)        ipcom_sa_get_len((const struct Ip_sockaddr *)sa)
#endif
/*
 *===========================================================================
 *                    Ip_iovec
 *===========================================================================
 * used for multiple entry output.
 */
struct Ip_iovec
{
    void      *iov_base;  /* Starting address.  */
    Ip_size_t  iov_len;   /* Length in bytes.  */
};


#ifdef IPCOM_USE_INET6
#include "ipcom_sock6.h"
#endif

/*
 *===========================================================================
 *                    Ip_in_addr
 *===========================================================================
 */
#ifdef s_addr
#undef s_addr
#endif

#include "ipcom_align16.h"
IP_PACKED1 struct Ip_in_addr
{
    Ip_u32   s_addr;    /* 32 bit IPv4 address in network byte order */
}
IP_PACKED2;


/*
 *===========================================================================
 *                    Ip_sockaddr
 *===========================================================================
 */
IP_PACKED1 struct Ip_sockaddr
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8             sa_len;
#endif
    Ip_sa_family_t    sa_family;    /* address family: AF_xxx value */
    char              sa_data[14];  /* protocol-specific address */
}
IP_PACKED2;


/*
 *===========================================================================
 *                    Ip_sockaddr_in
 *===========================================================================
 */
IP_PACKED1 struct Ip_sockaddr_in
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8             sin_len;
#endif
    Ip_sa_family_t    sin_family;   /* IP_AF_INET */
    Ip_in_port_t      sin_port;     /* 16 bit TCP/UDP port in network endian */

    struct Ip_in_addr sin_addr;

    char              sin_zero[8];  /* unused */
}
IP_PACKED2;


/*
 *===========================================================================
 *                    Ip_sockaddr_dl
 *===========================================================================
 */
IP_PACKED1 struct Ip_sockaddr_dl
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8             sdl_len;            /* Length of this struct */
#endif
    Ip_sa_family_t    sdl_family;         /* IP_AF_LINK (8bits) */
    Ip_u16            sdl_index;          /* System assigned index */
    Ip_u8             sdl_type;           /* IP_IFT_xxx */
    Ip_u8             sdl_nlen;           /* Name length, starting in sdl_data[0] */
    Ip_u8             sdl_alen;           /* Link-layer address length */
    Ip_u8             sdl_slen;           /* Link-layer selector len */
    char              sdl_data[12];       /* Minimum work area, can be larger
                                             contains if name and link-layer address */
}
IP_PACKED2;

#define IP_SOCKADDR_DL_LLADDR(sa) (&((struct Ip_sockaddr_dl*) sa)->sdl_data[((struct Ip_sockaddr_dl *)sa)->sdl_nlen])
#define IP_SOCKADDR_DL_STORE_NAME(dl,name,len) \
    do { \
        if (len) { \
            ipcom_strncpy((dl)->sdl_data, (name), (len)); \
            (dl)->sdl_nlen = (Ip_u8)(len); \
            if ((len) & 1) { \
                (dl)->sdl_data[len] = '\0'; \
                (dl)->sdl_nlen++; \
            } \
        } else { \
            (dl)->sdl_nlen = 0; \
        } \
    } while (0);

/*
 *===========================================================================
 *                    Ip_sockaddr_ll
 *===========================================================================
 */
IP_PACKED1 struct Ip_sockaddr_ll
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8           sll_len;        /* Length of this structure */
#endif
    Ip_sa_family_t  sll_family;     /* Always IP_AF_PACKET */
    Ip_u16          sll_protocol;   /* Link layer protocol (frame type), not used when sending frames on
                                       an IP_SOCK_RAW type socket */
    Ip_u32          sll_ifindex;    /* Interface index */
    Ip_u16          sll_hatype;     /* Link layer type (IP_IFT_xxx), only used for received frames */
    Ip_u8           sll_pkttype;    /* Packet type (IP_PACKET_xxx), only used for received frames */
    Ip_u8           sll_halen;      /* Length of link layer address */
    Ip_u8           sll_addr[8];    /* Link layer address */
}
IP_PACKED2;
#include "ipcom_align16.h"

/* Packet types 'sll_pkttype'.    Packet is addressed to: */
#define IP_PACKET_HOST       1    /* this host */
#define IP_PACKET_BROADCAST  2    /* all hosts on this link */
#define IP_PACKET_MULTICAST  3    /* a multicast group this host is a member of */
#define IP_PACKET_OTHERHOST  4    /* someone else */
#define IP_PACKET_OUTGOING   5    /* Packet was sent and read back */


/*
 *===========================================================================
 *                         Ip_in_addr_union
 *===========================================================================
 */
union Ip_in_addr_union
{
    struct Ip_in_addr    in;
#ifdef IPCOM_USE_INET6
    struct Ip_in6_addr   in6;
#endif
};


/*
 *===========================================================================
 *                         Ip_sockaddr_union
 *===========================================================================
 */
union Ip_sockaddr_union
{
    struct Ip_sockaddr         sa;
    struct Ip_sockaddr_in      sin;
#ifdef IPCOM_USE_INET6
    struct Ip_sockaddr_in6     sin6;
#endif
};


/*
 *===========================================================================
 *                         Ip_sockaddr_storage
 *===========================================================================
 * Protocol-independent placeholder for socket addresses [RFC3493].
 */

#if defined(IP_PORT_LKM) || defined(IP_PORT_LAS) || defined(IP_PORT_UNIX)
/* Make sure sizeof(Ip_sockaddr_storage) == sizeof(struct sockaddr_storage),
 * where struct sockaddr_storage refers to the definition used by Linux.
 */
#define IP_SS_MAXSIZE     128
#else
#define IP_SS_MAXSIZE     32
#endif
#define IP_SS_ALIGNSIZE   sizeof(Ip_u32)
#define IP_SS_PAD1SIZE    (IP_SS_ALIGNSIZE - sizeof(Ip_u8) - sizeof(Ip_sa_family_t))
#define IP_SS_PAD2SIZE    (IP_SS_MAXSIZE - sizeof(Ip_u8) - sizeof(Ip_sa_family_t) \
                           - IP_SS_PAD1SIZE - IP_SS_ALIGNSIZE)
struct Ip_sockaddr_storage
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8             sa_len;
#endif
    Ip_sa_family_t    sa_family;    /* address family: AF_xxx value */
    char            __ss_pad1[IP_SS_PAD1SIZE];
    Ip_u32          __ss_align;     /* force desired structure storage alignment */
    char            __ss_pad2[IP_SS_PAD2SIZE];
};


/*
 *===========================================================================
 *                    Ip_linger
 *===========================================================================
 */
struct Ip_linger
{
    int   l_onoff;    /* Option on/off */
    int   l_linger;   /* Linger time in seconds */
};


/*
 *===========================================================================
 *                    Ip_gre_key
 *===========================================================================
 */
struct Ip_gre_key
{
    int    gk_onoff;  /* Option on/off */
    Ip_u32 gk_key;    /* RFC 2890 GRE key in host byte order */
};


/*
 *===========================================================================
 *                    Ip_sovlan
 *===========================================================================
 */
struct Ip_sovlan
{
    /*
     * If so_onff is set, the vlan id and/or user priority will be copied
     * to the socket structure and IP_SO_VLAN so_option will be set. If so_onff
     * is not set, the IP_SO_VLAN so_option for the given socket will be cleared.
     */
    int            vlan_onoff;  /* on/off option */

    /*
     * The priority_tagged boolean must be set to true if application using
     * socket-based vlan requires to egress 802.1P priority-tagged frame
     * (i.e. the value of vid is zero). Defaults to false. If set to true,
     * the value specified by the vid will be ignored.
     */
    Ip_bool        priority_tagged;

    unsigned short vid;         /* VLAN ID, valid vid: 1..4094 */
    unsigned short upriority;   /* User Priority, valid priority: 0..7 */
};


/*
 *===========================================================================
 *                    Ip_timeval
 *===========================================================================
 */
#ifndef IP_TIMEVAL_TYPE
#define IP_TIMEVAL_TYPE
struct Ip_timeval
{
    long   tv_sec;         /* Seconds. */
    long   tv_usec;        /* Microseconds. */
};
#endif


/*
 *===========================================================================
 *                    Ip_msghdr
 *===========================================================================
 */
struct Ip_msghdr
{
    void            *msg_name;          /* optional address */
    int              msg_namelen;        /* size of address */
    struct Ip_iovec *msg_iov;           /* scatter/gather array */
    Ip_size_t       msg_iovlen;         /* # elements in msg_iov */

    void            *msg_control;       /* ancillary data */
    Ip_size_t       msg_controllen;     /* ancillary data buffer len */
    int             msg_flags;          /* flags on received message */
};

/* msg_flags values */
#ifndef IP_MSG_TRUNC
#define IP_MSG_TRUNC    0x0001  /* Is set if the packet was truncated */
#endif

#ifndef IP_MSG_CTRUNC
#define IP_MSG_CTRUNC   0x0002  /* Is set if the ancillary data was truncated */
#endif

/*
 *===========================================================================
 *                    Ip_cmsghdr
 *===========================================================================
 */
struct Ip_cmsghdr
{
    Ip_size_t       cmsg_len;    /* Length in bytes, including this structure */
    int             cmsg_level;  /* originating protocol */
    int             cmsg_type;   /* protocol-specific type */
    /* followed by Ip_u8 cmsg_data[] */
};

/* The alignment size for each "struct Ip_cmsghdr" */
#ifdef IP_PORT_OSE5
#define IP_CMSG_ALIGN_SIZE (2 * sizeof(Ip_u32))
#else
#define IP_CMSG_ALIGN_SIZE (sizeof(Ip_ptrdiff_t))
#endif

#define IP_CMSG_ALIGN(len) \
    ((Ip_size_t) (((len) + IP_CMSG_ALIGN_SIZE - 1) & ~(IP_CMSG_ALIGN_SIZE - 1)))

/* Returns a pointer to the first ancillary data */
#define IP_CMSG_FIRSTHDR(mhdrptr) \
    ((mhdrptr)->msg_controllen == 0 ? IP_NULL : (mhdrptr)->msg_control)

/* Returns a pointer to the next ancillary data */
IP_PUBLIC struct Ip_cmsghdr *ipcom_cmsg_nxthdr(IP_CONST struct Ip_msghdr *msgp, struct Ip_cmsghdr *cmsgp);
#define IP_CMSG_NXTHDR(mhdrptr, cmsgptr)  ipcom_cmsg_nxthdr(mhdrptr, cmsgptr)

/* Returns a pointer to the ancillary data */
#define IP_CMSG_DATA(cmsgptr) \
    ((void *) ((Ip_u8 *) (cmsgptr) + IP_CMSG_ALIGN(sizeof(struct Ip_cmsghdr))))

/* Size of header, data and padding */
#define IP_CMSG_SPACE(len) \
    (IP_CMSG_ALIGN(sizeof(struct Ip_cmsghdr)) + IP_CMSG_ALIGN(len))

/* Length to the end of the ancillary data */
#define IP_CMSG_LEN(len) \
    (IP_CMSG_ALIGN(sizeof(struct Ip_cmsghdr)) + len)


/*
 *===========================================================================
 *                    Ip_if_nameindex
 *===========================================================================
 * ipcom_if_nameindex() function structure.
 */
struct Ip_if_nameindex
{
    Ip_u32          if_index;   /* 1,2 ... */
    IP_CONST char  *if_name;    /* null terminated if name: "eth0", "lo0" */
};


/*
 *===========================================================================
 *                    Ip_ip_mreq - setsockopt
 *===========================================================================
 * IPv4 multicast socket options argument structure for IP_ADD_MEMBERSHIP
 * and IP_DROP_MEMBERSHIP ipcom_setsockopt() ioctl options.
 */
struct Ip_ip_mreq
{
    struct  Ip_in_addr   imr_multiaddr;   /* IPv4 class D multicast addr */
    struct  Ip_in_addr   imr_interface;   /* IPv4 address of local interface */
};


/*
 *===========================================================================
 *                    Ip_ip_mreqn - setsockopt
 *===========================================================================
 * IPv4 multicast socket options argument structure for IP_ADD_MEMBERSHIP
 * and IP_DROP_MEMBERSHIP ipcom_getsockopt() ioctl options.
 */
struct Ip_ip_mreqn
{
    struct  Ip_in_addr   imr_multiaddr;   /* IPv4 class D multicast addr */
    struct  Ip_in_addr   imr_address;     /* IPv4 address of local interface */
    Ip_u32               imr_ifindex;     /* Interface index of local interface */
};


/*
 *===========================================================================
 *                    Ip_group_req - setsockopt
 *===========================================================================
 * IPv4 multicast socket options argument structure for IP_MCAST_JOIN_GROUP
 * and IP_MCAST_LEAVE_GROUP ipcom_setsockopt() ioctl options.
 */
struct Ip_group_req
{
    Ip_u32                     gr_interface; /* interface index */
    struct Ip_sockaddr_storage gr_group;     /* group address */
};


/*
 *===========================================================================
 *                    Ip_group_source_req - setsockopt
 *===========================================================================
 * IPv4/IPv6 multicast socket options argument structure for
 * IP_MCAST_BLOCK_SOURCE, IP_MCAST_UNBLOCK_SOURCE, IP_MCAST_JOIN_SOURCE_GROUP
 *  and IP_MCAST_LEAVE_SOURCE_GROUP ipcom_setsockopt() ioctl options.
 */
struct Ip_group_source_req
{
    Ip_u32                     gsr_interface; /* interface index */
    struct Ip_sockaddr_storage gsr_group;     /* group address */
    struct Ip_sockaddr_storage gsr_source;    /* source address */
};


/*
 *===========================================================================
 *                    Ip_arpreq - ioctl
 *===========================================================================
 * Structure for ipcom_socketioctl() IP_SIOCXARP ioctls.
 */
struct Ip_arpreq
{
    struct Ip_sockaddr      arp_pa;     /* protocol address */
    struct Ip_sockaddr_dl   arp_ha;     /* hardware address */
    int                     arp_flags;  /* flags */
};


/*
 *===========================================================================
 *                    Ip_arpreqn - ioctl
 *===========================================================================
 * Structure for ipcom_socketioctl() IP_SIOCXARPN ioctls.
 */
struct Ip_arpreqn {
    struct Ip_sockaddr arp_pa;	             /* protocol address	*/
    struct Ip_sockaddr arp_ha;	             /* hardware address	*/
    int			       arp_flags;             /* flags			*/
    struct Ip_sockaddr arp_netmask;           /* netmask (only for proxy arps) */
    char			   arp_dev[IP_IFNAMSIZ]; /* Interface name */
};


/* ARP Flag values used for both Ip_arpreq and Ip_arpreqn */
#define IP_ATF_COM          0x02        /* completed entry (ha valid) */
#define IP_ATF_PERM         0x04        /* permanent entry */
#define IP_ATF_PUBL         0x08        /* publish entry */
#define IP_ATF_GRAT         0x100       /* send gratuitous ARP request */
#define IP_ATF_BLACKHOLE    0x200       /* set blackhole flag on entry */


/*
 *===========================================================================
 *                    Ip_ortentry - ioctl
 *===========================================================================
 * Following structure necessary for 4.3 compatibility;
 */
struct Ip_ortentry
{
    struct  Ip_sockaddr rt_dst;     /* key */
    struct  Ip_sockaddr rt_gateway; /* value */
    Ip_u16  rt_flags;               /* up/down?, host/net */
};



struct Ip_rtxmsg
{
	struct Ip_sockaddr	rt_dst;		         /* target address		*/
	struct Ip_sockaddr	rt_gateway;	         /* gateway addr (RTF_GATEWAY)	*/
	unsigned            rt_dst_prefixlen;    /* target network prefixlen	*/
	Ip_u16	            rt_flags;
    Ip_u16              rt_metric;
	Ip_u32              rt_ifindex;
	Ip_u32              rt_mtu;
	Ip_u32              rt_window;
	Ip_u16              rt_irtt;
};

/*
 *===========================================================================
 *                    Ip_rohcreq - ioctl
 *===========================================================================
 * Structure for ROHC ioctls.
 */
struct Ip_rohcreq
{
    Ip_bool  opened;                /* open/close ROHC */
    char     ifname[IP_IFNAMSIZ];   /* interface name */
    Ip_bool  have_ifnum;            /* whether have ifnum specified */
    Ip_u8    ifnum;		    /* interface number */
    Ip_u8    profileid;             /* profile id */
    Ip_u16   cid;                   /* context id */
};


/*
 *===========================================================================
 *                    Ip_in_pktinfo
 *===========================================================================
 * Structure for ipcom_socketioctl() IP_PKTINFO ioctl.
 */
struct Ip_in_pktinfo
{
    Ip_u32              ipi_ifindex;    /* send/recv interface index */
#if defined(IP_PORT_LKM) || defined(IP_PORT_LAS)
    struct Ip_in_addr   ipi_spec_dest;  /* routing destination address */
#endif
    struct Ip_in_addr   ipi_addr;       /* local src/dst IPv4 address of sent/recv packet */
};


/*
 *===========================================================================
 *                    Ip_hostent
 *===========================================================================
 */
struct Ip_hostent
{
    char    *h_name;        /* official name of host */
    char   **h_aliases;     /* alias list */
    int      h_addrtype;    /* host address type */
    int      h_length;      /* length of address */
    char   **h_addr_list;   /* list of addresses from name server */
#define h_addr  h_addr_list[0]  /* address, for backward compatiblity */
};


/*
 *===========================================================================
 *                    IPv4 multicast routing
 *===========================================================================
 */
typedef unsigned short Ip_vifi_t; /* Virtual interface index type */

#define IP_MAXVIFS   32  /* Maximum number of virtual interfaces that
                               can be installed */

/* Structure used by IP_SIOCGETVIFCNT ioctl.
 * Hold statistics for the virtual interface
 */
struct Ip_sioc_vif_req
{
    Ip_vifi_t    vifi;   /* Virtual interface index */
    Ip_u32       icount; /* Number of packets received on this interface */
    Ip_u32       ocount; /* Number of packets sent on this interface */
    Ip_u32       ibytes; /* Number of bytes received on this interface */
    Ip_u32       obytes; /* Number of bytes sent on this interface */
};

/* Structure used by IP_SIOCGETSGCNT ioctl
 * Holds statistsic for the route matching 'src' and 'grp'
 */
struct Ip_sioc_sg_req
{
    struct Ip_in_addr   src;        /* The source address */
    struct Ip_in_addr   grp;        /* The destination group address */
    Ip_u32              pktcnt;     /* Packets sent along this route */
    Ip_u32              bytecnt;    /* Bytes sent along this route */
    Ip_u32              wrong_if;   /* Number of packet received on the wrong VIF matching this route */
};

/* Virtual interface control structure, used to add/remove
   virtual interfaces used for multicast forwarding */
struct Ip_vifctl
{
    Ip_vifi_t           vifc_vifi;          /* Index of VIF */
    Ip_u8               vifc_flags;         /* IP_VIFF_xxx flags */
    Ip_u8               vifc_threshold;     /* ttl limit */
    Ip_u32              vifc_rate_limit;    /* Rate limiter values */
    struct Ip_in_addr   vifc_lcl_addr;      /* Local address */
    struct Ip_in_addr   vifc_rmt_addr;      /* Tunnel endpoint address,
                                               only used if IP_VIFF_TUNNEL */
};

#define IP_VIFF_TUNNEL       0x1     /* The VIF is a tunnel */
#define IP_VIFF_REGISTER     0x4     /* Receive PIM register message on this pseudo interface */

/* Multicast route control structure, used to add/remove
   multicast routes */
struct Ip_mfcctl
{
    struct Ip_in_addr   mfcc_origin;        /* The sender of the multicast packet */
    struct Ip_in_addr   mfcc_mcastgrp;      /* The group the packet is sent to */
    Ip_vifi_t           mfcc_parent;        /* The VIF that the packet is expected to arrive on */
    Ip_u8               mfcc_ttls[IP_MAXVIFS];   /* mfcc_ttls[VIF id] != 0 if packet
                                               sent along this route should be forwarded to that VIF */
};

/* Header for messages sent from the stack to mrouted */
struct Ip_igmpmsg
{
    Ip_u32              unused_1;
    Ip_u32              unused_2;
    Ip_u8               im_msgtype;     /* Message type, IP_IGMPMSG_xxx */
    Ip_u8               im_mbz;         /* Must be 0 */
    Ip_u8               im_vif;         /* Virtual interface */
    Ip_u8               unused_3;
    struct Ip_in_addr   im_src;         /* Source address of the packet that caused the message */
    struct Ip_in_addr   im_dst;         /* Destination address of the packet that caused the message */
};

#define IP_IGMPMSG_NOCACHE   1   /* Received packet that did not match any route entry */
#define IP_IGMPMSG_WRONGVIF  2   /* Packet arrived on the wrong interface */
#define IP_IGMPMSG_WHOLEPKT  3   /* PIM register processing */


/*
 *===========================================================================
 *                         PIM protocol header
 *===========================================================================
 */
typedef struct Ip_pkt_pim_struct
{
    Ip_u8  ver_type;        /* 4 bits version, 4 bits type */
    Ip_u8  reserved;
    Ip_u16 checksum;        /* The checksum for the PIM message */
    Ip_u32 flags;
}
Ip_pkt_pim;

/* PIM defines */
#define IP_PIM_VERSION           2  /* The PIM version supported by the stack */

#define IP_PIM_TYPE_REGISTER     1  /* PIM register message */

#define IP_PIM_FLAG_NULL_REGISTER (0x40000000)

/* PIM macros */
#define IP_PIM_GET_VERSION(pimhdr) ((pimhdr)->ver_type >> 4)
#define IP_PIM_GET_TYPE(pimhdr)    ((pimhdr)->ver_type & 0x0F)


/*
 *===========================================================================
 *                Ip_packet_mreq - set/getsockopt
 *===========================================================================
 * Defines interface and link layer address to join/leave for the
 * IP_PACKET_[ADD|DROP]_MEMBERSHIP socket options.
 */
struct Ip_packet_mreq
{
    Ip_u32  mr_ifindex;    /* Interface index */
    Ip_u8   mr_type;       /* Address type (IP_PACKET_MR_xxx) */
    Ip_u8   mr_alen;       /* Length of the link layer address */
    Ip_u8   mr_address[8]; /* A link layer address */
};

#define IP_PACKET_MR_MULTICAST 0  /* Multicast link address */
#define IP_PACKET_MR_PROMISC   1  /* Promiscuous address */
#define IP_PACKET_MR_ALLMULTI  2  /* All-multicast-addresses address */


/*
 *===========================================================================
 *                         Ipcom_socket_xselect
 *===========================================================================
 * External select code used in IPNET/IPLITE.
 */
#if defined(IPCOM_USE_SOCK_COMPAT) && defined(IP_PORT_OSE)
#define IPCOM_HAVE_SOCKET_XSELECT
struct Ipcom_socket_struct;
typedef void (*Ipcom_socket_xselect_cb)(struct Ipcom_socket_struct *sock);

typedef struct Ipcom_socket_xselect_struct
{
    void      *cookie;
    Ip_pid_t   pid;  /* pid of process that called select(). */
}
Ipcom_socket_xselect;
#endif


/*
 *===========================================================================
 *                    Ipcom_socket
 *===========================================================================
 * Proprietary socket structure used in the socket event callback.
 */

#ifdef IP_PORT_OSE5
struct Ipcom_desc_struct;
#endif

struct Ipcom_socket_struct;
enum Ipcom_socket_event_type {
    IP_SOEVENT_CB_NONE = 0,    /* No event */
    IP_SOEVENT_CB_INPKT,       /* Data packet 'pkt' was received */
    IP_SOEVENT_CB_OUTPKT,      /* Data packet 'pkt' was sent */
    IP_SOEVENT_CB_CLOSE,       /* The socket was closed */
    IP_SOEVENT_CB_ERRNO,       /* Asynchronous socket errno, e.g. received ICMP unreach */
    IP_SOEVENT_CB_ACCEPT,      /* Accept can now be called without blocking */
    IP_SOEVENT_CB_CONNECT,     /* The TCP handshake is finished */
    IP_SOEVENT_CB_TIMED_OUT,   /* The last operation on this socket has timed out */
    IP_SOEVENT_CB_ACK,         /* TCP got a pure ack with no data */
    IP_SOEVENT_CB_PKTAVAIL,    /* Packet memory _may_ now be available. */
    IP_SOEVENT_CB_TCP_STATE,   /* TCP state transition */
    IP_SOEVENT_CB_SCTP_STATE,  /* SCTP state transition */
    IP_SOEVENT_CB_OOB,         /* Out-of-band data received */
    IP_SOEVENT_CB_SELREAD,     /* The socket is now readable */
    IP_SOEVENT_CB_SELWRITE,    /* The socket is now writable */
    IP_SOEVENT_CB_SELEXCEPT    /* The socket is now exceptional */
};

typedef void (*Ipcom_socket_eventcb)(struct Ipcom_socket_struct *sock,
                                     IP_CONST Ipcom_pkt *pkt,
                                     enum Ipcom_socket_event_type event);

typedef struct Ipcom_socket_struct
{
    Ip_fd     fd;           /* Socket descriptor */
    Ip_pid_t  pid;          /* Socket owner */
    int       so_errno;     /* Socket errno (IP_ERRNO_xxx) */
    Ip_u16    type;         /* Socket type (IP_SOCK_xxx) */
    Ip_u16    domain;       /* Socket domain (IP_AF_xxx) */
    void     *cookie;       /* Port specific socket cookie */
    int       rcv_bytes;    /* Bytes in receive buffer. */
    Ipcom_socket_eventcb event_cb;

#ifdef IPCOM_HAVE_SOCKET_XSELECT
    Ipcom_socket_xselect_cb    xselect_cb; /* Used for OSE4 binary compat. */
    Ipcom_socket_xselect      *xselect; /* Used for OSE4 binary compat. */
#endif

#ifdef IP_PORT_OSE5
    struct Ipcom_desc_struct *descs;
    int       desc_ref_count;    /* Used for OSE5 dup() support */
    int       desc_block_count;  /* Used for OSE5 dup() support */
    int       msg_flags;
    void     *timeout;           /* Used for e.g. rcvtimeo support */
#endif

#ifdef IPIPSEC2
    Ip_u32    ipsec_registered;  /* Registered bitfield */
    void     *ipsec_flow_cache;  /* Flow cache pointer */
    Ip_u32    ipsec_policy_id;   /* Policy id (used to age flow_cache pointer) */
#endif
}
Ipcom_socket;

/*
 *===========================================================================
 *                    Ipcom_desc
 *===========================================================================
 * Proprietary descriptor structure used by OSE5 port.
 */
#ifdef IP_PORT_OSE5
typedef struct Ipcom_desc_struct
{
    struct Ipcom_desc_struct *next;
    Ip_fd     d;
#define IPCOM_OSE5_MAX_POLL 2
    void     *poll_sig[IPCOM_OSE5_MAX_POLL];   /* Maximum 2 processes may poll the same descriptor */
    Ip_pid_t  pid;
    Ip_bool   blocked;
}
Ipcom_desc;
#endif /* IP_PORT_OSE5 */


/*
 *===========================================================================
 *                    Ip_ifconf
 *===========================================================================
 * Structure used by IP_SIOCGIFCONF ioctl.
 */
struct Ip_ifconf
{
    int ifc_len;

    union
    {
        char         *ifcu_buf;
        struct Ip_ifreq *ifcu_req;
    } ifc_ifcu;
};

#define ip_ifc_buf       ifc_ifcu.ifcu_buf
#define ip_ifc_req       ifc_ifcu.ifcu_req


/*
 *===========================================================================
 *          Ip_icmp_filter - IP icmp message filter
 *===========================================================================
 * Argument structure for IP_ICMP_FILTER.
 */
#ifdef IP_PORT_LKM
#define IP_ICMP_FILTER_SIZE 1 /* Linux standard is 32 bits */
#else
#define IP_ICMP_FILTER_SIZE 8 /*   8*32 = 256 bits */
#endif

struct Ip_icmp_filter
{
    Ip_u32  icmp_filt[IP_ICMP_FILTER_SIZE];
};


/*
 *===========================================================================
 *                    IP_ICMP_FILTER_WILLPASS
 *===========================================================================
 * Description: Returns if the specified type is passed.
 * Parameters:  [in] type - An ICMPv6 message type.
 *              [in] filterp - The filter to check.
 * Returns:     IP_TRUE if the message type is passed.
 *
 */
#ifndef IP_ICMP_FILTER_WILLPASS
#define IP_ICMP_FILTER_WILLPASS(type, filterp) \
    ((((filterp)->icmp_filt[(type) >> 5]) & \
        (1 << ((type) & 31))) != 0)
#endif

/*
 *===========================================================================
 *                    IP_ICMP_FILTER_WILLBLOCK
 *===========================================================================
 * Description: Returns if the specified type is blocked.
 * Parameters:  [in] type - An ICMPv6 message type.
 *              [in] filterp - The filter to check.
 * Returns:     IP_TRUE if the message type is blocked.
 *
 */
#ifndef IP_ICMP_FILTER_WILLBLOCK
#define IP_ICMP_FILTER_WILLBLOCK(type, filterp) \
    ((((filterp)->icmp_filt[(type) >> 5]) & \
        (1 << ((type) & 31))) == 0)
#endif


/*
 *===========================================================================
 *                    IP_ICMP_FILTER_SETPASS
 *===========================================================================
 * Description: Modifies the filter to pass the specified message.
 * Parameters:  [in] type - The message type of message to pass to socket(s).
 *              [out] filterp - The filter to modify
 * Returns:
 *
 */
#ifndef IP_ICMP_FILTER_SETPASS
#define IP_ICMP_FILTER_SETPASS(type, filterp) \
    ((((filterp)->icmp_filt[(type) >> 5]) |= \
        (1 << ((type) & 31))))
#endif


/*
 *===========================================================================
 *                    IP_ICMP_FILTER_SETBLOCK
 *===========================================================================
 * Description: Modifies the filter to block the specified message.
 * Parameters:  [in] type - The message type of message to pass to socket(s).
 *              [out] filterp - The filter to modify
 * Returns:
 *
 */
#ifndef IP_ICMP_FILTER_SETBLOCK
#define IP_ICMP_FILTER_SETBLOCK(type, filterp) \
    ((((filterp)->icmp_filt[(type) >> 5]) &= \
        ~(1 << ((type) & 31))))
#endif


/*
 *===========================================================================
 *                    IP_ICMP_FILTER_SETPASSALL
 *===========================================================================
 * Description: Modifies the filter to pass all messages.
 * Parameters:  [out] filterp - The filter to modify
 * Returns:
 *
 */
#ifndef IP_ICMP_FILTER_SETPASSALL
#define IP_ICMP_FILTER_SETPASSALL(filterp) \
    ipcom_memset((filterp), 0xFF, sizeof(struct Ip_icmp_filter))
#endif


/*
 *===========================================================================
 *                    IP_ICMP_FILTER_SETBLOCKALL
 *===========================================================================
 * Description: Modifies the filter to block all messages.
 * Parameters:  [out] filterp - The filter to modify
 * Returns:
 *
 */
#ifndef IP_ICMP_FILTER_SETBLOCKALL
#define IP_ICMP_FILTER_SETBLOCKALL(filterp) \
    ipcom_memset((filterp), 0, sizeof(struct Ip_icmp_filter))
#endif

/* Netlink definitions */
#include "ipcom_netlink.h"


IP_EXTERN IP_CONST struct Ip_in_addr ip_inaddr_any;

/*
 ****************************************************************************
 * 6                    STANDARD FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_socket
 *===========================================================================
 * Description:  Open a new socket.
 * Parameters:   Address domain, type and protocol.
 * Returns:      Socket descriptor or IP_INVALID_SOCKET if failed.
 *
 */
IP_PUBLIC Ip_fd ipcom_socket(int domain, int type, int protocol);


/*
 *===========================================================================
 *                    ipcom_socketclose
 *===========================================================================
 * Description: Close a socket and free the resources held by it.
 * Parameters:  fd - The socket descriptor to close.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_PUBLIC int ipcom_socketclose(Ip_fd fd);


/*
 *===========================================================================
 *                    ipcom_shutdown
 *===========================================================================
 * Description: Causes all or parts of a full-duplex connection on the
 *              socket associated with 'fd' to be shut down.
 * Parameters:  fd - The socket to shutdown
 *              how - How the socket should be shutdown, must be one of the
 *              IP_SHUT_xx constants.
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int ipcom_shutdown(Ip_fd fd, int how);


/*
 *===========================================================================
 *                    ipcom_bind
 *===========================================================================
 * Description: Assigns a protocol address to a socket.
 * Parameters:  fd - The socket to name.
 *              name - The socket name (protocol address).
 *              namelen - The length of the name buffer.
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int ipcom_bind(Ip_fd fd, IP_CONST struct Ip_sockaddr *addr, Ip_socklen_t addrlen);


/*
 *===========================================================================
 *                    ipcom_connect
 *===========================================================================
 * Description: Connects the socket to a specified peer.
 * Parameters:  fd - The socket to connect.
 *              to - The address of the peer.
 *              tolen - The length of the to buffer.
 * Returns:     0= connection was successful, -1 = error.
 *
 */
IP_PUBLIC int ipcom_connect(Ip_fd fd, IP_CONST struct Ip_sockaddr *addr, Ip_socklen_t addrlen);


/*
 *===========================================================================
 *                    ipcom_accept
 *===========================================================================
 * Description: Accepts an incoming connection attempt on a socket.
 * Parameters:  fd - A socket that is in listen state (ipcom_listen()) has
 *              been called.
 *              name - Will hold the address of the peer (set to IP_NULL if
 *              that information is not wanted).
 *              addrlen - Must be initialized to the length of the 'name'
 *              buffer, will hold the actual 'name' length upon return.
 *              Set to IP_NULL if 'name' is IP_NULL.
 * Returns:     0>= the socket to use for communication (success), -1 = error.
 *
 */
IP_PUBLIC Ip_fd ipcom_accept(Ip_fd fd, struct Ip_sockaddr *addr, Ip_socklen_t *addrlenp);


/*
 *===========================================================================
 *                    ipcom_listen
 *===========================================================================
 * Description: Places a socket a state where it is listening for incoming
 *              connection(s).
 * Parameters:  fd - a bound, unconnected socket.
 *              backlog - The maximum length of the queue of pending
 *              connections.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_PUBLIC int ipcom_listen(Ip_fd fd, int backlog);


/*
 *===========================================================================
 *                    ipcom_socketwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_socketwrite(Ip_fd fd, IP_CONST void *buf, Ip_size_t nbytes);


/*
 *===========================================================================
 *                    ipcom_socketwritev
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_socketwritev(Ip_fd fd, IP_CONST struct Ip_iovec *iov, int iovlen);


/*
 *===========================================================================
 *                    ipcom_send
 *===========================================================================
 * Description: Sends a packet using a connected socket.
 * Parameters:  fd - The descriptor for the socket to use.
 *              msg - The buffer to send.
 *              len - The length of the buffer in octets.
 *              flags - Send flags.
 * Returns:     >=0 = number of bytes sent, IP_SOCKERR = error code is
 *              retrivered by using ipcom_errno.
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_send(Ip_fd fd, IP_CONST void *msg, Ip_size_t len, int flags);


/*
 *===========================================================================
 *                    ipcom_sendto
 *===========================================================================
 * Description: Send a packet to the address to which the socket is connected
 *              or to the 'to' address if the socket is not connected.
 * Parameters:  fd - The descriptor for the socket to use.
 *              buf - The buffer to send.
 *              len - The length of the buffer in octets.
 *              flags - Send flags.
 *              to - The destination for the data if the socket is
 *              unconnected, ignored for connected sockets.
 *              tolen - The size of the 'to' buffer in octets.
 * Returns:     >=0 = number of bytes sent, IP_SOCKERR = error code is
 *              retrivered by using ipcom_errno.
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_sendto(Ip_fd fd, IP_CONST void *msg, Ip_size_t len, int flags, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen);


/*
 *===========================================================================
 *                    ipcom_sendmsg
 *===========================================================================
 * Description: Send a packet on a connected socket.
 * Parameters:  fd    - A socket descriptor.
 *              msg   - Receive parameters.
 *              flags - Currently unused.
 * Returns:     0 = a packet was successfully received, -1 = error.
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_sendmsg(Ip_fd sock, IP_CONST struct Ip_msghdr *msg, int flags);


/*
 *===========================================================================
 *                    ipcom_socketread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_socketread(Ip_fd fd, void *buf, Ip_size_t nbytes);


/*
 *===========================================================================
 *                    ipcom_recv
 *===========================================================================
 * Description: Receives data from a socket.
 * Parameters:  fd - A socket descriptor.
 *              buf - A buffer to where the data should be copied.
 *              len - The length of buf buffer.
 *              flags - Currently unused (set to 0).
 * Returns:     >=0 = the number of bytes copied to buf, -1 = error.
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_recv(Ip_fd fd, void *buf, Ip_size_t len, int flags);


/*
 *===========================================================================
 *                    ipcom_recvfrom
 *===========================================================================
 * Description: Receives data from a socket.
 * Parameters:  fd - A socket descriptor.
 *              buf - A buffer to where the data should be copied.
 *              len - The length of buf buffer.
 *              flags - Currently unused (set to 0).
 *              from - The sender of the data.
 *              fromlen - The length of the from buffer, will be updated to
 *              contain the number of octets copied to the from buffer.
 * Returns:     >=0 = the number of bytes copied to buf, -1 = error.
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_recvfrom(Ip_fd fd, void *buf, Ip_size_t len, int flags, struct Ip_sockaddr *from, Ip_socklen_t *fromlenp);


/*
 *===========================================================================
 *                    ipcom_recvmsg
 *===========================================================================
 * Description: Receives data from a socket.
 * Parameters:  fd    - A socket descriptor.
 *              msg   - Receive parameters.
 *              flags - Currently unused.
 * Returns:     0 = a packet was successfully received, -1 = error.
 *
 */
IP_PUBLIC Ip_ssize_t ipcom_recvmsg(Ip_fd fd, struct Ip_msghdr *msg, int flags);


/*
 *===========================================================================
 *                    ipcom_getsockopt
 *===========================================================================
 * Description: Gets the specified option on a socket. Options may
 *              exist at multiple protocol levels.
 * Parameters:  fd - A socket descriptor.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Pointer to a buffer where the option data should be
 *              copied..
 *              optlen - The length of the buffer poited to by optval.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_PUBLIC int ipcom_getsockopt(Ip_fd fd, int level, int optname, void *optval, Ip_socklen_t *optlenp);


/*
 *===========================================================================
 *                    ipcom_setsockopt
 *===========================================================================
 * Description: Sets the specified option on a socket. Options may
 *              exist at multiple protocol levels.
 * Parameters:  fd - A socket descriptor.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Data to set for the option.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_PUBLIC int ipcom_setsockopt(Ip_fd fd, int level, int optname, IP_CONST void *optval, Ip_socklen_t optlen);


/*
 *===========================================================================
 *                    ipcom_getsockname
 *===========================================================================
 * Description: Returns the name of the specified socket.
 * Parameters:  fd - A socket descriptor.
 *              name - A buffer that will receive the name of the socket.
 *              namelenp - A pointer to the length of name, will be updated
 *              to contain the actual number of octets copied to name.
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int ipcom_getsockname(Ip_fd fd, struct Ip_sockaddr *name, Ip_socklen_t *namelenp);


/*
 *===========================================================================
 *                    ipcom_getpeername
 *===========================================================================
 * Description: Returns the name of the peer connected to the specified socket.
 * Parameters:  fd - A socket descriptor.
 *              name - A buffer that will receive the name of the peer.
 *              namelenp - A pointer to the length of name, will be updated
 *              to contain the actual number of octets copied to name.
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int ipcom_getpeername(Ip_fd fd, struct Ip_sockaddr *name, Ip_socklen_t *namelenp);


/*
 *===========================================================================
 *                    ipcom_inet_pton
 *===========================================================================
 * Description: Converts a string representation of a address to its binary
 *              form.
 * Parameters:  domain - The domain for the addresss (IP_AF_INET or
 *              IP_AF_INET6).
 *              strptr - A string representation of the address.
 *              addrptr - Points to the buffer that will receive the
 *              binary for of the address.
 * Returns:     1 = success, 0 = the string was not a valid address
 *              <0 = error (error code from ipcom_errno).
 *
 */
IP_PUBLIC int ipcom_inet_pton(int family, IP_CONST char *strptr, void *addrptr);


/*
 *===========================================================================
 *                    ipcom_inet_ntop
 *===========================================================================
 * Description: Converts the network address into a character string.
 * Parameters:  family - The address family for the address to convert.
 *              strptr - A pointer to the buffer that will receive the
 *              string representation of the address.
 *              len - The length of the strptr buffer.
 * Returns:     The strptr value if the call was successful or IP_NULL
 *              if failed (get error code from ipcom_errno).
 *
 */
IP_PUBLIC IP_CONST char *ipcom_inet_ntop(int family, IP_CONST void *addrptr, char *strptr, Ip_size_t len);


/*
 *===========================================================================
 *                    ipcom_socketioctl
 *===========================================================================
 * Description: IO control.
 * Parameters:  fd - A socket descriptor.
 *              request - The IO control request.
 *              argp - Request specific data.
 * Returns:     0 = success, -1 = error (error code from ipcom_errno).
 *
 */
IP_PUBLIC int ipcom_socketioctl(Ip_fd fd, unsigned long request, void *argp);


/*
 *===========================================================================
 *                    ipcom_if_nametoindex
 *===========================================================================
 * Description: maps an interface name into its corresponding index.
 * Parameters:  ifname - A null terminated interface name.
 * Returns:     The interface index of the specified name. 0 is returned
 *              if the function fails.
 *
 */
IP_PUBLIC Ip_u32 ipcom_if_nametoindex(IP_CONST char *ifname);


/*
 *===========================================================================
 *                    ipcom_if_indextoname
 *===========================================================================
 * Description: Maps an interface index into its corresponding name.
 * Parameters:  ifindex - The index of the interface to get the name for.
 *              ifname - must point to a buffer of at least
 *              IP_IFNAMSIZ bytes.

 * Returns:     The pointer value of ifname or IP_NULL if the function fails.
 *
 */
IP_PUBLIC char *ipcom_if_indextoname(Ip_u32 ifindex, char *ifname);


/*
 *===========================================================================
 *                    ipcom_if_nameindex
 *===========================================================================
 * Description: Returns an array of Ip_if_nameindex structures, one
 *              structure per interface. The end of the array of structures
 *              is indicated by a structure with an if_index of 0 and
 *              an if_name of IP_NULL.
 * Parameters:
 * Returns:     An array of Ip_if_nameindex structures.
 *
 */
IP_PUBLIC struct Ip_if_nameindex *ipcom_if_nameindex(void);

/*
 *===========================================================================
 *                    ipcom_if_freenameindex
 *===========================================================================
 * Description: Free the array returned from ipcom_if_nameindex().
 * Parameters:  pif - A pointer to an array returned from ipcom_if_nameindex().
 * Returns:
 *
 */
IP_PUBLIC void ipcom_if_freenameindex(struct Ip_if_nameindex *pif);


#ifndef IPCOM_USE_SA_LEN
IP_GLOBAL Ip_u8 ipcom_sa_get_len(const struct Ip_sockaddr *sa);
#endif


/*
 ****************************************************************************
 * 7                    PORT OVERRIDE INCLUDE
 ****************************************************************************
 */

#if IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
#include "ipcom_psock.h"
#endif


/*
 ****************************************************************************
 * 8                    PORT SPECIFIC TYPES
 ****************************************************************************
 * These types must be after the include of ipcom_psock.h since sizes may
 * be altered in ipcom_psock.h.
 */

/*
 *===========================================================================
 *                         Ip_ifreq
 *===========================================================================
 * Interface request structure used for socket ioctl's.  All interface
 * ioctl's must have parameter definitions which begin with ifr_name. The
 * remainder may be interface specific.
 */

struct Ip_ifreq
{
    char    ifr_name[IP_IFNAMSIZ];             /* if name, e.g. "eth0" */

    union
    {
        struct  Ip_sockaddr ifru_addr;
        struct  Ip_sockaddr ifru_dstaddr;
        struct  Ip_sockaddr ifru_broadaddr;
        Ip_u16  ifru_flags;
        int     ifru_metric;
        int     ifru_mtu;
        int     ifru_ivalue;
        Ip_u32  ifru_ifindex;
        void   *ifru_data;        /* interface dependent data */
        Ip_u16  ifru_vr;          /* virtual router */
        int     ifru_opt;         /* option: enable=1, disable=0 */
        int	    ifru_cap[2];
        char    ifru_newname[IP_IFNAMSIZ];             /* if name, e.g. "eth0" */
        char    ifru_slave[IP_IFNAMSIZ];             /* if name, e.g. "eth0" */
    }
    ifr_ifru;
};

#define ip_ifr_addr        ifr_ifru.ifru_addr      /* address */
#define ip_ifr_dstaddr     ifr_ifru.ifru_dstaddr   /* other end of p-to-p link */
#define ip_ifr_broadaddr   ifr_ifru.ifru_broadaddr /* broadcast address */
#define ip_ifr_flags       ifr_ifru.ifru_flags     /* flags */
#define ip_ifr_metric      ifr_ifru.ifru_metric    /* metric */
#define ip_ifr_ifindex     ifr_ifru.ifru_ifindex   /* Interface index */
#define ip_ifr_mtu         ifr_ifru.ifru_mtu       /* MTU. */
#define ip_ifr_data        ifr_ifru.ifru_data      /* for use by interface */
#define ip_ifr_vr          ifr_ifru.ifru_vr        /* Interpeak virtual router extension */
#define ip_ifr_opt         ifr_ifru.ifru_opt       /* Interpeak option extension */
#define ip_ifr_reqcap      ifr_ifru.ifru_cap[0]    /* requested capabilities */
#define ip_ifr_curcap      ifr_ifru.ifru_cap[1]    /* current capabilities */
#define ip_ifr_qlen        ifr_ifru.ifru_ivalue
#define ip_ifr_newname     ifr_ifru.ifru_newname   /* New name             */

/*
 *===========================================================================
 *                         Ip_ifaliasreq
 *===========================================================================
 * Generic alias structure
 */
struct Ip_ifaliasreq
{
    char                 ifra_name[IP_IFNAMSIZ];

    struct  Ip_sockaddr  ifra_addr;
    struct  Ip_sockaddr  ifra_dstaddr;
#define ip_ifra_broadaddr   ifra_dstaddr
    struct  Ip_sockaddr  ifra_mask;
};


/*
 *===========================================================================
 *                         Ip_in_aliasreq
 *===========================================================================
 * For use with IP_SIOCAIFADDR ioctl().
 */
struct Ip_in_aliasreq
{
    char                    ifra_name[IP_IFNAMSIZ];

    struct  Ip_sockaddr_in  ifra_addr;
    struct  Ip_sockaddr_in  ifra_dstaddr;
#define ip_ifra_broadaddr   ifra_dstaddr
    struct  Ip_sockaddr_in  ifra_mask;
};


/*
 *===========================================================================
 *                    Ip_vlanreq - ioctl
 *===========================================================================
 * Configuration structure for IP_SIOCSETVLAN and IP_SIOCGETVLAN ioctls.
 */
struct Ip_vlanreq
{
    char    vlr_parent[IP_IFNAMSIZ];
    Ip_u16  vlr_tag;  /* 12-bit vlan id, 1..4094, 0 for priority-tagged */
    Ip_u16  vlr_pri;  /* 3-bit class of service, 0..7 */
};


/*
 *===========================================================================
 *                    Ip_ip_tunnel_param - ioctl
 *===========================================================================
 * Tunnel parameter struct used with IP_SIOCxxxTUNNEL ioctls.
 */
struct Ip_ip_tunnel_param
{
    char    ifr_name[IP_IFNAMSIZ];          /* if name, e.g. "gif1", "gre1" */

    union Ip_sockaddr_union   local_addr;   /* The address to use as source
                                               address for outgoing packets, must
                                               be the address of a local interface */

    union Ip_sockaddr_union   remote_addr;  /* The address to use as destination
                                               address for outgoing packets */

    Ip_u8   ttl;        /* Value to put in time to live field */

    /* These fields are read only (returned by IP_SIOCGETTUNNEL) */
    Ip_u8   protocol;   /* Tunnel protocol: IP_IPPROTO_IPIP or IP_IPPROTO_GRE. */
    Ip_u16  o_flags;    /* Output flags (tunnel protocol specific). */
    Ip_u16  i_flags;    /* Input flags (tunnel protocol specific). */
};

#include "ipcom_align16.h"
IP_PACKED1 struct Ipcom_iphdr
{
#if defined(IP_IP_VHL)
        Ip_u8   vhl;
#elif defined(IP_LITTLE_ENDIAN)
        unsigned int    ihl:4,
                        version:4;
#else
        unsigned int    version:4,
                        ihl:4;
#endif

        Ip_u8   tos;
        Ip_u16  tot_len;
        Ip_u16  id;
        Ip_u16  frag_off;
        Ip_u8   ttl;
        Ip_u8   protocol;
        Ip_u16  check;
        Ip_u32  saddr;
        Ip_u32  daddr;
} IP_PACKED2;
#include "ipcom_align16.h"

/* Linux based tunnel parameter structure */
struct Ip_ip_tunnel_parm
{
        char                    name[IP_IFNAMSIZ];
        int                     link;
        Ip_u16                  i_flags;
        Ip_u16                  o_flags;
        Ip_u16                  i_key;
        Ip_u16                  o_key;
        struct Ipcom_iphdr      iph;
};

/* Checksum will be used on the GRE tunnel if this flag is set */
#define IPCOM_TUNNEL_FLAG_GRE_CHECKSUM 0x8000


/*
 *===========================================================================
 *                         Ip_fd_set
 *===========================================================================
 * ipcom_socketselect() type definitions.
 */
#ifndef IP_FD_SET_TYPE
#define IP_FD_SET_TYPE
typedef struct Ip_fd_set_struct
{
    Ip_u32 set[(IP_FD_SETSIZE + sizeof(Ip_u32) * 8 - 1) / (sizeof(Ip_u32) * 8)];
}
Ip_fd_set;

#define IP_FD_ZERO(pfdset)       ipcom_memset(pfdset, 0, sizeof(Ip_fd_set));
#define IP_FD_CLR(fd, pfdset)    ((pfdset)->set[(fd) >> 5] &= ~(1 << ((fd) & 31)))
#define IP_FD_SET(fd, pfdset)    ((pfdset)->set[(fd) >> 5] |= 1 << ((fd) & 31))
#define IP_FD_ISSET(fd, pfdset)  (0 != ((pfdset)->set[(fd) >> 5] & (1 << ((fd) & 31))))

#define	ip_howmany(x, y)	(((x)+((y)-1))/(y))
#endif


/*
 *===========================================================================
 *                    ipcom_socketselect
 *===========================================================================
 * Description: Checks if any of the sockets in a set is ready for reading,
 *              writing or have an exceptional condition.
 * Parameters:  ndfs - The first 'ndfs' descriptors are checked in each set
 *              i.e., the descriptors from 0 through nfds-1 in the descriptor
 *              sets are examined.
 *              readfds - The set of descriptors that should be checked for
 *              reading.
 *              writeds - The set of descriptors that should be checked for
 *              writing.
 *              exceptfds - The set of descriptors that should be checked for
 *              exceptions (not supported, must be IP_NULL).
 * Returns:     The total number of ready descriptors in each set.
 *
 */
IP_PUBLIC int ipcom_socketselect(int nfds, Ip_fd_set *readfds,
    Ip_fd_set *writeds, Ip_fd_set *exceptds, struct Ip_timeval *timeout);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
