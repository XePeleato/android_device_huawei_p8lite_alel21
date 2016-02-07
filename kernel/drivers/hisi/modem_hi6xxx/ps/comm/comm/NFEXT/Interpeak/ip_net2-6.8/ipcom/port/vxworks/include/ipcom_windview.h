/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_windview.h,v $ $Revision: 1.4 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_windview.h,v $
 *     $Author: jyang $ $Date: 2009-01-24 03:12:48 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_WINDVIEW_H
#define IPCOM_WINDVIEW_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_config.h"
#include "ipcom_type.h"
#include "ipcom_cstyle.h"

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
 *                      Class definitions
 *===========================================================================
 */
#define IPCOM_WV_NETD_IP4_DATAPATH_EVENT 0x00000008    /* IPv4 Data Path Events */
#define IPCOM_WV_NETD_IP6_DATAPATH_EVENT 0x00000010    /* IPv6 Data Path Events */
#define IPCOM_WV_NETD_IP4_CTRLPATH_EVENT 0x00000018    /* IPv4 Control Path Events */
#define IPCOM_WV_NETD_IP6_CTRLPATH_EVENT 0x00000020    /* IPv6 Control Path Events */


/*
 *===========================================================================
 *                      Event Level
 *===========================================================================
 */
#define IPCOM_WV_NETD_FATAL           1
#define IPCOM_WV_NETD_CRITICAL        2
#define IPCOM_WV_NETD_WARNING         3
#define IPCOM_WV_NETD_INFO            4
#define IPCOM_WV_NETD_VERBOSE         5


/*
 *===========================================================================
 *                      Data transfer status
 *===========================================================================
 */
#define IPCOM_WV_NETD_SEND 		1
#define IPCOM_WV_NETD_RECV 		2


/*
 *===========================================================================
 *                      Event Id
 *===========================================================================
 */
#define IPCOM_WV_NETDEVENT_FATAL       20400
#define IPCOM_WV_NETDEVENT_CRITICAL    20415
#define	IPCOM_WV_NETDEVENT_WARNING     20430
#define IPCOM_WV_NETDEVENT_INFO        20445
#define IPCOM_WV_NETDEVENT_START       20460
#define IPCOM_WV_NETDEVENT_FINISH      20475


/*
 *===========================================================================
 *                      Qualifier
 *===========================================================================
 */
#define IPCOM_WV_NETD_BADLEN            1
#define IPCOM_WV_NETD_BADSUM            2
#define IPCOM_WV_NETD_BADADDR           3
#define IPCOM_WV_NETD_BADSOCK           4
#define IPCOM_WV_NETD_BADSIZE           5
#define IPCOM_WV_NETD_BADHLEN           6
#define IPCOM_WV_NETD_BADWIDTH          7
#define IPCOM_WV_NETD_BADOPT            8
#define IPCOM_WV_NETD_BADVERS           9
#define IPCOM_WV_NETD_BADMBLK           10
#define IPCOM_WV_NETD_BADLEVEL          11
#define IPCOM_WV_NETD_BADROUTE          12
#define IPCOM_WV_NETD_BADREFCNT         13
#define IPCOM_WV_NETD_BADSYNC           14
#define IPCOM_WV_NETD_NOROUTEKEY        15
#define IPCOM_WV_NETD_NOROUTE           16
#define IPCOM_WV_NETD_NOPCB             17
#define IPCOM_WV_NETD_NOPROTO           18
#define IPCOM_WV_NETD_SHORTMSG          19
#define IPCOM_WV_NETD_BADFLAGS          20
#define IPCOM_WV_NETD_NOMEM             21
#define IPCOM_WV_NETD_NOSOCK            22
#define IPCOM_WV_NETD_BADBUF            23
#define IPCOM_WV_NETD_SEMTKFAILED       24
#define IPCOM_WV_NETD_NOFDREF           25
#define IPCOM_WV_NETD_BADGATEWAY        26
#define IPCOM_WV_NETD_UNREACH           27
#define IPCOM_WV_NETD_BADTYPE           28
#define IPCOM_WV_NETD_BADRULE           29
#define IPCOM_WV_NETD_SHORTBUF          30
#define IPCOM_WV_NETD_NOHDR             31
#define IPCOM_WV_NETD_INVALIDPARENT     32
#define IPCOM_WV_NETD_NOUSRREQS         33
#define IPCOM_WV_NETD_BADARG            34
#define IPCOM_WV_NETD_OUTOFDATA         35
#define IPCOM_WV_NETD_BADCHAIN          36
#define IPCOM_WV_NETD_INVAL             37
#define IPCOM_WV_NETD_RETRANSMITPENDING 38
#define IPCOM_WV_NETD_NOGATEWAY         39
#define IPCOM_WV_NETD_NOBUFS            40
#define IPCOM_WV_NETD_BADOFFSET         41
#define IPCOM_WV_NETD_AFNOSUPPORT       42
#define IPCOM_WV_NETD_ADDRNOTAVAIL      43
#define IPCOM_WV_NETD_ADDRINUSE         44
#define IPCOM_WV_NETD_NXIO              45
#define IPCOM_WV_NETD_OPNOTSUPP         46
#define IPCOM_WV_NETD_NETDOWN           47
#define IPCOM_WV_NETD_HOSTUNREACH       48
#define IPCOM_WV_NETD_NETUNREACH        49
#define IPCOM_WV_NETD_ACCES             50
#define IPCOM_WV_NETD_MSGSIZE           51
#define IPCOM_WV_NETD_NOPROTOOPT        52
#define IPCOM_WV_NETD_PKTLOST           53
#define IPCOM_WV_NETD_NOPORT            54
#define IPCOM_WV_NETD_FULLSOCK          55
#define IPCOM_WV_NETD_RTCLONE           56
#define IPCOM_WV_NETD_RTSEARCHGOOD      57
#define IPCOM_WV_NETD_SEARCHFAILED      58
#define IPCOM_WV_NETD_BADQUERY          59
#define IPCOM_WV_NETD_BADREPORT         60
#define IPCOM_WV_NETD_DUPLICATED        61
#define IPCOM_WV_NETD_OPSFAILED         62
#define IPCOM_WV_NETD_LOCKFAILED        63
#define IPCOM_WV_NETD_UNLOCKFAILED      64
#define IPCOM_WV_NETD_BADMASK           65
#define IPCOM_WV_NETD_ADDFAILED         66


/*
 *===========================================================================
 *                      Info event subtypes
 *===========================================================================
 */
#define IPCOM_WV_NETD_INFO_UP            1
#define IPCOM_WV_NETD_INFO_DOWN          2
#define IPCOM_WV_NETD_INFO_OPEN          3
#define IPCOM_WV_NETD_INFO_CLOSE         4
#define IPCOM_WV_NETD_INFO_SEND          5
#define IPCOM_WV_NETD_INFO_RECEIVE       6
#define IPCOM_WV_NETD_INFO_ALLOC         7
#define IPCOM_WV_NETD_INFO_FREE          8
#define IPCOM_WV_NETD_INFO_CONNECT       9
#define IPCOM_WV_NETD_INFO_DISCONNECT    10
#define IPCOM_WV_NETD_INFO_CREATE        11
#define IPCOM_WV_NETD_INFO_DELETE        12
#define IPCOM_WV_NETD_INFO_DROPPED       13
#define IPCOM_WV_NETD_INFO_RETRANSMIT    14
#define IPCOM_WV_NETD_INFO_TIMEOUT       15
#define IPCOM_WV_NETD_INFO_EXPIRED       16
#define IPCOM_WV_NETD_INFO_UPDATED       17
#define IPCOM_WV_NETD_INFO_QUEUE_FULL    18
#define IPCOM_WV_NETD_INFO_PKT_SEND      19
#define IPCOM_WV_NETD_INFO_PKT_RECV      20
#define IPCOM_WV_NETD_INFO_RESET         21
#define IPCOM_WV_NETD_INFO_WAIT          22
#define IPCOM_WV_NETD_INFO_NODEADD       23
#define IPCOM_WV_NETD_INFO_RTADDED       24
#define IPCOM_WV_NETD_INFO_RTCLONE       25
#define IPCOM_WV_NETD_INFO_RTALLOC       26

#define IPCOM_WV_NETD_INFO_CUSTOM_1      27
#define IPCOM_WV_NETD_INFO_CUSTOM_2      28
#define IPCOM_WV_NETD_INFO_CUSTOM_3      29
#define IPCOM_WV_NETD_INFO_CUSTOM_4      30
#define IPCOM_WV_NETD_INFO_CUSTOM_5      31
#define IPCOM_WV_NETD_INFO_CUSTOM_6      32


/*
 *===========================================================================
 *                      Module identifiers
 *===========================================================================
 *  Represented by an 8-bit field in wvNetDEventInfo. Use codes 0-127 for
 *  dual stack. Use codes 128-255 for Interpeak.
 */
#define IPCOM_WV_IPNET_MODULE              128     /* ipnet.c source file */
#define IPCOM_WV_IPNET_NETIF_MODULE        129     /* ipnet_netif.c source file */
#define IPCOM_WV_IPNET_IP4_MODULE          130     /* ipnet_ip4.c source file */
#define IPCOM_WV_IPNET_IP6_MODULE          131     /* ipnet_ip6.c source file */
#define IPCOM_WV_IPNET_SOCK_TCP_MODULE     132     /* ipnet_sock_tcp.c source file */
#define IPCOM_WV_IPNET_UDP_MODULE          133     /* ipnet_udp.c source file */
#define IPCOM_WV_IPNET_ETH_MODULE          134     /* ipnet_eth.c source file */
#define IPCOM_WV_IPNET_FRAG_MODULE         135     /* ipnet_frag.c source file */
#define IPCOM_WV_IPNET_ICMP6_MODULE        136     /* ipnet_icmp6.c source file */
#define IPCOM_WV_IPNET_NEIGH_MODULE        137     /* ipnet_neigh.c source file */
#define IPCOM_WV_IPNET_PKT_POOL_MODULE     138     /* ipnet_pkt_pool.c source file */
#define IPCOM_WV_IPNET_RAW_MODULE          139     /* ipnet_raw.c source file */
#define IPCOM_WV_IPNET_SOCK_MODULE         140     /* ipnet_sock.c source file */
#define IPCOM_WV_IPNET_TUNNEL_MODULE       141     /* ipnet_tunnel.c source file */
#define IPCOM_WV_IPTCP_MODULE              142     /* iptcp.c source file */
#define IPCOM_WV_IPSCTP_INPUT_MODULE       143     /* ipsctp_input.c source file */


/*
 *===========================================================================
 *                      Entity
 *===========================================================================
 *  Represented by an 8-bit field in wvNetDEventInfo.
 */
#define IPCOM_WV_NETD_ARP             1
#define IPCOM_WV_NETD_ICMP4           2
#define IPCOM_WV_NETD_ICMP6           3
#define IPCOM_WV_NETD_IP4             4
#define IPCOM_WV_NETD_IP6             5
#define IPCOM_WV_NETD_RAWIP           6
#define IPCOM_WV_NETD_ROUTE           7
#define IPCOM_WV_NETD_TCP4            8
#define IPCOM_WV_NETD_TCP6            9
#define IPCOM_WV_NETD_UDP4            10
#define IPCOM_WV_NETD_UDP6            11
#define IPCOM_WV_NETD_IF              12
#define IPCOM_WV_NETD_SOCKET          13
#define IPCOM_WV_NETD_IGMP            14
#define IPCOM_WV_NETD_NETBUF          15
#define IPCOM_WV_NETD_MEMBUF          16
#define IPCOM_WV_NETD_PACKET          17
#define IPCOM_WV_NETD_RAW_SOCKET      18
#define IPCOM_WV_NETD_IP_REASSEMBLE   19
#define IPCOM_WV_NETD_TCP_REASSMBLE   20
#define IPCOM_WV_NETD_DLINK           21
#define IPCOM_WV_NETD_ND              22
#define IPCOM_WV_NETD_SCTP4           23
#define IPCOM_WV_NETD_SCTP6           24


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL int  ipcom_cmd_wvnet(int argc, char **argv);
IP_PUBLIC void ipcom_wv_marker_1(Ip_u32 classId, Ip_u32 eventLevel, Ip_u32 eventOffset, Ip_u32 eventTag,
                                 Ip_u32 eventId, void *arg1, Ip_u8 module, Ip_u8 entity);
IP_PUBLIC void ipcom_wv_marker_2(Ip_u32 classId, Ip_u32 eventLevel, Ip_u32 eventOffset, Ip_u32 eventTag,
                                 Ip_u32 eventId, void *arg1, int arg2, Ip_u8 module, Ip_u8 entity);
IP_PUBLIC void ipcom_wv_event_2(Ip_u32 classId, Ip_u32 eventLevel, Ip_u32 eventOffset, Ip_u32 eventTag,
                                Ip_u32 eventId, Ip_u32 status, void *arg1, int arg2, Ip_u8 module, Ip_u8 entity);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef IPCOM_WINDVIEW_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
