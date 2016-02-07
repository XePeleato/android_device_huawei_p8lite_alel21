/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_vrrp.c,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_vrrp.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Implementation of RFC3768 - Virtual Router Redundancy Protocol (VRRP)
 * This file contains helper functions used within the stack.
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
#ifdef IPNET_USE_VRRP

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_vrrp.h"

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

struct Ipnet_vrrp_key
{
    Ipnet_netif *netif;
    Ip_u8        vrid;
};


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
 *===========================================================================
 *                    ipnet_vrrp_obj_hash
 *===========================================================================
 * Description: Computes a hash value for an virtual router address entry.
 * Parameters:  addr - A vrrp address entry.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_vrrp_obj_hash(Ipnet_vrrp_addr_t *addr)
{
    return ipcom_hash_update(&addr->netif->ipcom.ifindex,
                             sizeof(addr->netif->ipcom.ifindex),
                             addr->vrid);
}


/*
 *===========================================================================
 *                    ipnet_vrrp_hash_cmp
 *===========================================================================
 * Description: Compares the virtual router address entry and the search key.
 * Parameters:  addr - VRRP address entry.
 *              key - A VRRP search key.
 * Returns:     IP_TRUE if the entries matches.
 *
 */
IP_STATIC Ip_bool
ipnet_vrrp_hash_cmp(Ipnet_vrrp_addr_t *addr, Ipnet_vrrp_addr_t *key)
{
    return addr->netif == key->netif && addr->vrid == key->vrid;
}


/*
 *===========================================================================
 *                    ipnet_vrrp_addr_index
 *===========================================================================
 * Description: Returns the index of an address.
 * Parameters:  addr_entry - A vr addres entry.
 *              addr - The address to look for.
 * Returns:     The index of the address or -1 if it does not exists.
 *
 */
IP_STATIC int
ipnet_vrrp_addr_index(Ipnet_vrrp_addr_t *addr_entry, struct Ip_in_addr addr)
{
    int i;

    for (i = 0; i < (int)addr_entry->num_addrs; i++)
        if (addr_entry->addrs[i].s_addr == addr.s_addr)
            return i;
    return -1;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_vrrp_get_addr_entry
 *===========================================================================
 * Description: Returns the matching virtual router address entry.
 * Parameters:  netif - The network interface the VRIP must be assigned to.
 *              vrid - The VRID the address must be assigned to.
 * Returns:     A pointer to the entry or IP_NULL if no match exists.
 *
 */
IP_GLOBAL Ipnet_vrrp_addr_t *
ipnet_vrrp_get_addr_entry(Ipnet_netif *netif, Ip_u8 vrid)
{
    Ipnet_vrrp_addr_t key;

    if (ipnet->vrrp_addrs == IP_NULL)
        return IP_NULL;

    key.netif = netif;
    key.vrid  = vrid;
    return ipcom_hash_get(ipnet->vrrp_addrs, &key);
}


/*
 *===========================================================================
 *                     ipnet_vrrp_owns_addr
 *===========================================================================
 * Description: Checks if this host currently owns the specified VRIP.
 * Parameters:  netif - The network interface the VRIP should be assigned to.
 *              vrid - The VRID the address should be assigned to.
 * Returns:     IP_TRUE if this host owns the address, IP_FALSE otherwise.
 *
 */
IP_GLOBAL Ip_bool
ipnet_vrrp_owns_addr(Ipnet_netif *netif, Ip_u8 vrid, struct Ip_in_addr addr)
{
    Ipnet_vrrp_addr_t *addr_entry;

    addr_entry = ipnet_vrrp_get_addr_entry(netif, vrid);
    if (addr_entry == IP_NULL)
        return IP_FALSE;
    return ipnet_vrrp_addr_index(addr_entry, addr) >= 0;
}


/*
 *===========================================================================
 *                      ipnet_vrrp_add_addr
 *===========================================================================
 * Description: Adds a virtual router address.
 * Parameters:  netif - The network interface the VRIP will be assigned to.
 *              vrid - The VRID the address will be assigned to.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_vrrp_add_addr(Ipnet_netif *netif, Ip_u8 vrid, struct Ip_in_addr addr)
{
    Ipnet_vrrp_addr_t *addr_entry;

    if (ipnet->vrrp_addrs == IP_NULL)
    {
        ipnet->vrrp_addrs = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_vrrp_obj_hash,
                                           (Ipcom_hash_key_func) ipnet_vrrp_obj_hash,
                                           (Ipcom_hash_cmp_func) ipnet_vrrp_hash_cmp);
        if (ipnet->vrrp_addrs == IP_NULL)
            return -IP_ERRNO_ENOMEM;
    }

    addr_entry = ipnet_vrrp_get_addr_entry(netif, vrid);
    if (addr_entry == IP_NULL)
    {
        addr_entry = ipcom_malloc(sizeof(Ipnet_vrrp_addr_t));
        if (addr_entry == IP_NULL)
            return -IP_ERRNO_ENOMEM;
        IPNET_IF_LOCK(netif);
        addr_entry->netif     = netif;
        addr_entry->vrid      = vrid;
        addr_entry->num_addrs = 1;
        addr_entry->addrs[0]  = addr;
    }
    else
    {
        Ipnet_vrrp_addr_t *a;
        int                i;

        i = ipnet_vrrp_addr_index(addr_entry, addr);
        if (i >= 0)
            return -IP_ERRNO_EEXIST;

        (void)ipcom_hash_remove(ipnet->vrrp_addrs, addr_entry);
        a = ipcom_realloc(addr_entry,
                          sizeof(Ipnet_vrrp_addr_t) + addr_entry->num_addrs * sizeof(Ipnet_vrrp_addr_t));
        if (a == IP_NULL)
            return -IP_ERRNO_ENOMEM;
        a->addrs[a->num_addrs++] = addr;
        addr_entry = a;
    }

    if (ipcom_hash_add(ipnet->vrrp_addrs, addr_entry) != IPCOM_SUCCESS)
    {
        ipcom_free(addr_entry);
        return -IP_ERRNO_ENOMEM;
    }

    IPCOM_LOG2(INFO, "VRRP: added address %s on %s",
               ipcom_inet_ntop(IP_AF_INET, &addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
               netif->ipcom.name);

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_vrrp_del_addr
 *===========================================================================
 * Description: Deletes a virtual router address.
 * Parameters:  netif - The network interface the VRIP was assigned to.
 *              vrid - The VRID the address was assigned to.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_vrrp_del_addr(Ipnet_netif *netif, Ip_u8 vrid, struct Ip_in_addr addr)
{
    Ipnet_vrrp_addr_t *addr_entry;
    int                i;

    ip_assert(ipnet->vrrp_addrs != IP_NULL);

    addr_entry = ipnet_vrrp_get_addr_entry(netif, vrid);
    if (addr_entry == IP_NULL)
        return -IP_ERRNO_ESRCH;

    i = ipnet_vrrp_addr_index(addr_entry, addr);
    if (i < 0)
        return -IP_ERRNO_ESRCH;

    IPCOM_LOG2(INFO, "VRRP: deleted address %s on %s",
               ipcom_inet_ntop(IP_AF_INET, &addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
               netif->ipcom.name);

    if (--addr_entry->num_addrs > 0)
    {
        for (;i < addr_entry->num_addrs; i++)
            addr_entry->addrs[i] = addr_entry->addrs[i + 1];
    }
    else
    {
        (void)ipcom_hash_remove(ipnet->vrrp_addrs, addr_entry);
        ipcom_free(addr_entry);

        if (ipnet->vrrp_addrs->elem == 0)
        {
            ipcom_hash_delete(ipnet->vrrp_addrs);
            ipnet->vrrp_addrs = IP_NULL;
        }
    }

    return 0;
}

#endif /* IPNET_USE_VRRP */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

