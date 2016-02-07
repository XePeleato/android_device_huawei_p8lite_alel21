/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_neigh.h,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_neigh.h,v $
 *   $Author: kenneth $ $Date: 2009-02-04 20:04:26 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_NEIGH_H
#define IPNET_NEIGH_H

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

#include <ipcom_type.h>
#include <ipcom_cstyle.h>

#include "ipnet_route.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_NEIGH_FROM_RT(rt)   ((Ipnet_neigh_entry*) (rt)->data)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Forward declarations */
struct Ipnet_neigh_entry_struct;

/*
 *===========================================================================
 *                       neighbor entry states
 *===========================================================================
 */
/* The possible states a entry can have in the ND cache */
typedef enum Ipnet_nd_state_enum
{
    IPNET_ND_UNINITIALIZED, /* First time this entry is used */
    IPNET_ND_INCOMPLETE,    /* ND request has been sent, but no answer has be received */
    IPNET_ND_REACHABLE,     /* ND entry is valid */
    IPNET_ND_STALE,         /* ND entry is probably valid */
    IPNET_ND_DELAY,         /* Waiting for the peer to ask for our MAC address */
    IPNET_ND_PROBE,         /* ND request has been sent, old IP to MAC mapping exists */
    IPNET_ND_PROXY          /* Proxy ARP entry */
}
Ipnet_nd_state_t;


/*
 *===========================================================================
 *                    Ipnet_neigh_probecb
 *===========================================================================
 * Description: Callback that will send neighbor solicitation probe.
 * Parameters:  neigh - neighbor entry to send probe for.
 *              unicast_probe - IP_TRUE = send unicast probe
 *                              IP_FALSE = send multicast/broadcast probe
 * Returns:
 *
 */
typedef void (*Ipnet_neigh_probecb) (struct Ipnet_neigh_entry_struct *neigh,
                                     Ip_bool unicast_probe);


/*
 *===========================================================================
 *                    Ipnet_neigh_resolvedcb
 *===========================================================================
 * Description: Function that will be called for each pending packet
 *              when/if the IP to link address resolution is successful.
 * Parameters:  neigh - the neighbor entry that has been resolved.
 *              pkt - One of the pending packets.
 * Returns:
 *
 */
typedef void (*Ipnet_neigh_resolvedcb) (struct Ipnet_neigh_entry_struct *neigh,
                                        struct Ipcom_pkt_struct *pkt);


/*
 *===========================================================================
 *                    Ipnet_neigh_dropcb
 *===========================================================================
 * Description: Function that will be called if a packet has to be dropped.
 * Parameters:  pkt - A packet in the pending list.
 * Returns:
 *
 */
typedef void (*Ipnet_neigh_dropcb) (struct Ipcom_pkt_struct *pkt);


/* Used by ARP and NDP to store packets that are waiting for IP
   to link layer address resolution */
typedef struct Ipnet_neigh_res_wait_list_struct
{
    Ip_u8               max;      /* Maximum number of packets that can be waiting for
                                     IP to link address resoution */
    Ip_u8               num;      /* Number of packets that are waiting for IP to link
                                     address resoution */
    Ip_u8               linger;   /* IP_TRUE if this ND is linger after failure */
    Ipcom_pkt          *pkt_list; /* Packet to send when status -> IPNET_ND_REACHABLE */
    Ipnet_neigh_resolvedcb resolvedcb;/* Function that will be called when/if the IP to link address
                                     resolution is successful */
    Ipnet_neigh_dropcb  dropcb;   /* Function to call if a packet must be dropped */
} Ipnet_neigh_res_wait_list;


/*
 *===========================================================================
 *                         Ipnet_neigh_entry
 *===========================================================================
 * Neighbor cache entry
 */
typedef struct Ipnet_neigh_entry_struct
{
    /* State of this neighbor entry */
    Ipnet_nd_state_t   state;
    /* Number of remaining unicast probes to be done, unless an answer
       is received */
    Ip_s8              remaining_ucast_probes;
    /* Number of remaining multicast (broadcast on IPv4) probes to be
       done, unless an answer is received */
    Ip_s8              remaining_mcast_probes;
    /* Number of remaining probes to be done, unless an answer is
       received */
    Ip_s8              remaining_app_probes;
    /* Is set to IP_TRUE if the upper layer is seeing forward
       progress */
    Ip_u8              is_making_forward_progress;
    /* The route entry in which this data is embedded */
    Ipnet_route_entry *rt;

    /* Probe function for this neighbor */
    Ipnet_neigh_probecb probecb;

    /* List of packets waiting for this nighbor entry to be
       resolved */
    Ipnet_neigh_res_wait_list pending;
#ifdef IPCOM_STORE_PHYSICAL_IF
    Ip_u32             physical_ifindex;
#endif
}
Ipnet_neigh_entry;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void
ipnet_neigh_queue_until_resolved(Ipnet_neigh_res_wait_list *wl, Ipcom_pkt *pkt);

IP_GLOBAL void
ipnet_neigh_free_all_pending(Ipnet_neigh_res_wait_list *wl);

IP_GLOBAL void
ipnet_neigh_send_all_pending(Ipnet_neigh_res_wait_list *wl,
                             struct Ipnet_netif_struct *netif);

IP_GLOBAL void
ipnet_neigh_init(Ipnet_route_entry *rt,
                 Ipnet_route_timedoutcb timeoutcb,
                 Ipnet_neigh_probecb probecb,
                 Ipnet_neigh_dropcb dropcb,
                 Ipnet_neigh_resolvedcb resolvedcb);
IP_GLOBAL void
ipnet_neigh_set_state(int domain,
                      Ipnet_neigh_entry *neigh,
                      Ipnet_nd_state_t new_state,
                      IP_CONST void *link_addr);
IP_GLOBAL void
ipnet_neigh_timeout(int domain,
                    Ipnet_neigh_entry *neigh,
                    Ip_bool is_timeout);


IP_INLINE Ip_bool
ipnet_neigh_has_pending(Ipnet_neigh_res_wait_list *wl)
{
    return wl->pkt_list != IP_NULL;
}

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
