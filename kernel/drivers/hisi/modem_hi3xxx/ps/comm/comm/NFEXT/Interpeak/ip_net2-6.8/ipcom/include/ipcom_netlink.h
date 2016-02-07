
/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_netlink.h,v $ $Revision: 1.12 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_netlink.h,v $
 *     $Author: ulf $ $Date: 2009-06-08 11:04:41 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Joel Hortelius <joel.hortelius@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_NETLINK_H
#define IPCOM_NETLINK_H

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
 *                    NETLINK DEFINITIONS
 *===========================================================================
 */
#define IP_NETLINK_ROUTE           0       /* Routing/device hook                          */

struct Ip_sockaddr_nl
{
#ifdef IPCOM_USE_SA_LEN
    Ip_u8           nl_len;
#endif
    Ip_sa_family_t  nl_family;      /* AF_NETLINK   */
    Ip_u16          nl_pad;         /* zero         */
    Ip_u32          nl_pid;         /* process pid  */
    Ip_u32          nl_groups;      /* multicast groups mask */
#ifdef IP_PORT_VXWORKS
    Ip_u32          __nl_pad_to_sockaddr_size;
#endif
};

struct Ip_nlmsghdr
{
    Ip_u32          nlmsg_len;      /* Length of message including header */
    Ip_u16          nlmsg_type;     /* Message content */
    Ip_u16          nlmsg_flags;    /* Additional flags */
    Ip_u32          nlmsg_seq;      /* Sequence number */
    Ip_u32          nlmsg_pid;      /* Sending process PID */
};

/* Flags values */

#define IP_NLM_F_REQUEST           1       /* It is request message.       */
#define IP_NLM_F_MULTI             2       /* Multipart message, terminated by NLMSG_DONE */
#define IP_NLM_F_ACK               4       /* Reply with ack, with zero or error code */
#define IP_NLM_F_ECHO              8       /* Echo this request            */

/* Modifiers to GET request */
#define IP_NLM_F_ROOT      0x100   /* specify tree root    */
#define IP_NLM_F_MATCH     0x200   /* return all matching  */
#define IP_NLM_F_ATOMIC    0x400   /* atomic GET           */
#define IP_NLM_F_DUMP      (IP_NLM_F_ROOT|IP_NLM_F_MATCH)

/* Modifiers to NEW request */
#define IP_NLM_F_REPLACE   0x100   /* Override existing            */
#define IP_NLM_F_EXCL      0x200   /* Do not touch, if it exists   */
#define IP_NLM_F_CREATE    0x400   /* Create, if it does not exist */
#define IP_NLM_F_APPEND    0x800   /* Add to end of list           */

/* Modifiers to ignore VR */
#define IP_NLM_F_VR_UNSPEC 0x8000



#define IP_NLMSG_ALIGNTO	        4
#define IP_NLMSG_ALIGN(len)      (((len)+IP_NLMSG_ALIGNTO-1) & ~(IP_NLMSG_ALIGNTO-1))
#define IP_NLMSG_LENGTH(len)     ((len) + IP_NLMSG_ALIGN(sizeof(struct Ip_nlmsghdr)))
#define IP_NLMSG_SPACE(len)      IP_NLMSG_ALIGN(IP_NLMSG_LENGTH(len))
#define IP_NLMSG_DATA(nlh)       ((void*)(((char*)nlh) + IP_NLMSG_LENGTH(0)))
#define IP_NLMSG_NEXT(nlh,len)   ((len) -= IP_NLMSG_ALIGN((nlh)->nlmsg_len), \
                                  (struct Ip_nlmsghdr*)(((char*)(nlh)) + IP_NLMSG_ALIGN((nlh)->nlmsg_len)))
#define IP_NLMSG_OK(nlh,len)     ((len) > 0 \
                                     && (nlh)->nlmsg_len >= sizeof(struct Ip_nlmsghdr) \
                                     && (nlh)->nlmsg_len <= (len))
#define IP_NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - IP_NLMSG_SPACE((len)))

#define IP_NLMSG_NOOP		    0x1	/* Nothing */
#define IP_NLMSG_ERROR		    0x2	/* Error */
#define IP_NLMSG_DONE		    0x3	/* End of a dump */
#define IP_NLMSG_OVERRUN		0x4	/* Data lost */

/* IP_NLMSG_ERROR */
struct Ip_nlmsgerr
{
	int		                error;
	struct Ip_nlmsghdr      msg;
};

#define IP_NETLINK_ADD_MEMBERSHIP  1
#define IP_NETLINK_DROP_MEMBERSHIP 2
#define IP_NETLINK_PKTINFO         3

struct Ip_nl_pktinfo
{
        Ip_u32   group;
};

#define IP_NETLINK_UNCONNECTED  0
#define IP_NETLINK_CONNECTED    1

struct Ip_nlattr
{
        Ip_u16           nla_len;
        Ip_u16           nla_type;
};

#define IP_NLA_ALIGNTO             4
#define IP_NLA_ALIGN(len)          (((len) + IP_NLA_ALIGNTO - 1) & ~(IP_NLA_ALIGNTO - 1))
#define IP_NLA_HDRLEN              ((int) IP_NLA_ALIGN(sizeof(struct Ip_nlattr)))


/*
 *===========================================================================
 *                    RTNETLINK DEFINITIONS
 *===========================================================================
 */

enum {
        IP_RTM_BASE        = 16,
        IP_RTM_NEWLINK     = 16,
        IP_RTM_DELLINK,
        IP_RTM_GETLINK,
        IP_RTM_SETLINK,

        IP_RTM_NEWADDR     = 20,
        IP_RTM_DELADDR,
        IP_RTM_GETADDR,

        IP_RTM_NEWROUTE    = 24,
        IP_RTM_DELROUTE,
        IP_RTM_GETROUTE,

        IP_RTM_NEWNEIGH    = 28,
        IP_RTM_DELNEIGH,
        IP_RTM_GETNEIGH,

        IP_RTM_NEWRULE     = 32,
        IP_RTM_DELRULE,
        IP_RTM_GETRULE,

        IP_RTM_NEWQDISC    = 36,
        IP_RTM_DELQDISC,
        IP_RTM_GETQDISC,

        IP_RTM_NEWTCLASS   = 40,
        IP_RTM_DELTCLASS,
        IP_RTM_GETTCLASS,

        IP_RTM_NEWTFILTER  = 44,
        IP_RTM_DELTFILTER,
        IP_RTM_GETTFILTER,

        IP_RTM_NEWACTION   = 48,
        IP_RTM_DELACTION,
        IP_RTM_GETACTION,
        IP_RTM_NEWPREFIX   = 52,
        IP_RTM_GETPREFIX   = 54,

        IP_RTM_GETMULTICAST = 58,

        IP_RTM_GETANYCAST  = 62,

        IP_RTM_NEWNEIGHTBL = 64,
        IP_RTM_GETNEIGHTBL = 66,
        IP_RTM_SETNEIGHTBL,

        /* CONFIG INTERPEAK >>> */
        IP_RTM_NEWVR       = 100,
        IP_RTM_DELVR,
        IP_RTM_GETVR,
        IP_RTM_CHANGEVR,

        IP_RTM_NEWMIP      = 104,
        IP_RTM_DELMIP,
        IP_RTM_GETMIP,
        IP_RTM_SETMIP,

        IP_RTM_NEWSEND     = 112,
        IP_RTM_GETSEND     = 114,
        IP_RTM_SEND_SIGN_REQ     = 115,

        /* <<< CONFIG INTERPEAK */

        __IP_RTM_MAX
};
#define IP_RTM_MAX         (((__IP_RTM_MAX + 3) & ~3) - 1)

#define IP_RTM_NR_MSGTYPES (IP_RTM_MAX + 1 - IP_RTM_BASE)
#define IP_RTM_NR_FAMILIES (IP_RTM_NR_MSGTYPES >> 2)
#define IP_RTM_FAM(cmd)    (((cmd) - IP_RTM_BASE) >> 2)

struct Ip_rtattr
{
        Ip_u16  rta_len;
        Ip_u16  rta_type;
};

#define IP_RTA_ALIGNTO              4
#define IP_RTA_ALIGN(len)           ( ((len)+IP_RTA_ALIGNTO-1) & ~(IP_RTA_ALIGNTO-1) )
#define IP_RTA_OK(rta,len)          ((len) >= (int)sizeof(struct Ip_rtattr) && \
                                    (rta)->rta_len >= sizeof(struct Ip_rtattr) && \
                                    (rta)->rta_len <= (len))
#define IP_RTA_NEXT(rta,attrlen)    ((attrlen) -= IP_RTA_ALIGN((rta)->rta_len), \
                                    (struct Ip_rtattr*)(((char*)(rta)) + IP_RTA_ALIGN((rta)->rta_len)))
#define IP_RTA_LENGTH(len)          (IP_RTA_ALIGN(sizeof(struct Ip_rtattr)) + (len))
#define IP_RTA_SPACE(len)           IP_RTA_ALIGN(IP_RTA_LENGTH(len))
#define IP_RTA_DATA(rta)            ((void*)(((char*)(rta)) + IP_RTA_LENGTH(0)))
#define IP_RTA_PAYLOAD(rta)         ((int)((rta)->rta_len) - IP_RTA_LENGTH(0))

struct Ip_rtmsg
{
        Ip_u8           rtm_family;
        Ip_u8           rtm_dst_len;
        Ip_u8           rtm_src_len;
        Ip_u8           rtm_tos;

        Ip_u8           rtm_table;      /* Routing table id */
        Ip_u8           rtm_protocol;   /* Routing protocol; see below  */
        Ip_u8           rtm_scope;      /* See below */
        Ip_u8           rtm_type;       /* See below    */

        Ip_u32          rtm_flags;
};

enum
{
        IP_RTN_UNSPEC,
        IP_RTN_UNICAST,            /* Gateway or direct route      */
        IP_RTN_LOCAL,              /* Accept locally               */
        IP_RTN_BROADCAST,          /* Accept locally as broadcast,
                                   send as broadcast */
        IP_RTN_ANYCAST,            /* Accept locally as broadcast,
                                   but send as unicast */
        IP_RTN_MULTICAST,          /* Multicast route              */
        IP_RTN_BLACKHOLE,          /* Drop                         */
        IP_RTN_UNREACHABLE,        /* Destination is unreachable   */
        IP_RTN_PROHIBIT,           /* Administratively prohibited  */
        IP_RTN_THROW,              /* Not in this table            */
        IP_RTN_NAT,                /* Translate this address       */
        IP_RTN_XRESOLVE,           /* Use external resolver        */


        /* CONFIG INTERPEAK >>> */
    	IP_RTN_PROXY        = 32,  /* Proxy ARP route */
        IP_RTN_CLONE,              /* Cloning route */
        /* <<< CONFIG INTERPEAK */

        __IP_RTN_MAX
};

#define IP_RTN_MAX (__IP_RTN_MAX - 1)

#define IP_RTPROT_UNSPEC   0
#define IP_RTPROT_REDIRECT 1       /* Route installed by ICMP redirects;
                                   not used by current IPv4 */
#define IP_RTPROT_KERNEL   2       /* Route installed by kernel            */
#define IP_RTPROT_BOOT     3       /* Route installed during boot          */
#define IP_RTPROT_STATIC   4       /* Route installed by administrator     */

#define IP_RTPROT_GATED    8       /* Apparently, GateD */
#define IP_RTPROT_RA       9       /* RDISC/ND router advertisements */
#define IP_RTPROT_MRT      10      /* Merit MRT */
#define IP_RTPROT_ZEBRA    11      /* Zebra */
#define IP_RTPROT_BIRD     12      /* BIRD */
#define IP_RTPROT_DNROUTED 13      /* DECnet routing daemon */
#define IP_RTPROT_XORP     14      /* XORP */
#define IP_RTPROT_NTK      15      /* Netsukuku */

enum Ip_rt_scope_t
{
        IP_RT_SCOPE_UNIVERSE=0,
        IP_RT_SCOPE_SITE=200,
        IP_RT_SCOPE_LINK=253,
        IP_RT_SCOPE_HOST=254,
        IP_RT_SCOPE_NOWHERE=255
};

#define IP_RTM_F_NOTIFY            0x100   /* Notify user of route change  */
#define IP_RTM_F_CLONED            0x200   /* This route is cloned         */
#define IP_RTM_F_EQUALIZE          0x400   /* Multipath equalizer: NI      */
#define IP_RTM_F_PREFIX            0x800   /* Prefix addresses             */

enum Ip_rt_class_t
{
        IP_RT_TABLE_UNSPEC=0,
        IP_RT_TABLE_DEFAULT=253,
        IP_RT_TABLE_MAIN=254,
        IP_RT_TABLE_LOCAL=255,
        __IP_RT_TABLE_MAX
};
#define IP_RT_TABLE_MAX (__IP_RT_TABLE_MAX - 1)


enum Ip_rtattr_type_t
{
        IP_RTA_UNSPEC,
        IP_RTA_DST,
        IP_RTA_SRC,
        IP_RTA_IIF,
        IP_RTA_OIF,
        IP_RTA_GATEWAY,
        IP_RTA_PRIORITY,
        IP_RTA_PREFSRC,
        IP_RTA_METRICS,
        IP_RTA_MULTIPATH,
        IP_RTA_PROTOINFO,
        IP_RTA_FLOW,
        IP_RTA_CACHEINFO,
        IP_RTA_SESSION,
        IP_RTA_MP_ALGO,
        IP_RTA_TABLE,

        /* CONFIG INTERPEAK >>> */
        /* unsigned long. Used for MPLS shortcut routes */
        IP_RTA_NH_PROTO = 32,
        /* unsigned long. Used for MPLS shortcut routes */
        IP_RTA_NH_PROTO_DATA,
        /* 6 * unsigned char. For storing MAC address in proxy arp routes */
        IP_RTA_PROXY_ARP_LLADDR,
        /* unsigned long. Virtual router index. */
        IP_RTA_VR,
        /* string (max 16 characters). Route table name */
        IP_RTA_TABLE_NAME,
        /* <<< CONFIG INTERPEAK */

        __IP_RTA_MAX
};

#define IP_RTA_MAX (__IP_RTA_MAX - 1)

#define IP_RTM_RTA(r)  ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_rtmsg))))
#define IP_RTM_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_rtmsg))


/* CONFIG INTERPEAK >>> */

/* Used as NH_PROTO specifier for MPLS shortcut routes */
#define IP_NH_PROTO_MPLS 0x8847

/* <<< CONFIG INTERPEAK */


struct Ip_rtnexthop
{
        Ip_u16          rtnh_len;
        Ip_u8           rtnh_flags;
        Ip_u8           rtnh_hops;
        Ip_s32          rtnh_ifindex;
};

#define IP_RTNH_F_DEAD             1       /* Nexthop is dead (used by multipath)  */
#define IP_RTNH_F_PERVASIVE        2       /* Do recursive gateway lookup  */
#define IP_RTNH_F_ONLINK           4       /* Gateway is forced on link    */

#define IP_RTNH_ALIGNTO         4
#define IP_RTNH_ALIGN(len)      ( ((len)+IP_RTNH_ALIGNTO-1) & ~(IP_RTNH_ALIGNTO-1) )
#define IP_RTNH_OK(rtnh,len)    ((rtnh)->rtnh_len >= sizeof(struct Ip_rtnexthop) && \
                                 ((int)(rtnh)->rtnh_len) <= (len))
#define IP_RTNH_NEXT(rtnh)      ((struct Ip_rtnexthop*)(((char*)(rtnh)) + IP_RTNH_ALIGN((rtnh)->rtnh_len)))
#define IP_RTNH_LENGTH(len)     (IP_RTNH_ALIGN(sizeof(struct Ip_rtnexthop)) + (len))
#define IP_RTNH_SPACE(len)      IP_RTNH_ALIGN(RTNH_LENGTH(len))
#define IP_RTNH_DATA(rtnh)      ((struct Ip_rtattr*)(((char*)(rtnh)) + IP_RTNH_LENGTH(0)))

struct Ip_rta_cacheinfo
{
        Ip_u32   rta_clntref;
        Ip_u32   rta_lastuse;
        Ip_s32   rta_expires;
        Ip_u32   rta_error;
        Ip_u32   rta_used;

#define IP_RTNETLINK_HAVE_PEERINFO 1
        Ip_u32   rta_id;
        Ip_u32   rta_ts;
        Ip_u32   rta_tsage;
};

enum
{
        IP_RTAX_UNSPEC,
        IP_RTAX_LOCK,
        IP_RTAX_MTU,
        IP_RTAX_WINDOW,
        IP_RTAX_RTT,
        IP_RTAX_RTTVAR,
        IP_RTAX_SSTHRESH,
        IP_RTAX_CWND,
        IP_RTAX_ADVMSS,
        IP_RTAX_REORDERING,
        IP_RTAX_HOPLIMIT,
        IP_RTAX_INITCWND,
        IP_RTAX_FEATURES,
        IP_RTAX_RTO_MIN,
        __IP_RTAX_MAX
};

#define IP_RTAX_MAX (__IP_RTAX_MAX - 1)

#define IP_RTAX_FEATURE_ECN        0x00000001
#define IP_RTAX_FEATURE_SACK       0x00000002
#define IP_RTAX_FEATURE_TIMESTAMP  0x00000004
#define IP_RTAX_FEATURE_ALLFRAG    0x00000008
struct Ip_rta_session
{
        Ip_u8    proto;
        Ip_u8    pad1;
        Ip_u16   pad2;

        union {
                struct {
                        Ip_u16   sport;
                        Ip_u16   dport;
                } ports;

                struct {
                        Ip_u8    type;
                        Ip_u8    code;
                        Ip_u16   ident;
                } icmpt;

                Ip_u32           spi;
        } u;
};


/*********************************************************
 *              Interface address.
 ****/

struct Ip_ifaddrmsg
{
        Ip_u8   ifa_family;
        Ip_u8   ifa_prefixlen;  /* The prefix length            */
        Ip_u8   ifa_flags;      /* Flags                        */
        Ip_u8   ifa_scope;      /* See above                    */
        Ip_s32  ifa_index;      /* Link index                   */
};

enum
{
        IP_IFA_UNSPEC,
        IP_IFA_ADDRESS,
        IP_IFA_LOCAL,
        IP_IFA_LABEL,
        IP_IFA_BROADCAST,
        IP_IFA_ANYCAST,
        IP_IFA_CACHEINFO,
        IP_IFA_MULTICAST,

        /* CONFIG_INTERPEAK >>> */
     	IP_IFA_VR       = 32,
     	IP_IFA_TABLE,
     	IP_IFA_TABLE_NAME,
     	IP_IFA_X_INFO,
        /* <<< CONFIG_INTERPEAK */

        __IP_IFA_MAX
};

#define IP_IFA_MAX (__IP_IFA_MAX - 1)

#define IP_IFA_F_SECONDARY         0x01
#define IP_IFA_F_TEMPORARY         IP_IFA_F_SECONDARY

#define IP_IFA_F_NODAD             0x02
#define IP_IFA_F_OPTIMISTIC        0x04
#define IP_IFA_F_CGA               0x08
#define IP_IFA_F_HOMEADDRESS       0x10

#define IP_IFA_F_DEPRECATED        0x20
#define IP_IFA_F_TENTATIVE         0x40
#define IP_IFA_F_PERMANENT         0x80

struct Ip_ifa_x_info
{
    Ip_u32  ifa_x_flags;
};

#define IP_IFA_F_X_AUTONOM      0x01
#define IP_IFA_F_X_ONLINK       0x02
#define IP_IFA_F_X_AUTOMATIC    0x04
#define IP_IFA_F_X_INACCESSIBLE 0x08
#define IP_IFA_F_X_PREFERRED    0x10

struct Ip_ifa_cacheinfo
{
        Ip_u32   ifa_prefered;
        Ip_u32   ifa_valid;
        Ip_u32   cstamp; /* created timestamp, hundredths of seconds */
        Ip_u32   tstamp; /* updated timestamp, hundredths of seconds */
};
#define IP_IFA_RTA(r)  ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_ifaddrmsg))))
#define IP_IFA_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_ifaddrmsg))


struct Ip_ndmsg
{
        Ip_u8   ndm_family;
        Ip_u8   ndm_pad1;
        Ip_u16  ndm_pad2;
        Ip_s32  ndm_ifindex;    /* Link index                   */
        Ip_u16  ndm_state;
        Ip_u8   ndm_flags;
        Ip_u8   ndm_type;
};

enum
{
        IP_NDA_UNSPEC,
        IP_NDA_DST,
        IP_NDA_LLADDR,
        IP_NDA_CACHEINFO,
        IP_NDA_PROBES,

        /* CONFIG_INTERPEAK >>>*/
        IP_NDA_VR   = 32,
        IP_NDA_TABLE,
        IP_NDA_TABLE_NAME,
        IP_NDA_REALIFINDEX,
        /* <<< CONFIG_INTERPEAK */

        __IP_NDA_MAX
};

#define IP_NDA_MAX (__IP_NDA_MAX - 1)

#define IP_NDA_RTA(r)  ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_ndmsg))))
#define IP_NDA_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_ndmsg))

#define IP_NTF_PROXY       0x08    /* == ATF_PUBL */
#define IP_NTF_ROUTER      0x80
/* CONFIG INTERPEAK >>>*/
#define IP_NTF_GRAT        0x10
/* <<< CONFIG INTERPEAK */

#define IP_NUD_INCOMPLETE  0x01
#define IP_NUD_REACHABLE   0x02
#define IP_NUD_STALE       0x04
#define IP_NUD_DELAY       0x08
#define IP_NUD_PROBE       0x10
#define IP_NUD_FAILED      0x20

/* Dummy states */
#define IP_NUD_NOARP       0x40
#define IP_NUD_PERMANENT   0x80
#define IP_NUD_NONE        0x00


struct Ip_nda_cacheinfo
{
        Ip_u32           ndm_confirmed;
        Ip_u32           ndm_used;
        Ip_u32           ndm_updated;
        Ip_u32           ndm_refcnt;
};
struct Ip_ndt_stats
{
        Ip_u64           ndts_allocs;
        Ip_u64           ndts_destroys;
        Ip_u64           ndts_hash_grows;
        Ip_u64           ndts_res_failed;
        Ip_u64           ndts_lookups;
        Ip_u64           ndts_hits;
        Ip_u64           ndts_rcv_probes_mcast;
        Ip_u64           ndts_rcv_probes_ucast;
        Ip_u64           ndts_periodic_gc_runs;
        Ip_u64           ndts_forced_gc_runs;
};

enum {
        IP_NDTPA_UNSPEC,
        IP_NDTPA_IFINDEX,                  /* __u32, unchangeable */
        IP_NDTPA_REFCNT,                   /* __u32, read-only */
        IP_NDTPA_REACHABLE_TIME,           /* __u64, read-only, msecs */
        IP_NDTPA_BASE_REACHABLE_TIME,      /* __u64, msecs */
        IP_NDTPA_RETRANS_TIME,             /* __u64, msecs */
        IP_NDTPA_GC_STALETIME,             /* __u64, msecs */
        IP_NDTPA_DELAY_PROBE_TIME,         /* __u64, msecs */
        IP_NDTPA_QUEUE_LEN,                /* __u32 */
        IP_NDTPA_APP_PROBES,               /* __u32 */
        IP_NDTPA_UCAST_PROBES,             /* __u32 */
        IP_NDTPA_MCAST_PROBES,             /* __u32 */
        IP_NDTPA_ANYCAST_DELAY,            /* __u64, msecs */
        IP_NDTPA_PROXY_DELAY,              /* __u64, msecs */
        IP_NDTPA_PROXY_QLEN,               /* __u32 */
        IP_NDTPA_LOCKTIME,                 /* __u64, msecs */
        __IP_NDTPA_MAX
};
#define IP_NDTPA_MAX (__IP_NDTPA_MAX - 1)

struct Ip_ndtmsg
{
        Ip_u8            ndtm_family;
        Ip_u8            ndtm_pad1;
        Ip_u16           ndtm_pad2;
};

struct Ip_ndt_config
{
        Ip_u16           ndtc_key_len;
        Ip_u16           ndtc_entry_size;
        Ip_u32           ndtc_entries;
        Ip_u32           ndtc_last_flush;        /* delta to now in msecs */
        Ip_u32           ndtc_last_rand;         /* delta to now in msecs */
        Ip_u32           ndtc_hash_rnd;
        Ip_u32           ndtc_hash_mask;
        Ip_u32           ndtc_hash_chain_gc;
        Ip_u32           ndtc_proxy_qlen;
};

enum {
        IP_NDTA_UNSPEC,
        IP_NDTA_NAME,                      /* char *, unchangeable */
        IP_NDTA_THRESH1,                   /* __u32 */
        IP_NDTA_THRESH2,                   /* __u32 */
        IP_NDTA_THRESH3,                   /* __u32 */
        IP_NDTA_CONFIG,                    /* struct ndt_config, read-only */
        IP_NDTA_PARMS,                     /* nested TLV NDTPA_* */
        IP_NDTA_STATS,                     /* struct ndt_stats, read-only */
        IP_NDTA_GC_INTERVAL,               /* __u64, msecs */
        __IP_NDTA_MAX
};
#define IP_NDTA_MAX (__IP_NDTA_MAX - 1)

#define IP_NDTA_RTA(r) ((struct Ip_rtattr*)(((char*)(r)) + \
                     IP_NLMSG_ALIGN(sizeof(struct Ip_ndtmsg))))
#define IP_NDTA_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_ndtmsg))


struct Ip_rtgenmsg
{
        Ip_u8           rtgen_family;
};

struct Ip_ifinfomsg
{
        Ip_u8   ifi_family;
        Ip_u8   __ifi_pad;
        Ip_u16  ifi_type;               /* ARPHRD_* */
        Ip_s32  ifi_index;              /* Link index   */
        Ip_u32  ifi_flags;              /* IFF_* flags  */
        Ip_u32  ifi_change;             /* IFF_* change mask */
};

/********************************************************************
 *              prefix information
 ****/

struct Ip_prefixmsg
{
        Ip_u8   prefix_family;
        Ip_u8   prefix_pad1;
        Ip_u16  prefix_pad2;
        Ip_s32  prefix_ifindex;
        Ip_u8   prefix_type;
        Ip_u8   prefix_len;
        Ip_u8   prefix_flags;
        Ip_u8   prefix_pad3;
};
enum
{
        IP_PREFIX_UNSPEC,
        IP_PREFIX_ADDRESS,
        IP_PREFIX_CACHEINFO,

    /* CONFIG_INTERPEAK >>> */
        IP_PREFIX_VR    = 16,
    /* <<< CONFIG_INTERPEAK */

        __IP_PREFIX_MAX
};

#define IP_PREFIX_MAX      (__IP_PREFIX_MAX - 1)

struct Ip_prefix_cacheinfo
{
        Ip_u32   preferred_time;
        Ip_u32   valid_time;
};

struct Ip_rtnl_link_stats
{
        Ip_u32   rx_packets;             /* total packets received       */
        Ip_u32   tx_packets;             /* total packets transmitted    */
        Ip_u32   rx_bytes;               /* total bytes received         */
        Ip_u32   tx_bytes;               /* total bytes transmitted      */
        Ip_u32   rx_errors;              /* bad packets received         */
        Ip_u32   tx_errors;              /* packet transmit problems     */
        Ip_u32   rx_dropped;             /* no space in linux buffers    */
        Ip_u32   tx_dropped;             /* no space available in linux  */
        Ip_u32   multicast;              /* multicast packets received   */
        Ip_u32   collisions;

        /* detailed rx_errors: */
        Ip_u32   rx_length_errors;
        Ip_u32   rx_over_errors;         /* receiver ring buff overflow  */
        Ip_u32   rx_crc_errors;          /* recved pkt with crc error    */
        Ip_u32   rx_frame_errors;        /* recv'd frame alignment error */
        Ip_u32   rx_fifo_errors;         /* recv'r fifo overrun          */
        Ip_u32   rx_missed_errors;       /* receiver missed packet       */

        /* detailed tx_errors */
        Ip_u32   tx_aborted_errors;
        Ip_u32   tx_carrier_errors;
        Ip_u32   tx_fifo_errors;
        Ip_u32   tx_heartbeat_errors;
        Ip_u32   tx_window_errors;

        /* for cslip etc */
        Ip_u32   rx_compressed;
        Ip_u32   tx_compressed;
};

struct Ip_rtnl_link_ifmap
{
        Ip_u64   mem_start;
        Ip_u64   mem_end;
        Ip_u64   base_addr;
        Ip_u16   irq;
        Ip_u8    dma;
        Ip_u8    port;
};

enum
{
        IP_IFLA_UNSPEC,
        IP_IFLA_ADDRESS,
        IP_IFLA_BROADCAST,
        IP_IFLA_IFNAME,
        IP_IFLA_MTU,
        IP_IFLA_LINK,
        IP_IFLA_QDISC,
        IP_IFLA_STATS,
        IP_IFLA_COST,
        IP_IFLA_PRIORITY,
        IP_IFLA_MASTER,
        IP_IFLA_WIRELESS,          /* Wireless Extension event - see wireless.h */
        IP_IFLA_PROTINFO,          /* Protocol specific information for a link */
        IP_IFLA_TXQLEN,
        IP_IFLA_MAP,
        IP_IFLA_WEIGHT,
        IP_IFLA_OPERSTATE,
        IP_IFLA_LINKMODE,
        IP_IFLA_LINKINFO,

        /* CONFIG_INTERPEAK >>>*/
        IP_IFLA_VR = 32,
        /* <<< CONFIG_INTERPEAK */
        __IP_IFLA_MAX
};


#define IP_IFLA_MAX (__IP_IFLA_MAX - 1)

#define IP_IFLA_RTA(r)  ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_ifinfomsg))))
#define IP_IFLA_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_ifinfomsg))

enum
{
        IP_IFLA_INET6_UNSPEC,
        IP_IFLA_INET6_FLAGS,       /* link flags                   */
        IP_IFLA_INET6_CONF,        /* sysctl parameters            */
        IP_IFLA_INET6_STATS,       /* statistics                   */
        IP_IFLA_INET6_MCAST,       /* MC things. What of them?     */
        IP_IFLA_INET6_CACHEINFO,   /* time values and max reasm size */
        IP_IFLA_INET6_ICMP6STATS,  /* statistics (icmpv6)          */
        __IP_IFLA_INET6_MAX
};

#define IP_IFLA_INET6_MAX  (__IP_IFLA_INET6_MAX - 1)

struct Ip_ifla_cacheinfo
{
        Ip_u32   max_reasm_len;
        Ip_u32   tstamp;         /* ipv6InterfaceTable updated timestamp */
        Ip_u32   reachable_time;
        Ip_u32   retrans_time;
};

enum
{
        IP_IFLA_INFO_UNSPEC,
        IP_IFLA_INFO_KIND,
        IP_IFLA_INFO_DATA,
        IP_IFLA_INFO_XSTATS,
        __IP_IFLA_INFO_MAX
};

#define IP_IFLA_INFO_MAX   (__IP_IFLA_INFO_MAX - 1)

enum
{
        IP_IFLA_VLAN_UNSPEC,
        IP_IFLA_VLAN_ID,
        IP_IFLA_VLAN_FLAGS,
        IP_IFLA_VLAN_EGRESS_QOS,
        IP_IFLA_VLAN_INGRESS_QOS,
        __IP_IFLA_VLAN_MAX
};

#define IP_IFLA_VLAN_MAX   (__IP_IFLA_VLAN_MAX - 1)

struct Ip_ifla_vlan_flags {
        Ip_u32   flags;
        Ip_u32   mask;
};

enum
{
        IP_IFLA_VLAN_QOS_UNSPEC,
        IP_IFLA_VLAN_QOS_MAPPING,
        __IP_IFLA_VLAN_QOS_MAX
};

#define IP_IFLA_VLAN_QOS_MAX       (__IP_IFLA_VLAN_QOS_MAX - 1)

struct Ip_ifla_vlan_qos_mapping
{
        Ip_u32 from;
        Ip_u32 to;
};


/*****************************************************************
 *              Traffic control messages.
 ****/

struct Ip_tcmsg
{
        Ip_u8   tcm_family;
        Ip_u8   tcm__pad1;
        Ip_u16  tcm__pad2;
        Ip_s32  tcm_ifindex;
        Ip_u32  tcm_handle;
        Ip_u32  tcm_parent;
        Ip_u32  tcm_info;
};

enum
{
        IP_TCA_UNSPEC,
        IP_TCA_KIND,
        IP_TCA_OPTIONS,
        IP_TCA_STATS,
        IP_TCA_XSTATS,
        IP_TCA_RATE,
        IP_TCA_FCNT,
        IP_TCA_STATS2,
        __IP_TCA_MAX
};

#define IP_TCA_MAX (__IP_TCA_MAX - 1)

#define IP_TCA_RTA(r)  ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_tcmsg))))
#define IP_TCA_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_tcmsg))

/* RTnetlink multicast groups - backwards compatibility for userspace */

/* RTnetlink multicast groups */
enum Ip_rtnetlink_groups {
        IP_RTNLGRP_NONE,
        IP_RTNLGRP_LINK,
        IP_RTNLGRP_NOTIFY,
        IP_RTNLGRP_NEIGH,
        IP_RTNLGRP_TC,
        IP_RTNLGRP_IPV4_IFADDR,
        IP_RTNLGRP_IPV4_MROUTE,
        IP_RTNLGRP_IPV4_ROUTE,
        IP_RTNLGRP_IPV4_RULE,
        IP_RTNLGRP_IPV6_IFADDR,
        IP_RTNLGRP_IPV6_MROUTE,
        IP_RTNLGRP_IPV6_ROUTE,
        IP_RTNLGRP_IPV6_IFINFO,
        IP_RTNLGRP_DECnet_IFADDR,
        IP_RTNLGRP_NOP2,
        IP_RTNLGRP_DECnet_ROUTE,
        IP_RTNLGRP_DECnet_RULE,
/* CONFIG INTERPEAK >>> */
        IP_RTNLGRP_RTAB,
/* <<< CONFIG INTERPEAK */
        IP_RTNLGRP_NOP4,
        IP_RTNLGRP_IPV6_PREFIX,
        IP_RTNLGRP_IPV6_RULE,
        IP_RTNLGRP_IPV6_SEND,  /* Secure NDP, RFC 3971 */
        __IP_RTNLGRP_MAX
};
#define IP_RTNLGRP_MAX     (__IP_RTNLGRP_MAX - 1)


#define IP_RTMGRP_LINK             IP_RTNLGRP_LINK
#define IP_RTMGRP_NOTIFY           IP_RTNLGRP_NOTIFY
#define IP_RTMGRP_NEIGH            IP_RTNLGRP_NEIGH
#define IP_RTMGRP_TC               IP_RTNLGRP_TC

#define IP_RTMGRP_IPV4_IFADDR      IP_RTNLGRP_IPV4_IFADDR
#define IP_RTMGRP_IPV4_MROUTE      IP_RTNLGRP_IPV4_MROUTE
#define IP_RTMGRP_IPV4_ROUTE       IP_RTNLGRP_IPV4_ROUTE
#define IP_RTMGRP_IPV4_RULE        IP_RTNLGRP_IPV4_RULE

#define IP_RTMGRP_IPV6_IFADDR      IP_RTNLGRP_IPV6_IFADDR
#define IP_RTMGRP_IPV6_MROUTE      IP_RTNLGRP_IPV6_MROUTE
#define IP_RTMGRP_IPV6_ROUTE       IP_RTNLGRP_IPV6_ROUTE
#define IP_RTMGRP_IPV6_IFINFO      IP_RTNLGRP_IPV6_IFINFO

#define IP_RTMGRP_DECnet_IFADDR    IP_RTNLGRP_DECnet_IFADDR
#define IP_RTMGRP_DECnet_ROUTE     IP_RTNLGRP_DECnet_ROUTE

#define IP_RTMGRP_IPV6_PREFIX      IP_RTNLGRP_IPV6_PREFIX
/* CONFIG INTERPEAK >>> */
#define IP_RTMGRP_RTAB             IP_RTNLGRP_RTAB
/* <<< CONFIG INTERPEAK */


/* TC action piece */
struct Ip_tcamsg
{
        Ip_u8   tca_family;
        Ip_u8   tca__pad1;
        Ip_u16  tca__pad2;
};
#define IP_TA_RTA(r)  ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_tcamsg))))
#define IP_TA_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_tcamsg))
#define IP_TCA_ACT_TAB 1 /* attr type must be >=1 */
#define IP_TCAA_MAX 1

/* RULE handling */

#define IP_FIB_RULE_PERMANENT	    0x00000001
#define IP_FIB_RULE_INVERT		    0x00000002
#define IP_FIB_RULE_UNRESOLVED	    0x00000004
#define IP_FIB_RULE_DEV_DETACHED	0x00000008

#define IP_FIB_RULE_FIND_SADDR	    0x00010000

struct Ip_fib_rule_hdr
{
	Ip_u8		family;
	Ip_u8		dst_len;
	Ip_u8		src_len;
	Ip_u8		tos;

	Ip_u8		table;
	Ip_u8		res1;	/* reserved */
	Ip_u8		res2;	/* reserved */
	Ip_u8		action;

	Ip_u32	    flags;
};

enum
{
	IP_FRA_UNSPEC,
	IP_FRA_DST,
	IP_FRA_SRC,
	IP_FRA_IFNAME,
	IP_FRA_GOTO,
	IP_FRA_UNUSED2,
	IP_FRA_PRIORITY,
	IP_FRA_UNUSED3,
	IP_FRA_UNUSED4,
	IP_FRA_UNUSED5,
	IP_FRA_FWMARK,
	IP_FRA_FLOW,
	IP_FRA_UNUSED6,
	IP_FRA_UNUSED7,
	IP_FRA_UNUSED8,
	IP_FRA_TABLE,
	IP_FRA_FWMASK,

/* CONFIG INTERPEAK >>> */
    IP_FRA_PROTOCOL     = 32,
    IP_FRA_SCOPE,
    IP_FRA_PKTRULE,
    IP_FRA_PKTMASK,
    IP_FRA_EXTACT,
    IP_FRA_VR,
    IP_FRA_ID,
/* <<< CONFIG INTERPEAK */

	__IP_FRA_MAX
};

#define IP_FRA_MAX (__IP_FRA_MAX - 1)

/* CONFIG INTERPEAK >>> */
#define IP_FRA_PKT_TUNNELED     0x00000001
#define IP_FRA_PKT_FORWARDED    0x00000002

#define IP_FRA_EXT_RT_SRC_ADDR  0x00000001
/* <<< CONFIG INTERPEAK */

enum
{
	IP_FR_ACT_UNSPEC,
	IP_FR_ACT_TO_TBL,
	IP_FR_ACT_GOTO,
	IP_FR_ACT_NOP,
	IP_FR_ACT_RES3,
	IP_FR_ACT_RES4,
	IP_FR_ACT_BLACKHOLE,
	IP_FR_ACT_UNREACHABLE,
	IP_FR_ACT_PROHIBIT,
	__IP_FR_ACT_MAX
};


/* CONFIG INTERPEAK >>> */
/* VR attributes */
struct Ip_vrmsg
{
        Ip_u8   vr_family;
        Ip_u8   __vr_pad1;
        Ip_u16  __vr_pad2;
        Ip_u32  vr_vr;
};

enum Ip_vrattr_type_t
{
    IP_VR_UNSPEC,
    IP_VR_VR,
    IP_VR_IFNAME,
    IP_VR_TABLE,
    IP_VR_TABLE_NAME,
    IP_VR_IFNAMES,
    IP_VR_TABLES,
    __IP_VR_MAX
};
#define IP_VR_MAX (__IP_VR_MAX - 1)

enum Ip_vrxattr_type_t
{
    IP_VRX_UNSPEC,
    IP_VRX_IFNAME,
    IP_VRX_TABLE,
    IP_VRX_TABLE_NAME,
    __IP_VRX_MAX
};

#define IP_VRX_MAX (__IP_VRX_MAX - 1)
#define IP_VR_RTA(r)  ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_vrmsg))))
#define IP_VR_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_vrmsg))

struct Ip_mipmsg
{
        Ip_u8   mip_family;
        Ip_u8   mip_flags;
        Ip_u16  mip_type;
};

enum Ip_mipattr_type_t
{
    IP_MIP_UNSPEC,
    IP_MIP_ENCAP,
    __IP_MIP_MAX
};

#define IP_MIP_MAX (__IP_MIP_MAX - 1)

#define IP_MIP_RTA(r)     ((struct Ip_rtattr*)(((char*)(r)) + IP_NLMSG_ALIGN(sizeof(struct Ip_mipmsg))))
#define IP_MIP_PAYLOAD(n) IP_NLMSG_PAYLOAD(n,sizeof(struct Ip_rtgenmsg))

#define IP_NLMSG_GROUP_MASK(group)  ((group)? (1 << ((group) - 1)) : 0)

/* <<< CONFIG INTERPEAK */


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

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
