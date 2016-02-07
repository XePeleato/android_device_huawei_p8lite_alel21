/*
 ******************************************************************************
 *                     IPNET USER CONFIGURATION FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_config.c,v $ $Revision: 1.13.12.3.2.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipnet_config.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01s,02oct08,jre  Added missing sysvar DeleteAddressOnDuplicateDetect.
01r,03sep08,brg  Added ipnet_link_stub interface to support custom
                 link layers in MUX/END
01q,28mar08,kjn  x
01p,08aug07,dlk  Support  INCLUDE_IPNET_IFQUEUE_FIFO_DEFAULT (WIND00095683)
01o,13jul07,dlk  Move IPNET_RTHDR0 parameter to section guarded by
                 INCLUDE_IPCOM_USE_INET6.
01n,13jul07,kjn  Added configuration for routing header type 0 (IPv6)
01m,10may07,kch  Define IPCOM_USE_INET6 if INCLUDE_IPCOM_USE_INET6 is defined.
01l,06mar07,kch  Removed unsupported static configurations from the
                 ipnet_conf_boot_cmd[] (WIND00088900).
01k,27jan07,tkf  Fix compiler warning in ipnet_register_shell_commands() when
                 none of the components are defined.
01j,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01i,09dec06,kch  Restore external prototype forward declarations.
01h,01dec06,kch  Added ipnet_cmd.h and ipnet_netif.h includes. Also removed
                 duplicate interface type definitions and function prototypes.
01g,20oct06,tkf  Added more entries to ipnet_conf_pkt_pool_buf[].
01f,13sep06,kch  Refresh with Sept 08, 2006 code drop. Also rename
                 INCLUDE_IPPPP to INCLUDE_IPNET_USE_PPP.
01e,28aug06,kch  Update based on Aug 22, 2006 code drop - added ipnet_config.h
                 include to support IPv6 autoconf privacy extensions, IPv4
                 autoIP and NAT configurations. Created ipnet_conf_sysvar_ext[]
                 to support per-interface configurations for IPv6 autoconf
                 privacy extensions, IPv4 autoIP and proxyArp.
01d,03aug06,kch  Renamed INCLUDE_Xxx_CMD to INCLUDE_IPXxx_CMD for consistency.
01c,17jul06,kch  Renamed IPNET_ROUTE_CMD to INCLUDE_ROUTE_CMD and
                 IPNET_SCALETEST_CMD to INCLUDE_IPNET_SCALETEST_CMD.
01b,05jul06,kch  Guarded ipnet_conf_ip4_min_mtu and ipnet_conf_ip6_min_mtu
                 constants with proper macros.
01a,29jun06,kch  Updated IPNET packet pool entries configuration macros.
*/


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * User IPNET configuration file.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#ifdef INCLUDE_IPCOM_USE_INET6
#ifndef IPCOM_USE_INET6
#define IPCOM_USE_INET6
#endif
#endif

#include <ipcom_err.h>
#include <ipcom_pkt.h>
#include <ipcom_shell.h>
#include <ipcom_sysvar.h>

#include "ipnet_config.h"

#include "ipnet_loopback.h"

/*
 * includes the ipnet_netif.h header mainly for the IP_IFT_Xxx interface
 * type defines. The Ipnet_netif structure alignment is a non-issue here
 * since that structure is not used in this context.
 */
#include "ipnet_netif.h"

#ifdef INCLUDE_IPNET_IFQUEUE_FIFO_DEFAULT
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_sock.h>
#include <ipnet.h>
#include <ipnet_qos.h>
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Definition of default values */

#ifndef IPNET_ROUTE_MAX_CLONE_COUNT
#define IPNET_ROUTE_MAX_CLONE_COUNT "1000"
#endif

#ifndef IPNET_MAX_REASSEMBLY_LIST_LEN
#define IPNET_MAX_REASSEMBLY_LIST_LEN 10
#endif

#ifndef IPNET_MAX_DGRAM_FRAG_LIST_LEN
#define IPNET_MAX_DGRAM_FRAG_LIST_LEN 100
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef INCLUDE_IPCOM_USE_ETHERNET
IP_EXTERN int ipnet_eth_if_init();
#ifdef INCLUDE_IPNET_USE_VLAN
IP_EXTERN int ipnet_vlan_if_init();
#endif /* INCLUDE_IPNET_USE_VLAN */
#endif /* INCLUDE_IPCOM_USE_ETHERNET */

#ifdef INCLUDE_IPNET_USE_PPP
IP_EXTERN int ipnet_ppp_if_init();
#endif /* INCLUDE_IPNET_USE_PPP */

#ifdef INCLUDE_IPNET_USE_TUNNEL
IP_EXTERN int ipnet_tunnel_if_init();
#endif /* INCLUDE_IPNET_USE_TUNNEL */

#ifdef INCLUDE_IPNET_USE_WLAN
IP_EXTERN int ipnet_wlan_if_init();
#endif /* INCLUDE_IPNET_USE_WLAN */

#ifdef INCLUDE_IPMPLS_TUNNEL
IP_EXTERN int ipnet_mpls_if_init();
#endif

#ifdef INCLUDE_IPCOM_USE_LINK_STUB
IP_EXTERN int ipnet_link_stub_if_init();
#endif /* INCLUDE_IPCOM_USE_LINK_STUB */

#ifdef INCLUDE_IPNET_USE_SOCKDEV
IP_EXTERN int ipnet_sockdev_if_init(Ipnet_netif *netif);
#endif /* INCLUDE_IPNET_USE_SOCKDEV */

#ifdef INCLUDE_IPIFCONFIG_CMD
IP_EXTERN int ipnet_cmd_ifconfig(int argc, char **argv);
#endif

#ifdef INCLUDE_IPSYSCTL_CMD
IP_EXTERN int ipnet_cmd_sysctl(int argc, char **argv);
#endif

#ifdef INCLUDE_IPQUEUE_CONFIG_CMD
IP_EXTERN int ipnet_cmd_qc(int argc, char **argv);
#endif

#ifdef INCLUDE_IPQOS_CMD
IP_EXTERN int ipnet_cmd_qos(int argc, char **argv);
#endif

#ifdef INCLUDE_IPROUTE_CMD
IP_EXTERN int ipnet_cmd_route(int argc, char **argv);
#endif

#ifdef INCLUDE_IPNETSTAT_CMD
IP_EXTERN int ipnet_cmd_netstat(int argc, char **argv);
#endif

#ifdef INCLUDE_IPNET_CTRL_CMD
IP_EXTERN int ipnet_cmd_ctrl(int argc, char **argv);
#endif

#ifdef INCLUDE_IPARP_CMD
IP_EXTERN int ipnet_cmd_arp(int argc, char **argv);
#endif

#ifdef INCLUDE_IPPING_CMD
IP_EXTERN int ipnet_cmd_ping(int argc, char **argv);
#endif

#ifdef INCLUDE_IPNDP_CMD
IP_EXTERN int ipnet_cmd_ndp(int argc, char **argv);
#endif

#ifdef INCLUDE_IPPING6_CMD
IP_EXTERN int ipnet_cmd_ping6(int argc, char **argv);
#endif

#ifdef INCLUDE_IPNAT_CMD
IP_EXTERN int ipnet_cmd_nat(int argc, char **argv);
#endif

#ifdef INCLUDE_IPNET_PCAP_CMD
IP_EXTERN int ipnet_cmd_pcap(int argc, char **argv);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      INTERFACE CONFIGURATION
 *===========================================================================
 *
 *   Found interface will be configured during stack boot by a configuration
 *   daemon process.
 *
 *  It is possible to have 0 or more IPv4 (inet) or/and IPv6 (inet6)
 *  addresses per interface. Addresses must be specified together with
 *  network bits 'addr/netbits'.
 *  inet6 specific information:
 *      * Duplicate addresses detection can be enabled for IPv6 by adding
 *        'tentative' after 'inet6'.
 *
 *  Only one interface can have a default gateway (gateway or gateway6).
 *  gateway6 specific information:
 *      * '::' can is used to tell IPNET to use routers announced on the
 *        link in router advertisement message
 *      * scoped addresses must be specified with scope (ex: 'fe80::1%lo0')
 */
IP_CONST char *ipnet_conf_interfaces[] =
{
#ifdef INCLUDE_IPNET_LOOPBACK_CONFIG
    IFCONFIG_LOOPBACK,
	IP_NULL,
#endif /* INCLUDE_IPNET_LOOPBACK_CONFIG */

    /* Configuration parameter for Ethernet interface #1 */
#ifdef INCLUDE_IPNET_IFCONFIG_1
	IFCONFIG_1,
	IP_NULL,   /* Interface configuration end marker */
#endif /* INCLUDE_IPNET_IFCONFIG_1 */

     /* Configuration parameter for Ethernet interface #2 */
#ifdef INCLUDE_IPNET_IFCONFIG_2
	IFCONFIG_2,
	IP_NULL,   /* Interface configuration end marker */
#endif /* INCLUDE_IPNET_IFCONFIG_2 */

    /* Configuration parameter for Ethernet interface #3 */
#ifdef INCLUDE_IPNET_IFCONFIG_3
	IFCONFIG_3,
	IP_NULL,   /* Interface configuration end marker */
#endif /* INCLUDE_IPNET_IFCONFIG_3 */

    /* Configuration parameter for Ethernet interface #4 */
#ifdef INCLUDE_IPNET_IFCONFIG_4
	IFCONFIG_4,
	IP_NULL,   /* Interface configuration end marker */
#endif /* INCLUDE_IPNET_IFCONFIG_4 */

	IP_NULL                                 /* Structure end marker */
};


/*
 *===========================================================================
 *                      BOOT CONFIGURATION
 *===========================================================================
 *
 * Specifies a list of shell commands to run at boot. This is run before
 * the addresses are set on the interfaces so pseudo interfaces could be
 * created here and be included in the 'ipnet_conf_interfaces' list.
 * Supported commands are currently 'route', 'ifconfig' and 'nat'
 */
IP_CONST char *ipnet_conf_boot_cmd[] =
{
    /* Ex: "qc queue add dev eth1 root handle 1 fifo limit 16", */
#ifdef INCLUDE_IPNET_USE_LOOPBACK
#ifdef INCLUDE_IPCOM_USE_INET
    "route add -inet -dev " IPNET_IF_LOOPBACK_NAME " -net -iface -nostatic -prefixlen 8 -reject 127.0.0.0 127.0.0.1",
#endif
#ifdef INCLUDE_IPCOM_USE_INET6
    "route add -inet6 -dev " IPNET_IF_LOOPBACK_NAME " -host -reject ::",
#ifdef IPNET_USE_RFC3879  /* defined in ipnet_config.h */
    /* RFC 3879 chapter 4
       ...
       However, router implementations SHOULD be configured to prevent
       routing of this prefix [fec0::/10] by default.
       ... */
    "route add -inet6 -dev " IPNET_IF_LOOPBACK_NAME " -net -prefixlen 10 -blackhole fec0::",
#endif
#endif /* INCLUDE_IPCOM_USE_INET6 */
#endif /* INCLUDE_IPNET_USE_LOOPBACK */
    IP_NULL
};

IP_PUBLIC Ip_u16 ipnet_conf_cache_bufsiz = IPNET_CACHE_BUFSIZE;

IP_CONST int ipnet_conf_inq_max = IPNET_MAX_INPUT_PKTS;

IP_CONST int ipnet_conf_reassembly_timeout = IPNET_REASSEMBLY_TIMEOUT;  /* RFC default is 60 seconds. */

#ifndef IPNET_MEMORY_LIMIT
#define IPNET_MEMORY_LIMIT 16 * 1024 * 1024; /* 16Mb by default */
#endif
IP_CONST unsigned ipnet_conf_memory_limit = IPNET_MEMORY_LIMIT;

/*
 *===========================================================================
 *                         SYSTEM VARIABLES
 *===========================================================================
 */
IP_STATIC IP_CONST Ipcom_sysvar ipnet_conf_sysvar[] =
{
#ifdef INCLUDE_IPNET_SOCKET
    /* Defines the lowest port number used as an ephemeral port
     * number (source port for sockets that has not been named
     * with a call to ipcom_bind()). */
    { "ipnet.sock.AnonPortMin", IPNET_SOCK_ANON_PORT_MIN},

    /* Defines the highest port number used as an ephemeral port
     * number (source port for sockets that has not been named
     * with a call to ipcom_bind()).*/
    { "ipnet.sock.AnonPortMax", IPNET_SOCK_ANON_PORT_MAX},

    /* The default size of the socket send buffer */
    { "ipnet.sock.DefaultSendBufSize", IPNET_SOCK_DEFAULT_SEND_BUFSIZE},

    /* The default size of the socket receive buffer */
    { "ipnet.sock.DefaultRecvBufSize", IPNET_SOCK_DEFAULT_RECV_BUFSIZE},

    /* Set to >0 to get notification on sockets bound to an
       address that has just been removed */
    { "ipnet.sock.AddrNotify", IPNET_SOCK_ADDR_NOTIFY},

    /* Set to >0 to get notification on sockets bound to an
       address that has just been removed */
    { "ipnet.route.MaxCloneCount", IPNET_ROUTE_MAX_CLONE_COUNT},
#endif /* INCLUDE_IPNET_SOCKET */

#ifdef INCLUDE_IPCOM_USE_INET
    /* The default hop limit for IPv4 packets */
    { "ipnet.inet.BaseHopLimit", INET_BASE_HOP_LIMIT },

    /* Send gratuitous ARP when addresses are added? */
    { "ipnet.inet.SendGratuitousArp", INET_SEND_GRATUITOUS_ARP },

    /*
     * != 0 if the stack shall automatically remove a configured
     * address upon duplicate address detection (Gratuitous ARP)
     */
    { "ipnet.inet.DeleteAddressOnDuplicateDetect", INET_DELETE_ADDR_ON_DUPLICATE_DETECT },

    /* Number of packets that can be waiting for IPv4 to link layer
       resolution to finish */
    { "ipnet.inet.MaxPending", INET_MAX_PKTS_PENDING},
    /* Number of unicast ARP request messages should be sent
       before starting to use multicast solicits */
    { "ipnet.inet.MaxUnicastSolicit", INET_MAX_UNICAST_SOLICIT},

    /* Number of ARP request messages that should be sent before starting to use
       application probes */
    { "ipnet.inet.MaxMulticastSolicit", INET_MAX_MULTICAST_SOLICIT},

    /* Number of resolve message that is sent to routing/netlink sockets if ARP probes
       failed (or is disabled) */
    { "ipnet.inet.MaxApplicationSolicit", INET_MAX_APP_SOLICIT},

    /* Duration, in seconds, that an entry in the ARP cache is in reachable state
       RFC 1122 chapter 2.3.2.1 recommends a value in the order of minutes and
       the equivalent feature in IPv6 (NDP) requires it to be 30 seconds.
       It is set to 1 hour by default in order to make performance testing with Spirent Smartbits
       possible. The Smartbits will only answer to ARP before each test starts.
    */
    { "ipnet.inet.BaseReachableTime", INET_BASE_REACHABLE_TIME},

    /* Number of seconds to wait between retransmits */
    { "ipnet.inet.BaseRetransmitTime", INET_BASE_RETRANSMIT_TIME},

    /* Duration, in seconds, to wait for a stale ARP entry to become reachable before
     * forcing a probe, packets will be sent using the stale entry during this time */
    { "ipnet.inet.DelayFirstProbeTime",  INET_DELAY_FIRST_PROBE_TIME},

    /* Duration, in seconds, that can pass before a stale entry in the ARP cache is deleted */
    { "ipnet.inet.NeighborCacheToLive", INET_NBR_CACHE_TO_LIVE_TIME},

    /* Duration, in seconds, that can pass before an entry in the destination cache is
       deleted, this lifetime is used by on entries created as a response to redirect
       and need frag ICMP messages */
    { "ipnet.inet.DstCacheToLive", INET_DST_CACHE_TO_LIVE_TIME},

    /* IcmpRatelimitBucketsize is the number of ICMP messages the stack is allowed to per
     * IcmpRatelimitInterval millisecond interval. Set IcmpRatelimitInterval to 0 to
     * disable the ratelimiter */
    { "ipnet.inet.IcmpRatelimitBucketsize", INET_ICMP_RATE_LIMIT_BUCKET_SIZE},
    { "ipnet.inet.IcmpRatelimitInterval", INET_ICMP_RATE_LIMIT_INTERVAL},

    /*  2 if a redirect should be sent and the original message should be forwarded (default)
     *  1 if a redirect should be sent but the original message should be discarded
     *  0 if no redirect should be sent but the original message should be forwarded
     * -1 if no redirect should be sent and the original message should be discarded
     * This configuration can also be set per interface by replacing "ipnet." with
     * "ifname." (ex: eth0.inet.IcmpRedirectSend) */
    { "ipnet.inet.IcmpRedirectSend", INET_ICMP_REDIRECT_SEND},

    /* != 0 if redirect messages should be accepted and processed */
    { "ipnet.inet.IcmpRedirectReceive", INET_ICMP_REDIRECT_RECEIVE},

    /* == 0 if the stack should answer to echo requests */
    { "ipnet.inet.IcmpIgnoreEchoRequest", INET_ICMP_IGNORE_ECHO_REQ},

    /* == 0 if the stack should answer to timestamp requests */
    { "ipnet.inet.IcmpIgnoreTimestampRequest", INET_ICMP_IGNORE_TIMESTAMP_REQ},

    /* != 0 if the stack should send time exceeded messages */
    { "ipnet.inet.IcmpSendTimeExceeded", INET_ICMP_SEND_TIME_EXCEEDED},

    /* != 0 if the stack should send destination unreachable */
    { "ipnet.inet.IcmpSendDestinationUnreachable",
      INET_ICMP_SEND_DST_UNREACHABLE},

    /* == 0 if the stack should process ICMPs with src addr =
     * broadcast (1 to disable SMURF attack)  */
    { "ipnet.inet.IcmpIgnoreBroadcast",   INET_ICMP_IGNORE_BROADCAST},

    /* == 1 if the stack shouldn't forward broadcasts(directed or otherwise)  */
    { "ipnet.inet.DontForwardBroadcast",  INET_DISABLE_BROADCAST_FWD},


#ifdef INCLUDE_IPPROXYARP
    /* != 0 if the stack shall do proxy arping for network routes tagged with the proxy arp flag.
     * This configuration can also be set per interface by replacing "ipnet." with
     * "ifname." (ex: eth0.inet.EnableNetworkProxyArp) */
    { "ipnet.inet.EnableNetworkProxyArp", INET_ENABLE_PROXY_ARP},

    /* != 0 if the stack automatically shall tag all interface address network routes as proxy arp
     *  This configuration can also be set per interface by replacing "ipnet." with
     * "ifname." (ex: eth0.inet.AutoProxyArp) */
    { "ipnet.inet.AutoProxyArp", INET_AUTO_PROXY_ARP},


#endif /* INCLUDE_IPPROXYARP */

    /*
     * The following configuration parameters support RFC3927, Dynamic
     * Configuration of IPv4 Link-Local Addresses. These configuration
     * parameters are only meaningful if IPNET is compiled with the
     * IPNET_USE_RFC3927 directive.
     */
#ifdef IPNET_USE_RFC3927    /* defined in ipnet_config.h */
#ifdef INCLUDE_IPAIP
    /* List of interfaces that can configure a link-local IPv4 address */
    { "ipnet.inet.linklocal.interfaces",  INET_IPAIP_IFNAME_LIST },

    /* All RFC 3927 options that follows can be configured globally or
     * per interface. Replace "ipnet." with "ifname." to configure per
     * interface. See ipnet_conf_sysvar_ext[] for per-interface configuration
     * support.
     */
#ifdef INCLUDE_IPAIP_GLOBAL_CONFIGS
    /* The host will wait [0..PROBE_WAIT] seconds before creating a link
     * local IPv4 and start checking it for uniqueness
     */
    { "ipnet.inet.linklocal.PROBE_WAIT", INET_IPAIP_PROBE_WAIT},

    /*
     * Number of times a link local address is checked for uniqness before
     * being assigned
     */
    { "ipnet.inet.linklocal.PROBE_NUM", INET_IPAIP_PROBE_NUM},

    /* The next probe is sent after [PROBE_MIN..PROBE_MAX] seconds */
    { "ipnet.inet.linklocal.PROBE_MIN", INET_IPAIP_PROBE_MIN },
    { "ipnet.inet.linklocal.PROBE_MAX", INET_IPAIP_PROBE_MAX},

    /* The address has been assigned to the interface when the annouce period starts */
    /* The time to */
    { "ipnet.inet.linklocal.ANNOUNCE_WAIT", INET_IPAIP_ANNOUNCE_WAIT},

    /* The number of announcements to send */
    { "ipnet.inet.linklocal.ANNOUNCE_NUM", INET_IPAIP_ANNOUNCE_NUM},

    /* Seconds between announcements */
    { "ipnet.inet.linklocal.ANNOUNCE_INTERVAL",
      INET_IPAIP_ANNOUNCE_INTERVAL},

    /* Max conflicts before rate limiting */
    { "ipnet.inet.linklocal.MAX_CONFLICTS",  INET_IPAIP_MAX_CONFLICTS},

    /* Delay between successive attempts */
    { "ipnet.inet.linklocal.RATE_LIMIT_INTERVAL",
      INET_IPAIP_RATE_LIMIT_INTERVAL},

    /* minimum interval between defensive ARPs */
    { "ipnet.inet.linklocal.DEFEND_INTERVAL",  INET_IPAIP_DEFEND_INTERVAL},
#endif /* INCLUDE_IPAIP_GLOBAL_CONFIGS */
#endif /* INCLUDE_IPAIP */
#endif /* IPNET_USE_RFC3927 */

#endif /* INCLUDE_IPCOM_USE_INET */

#ifdef INCLUDE_IPCOM_USE_INET6
    /* The default hop limit for IPv6 packets */
    { "ipnet.inet6.BaseHopLimit", INET6_BASE_HOP_LIMIT},

    /* Number of packets that can be waiting for IPv6 to link layer
       resolution to finish */
    { "ipnet.inet6.MaxPending", INET6_MAX_PKTS_PENDING},

    /* Number of unicast neighbor discovery messages should be sent for ND entry update
       before starting to use multicast solicit */
    { "ipnet.inet6.MaxUnicastSolicit", INET6_MAX_UNICAST_SOLICIT},

    /* Number of neighbor discovery messages should be resent before staring to
       use application solicit. setting this !=0 will cause IPv6 ready tests
       to fail, so change it only if you know what you are doing */
    { "ipnet.inet6.MaxMulticastSolicit", INET6_MAX_MULTICAST_SOLICIT},

    /* Number of resolve message that is sent to routing/netlink sockets if ND probes
       failed (or is disabled) */
    { "ipnet.inet6.MaxApplicationSolicit", INET6_MAX_APP_SOLICIT},

    /* Number of times the node should test the address for uniqueness before
     * assigning it to the interface (set to 0 to turn of Duplicate Address Detection)
     * This configuration can also be set per interface by replacing "ipnet." with
     * "ifname." (ex: eth0.inet6.DupAddrDetectTransmits) */
    { "ipnet.inet6.DupAddrDetectTransmits", INET6_DAD_TRANSMITS},

    /* Duration, in seconds, that an entry in the NDP cache is in reachable state */
    { "ipnet.inet6.BaseReachableTime",  INET6_BASE_REACHABLE_TIME},

    /*  Number of seconds to wait between retransmits */
    { "ipnet.inet6.BaseRetransmitTime", INET6_BASE_RETRANSMIT_TIME},

    /* Duration, in seconds, to wait for a stale ND entry to become reachable before
     * forcing a probe, packets will be sent using the stale entry during this time */
    { "ipnet.inet6.DelayFirstProbeTime", INET6_DELAY_FIRST_PROBE_TIME},

    /* Duration, in seconds, that can pass before a stale entry in the neighbor cache is deleted */
    { "ipnet.inet6.NeighborCacheToLive", INET6_NBR_CACHE_TO_LIVE_TIME},

    /* Duration, in seconds, that can pass before an entry in the destination cache is
       deleted, this lifetime is used by on entries created as a response to redirect
       and need frag ICMP messages */
    { "ipnet.inet6.DstCacheToLive", INET6_DST_CACHE_TO_LIVE_TIME},

    /* The number of seconds this router can be used as gateway after a router advertisement
     * had been sent, only used when configured as a router */
    { "ipnet.inet6.RouterLifetime", INET6_ROUTER_LIFETIME},

    /* IcmpRatelimitBucketsize is the number of ICMPv6 messages the stack is allowed to per
     * IcmpRatelimitInterval millisecond interval. Set IcmpRatelimitInterval to 0 to
     * disable the ratelimiter */
    { "ipnet.inet6.IcmpRatelimitBucketsize", INET6_ICMP_RATE_LIMIT_BUCKET_SIZE},
    { "ipnet.inet6.IcmpRatelimitInterval", INET6_ICMP_RATE_LIMIT_INTERVAL},

    /*  2 if a redirect should be sent and the original message should be forwarded (default)
     *  1 if a redirect should be sent but the original message should be discarded
     *  0 if no redirect should be sent but the original message should be forwarded
     * -1 if no redirect should be sent and the original message should be discarded
     * This configuration can also be set per interface by replacing "ipnet." with
     * "ifname." (ex: eth0.inet6.IcmpRedirectSend)
     */
    { "ipnet.inet6.IcmpRedirectSend", INET6_ICMP_REDIRECT_SEND},

    /* != 0 if redirect messages should be accepted and processed */
    { "ipnet.inet6.IcmpRedirectReceive", INET6_ICMP_REDIRECT_RECEIVE },

    /* == 0 if the stack should answer to echo requests */
    { "ipnet.inet6.IcmpIgnoreEchoRequest", INET6_ICMP_IGNORE_ECHO_REQ},

    /* != 0 if the stack should send time exceeded messages */
    { "ipnet.inet6.IcmpSendTimeExceeded", INET6_ICMP_SEND_TIME_EXCEEDED},

    /* != 0 if the stack should send destination unreachable */
    { "ipnet.inet6.IcmpSendDestinationUnreachable",
                                         INET6_ICMP_SEND_DST_UNREACHABLE},

    /* Enable/disable processing of router advertisements message */
    { "ipnet.inet6.AcceptRtAdv", INET6_ACCEPT_RTADV},

    /* Enable/disable address autoconfiguration  */
    /* This configuration can also be set per interface by replacing "ipnet." with
     * "ifname." (ex: eth0.inet6.AutoConf) */
    { "ipnet.inet6.AutoConf", INET6_AUTO_CONFIG},

#ifdef IPNET_USE_RFC3041  /* defined in ipnet_config.h */
#if defined(INCLUDE_IPNET6_AUTOCONF_PRIVACY_EXT) && \
    defined(INCLUDE_IPNET6_PRIVACY_GLOBAL_CONFIGS)
    /*
     * All RFC3041 configuration options can also be set per interface by
     * replacing ipnet." with "ifname." (ex: eth0.inet6.TempValidLifetime).
     * See ipnet_conf_sysvar_ext[] for per-interface configuration support.
     */
    /* Enable/disable privacy extensions */
    { "ipnet.inet6.EnablePrivacyExtensions", INET6_ENABLE_PRIVACY_EXTENSONS},

    /*  Maximum lifetime, in seconds, of temporary addresses
     *  default: 1 week */
    { "ipnet.inet6.TempValidLifetime", INET6_TEMP_VALID_LIFETIME},

    /*  Maximum preferred time, in seconds, of temporary addresses
     *  default: 1 day */
    { "ipnet.inet6.TempPreferredLifetime", INET6_TEMP_PREFERRED_LIFETIME},

    /*  Number of seconds before the current temprary address goes
     *  non-preferred, that a new address will be generated.
     *  default: 5 seconds by RFC, but that is very tight so 30 seconds is used instead */
    { "ipnet.inet6.RegenAdvance", INET6_REGEN_ADVANCE},
#endif /* INCLUDE_IPNET6_AUTOCONF_PRIVACY_EXT */
#endif /* IPNET_USE_RFC3041 */

    /* Enable to allow processing of IPv6 Routing Header type 0, it is
     * disabled by default since it is a potential security problem */
    { "ipnet.inet6.RtHdr0", INET6_RTHDR0 },


#ifdef INCLUDE_IPSEND
    /* The path to the RSA key used to sign secure NDP messages */
    { "ipnet.inet6.send.rsa_key_file",            IPNET_SEND_KEY_FILE },

    /* The path to X509 certificate (PEM format) file for the RSA key above */
    { "ipnet.inet6.send.cert_file",            IPNET_SEND_CERT_FILE },

    /* The path to the file holding trusted CA certificates in PEM format.
     * The order of the certificates in the file is not relevant. */
    { "ipnet.inet6.send.ca_cert_file",            IPNET_SEND_CA_FILE },

    /* The path to the file used for intermediate CAs in PEM format, used to
     * create a certificate chain. */
    { "ipnet.inet6.send.intermediate_ca_cert_file", IPNET_SEND_INTERMEDIATE_CA_FILE },

    /* The allowed difference between local time and the timestamp in received
     * signed NDP messages */
    { "ipnet.inet6.send.timestamp_delta",         IPNET_SEND_TIMESTAMP_DELTA },

    { "ipnet.inet6.send.mode",         IPNET_SEND_MODE },
    /*  0 - off
     *  1 - host,   a certification path solication message will be sent when
     *              a router advertisement is received
     *  2 - router, will send certification path advertisments in response to
     *              cert path solicitations */
    { "ipnet.inet6.send.cert_path_mode", IPNET_SEND_CERT_PATH_MODE },
#endif /* INCLUDE_IPSEND */
#endif /* INCLUDE_IPCOM_USE_INET6 */

#ifdef INCLUDE_IPNET_USE_NAT
    /* The timeout until an icmp mapping expires */
    { "ipnet.nat.timeout.icmp", IPNAT_ICMP_MAPPING_TIMEOUT},

    /* The timeout until an udp mapping expires */
    { "ipnet.nat.timeout.udp", IPNAT_UDP_MAPPING_TIMEOUT},

    /* The timeout until a tcp mapping expires */
    { "ipnet.nat.timeout.tcp", IPNAT_TCP_MAPPING_TIMEOUT},

    /* The timeout until other mappings expires */
    { "ipnet.nat.timeout.other", IPNAT_OTHER_MAPPING_TIMEOUT},

    /* The maximum number of active NAT mappings */
    { "ipnet.nat.capacity.mappings", IPNAT_MAX_MAPPING},

    /* Start of the automatically NATed ports interval */
    { "ipnet.nat.autoport.start", IPNAT_AUTOPORT_START_INTERVAL},

    /* End of the automatically NATed ports interval */
    { "ipnet.nat.autoport.end", IPNAT_AUTOPORT_END_INTERVAL},
#endif  /* INCLUDE_IPNET_USE_NAT */

    { IP_NULL, IP_NULL }
};

/* Interface specific list configurations for IPv4 autoIP */
IP_CONST Ipcom_sysvar_ext ipnet_conf_sysvar_ext[] =
{
#ifdef INCLUDE_IPCOM_USE_INET

#ifdef INCLUDE_IPPROXYARP
    /*
     * != 0 if the stack shall do proxy arping for network routes tagged with
     * the proxy arp flag. This parameter can be configure on a per-interface
     * basis in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. E.g, "eth0=1".
     */
    { IP_NULL, "inet.EnableNetworkProxyArp", INET_IFLIST_ENABLE_PROXY_ARP},

    /*
     * != 0 if the stack automatically shall tag all interface address network
     * routes as proxy arp. This parameter can be configure on a per-interface
     * basis in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. E.g, "eth0=1"
     */
    { IP_NULL, "inet.AutoProxyArp", INET_IFLIST_AUTO_PROXY_ARP},
#endif /* INCLUDE_IPPROXYARP */

#ifdef IPNET_USE_RFC3927    /* defined in ipnet_config.h */
#if defined(INCLUDE_IPAIP) && defined(INCLUDE_IPAIP_INTERFACE_CONFIGS)

    /*
     * Time (in seconds) the host will wait before creating a link local IPv4
     * and start checking it for uniqueness. This parameter can be specified
     * for each interface specified in the INET_IPAIP_IFNAME_LIST in the form
     * of  "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=1".
     */
    { IP_NULL, "inet.linklocal.PROBE_WAIT", INET_IPAIP_IFLIST_PROBE_WAIT},

    /*
     * Number of times a link local address is checked for uniqness before
     * being assigned. This parameter can be specified for each interface
     * specified in the INET_IPAIP_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=3".
     */
    { IP_NULL, "inet.linklocal.PROBE_NUM", INET_IPAIP_IFLIST_PROBE_NUM},

    /*
     * The minimum time (in seconds) where the next probe is sent. This
     * parameter can be specified for each interface specified in the
     * INET_IPAIP_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair
     * of "<ifparam>=<value>" is semicolon separated. E.g, "eth0=1".
     */
    { IP_NULL, "inet.linklocal.PROBE_MIN", INET_IPAIP_IFLIST_PROBE_MIN },

    /*
     * The maximum time (in seconds) where the next probe is sent. This
     * parameter can be specified for each interface specified in the
     * INET_IPAIP_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair
     * of "<ifparam>=<value>" is semicolon separated. E.g, "eth0=3".
     */
    { IP_NULL, "inet.linklocal.PROBE_MAX", INET_IPAIP_IFLIST_PROBE_MAX},

    /*
     * The time to wait (in seconds) before the assigned IPv4 address is
     * announced. This parameter can be specified for each interface specified
     * in the INET_IPAIP_IFNAME_LIST in the form of "<ifparam>=<value>". Each
     * pair of "<ifparam>=<value>" is semicolon separated. E.g, "eth0=2".
     */
    { IP_NULL, "inet.linklocal.ANNOUNCE_WAIT",
      INET_IPAIP_IFLIST_ANNOUNCE_WAIT},

    /*
     * The number of announcements to send. This parameter can be specified
     * for each interface specified in the INET_IPAIP_IFNAME_LIST in the form
     * of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=2".
     */
    { IP_NULL, "inet.linklocal.ANNOUNCE_NUM",
      INET_IPAIP_IFLIST_ANNOUNCE_NUM},

    /*
     * The number of seconds between announcements. This parameter can be
     * specified for each interface specified in the INET_IPAIP_IFNAME_LIST in
     * the form of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is
     * semicolon separated. E.g, "eth0=2".
     */
    { IP_NULL, "inet.linklocal.ANNOUNCE_INTERVAL",
      INET_IPAIP_IFLIST_ANNOUNCE_INTERVAL},

    /*
     * Max conflicts before rate limiting. This parameter can be specified for
     * each interface specified in the INET_IPAIP_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=10".
     */
    { IP_NULL, "inet.linklocal.MAX_CONFLICTS",
      INET_IPAIP_IFLIST_MAX_CONFLICTS},

    /*
     * Delay between successive attempts. This parameter can be specified for
     * each interface specified in the INET_IPAIP_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=60".
     */
    { IP_NULL, "inet.linklocal.RATE_LIMIT_INTERVAL",
      INET_IPAIP_IFLIST_RATE_LIMIT_INTERVAL},

    /*
     * Minimum interval between defensive ARPs. This parameter can be specified
     * for each interface specified in the INET_IPAIP_IFNAME_LIST in the form
     * of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=10".
     */
    { IP_NULL, "inet.linklocal.DEFEND_INTERVAL",
      INET_IPAIP_IFLIST_DEFEND_INTERVAL},
#endif /* INCLUDE_IPAIP && INCLUDE_IPAIP_INTERFACE_CONFIGS */
#endif /* IPNET_USE_RFC3927 */

#endif /* INCLUDE_IPCOM_USE_INET */

#ifdef INCLUDE_IPCOM_USE_INET6

#ifdef IPNET_USE_RFC3041     /* defined in ipnet_config.h */
#if defined(INCLUDE_IPNET6_AUTOCONF_PRIVACY_EXT) && \
    defined(INCLUDE_IPNET6_PRIVACY_INTERFACE_CONFIGS)

    /*
     * All RFC3041 configuration options can also be set per interface by
     * replacing "ipnet." with "ifname." (ex: eth0.inet6.TempValidLifetime)
     */

    /*
     * Enable/disable privacy extensions. This parameter can be configure on
     * a per-interface basis in the form of "<ifparam>=<value>". Each pair
     * of "<ifparam>=<value>" is semicolon separated. E.g, "eth0=1".
     */
    { IP_NULL, "inet6.EnablePrivacyExtensions",
      INET6_IFLIST_ENABLE_PRIVACY_EXTENSONS},

    /*
     * Maximum lifetime, in seconds, of temporary addresses. default: 1 week
     * This parameter can be configure on a per-interface basis in the form
     * of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=604800".
     */
    { IP_NULL, "inet6.TempValidLifetime", INET6_IFLIST_TEMP_VALID_LIFETIME},

    /* Maximum preferred time, in seconds, of temporary addresses.
     * default: 1 day
     * This parameter can be configure on a per-interface basis in the form
     * of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=86400".
     */
    { IP_NULL, "inet6.TempPreferredLifetime",
      INET6_IFLIST_TEMP_PREFERRED_LIFETIME},

    /*
     * Number of seconds before the current temprary address goes
     * non-preferred, that a new address will be generated.
     * default: 5 seconds by RFC, but that is very tight so 30 seconds is
     * used instead.
     * This parameter can be configure on a per-interface basis in the form
     * of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. E.g, "eth0=30".
     */
    { IP_NULL, "inet6.RegenAdvance", INET6_IFLIST_REGEN_ADVANCE},
#endif /* INCLUDE_IPNET6_AUTOCONF_PRIVACY_EXT */
#endif /* IPNET_USE_RFC3041 */

#endif /* INCLUDE_IPCOM_USE_INET6 */

    { IP_NULL, IP_NULL, IP_NULL }

};


/*
 *===========================================================================
 *                         SOCKET
 *===========================================================================
 */
#ifdef INCLUDE_IPNET_SOCKET
IP_CONST int ipnet_conf_max_sockets = IPNET_SOCK_MAX;
#endif /* INCLUDE_IPNET_SOCKET */

/*
 *===========================================================================
 *                         LINK
 *===========================================================================
 */
IP_CONST Ipnet_conf_link_layer_init ipnet_conf_link_layer[] =
{
#ifdef INCLUDE_IPCOM_USE_ETHERNET
    { IP_IFT_ETHER,     ipnet_eth_if_init },
#ifdef INCLUDE_IPNET_USE_VLAN
    { IP_IFT_L2VLAN,    ipnet_vlan_if_init },
#endif
#endif /* INCLUDE_IPCOM_USE_ETHERNET */
#ifdef INCLUDE_IPNET_USE_PPP
    { IP_IFT_PPP,       ipnet_ppp_if_init },
#endif
#ifdef INCLUDE_IPNET_USE_LOOPBACK
    { IP_IFT_LOOP,      ipnet_loopback_if_init },
#endif
#ifdef INCLUDE_IPNET_USE_TUNNEL
    { IP_IFT_TUNNEL,    ipnet_tunnel_if_init },
#endif
#ifdef INCLUDE_IPNET_USE_WLAN
    { IP_IFT_IEEE80211, ipnet_wlan_if_init },
#endif
#ifdef INCLUDE_IPMPLS_TUNNEL
    { IP_IFT_MPLS_TUNNEL, ipnet_mpls_if_init },
#endif
#ifdef INCLUDE_IPNET_USE_SOCKDEV
    { IP_IFT_SOCKDEV,    ipnet_sockdev_if_init },
#endif
#ifdef INCLUDE_IPCOM_USE_LINK_STUB
    { IP_IFT_LINK_STUB,   ipnet_link_stub_if_init },
#endif
    { 0,            IP_NULL  }
};


/*
 *===========================================================================
 *                         NETIF
 *===========================================================================
 */

#ifdef INCLUDE_IPCOM_USE_INET
/* RFC 1191 chapter 3 p4
 * A host MUST never reduce its estimate of the Path MTU below 68 octets.
 */
IP_CONST Ip_u16 ipnet_conf_ip4_min_mtu = INET_MIN_MTU_SIZE;
#endif /* INCLUDE_IPCOM_USE_INET */

#ifdef INCLUDE_IPCOM_USE_INET6
IP_CONST Ip_u16 ipnet_conf_ip6_min_mtu = INET6_MIN_MTU_SIZE;
#endif /* INCLUDE_IPCOM_USE_INET6 */

#ifdef INCLUDE_IPNET_IFQUEUE_FIFO_DEFAULT
IP_PUBLIC int ipnet_qc_unif_fifo_config (const char * ifname);
int (*ipnet_qc_unif_config_hook) (const char * ifname) = ipnet_qc_unif_fifo_config;
#else
int (*ipnet_qc_unif_config_hook) (const char * ifname) = IP_NULL;
#endif /* INCLUDE_IPNET_IFQUEUE_FIFO_DEFAULT */

/*
 * Maximum number of partially reassembled fragmentation chains that
 * is allowed to be present at any time. The oldest fragmentation list
 * will be dropped if this this limit is exceeded.
 */
IP_CONST unsigned ipnet_conf_max_reassembly_list_len = IPNET_MAX_REASSEMBLY_LIST_LEN;

/*
 * Maximum number of fragments each IP datagram can consist of. The
 * partly reassembled IP-datagrams will be dropped if it exceeds this
 * limit.
 */
IP_CONST unsigned ipnet_conf_max_dgram_frag_list_len = IPNET_MAX_DGRAM_FRAG_LIST_LEN;

/*
 ****************************************************************************
 * 7                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_register_shell_commands
 *===========================================================================
 * Description: Registers all IPNET shell commands
 * Parameters:
 * Returns:
 *
 */
#ifdef INCLUDE_IPCOM_SHELL_CMD
IP_STATIC Ip_err
ipnet_register_shell_commands(void)
{
    Ip_err err = IPCOM_SUCCESS;

#ifdef INCLUDE_IPIFCONFIG_CMD
    err = ipcom_shell_add_cmd("ifconfig", "ifconfig [ options ] <modifier...>",
                              "IPNET interface configuration",
                              ipnet_cmd_ifconfig,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPIFCONFIG_CMD */

#ifdef INCLUDE_IPSYSCTL_CMD
    err = ipcom_shell_add_cmd("sysctl", "sysctl [ options ] <variable...>",
                              "IPNET sysctl configuration", ipnet_cmd_sysctl,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPSYSCTL_CMD */

#ifdef INCLUDE_IPQUEUE_CONFIG_CMD
    err = ipcom_shell_add_cmd("qc", "qc [ options ]",
                              "IPNET output queue configuration", ipnet_cmd_qc,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPQUEUE_CONFIG_CMD */

#ifdef INCLUDE_IPQOS_CMD
    err = ipcom_shell_add_cmd("qos", "qos [ options ]",
                              "IPNET Quality of Service configuration",
                               ipnet_cmd_qos,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPQOS_CMD */

#ifdef INCLUDE_IPROUTE_CMD
    err = ipcom_shell_add_cmd("route", "route [ options ] <modifier...>",
                              "IPNET route table control", ipnet_cmd_route,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPROUTE_CMD */

#ifdef INCLUDE_IPNETSTAT_CMD
    err = ipcom_shell_add_cmd("netstat", "netstat [ options ]",
                              "IPNET socket and route stats", ipnet_cmd_netstat,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPNETSTAT_CMD */

#ifdef INCLUDE_IPNET_CTRL_CMD
    err = ipcom_shell_add_cmd("ipnet_ctrl", "ipnet_ctrl [ options ]",
                              "IPNET runtime debugger util", ipnet_cmd_ctrl,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_LARGE);
    if (err < 0) return err;
#endif /* INCLUDE_IPNET_CTRL_CMD */

#ifdef INCLUDE_IPARP_CMD
    err = ipcom_shell_add_cmd("arp", "arp [ options ] <modifier...>",
                              "IPNET arp control", ipnet_cmd_arp,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPARP_CMD */

#ifdef INCLUDE_IPPING_CMD
    err = ipcom_shell_add_cmd("ping", "ping [ options ] <modifier...>",
                              "IPNET ping utility", ipnet_cmd_ping,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPPING_CMD */

#ifdef INCLUDE_IPNDP_CMD
    err = ipcom_shell_add_cmd("ndp", "ndp [ options ] <modifier...>",
                              "IPNET ndp control", ipnet_cmd_ndp,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPNDP_CMD */

#ifdef INCLUDE_IPPING6_CMD
    err = ipcom_shell_add_cmd("ping6", "ping6 [ options ] <modifier...>",
                              "IPNET ping6 utility", ipnet_cmd_ping6,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err < 0) return err;
#endif /* INCLUDE_IPPING6_CMD */

#ifdef INCLUDE_IPNAT_CMD
    err = ipcom_shell_add_cmd("nat", "nat [ options ] <modifier...>",
                              "NAT control", ipnet_cmd_nat,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_SMALL);
    if (err < 0) return err;
#endif /* INCLUDE_IPNAT_CMD */

#ifdef INCLUDE_IPNET_PCAP_CMD
    err = ipcom_shell_add_cmd("pcap", "pcap [ options ] start|stop",
                              "Packet capture utility", ipnet_cmd_pcap,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_SMALL);
    if (err < 0) return err;
#endif /* INCLUDE_IPNET_PCAP_CMD */

    return err;
}
#endif /* INCLUDE_IPCOM_SHELL_CMD */


/*
 *===========================================================================
 *                    ipnet_configure
 *===========================================================================
 * Description: Configure IPNET
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_configure(void)
{
    Ip_err retval = IPCOM_SUCCESS;

    /* Set IPNET system variables. */
    if (ipcom_sysvar_setv(ipnet_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
        /* now configure the interface-specific sysvars */
        retval = ipcom_sysvar_ext_setv(ipnet_conf_sysvar_ext, 0);
    }

#ifdef INCLUDE_IPCOM_SHELL_CMD
    if (retval == IPCOM_SUCCESS)
    {
        /* Add IPNET shell commands: */
        retval = ipnet_register_shell_commands();
    }
#endif /* INCLUDE_IPCOM_SHELL_CMD */

    return retval;
}

#ifdef INCLUDE_IPNET_IFQUEUE_FIFO_DEFAULT

/*
 *===========================================================================
 *                    ipnet_qc_min
 *===========================================================================
 * Description: minimal interface queue configuration (root fifo queues only)
 * Parameters:
 * Returns:
 *
 */

#ifdef INCLUDE_IPQUEUE_CONFIG_CMD

IP_PUBLIC int
ipnet_qc_min(const char * ifname, int qlimit)
{
    char * argv [10];
    char numbuf [24];
    int argc;

    if (ifname == IP_NULL || qlimit < 0)
	return -1;

    argv[0] = "qc";
    argv[1] = "queue";
    argv[3] = "dev";
    argv[4] = (char *)ifname; /* ipnet_cmd_qc won't modify the characters */
    argv[5] = "root";

    if (qlimit == 0)
    {
        argv[2] = "del";
	argv[6] = IP_NULL;
	argc = 6;
    }
    else
    {
        argv[2] = "add";
	argv[6] = "fifo";
	argv[7] = "limit";
	ipcom_sprintf (numbuf, "%d", qlimit);
	argv[8] = numbuf;
	argv[9] = IP_NULL;
	argc = 9;
    }
    return ipnet_cmd_qc (argc, argv);
}

#else

IP_PUBLIC int
ipnet_qc_min(const char * ifname, int qlimit)
{
    int fd;
    struct Ipnet_ifqueue ifq;
    int ret;

    if (ifname == IP_NULL || qlimit < 0)
	return -1;

    ipcom_memset (&ifq, 0, sizeof (ifq));

    if (ipcom_strlcpy (ifq.ifq_name, ifname, sizeof (ifq.ifq_name)) >=
	sizeof (ifq.ifq_name))
	return -1;

#ifdef IPCOM_USE_INET
    fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
#else
    fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
#endif
    if (fd == IP_SOCKERR)
	return -1;

    ipcom_strcpy (ifq.ifq_type, qlimit == 0 ? "none" : "fifo");

    ifq.ifq_id = IP_IFQ_ID_NONE;
    ifq.ifq_parent_id = IP_IFQ_ID_NONE;
    ifq.ifq_data.fifo.fifo_limit = qlimit;

    ret = ipcom_socketioctl(fd, IPNET_SIOCSIFQUEUE, &ifq);

    ipcom_socketclose (fd);
    return ret;
}

#endif

IP_PUBLIC int
ipnet_qc_unif_fifo_config(const char * ifname)
{
    int i;
    static char * excludes[] = IFQUEUE_FIFO_EXCLUDE_IFACES;

    /* Don't add a queue for the excluded interfaces (usually lo0) */
    for (i = 0; i < NELEMENTS (excludes); ++i)
	if (ipcom_strcmp (ifname, excludes[i]) == 0)
	    return 0;
    /*
     * We could potentially more add logic here to set different queue
     * configurations based upon the interface name. For now,
     * we support only setting the same root fifo configuration by default
     * for all non-excluded interfaces.
     */
    return ipnet_qc_min(ifname, IFQUEUE_FIFO_QLIMIT);
}
#endif /* INCLUDE_IPNET_IFQUEUE_FIFO_DEFAULT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
