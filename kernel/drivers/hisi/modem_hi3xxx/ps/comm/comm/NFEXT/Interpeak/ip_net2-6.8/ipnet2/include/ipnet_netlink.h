/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_netlink.h,v $ $Revision: 1.41 $
 *     $Source:
 *     $Author: jonas $ $Date: 2009-03-03 13:35:04 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

#ifndef IPNET_NETLINK_H
#define IPNET_NETLINK_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This is the header file for the Linux compatible NETLINK interface
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
#include <ipcom_netlink.h>
#include "ipnet_route.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

    /*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ipnet_netif_struct;


/*
 *===========================================================================
 *                         Ipnet_netlink_callback
 *===========================================================================
 */
struct Ipnet_netlink_callback
{
    Ip_u8               *buf;
	struct Ip_nlmsghdr  *nlh;
	int		     (*dump)(Ip_u8 *buf, Ip_size_t len, struct Ipnet_netlink_callback *cb);
	int		     (*done)(struct Ipnet_netlink_callback *cb);
	int		     family;
	Ip_ptrdiff_t args[4];
};


typedef struct Ipnet_netlink_mem
{
    /* Pointer to start of data */
    char        *ptr;

    /* Total size */
    Ip_size_t   size;

    /* Working offset */
    Ip_size_t   offset;

    /* Current used size */
    Ip_size_t   used;

    /* Ancillary data */
    Ip_pid_t    pid;
    Ip_u16       vr;

} Ipnet_netlink_mem_t;


/**/
typedef int (*Ipnet_netlink_dump_t) (Ipnet_netlink_mem_t *mem,
                                     struct Ip_nlmsghdr *nlmsg,
                                     int                 family,
                                     Ip_ptrdiff_t        *args);


/*
 ****************************************************************************
 * 6                    PROTOTYPES
 ****************************************************************************
 */

#ifdef IP_PORT_LKM
IP_PUBLIC void
ipcom_netlink_post(Ipcom_pkt *pkt, int msg_group);
#endif /* IP_PORT_LKM */

#ifdef __cplusplus
}
#endif

#endif	/* IPNET_NETLINK_H */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

