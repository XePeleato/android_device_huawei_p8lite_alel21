/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ctrl.h,v $ $Revision: 1.20 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/include/ipnet_ctrl.h,v $
 *     $Author: fguo $ $Date: 2009-07-22 09:38:56 $
 *     $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_CTRL_H
#define IPNET_CTRL_H

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

#include "ipnet_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipnet.h"
#include "ipnet_neigh.h"
#ifdef IPTCP
#include "iptcp.h"
#endif

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
#if !defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)
#define ipnet_ctrl   ipnet_ctrl_usr
#endif


/*
 *===========================================================================
 *            IPNET_CTRL_
 *===========================================================================
 */
#define IPNET_CTRL_SOCK_BASE           0x0800
#define IPNET_CTRL_NC_ENTRY_BASE       0x1000
#define IPNET_CTRL_PREFIX_ENTRY_BASE   0x4000
#define IPNET_CTRL_ROUTER_ENTRY_BASE   0x8000


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                  ipnet tag
 *===========================================================================
 */

struct Ipnet_tag
{
    Ip_u16 type;   /* Tag type (IPNET_CTRL_xxx value) */
    Ip_u16 size;   /* The size of the tag in octets */
};


/*
 *===========================================================================
 *                         sock tag
 *===========================================================================
 */

struct Ipnet_tag_sock_data
{
    struct Ipnet_tag  hdr;
    /* The filedescriptor for the socket */
    int      fd;
    Ip_pid_t pid;
    Ip_u16   vr_index;
    int      domain;
    int      type;
    int      protocol;
    int      snd_bytes;
    int      rcv_bytes;
#ifdef IPTCP
    /* Only valid if protocol is IP_IPPROTO_TCP */
    Iptcp_state_t tcp_state;
#endif

    /* Source and destination addresses */
    union
    {
        struct Ip_sockaddr      addr;
        struct Ip_sockaddr_in   in_addr;
        struct Ip_sockaddr_in6  in6_addr;
    } saddr;
    union
    {
        struct Ip_sockaddr      addr;
        struct Ip_sockaddr_in   in_addr;
        struct Ip_sockaddr_in6  in6_addr;
    } daddr;
#ifdef IPSCTP
    void *priv_data;
#endif
};


/* Start a new enumeration of socket data */
#define IPNET_CTRL_NEW_ENUM_SOCK_DATA  (IPNET_CTRL_SOCK_BASE | 1)

/* Continue enumeration of socket data */
#define IPNET_CTRL_CONT_ENUM_SOCK_DATA (IPNET_CTRL_SOCK_BASE | 2)


/*
 *===========================================================================
 *                         ip6 addr tag
 *===========================================================================
 */

#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                         ip6 nc tag
 *===========================================================================
 */

struct Ipnet_tag_nc_entry
{
    struct Ipnet_tag  hdr;

    /* Filled by caller */
    /* Virtual router to use (0 = default) */
    Ip_u16            vr_index;
    /* Filled by caller for ADD, by callee for ENUM and ignored for DEL */
    /* Interface index (set to 0 to let the stack choose). rtab_index
       is ignored if this value is set, the route table to use is
       read from the interface */
    Ip_u32            ifindex;
    /* Filled by by callee for ENUM and ignored for ADD and DEL */
    /* State */
    Ipnet_nd_state_t  state;
    /* Is this a router (currently ignored for ADD) */
    Ip_bool           is_router;
    /* Filled by caller for ADD, by callee for ENUM and ignored for DEL */
    /* Number of seconds before the entry expires (or IPNET_INFINTE) */
    Ip_u32            expires_sec;

    /* Filled by caller for ADD and DEL and by callee for ENUM */
    /* A IPv6 address */
    struct Ip_sockaddr_in6  addr;
    /* Link address */
    struct Ip_sockaddr_dl   link_addr;
};

/* Start a new enumeration of neighbor cache entries */
#define IPNET_CTRL_NEW_ENUM_NC_ENTRY   (IPNET_CTRL_NC_ENTRY_BASE | 1)

/* Continue enumeration of neighbor cache entries */
#define IPNET_CTRL_CONT_ENUM_NC_ENTRY  (IPNET_CTRL_NC_ENTRY_BASE | 2)

/* Add neighbor cache entry */
#define IPNET_CTRL_ADD_NC_ENTRY        (IPNET_CTRL_NC_ENTRY_BASE | 3)

/* Delete neighbor cache entry */
#define IPNET_CTRL_DEL_NC_ENTRY        (IPNET_CTRL_NC_ENTRY_BASE | 4)


/*
 *===========================================================================
 *                         ip6 dc tag
 *===========================================================================
 */

struct Ipnet_tag_dc_entry
{
    struct Ipnet_tag  hdr;

    /* Filled by callee for ENUM */
    /* Number of seconds before the entry expires (or IPNET_INFINTE) */
    Ip_u32 expires_sec;
    /* Path MTU to this destination */
    Ip_u32 path_mtu;

    /* The final destination address */
    struct Ip_in6_addr dst_addr;
    /* The first hop address (is always a neighbor) */
    struct Ip_in6_addr next_hop_addr;
};

/*
 *===========================================================================
 *                         ip6 router tag
 *===========================================================================
 */

struct Ipnet_tag_router_entry
{
    struct Ipnet_tag  hdr;

    /* Filled by caller */
    /* Virtual router to use (0 = default) */
    Ip_u16  vr_index;
    /* Filled by callee for GET and ignored for DEL and ENUM */
    /* Interface index */
    Ip_u32  ifindex;

    /* Filled by callee for GET and ignored for DEL */
    /* Number of seconds before the entry expires (or IPNET_INFINTE) */
    Ip_u32 expires_sec;

    /* Filled by caller for DEL and by callee for GET */
    /* The router address (is a direct neighbor) */
    struct Ip_in6_addr addr;
};

/* Start a new enumeration of default router entries */
#define IPNET_CTRL_NEW_ENUM_ROUTER_ENTRY   (IPNET_CTRL_ROUTER_ENTRY_BASE | 1)

/* Continue enumeration of enumeration of default router entries */
#define IPNET_CTRL_CONT_ENUM_ROUTER_ENTRY  (IPNET_CTRL_ROUTER_ENTRY_BASE | 2)

/* Delete router entry */
#define IPNET_CTRL_DEL_ROUTER_ENTRY        (IPNET_CTRL_ROUTER_ENTRY_BASE | 3)


/*
 *===========================================================================
 *                         ip6 prefix tag
 *===========================================================================
 */

/* IPNET_CTRL_[ENUM|ADD|DEL]_PREFIX */
struct Ipnet_tag_prefix_entry
{
    struct Ipnet_tag  hdr;

    /* Filled by caller */
    /* Virtual router to use (0 = default) */
    Ip_u16  vr_index;
    /* Filled by caller for SET, by callee for ENUM and ignored for DEL */
    /* Interface name */
    Ip_u32  ifindex;
    /* Number of seconds before the entry expires (or IPNET_INFINTE) */
    Ip_u32 expires_sec;

    /* Filled by caller for SET and DEL and by callee for ENUM */
    /* Valid bits in prefix */
    int         bits;
    /* A IPv6 address */
    struct Ip_sockaddr_in6 prefix;
};

/* Start a new enumeration of address prefixes entries */
#define IPNET_CTRL_NEW_ENUM_PREFIX_ENTRY   (IPNET_CTRL_PREFIX_ENTRY_BASE | 1)

/* Continue enumeration of address prefixes entries */
#define IPNET_CTRL_CONT_ENUM_PREFIX_ENTRY  (IPNET_CTRL_PREFIX_ENTRY_BASE | 2)

#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 6                    PROTOTYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ctrl_islocked
 *===========================================================================
 * Description: SNMP helper function to access ndp tables.
 *              Code semaphore must be locked.
 * Parameters:  tag - The data to set or get.
 * Returns:     0 = succcess, -1 = error
 *              (get error code from ipcom_errno)
 *
 */
#if defined(IPCOM_USE_MIB2) && defined(IPCOM_USE_INET6)
IP_GLOBAL int ipnet_ctrl_islocked(struct Ipnet_tag *tag);
#endif


/*
 *===========================================================================
 *                    ipnet_ctrl
 *===========================================================================
 * Description: Returns, sets or deletes addresses, socket data, etc. in IPNET.
 * Parameters:  tag - Pointer to a tag, the memory area must be initialized
 *              to zero before excecuting a GET for the first time.
 *              The content returned must be passed if a list of values should
 *              be returned. tag->hdr.type must be on of the IPNET_CTRL_xxx
 *              values defined below.
 * Returns:     0 >= success (IPNET_CTRL_SUCCESS or IPNET_CTRL_ENUM_DONE)
 *              -1 = error (get errorcode from IPNET_P_GET_ERRNO()).
 *
 */
IP_PUBLIC int ipnet_ctrl(struct Ipnet_tag *tag);

/* ipnet_ctrl() return values */
#define IPNET_CTRL_SUCCESS     0   /* Success */
#define IPNET_CTRL_ENUM_DONE   1   /* Success, enumerated past last element */


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
