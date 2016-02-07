/* ipcom_route.h - API of IPCOM route */

/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_route.h,v $ $Revision: 1.38 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_route.h,v $
 *     $Author: jonas $ $Date: 2008-12-04 14:04:12 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_ROUTE_H
#define IPCOM_ROUTE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for IPCOM route.
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
#ifdef IP_PORT_LKM
#include "ipcom_netif.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if !defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)
#define ipcom_route_lookup ipcom_route_lookup_usr
#define ipcom_route_new_table ipcom_route_new_table_usr
#define ipcom_route_free_table ipcom_route_free_table_usr
#define ipcom_route_add ipcom_route_add_usr
#define ipcom_route_remove ipcom_route_remove_usr
#define ipcom_route_walk_tree ipcom_route_walk_tree_usr
#define ipcom_route_walk_tree_backwards ipcom_route_walk_tree_backwards_usr
#define ipcom_route_key_cmp ipcom_route_key_cmp_usr
#define ipcom_route_init_subtree_mask ipcom_route_init_subtree_mask_usr
#endif

    /* Ipcom_route_notify_func codes */
#define IPCOM_ROUTE_CODE_ADD        1   /* The entry is added to the route table */
#define IPCOM_ROUTE_CODE_REMOVE     2   /* The entry is removed from the route table */
#define IPCOM_ROUTE_CODE_DUPLICATE  3   /* The entry is removed from the route table
                                           since a new entry was added with the same key */

    /* ipcom_route_key_cmp() return values */
#define IPCOM_ROUTE_KEY_CMP_MASK_MATCH    -1 /* The keys matches if the mask is applied */
#define IPCOM_ROUTE_KEY_CMP_NO_MATCH       0 /* The keys did not match each others */
#define IPCOM_ROUTE_KEY_CMP_PREFECT_MATCH  1 /* The keys are identical */


    /* Internal flags */
#define IPCOM_ROUTE_FLAG_END        0x10000000 /* Patricia tree endmarker */
#define IPCOM_ROUTE_FLAG_INIT       0x20000000 /* Entry was create when Patricia tree was created */
#define IPCOM_ROUTE_FLAG_INTERNAL   0x40000000 /* Internal node */
#define IPCOM_ROUTE_FLAG_RES_3      0x80000000

#define IPCOM_ROUTE_FLAG_MASK       0x0FFFFFFF /* Mask for non-internal flags */


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

    /* Route entry definition */
typedef struct Ipcom_route_entry_struct
{
    struct Ipcom_route_entry_struct *parent;  /* Parent node */
    struct Ipcom_route_entry_struct *on;      /* Go here if bit number 'bit' is set */
    struct Ipcom_route_entry_struct *off;     /* Go here if bit number 'bit' is cleared */

    Ip_u32   flags;     /* IPCOM_ROUTE_FLAG_xxx, bit 28-31 is used by IPCOM,
                           bit 0-27 can be used by applications */
    Ip_u16   bit;       /* (only used by internal nodes) The partition bit */
    Ip_u8    status;    /* The status of the entry when traversing the tree */
    void    *key;       /* (only used by leaf nodes) A pointer to the the search key */
    void    *mask;      /* A pointer to the the mask (or IP_NULL for no mask) */
}
Ipcom_route_entry;


struct Ipcom_route_struct;


/*
 *===========================================================================
 *                    Ipcom_route_walk_cb
 *===========================================================================
 * Description: Callback function used when walking the route tree, this
 *              function will be called for each entry in the tree.
 * Parameters:  entry - A entry in the tree.
 *              data - User defined data.
 * Returns:     IP_TRUE if the 'entry' should be deleted.
 *
 */
typedef Ip_bool (*Ipcom_route_walk_cb)(Ipcom_route_entry *entry, void *data);


/*
 *===========================================================================
 *                    Ipcom_route_notify_func
 *===========================================================================
 * Description: Callback function for route table events.
 * Parameters:  rtab - The route table the event happend on.
 *              entry - The route entry affected by the event.
 *              code - One of the IPCOM_ROUTE_CODE_XXX constants.
 * Returns:
 *
 */
typedef void (*Ipcom_route_notify_func)(struct Ipcom_route_struct *rtab,
                                        Ipcom_route_entry *entry,
                                        int code);


/* Route table head definition */
typedef struct Ipcom_route_struct
{
    Ip_u16     bit_count;          /* The number of bits in the mask and the key */
    Ip_u16     octet_count;        /* The number of allocated octets for the mask and key */
    Ip_u16     user1;              /* User defined value, not used by IPCOM */
    Ip_u16     user2;              /* User defined value, not used by IPCOM */
    Ip_u32     user3;              /* User defined value, not used by IPCOM */
    Ipcom_route_notify_func notify_func; /* Notify function */
    Ipcom_route_entry   *top;      /* Top of the Patricia route tree */
    Ipcom_route_entry   *org_on;   /* The orginal all bits 1 entry */
    Ipcom_route_entry   *org_off;  /* The orginal all bits 0 entry */
    Ip_bool    consecutive_mask;   /* Entry mask can be described with prefixlen */
    Ip_bool    restart_walk;       /* Set to true when a node was deleted during a tree walk */
}
Ipcom_route;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

#ifdef __cplusplus
}
#endif

/*******************************************************************************
*
* ipcom_route_new_table - create a new route table
*
* This routine creates a new route table.
*
* Parameters:
* \is
* \i <bit_count>
* Number of bits in the search key.
* \i <notify_func>
* Function that will called when a new entry is added, delete or a duplicate is
* found. The arguments for the callback are: the route table, the affected entry
* and the event code ('IPCOM_ROUTE_CODE_'<x>).
* \i <malloc_func>
* Function that will be used when memory is allocated for internal node.
* ipcom_malloc() can be used if no special handling of the nodes are needed.
* \i <free_func>
* Function that can free memory allocated by malloc_func().
* \ie
*
* RETURNS: The new route table or 'IP_NULL' if not enough memory could be
* allocated.
*
* ERRNO:
*/
IP_PUBLIC Ipcom_route *ipcom_route_new_table
(
    Ip_u16 bit_count,
    Ipcom_route_notify_func notify_func
);


/*******************************************************************************
*
* ipcom_route_free_table - free the route table
*
* This routine frees all entries in the route table and the route table itself.
* The callback function is called for each deleted element.
*
* Parameter:
* \is
* \i <rtab>
* The route table to free.
* \ie
*
* RETURNS:
*
* ERRNO:
*/
IP_PUBLIC void ipcom_route_free_table
(
    Ipcom_route *rtab
);


/*******************************************************************************
*
* ipcom_route_add - add a new entry to the route table
*
* This routine adds a new entry to the route table. The free_func() routine
* specified when creating the route table must be able to free the added entry.
*
* Parameters:
* \is
* \i <rtab>
* The route table to add the element to.
* \i <elem_to_add>
* Element to be added to the route table. The element must be freed by the
* called [* caller?] if the function fails.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or the following error:
* \is
* \i 'IPCOM_ERR_NO_MEMORY'
* Not enough memory could be allocated to add the entry.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_route_add
(
    Ipcom_route *rtab,
    Ipcom_route_entry *elem_to_add
);


/*******************************************************************************
*
* ipcom_route_remove - remove an entry
*
* This routine removes an entry from the route table. The free_func() specified
* when creating the route table must be able to free the entry.
*
* Parameters:
* \is
* \i <rtab>
* The route table to remove the element from.
* \i <elem_to_delete>
* Element to remove from the route table.
* \ie
*
* RETURNS: \&'IPCOM_SUCCESS'
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_route_remove
(
    Ipcom_route *rtab,
    Ipcom_route_entry *elem_to_remove
);


/*******************************************************************************
*
* ipcom_route_lookup - return the longest match to the specified key
*
* This routine returns the longest match to the specified key.
*
* Parameters:
* \is
* \i <rtab>
* The route table to search.
* \i <key>
* The search <key>, must be the same bit length as specified in the <bit_count>
* parameter of the ipcom_route_new_table() routine.
* \i <perfect_match>
* Set to 'IP_TRUE', if all the bits in the keys exactly match the returned
* entry. Set to 'IP_FALSE', if the key matched the returned entry after applying
* the entry mask. Can be 'IP_NULL', if this information is not needed.
* \ie
*
* RETURNS: The entry with the longest match is returned, or 'IP_NULL', if there
* is no match.
*
* ERRNO:
*/
IP_PUBLIC Ipcom_route_entry *ipcom_route_lookup
(
    Ipcom_route *rtab,
    const void *key,
    Ip_bool *perfect_match
);


/*
 *===========================================================================
 *                    ipcom_route_walk_tree
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_route_walk_tree(Ipcom_route *rtab,
                      Ipcom_route_walk_cb cb,
                      void *data);


/*
 *===========================================================================
 *                    ipcom_route_walk_tree_backwards
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_route_walk_tree_backwards(Ipcom_route *rtab,
                                Ipcom_route_walk_cb cb,
                                void *data);


/*
 *===========================================================================
 *                    ipcom_route_key_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_route_key_cmp(Ip_u32 key_bit_size,
                    const void *key1,
                    const void *key2,
                    const void *mask);


/*
 *===========================================================================
 *                    ipcom_route_init_subtree_mask
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_route_init_subtree_mask(Ipcom_route_entry *rt);


IP_GLOBAL Ipcom_route_entry *ipcom_route_next_entry(Ipcom_route_entry *rt);
IP_GLOBAL Ipcom_route_entry *ipcom_route_first_entry(Ipcom_route *rtab);

#ifdef IP_PORT_LKM
IP_PUBLIC Ip_u16 ipcom_get_user_vr(void);
IP_PUBLIC int ipcom_create_vr_loopback(Ip_u16 vr);
IP_PUBLIC int ipcom_destroy_vr_loopback(Ip_u16 vr);
IP_PUBLIC Ip_bool ipcom_netif_is_vr_bound(Ipcom_netif *ipcomif);

#ifdef IPNET_USE_TUNNEL
IP_PUBLIC void ipcom_create_base_tunnels(Ip_u16 vr);
IP_PUBLIC void ipcom_destroy_base_tunnels(Ip_u16 vr);
#endif

#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
