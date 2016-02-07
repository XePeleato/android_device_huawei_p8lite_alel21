/*
 ******************************************************************************
 *                     INTERPEAK HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_routesock.h,v $ $Revision: 1.24 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/include/ipnet_routesock.h,v $
 *   $Author: kenneth $ $Date: 2006-10-27 14:57:04 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_ROUTESOCK_H
#define IPNET_ROUTESOCK_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipnet_config.h"

#include "ipnet.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_RTM_VERSION  4

/* T = message can be sent to IPNET
   F = message can be sent from IPNET
   1 = message format is struct Ipnet_rt_msghdr
   2 = message format is struct Ipnet_ifa_msghdr
   3 = message format is struct Ipnet_if_msghdr
   4 = message format is struct Ipnet_if_announcemsghdr
 */
#define IPNET_RTM_ADD        0x1   /* TF1  Add route */
#define IPNET_RTM_DELETE     0x2   /* TF1  Delete route */
#define IPNET_RTM_CHANGE     0x3   /* TF1  Change in metric or flags */
#define IPNET_RTM_GET        0x4   /* TF1  Report metrics and other route information */
#define IPNET_RTM_LOOSING    0x5   /*  F1  Kernel suspects route is failing */
#define IPNET_RTM_REDIRECT   0x6   /*  F1  Kernel told to use a different route */
#define IPNET_RTM_MISS       0x7   /*  F1  Lookup failed on this address */
#define IPNET_RTM_LOCK       0x8   /* TF1  Lock specified metric */
#define IPNET_RTM_OLDADD     0x9   /* Unsupported! */
#define IPNET_RTM_OLDDEL     0xa   /* Unsupported! */
#define IPNET_RTM_RESOLVE    0xb   /*  F1  Request to resolve destination to link-layer address */
#define IPNET_RTM_NEWADDR    0xc   /* TF2  Address is beeing added to interface */
#define IPNET_RTM_DELADDR    0xd   /* TF2  Address is beeing removed from interface */
#define IPNET_RTM_IFINFO     0xe   /*  F3  Interface status or flag(s) is changing */
#define IPNET_RTM_IFANNOUNCE 0x10  /*  F4  Interface attached/detached */
/* Interpeak extension for Virtual Router */
#define IPNET_RTM_NEWVR     0xf1  /* TF1 Add a new virtual router */
#define IPNET_RTM_DELVR     0xf2  /* TF1 Delete a virtual router, the default (0)
                                       route table cannot be remmoved  */


/* rtm_addrs values */
#define IPNET_RTA_DST      0x01    /* Destination address Ip_sockaddr present */
#define IPNET_RTA_GATEWAY  0x02    /* Gateway address Ip_sockaddr present */
#define IPNET_RTA_NETMASK  0x04    /* Netmask address Ip_sockaddr present */
#define IPNET_RTA_GENMASK  0x08    /* Cloning mask Ip_sockaddr present */
#define IPNET_RTA_IFP      0x10    /* Interface name Ip_sockaddr present */
#define IPNET_RTA_IFA      0x20    /* Interface addr Ip_sockaddr present */
#define IPNET_RTA_AUTHOR   0x40    /* Author of redirect Ip_sockaddr present */
#define IPNET_RTA_BRD      0x80    /* Broadcast or point-to-point destination address Ip_sockaddr present */

#define IPNET_RTAX_DST     0       /* Destination address Ip_sockaddr present */
#define IPNET_RTAX_GATEWAY 1       /* Gateway address Ip_sockaddr present */
#define IPNET_RTAX_NETMASK 2       /* Netmask address Ip_sockaddr present */
#define IPNET_RTAX_GENMASK 3       /* Cloning mask Ip_sockaddr present */
#define IPNET_RTAX_IFP     4       /* Interface name Ip_sockaddr present */
#define IPNET_RTAX_IFA     5       /* Interface addr Ip_sockaddr present */
#define IPNET_RTAX_AUTHOR  6       /* Author of redirect Ip_sockaddr present */
#define IPNET_RTAX_BRD     7       /* Broadcast or point-to-point destination address Ip_sockaddr present */
#define IPNET_RTAX_MAX     8       /* Size of array to allocate */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Generic message header */
struct Ipnet_rs_msghdr
{
    Ip_u16      msglen;         /* The length of this message */
    Ip_u8       version;        /* The version needed to understand the message */
    Ip_u8       type;           /* Messge type IPNET_RTM_xxx */
};


struct Ipnet_rt_msghdr
{
    Ip_u16      rtm_msglen;     /* The length of this message */
    Ip_u8       rtm_version;    /* The version needed to understand the message */
    Ip_u8       rtm_type;       /* Messge type IPNET_RTM_xxx */

    Ip_u32      rtm_index;      /* Interface index of associated network interface */
    Ip_u32      rtm_flags;      /* Flags, incl kern & message (IPNET_RTF_xxx) */
    Ip_u32      rtm_addrs;      /* Bitmask identifying sockaddrs in msg (IPNET_RTA_xxx) */
    Ip_pid_t    rtm_pid;        /* Identifies sender */
    Ip_u32      rtm_seq;        /* For sender to identify action */
    Ip_u32      rtm_errno;      /* Error code */
    Ip_u32      rtm_use;
    Ip_u32      rtm_inits;      /* IPNET_RTV_xxx flags to identify which fields is valid in 'rtm_rmx' */
    Ip_u32      rtm_table;      /* Route table index (IPCOM_ROUTE_TABLE_DEFAULT = default) */
    /* The actual routing table used is defined by socket vr + rtm_table */
    struct Ipnet_rt_metrics rtm_rmx;
};

struct Ipnet_ifa_msghdr
{
    Ip_u16      ifam_msglen;    /* The length of this message */
    Ip_u8       ifam_version;   /* The version needed to understand the message */
    Ip_u8       ifam_type;      /* Messge type IPNET_RTM_xxx */

    Ip_u32      ifam_addrs;     /* Bitmask identifying sockaddrs in msg (IPNET_RTA_xxx) */
    Ip_u32      ifam_flags;     /* Subset route flags (IPNET_RTF_xxx) */
    Ip_u32      ifam_index;     /* Interface index */
    Ip_u32      ifam_metric;    /* Metric */
};


struct Ipnet_if_data
{
    /* generic interface information */
    Ip_u8   ifi_type;               /* Ethernet, tokenring, etc. */
    Ip_u8   ifi_addrlen;            /* Media address length */
    Ip_u8   ifi_hdrlen;             /* Media header length */
    Ip_u8   ifi_link_state;         /* Current link state */
    Ip_u32  ifi_mtu;                /* Maximum transmission unit */
    Ip_u32  ifi_metric;             /* Routing metric (external only) */
    Ip_u32  ifi_baudrate;           /* Linespeed */
    /* volatile statistics */
    Ip_u32  ifi_ipackets;           /* Packets received on interface */
    Ip_u32  ifi_ierrors;            /* Input errors on interface */
    Ip_u32  ifi_opackets;           /* Packets sent on interface */
    Ip_u32  ifi_oerrors;            /* Output errors on interface */
    Ip_u32  ifi_collisions;         /* Collisions on csma interfaces */
    Ip_u32  ifi_ibytes;             /* Total number of octets received */
    Ip_u32  ifi_obytes;             /* Total number of octets sent */
    Ip_u32  ifi_imcasts;            /* Packets received via multicast */
    Ip_u32  ifi_omcasts;            /* Packets sent via multicast */
    Ip_u32  ifi_iqdrops;            /* Dropped on input, this interface */
    Ip_u32  ifi_noproto;            /* Destined for unsupported protocol */
    struct  Ip_timeval ifi_lastchange; /* last updated */
};

struct Ipnet_if_msghdr
{
    Ip_u16      ifm_msglen;     /* The length of this message */
    Ip_u8       ifm_version;    /* The version needed to understand the message */
    Ip_u8       ifm_type;       /* Messge type IPNET_RTM_xxx */

    int         ifm_addrs;      /* Bitmask identifying sockaddrs in msg (IPNET_RTA_xxx) */
    int         ifm_flags;      /* Interface flags (IP_IFF_xxx) */
    Ip_u32      ifm_index;      /* Interface index */
    struct Ipnet_if_data ifm_data;   /* Statistics and other interface info */
};

struct Ipnet_if_announcemsghdr
{
    Ip_u16      ifan_msglen;    /* The length of this message */
    Ip_u8       ifan_version;   /* The version needed to understand the message */
    Ip_u8       ifan_type;      /* Message type IPNET_RTM_xxx */

    Ip_u32      ifan_index;     /* Interface index */
    char        ifan_name[IP_IFNAMSIZ]; /* Interface name, e.g. "eth0" */
    Ip_u16      ifan_what;      /* what type of announcement */
};

#define IPNET_IFAN_ARRIVAL    0  /* interface attached (arrival) */
#define IPNET_IFAN_DEPARTURE  1  /* interface detached (departure) */


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
