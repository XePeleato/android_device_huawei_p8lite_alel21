/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_drv_eth_filter.c,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_drv_eth_filter.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Functions for filtering Ethernet unicast and multicast addresses that the
   local not is not listening on.
   The functions here should only be used by targets where the hardware
   filtering for Ethernet multicast is broken or not not implemented.
   The Ethernet chip must be placed in promiscuous mode when this
   using this module.
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

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_errno.h"
#include "ipcom_drv_eth_filter.h"


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

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_pkt_to_me
 *===========================================================================
 * Description: Returns if this packet was sent to this node.
 * Parameters:	filter - The current filter for this interface.
 *              frame - An Ethernet frame.
 * Returns:	    IP_TRUE if this packet was sent to this node.
 *
 */
IP_PUBLIC Ip_bool
ipcom_drv_eth_filter_pkt_to_me(Ipcom_drv_eth_filter *filter,
                               Ipcom_eth_hdr *frame)
{
    int  i;

    if (filter->promisc)
        /* Driver in software-promiscuous mode, accept all packets. */
        return IP_TRUE;

    if (frame->dst[0] == 0xFF)
        /* Ethernet broadcast is always accepted */
        return IP_TRUE;

    if (IP_BIT_ISFALSE(frame->dst[0], 0x01))
    {
        /* Unicast packet */
        if (ipcom_memcmp(filter->unicast_addr, frame->dst, IPCOM_DRV_ETH_ADDR_SIZE) != 0)
            /* Not my MAC address */
            return IP_FALSE;
#ifdef IPCOM_USE_INET
        if (filter->in_block_addr.s_addr
            && ip_ntohs(frame->type) == 0x0800
            && ipcom_memcmp(&filter->in_block_addr, &frame->data[16], 4) == 0) /*lint !e416 !e420 */
            return IP_FALSE;
#endif
        return IP_TRUE;
    }

    /* Check multicast groups */
    for (i = 0; i < IPCOM_DRV_ETH_MAX_MULTIADDR; i++)
        if (filter->ref[i] != 0
            && ipcom_memcmp(filter->mcast_addr[i],
                            frame->dst,
                            IPCOM_DRV_ETH_ADDR_SIZE) == 0)
            return IP_TRUE;

    /* This multicast frame was not sent to this interface */
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_set_unicast_addr
 *===========================================================================
 * Description: Sets the unicast address of the local interface.
 * Parameters:	filter - The current filter for this interface.
 *              eth_unicast_addr - The unicast address for this node.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_drv_eth_filter_set_unicast_addr(Ipcom_drv_eth_filter *filter,
                                      Ip_u8 eth_unicast_addr[IPCOM_DRV_ETH_ADDR_SIZE])
{
    ipcom_memcpy(filter->unicast_addr,
                 eth_unicast_addr,
                 IPCOM_DRV_ETH_ADDR_SIZE);
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_add_mcast_addr
 *===========================================================================
 * Description: Adds a multicast Ethernet address that should be passed
 *              by the filter.
 * Parameters:	filter - A filter to where the address should be added.
 *              eth_mcast_addr - The address to add.
 * Returns:	    0 = success, <0 = error code.
 *
 */
IP_PUBLIC int
ipcom_drv_eth_filter_add_mcast_addr(Ipcom_drv_eth_filter *filter,
                                    Ip_u8 eth_mcast_addr[IPCOM_DRV_ETH_ADDR_SIZE])
{
    int i;
    int insert = -1;

    for (i = IPCOM_DRV_ETH_MAX_MULTIADDR - 1; i >= 0 ; i--)
    {
        if (filter->ref[i] == 0)
            insert = i;
        else
        {
            if (ipcom_memcmp(eth_mcast_addr, filter->mcast_addr[i], IPCOM_DRV_ETH_ADDR_SIZE) == 0)
            {
                filter->ref[i]++;
                return 0;
            }
        }
    }

    if (insert < 0)
        /* Cannot join any more Ethernet multicast addresses */
        return -IP_ERRNO_ENOSPC;

    /* Add this multicast address */
    filter->ref[insert] = 1;
    ipcom_memcpy(filter->mcast_addr[insert], eth_mcast_addr, IPCOM_DRV_ETH_ADDR_SIZE);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_remove_mcast_addr
 *===========================================================================
 * Description: Removes a multicast Ethernet address that should be passed
 *              by the filter.
 * Parameters:	filter - A filter from where the address should be removed.
 *              eth_mcast_addr - The address to remove.
 * Returns:	    0 = success, <0 = error code.
 *
 */
IP_PUBLIC int
ipcom_drv_eth_filter_remove_mcast_addr(Ipcom_drv_eth_filter *filter,
                                       Ip_u8 eth_mcast_addr[IPCOM_DRV_ETH_ADDR_SIZE])
{
    int i;

    for (i = 0; i < IPCOM_DRV_ETH_MAX_MULTIADDR; i++)
    {
        if (filter->ref[i] != 0
            && ipcom_memcmp(eth_mcast_addr, filter->mcast_addr[i], IPCOM_DRV_ETH_ADDR_SIZE) == 0)
        {
            /* Address found */
            if (--filter->ref[i] == 0)
                /* Last reference, remove the address */
                ipcom_memset(filter->mcast_addr[i], 0, IPCOM_DRV_ETH_ADDR_SIZE);
            return 0;
        }
    }
    /* Unknown address */
    return -IP_ERRNO_EADDRNOTAVAIL;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_is_promiscuous
 *===========================================================================
 * Description: Returns if this filter should pass all frames.
 * Parameters:	filter - A filter.
 * Returns:
 *
 */
IP_PUBLIC Ip_bool
ipcom_drv_eth_filter_is_promiscuous(Ipcom_drv_eth_filter *filter)
{
    return filter->promisc != 0;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_set_promiscuous
 *===========================================================================
 * Description: Sets the filter in premiscuous mode.
 * Parameters:	filter - A filter.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_drv_eth_filter_set_promiscuous(Ipcom_drv_eth_filter *filter)
{
    ++filter->promisc;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_clr_promiscuous
 *===========================================================================
 * Description: Decreases the reference count on promiscuous mode and
 *              disable it if this was the last reference.
 * Parameters:	filter - A filter.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_drv_eth_filter_clr_promiscuous(Ipcom_drv_eth_filter *filter)
{
    if (filter->promisc > 0)
        --filter->promisc;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_filter_block_inet_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_drv_eth_filter_block_inet_addr(Ipcom_drv_eth_filter *filter,
                                     struct Ip_in_addr *addr)
{
#ifdef IPCOM_USE_INET
    filter->in_block_addr.s_addr = addr->s_addr;
#else
    (void)filter;
    (void)addr;
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

