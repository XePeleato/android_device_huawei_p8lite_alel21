/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_wlan.c,v $ $Revision: 1.16 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_wlan.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

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

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_WLAN

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_clib.h>
#include <ipcom_syslog.h>
#include <ipcom_netif.h>
#ifdef IPPRISM
#include <ipcom_wlan.h>

#else
#include <ipwlan_wlan.h>
#endif
#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_wlan.h"

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

IP_PUBLIC int ipnet_eth_if_init(Ipnet_netif *netif);


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int ipnet_wlan_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data);


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
 *===========================================================================
 *                    ipnet_wlan_ioctl
 *===========================================================================
 * Description: WLAN IO control.
 * Parameters:  netif - The network interface to operate on.
 *              command -
 *              data - Depends on command.
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_wlan_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    int             ret = 0;
    Ipcom_iwreq    *iw = data;
    Ipcom_wlan_func wlan_if;

    ip_assert(netif->ipcom.link_addr_size == IPNET_WLAN_ADDR_SIZE);

    switch(command)
    {
    case IP_SIOCSIWATTACHFUNC:
    case IP_SIOCSIWREMOVEFUNC:

        if(command == IP_SIOCSIWATTACHFUNC)
        {
            netif->wlan->aux_link_ioctl = (int (*)(Ipnet_netif *, Ip_u32, void *))iw->u.data.pointer;
        }

        if(netif->wlan->aux_link_ioctl != IP_NULL)
        {
            wlan_if.link_input = (int (*)(Ipcom_netif *, Ipcom_pkt *))netif->link_input;
            ret = netif->wlan->aux_link_ioctl(netif, command, &wlan_if);
            netif->link_input = (Ipnet_link_input)wlan_if.link_input;
        }

        if(command == IP_SIOCSIWREMOVEFUNC)
        {
            netif->wlan->aux_link_ioctl = IP_NULL;
        }

        break;

    default:

        if(netif->wlan->aux_link_ioctl)
        {
            ret = netif->wlan->aux_link_ioctl(netif, command, data);
            if(ret <= 0)
            {
                break;
            }
        }
        ret = netif->wlan->eth_link_ioctl(netif, command, data);
    }

    return ret;
}

/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_wlan_if_init
 *===========================================================================
 * Description: WLAN uses the Ethernet interface with a few modifications
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_wlan_if_init(Ipnet_netif *netif)
{
    int i;

    ipnet_eth_if_init(netif);

    netif->wlan = ipcom_malloc(sizeof(Ipnet_netif_wlan));
    if (netif->wlan == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    ipcom_memset(netif->wlan, 0, sizeof(Ipnet_netif_wlan));

    i = 0;
    do
    {
        ipcom_sprintf(netif->ipcom.name, "wlan%d", i++);
    } while (ipnet_if_nametonetif(netif->vr_index, netif->ipcom.name) != IP_NULL);


    /* Setup link functions for WLAN use. */
    netif->wlan->eth_link_ioctl = (int (*)(Ipnet_netif *, Ip_u32, void *))netif->link_ioctl;
    netif->link_ioctl = (Ipnet_link_ioctl)ipnet_wlan_ioctl;

    return 0;
}

#endif /* IPNET_USE_WLAN */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
