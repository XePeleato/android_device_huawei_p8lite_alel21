/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_route.c,v $ $Revision: 1.48 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_route.c,v $
 *     $Author: markus $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/* Route table functions, based on Patricia tree */

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
#include "ipcom_route.h"
#include "ipcom_clib.h"
#include "ipcom_err.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                       IPCOM_IS_BIT_SET
 *===========================================================================
 * Description: Tests if a specifed bit in a buffer is set.
 * Parameters:  [in] data - const void * pointer.
 *              [in] bit - The bit to test, counted from left.
 * Returns:     0 if the bit is not set, !=0 if the bit the is set.
 *
 */
#define IPCOM_IS_BIT_SET(data, bit) \
    (((Ip_u8 *) (data))[(bit) >> 3] & (0x80 >> ((bit) & 0x7)))


/* Status of internal nodes during tree traversal */
#define IPCOM_ROUTE_STATUS_INIT           0
#define IPCOM_ROUTE_STATUS_OFF_SUBTREE    1
#define IPCOM_ROUTE_STATUS_ON_SUBTREE     2
#define IPCOM_ROUTE_STATUS_DONE           3

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                      Ipcom_route_walk_subtree_cb
 *===========================================================================
 * Description: Will be called for all leaf nodes when walking the tree.
 * Parameters:  leaf - A leaf node (leaf->on == leaf->off == IP_NULL).
 *              user - User defined data.
 * Returns:
 *
 */
typedef void (*Ipcom_route_walk_subtree_cb)(Ipcom_route_entry *leaf,
                                            void *user);

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

IP_STATIC void
ipcom_route_init_subtree_mask2(Ipcom_route_entry *rt, Ip_bool consecutive_mask);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_patricia_search
 *===========================================================================
 * Description: Search a patricia tree for the the entry that best matches the
 *              search criteria.
 * Parameters:  top - The top of the patricia (sub)tree.
 *              key - The key to search for,
 *              mask - The mask to apply to the key before doing any test.
 * Returns:     The best match entry.
 *
 */
IP_STATIC Ipcom_route_entry*
ipcom_patricia_search(Ipcom_route_entry *top, const void *key, const void *mask)
{
    Ipcom_route_entry *current = top;

    while (current->off != IP_NULL)
    {
        if ((mask == IP_NULL
             || IPCOM_IS_BIT_SET(mask, current->bit))
            && IPCOM_IS_BIT_SET(key, current->bit))
            current = current->on;
        else
            current = current->off;
    }
    ip_assert(current->on == IP_NULL);
    return current;
}


/*
 *===========================================================================
 *                    ipcom_patricia_insert
 *===========================================================================
 * Description: Insert route entry into the patricia route tree.
 * Parameters:  domain - The address domain.
 *              new_leaf - The new node to insert. The following fields must
 *              have valid values:
 *                  flags must contain any user defined flags.
 *                  key must point to the search key
 *                  mask must point to the mask or be IP_NULL if the mask is all 1:s.
 *              all other fields are initialized by this function.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC Ip_err
ipcom_patricia_insert(Ipcom_route *rtab, Ipcom_route_entry *new_leaf)
{
    Ipcom_route_entry *new_internal_node;
    Ipcom_route_entry *current;
#ifdef IP_ASSERT
    Ip_u16 max_bit;
#endif
    Ip_u16 partition_bit;
    void  *matched_key;

    current = ipcom_patricia_search(rtab->top, new_leaf->key, new_leaf->mask);

    if (ipcom_route_key_cmp(rtab->bit_count, current->key, new_leaf->key, new_leaf->mask)
        == IPCOM_ROUTE_KEY_CMP_PREFECT_MATCH)
    {
        /* Perfect match, replace this entry */
        new_leaf->parent = current->parent;
        if (new_leaf->parent->on == current)
            new_leaf->parent->on = new_leaf;
        else
        {
            ip_assert(new_leaf->parent->off == current);
            new_leaf->parent->off = new_leaf;
        }

        if (IP_BIT_ISSET(current->flags, IPCOM_ROUTE_FLAG_END))
            IP_BIT_SET(new_leaf->flags, IPCOM_ROUTE_FLAG_END);

        if (current->mask != IP_NULL)
        {
            /* current->mask pointer will become invalid, remove it */
            Ipcom_route_entry *internal;

            for (internal = current->parent; internal != IP_NULL; internal = internal->parent)
                if (current->mask == internal->mask)
                    internal->mask = IP_NULL;
        }

        ipcom_route_init_subtree_mask2(rtab->top, rtab->consecutive_mask);
        ipcom_route_init_subtree_mask2(new_leaf, rtab->consecutive_mask);

        current->parent = IP_NULL;
        ip_assert(rtab->notify_func != IP_NULL);
        if (IP_BIT_ISFALSE(current->flags, IPCOM_ROUTE_FLAG_INIT))
        {
            rtab->notify_func(rtab, current, IPCOM_ROUTE_CODE_DUPLICATE);
        }

        return IPCOM_SUCCESS;
    }

    /* Find out where the entry should be inserted */
    IP_ASSERTLINE(max_bit = (Ip_u16) (rtab->bit_count - 1));
    matched_key = current->key;
    for (;;)
    {
        if (ipcom_route_key_cmp(current->parent->bit, new_leaf->key, matched_key, IP_NULL)
            != IPCOM_ROUTE_KEY_CMP_NO_MATCH)
            break;
        /* Need to compare fewer bits to make the addresses match */
        current = current->parent;
        IP_ASSERTLINE(max_bit = (Ip_u16) (current->bit - 1));
    }
    partition_bit = (Ip_u16) (current->parent->bit + 1);
    /* current->key and new_leaf->key differs in a bit between 'partition_bit'
       and 'max_bit' */
    while (IPCOM_IS_BIT_SET(new_leaf->key, partition_bit)
           == IPCOM_IS_BIT_SET(matched_key, partition_bit))
    {
        partition_bit++;
    }
    /* The key differ in bit 'partition_bit' */
    ip_assert(max_bit >= partition_bit);

    /* Allocate memory for a new patricia tree internal node */
    new_internal_node = ipcom_malloc(sizeof(Ipcom_route_entry));
    if (new_internal_node == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;
    ipcom_memset(new_internal_node, 0, sizeof(Ipcom_route_entry));

    new_internal_node->bit   = partition_bit;
    new_internal_node->flags = IPCOM_ROUTE_FLAG_INTERNAL;
    new_internal_node->key   = IP_NULL;
    new_internal_node->mask  = IP_NULL;

    /* Insert the new internal and leaf nodes */
    new_internal_node->parent = current->parent;
    /* Add the new internal node as child of the 'current' parent */
    if (current->parent->off == current)
        current->parent->off = new_internal_node;
    else
        current->parent->on = new_internal_node;
    current->parent = new_internal_node;
    new_leaf->parent = new_internal_node;
    /* Add the new leaf and the 'current' node as child to the new internal node */
    if (IPCOM_IS_BIT_SET(new_leaf->key, partition_bit))
    {
        new_internal_node->off = current;
        new_internal_node->on  = new_leaf;
    }
    else
    {
        new_internal_node->on  = current;
        new_internal_node->off = new_leaf;
    }

    ipcom_route_init_subtree_mask2(new_internal_node, rtab->consecutive_mask);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_route_walk_subtree
 *===========================================================================
 * Description: Visits all nodes rooted at 'subtree'.
 * Parameters:  subtree - The root of the subtree to travers.
 *              only_off_subtree - Visit only the nodes with the partition
 *                                 bits cleared.
 *              include_init_nodes - IP_TRUE if the user function should be
 *                                   called even for initial nodes.
 *              cb - The user function that will be called for leaf nodes.
 *              user - User defined data passed to 'cb'.
 * Returns:
 *
 */
IP_STATIC void
ipcom_route_walk_subtree(Ipcom_route_entry *subtree,
                         Ip_bool only_off_subtree,
                         Ip_bool include_init_nodes,
                         Ipcom_route_walk_subtree_cb cb,
                         void *user)
{
    Ipcom_route_entry *rt = subtree;
    Ip_bool            done = IP_FALSE;

    rt->status = IPCOM_ROUTE_STATUS_INIT;

    do {
        if (rt->off != IP_NULL
            && rt->status == IPCOM_ROUTE_STATUS_INIT)
        {
            /* Go down the sub-tree with partition bit cleared */
            ip_assert(rt->on != IP_NULL);
            rt->status = IPCOM_ROUTE_STATUS_OFF_SUBTREE;
            rt = rt->off;
            rt->status = IPCOM_ROUTE_STATUS_INIT;
        }
        else if (!only_off_subtree
                 && rt->on != IP_NULL
                 && rt->status == IPCOM_ROUTE_STATUS_OFF_SUBTREE)
        {
            /* Go down the sub-tree with partition bit set */
            ip_assert(rt->off != IP_NULL);
            rt->status = IPCOM_ROUTE_STATUS_ON_SUBTREE;
            rt = rt->on;
            rt->status = IPCOM_ROUTE_STATUS_INIT;
        }
        else
        {
            Ipcom_route_entry *leaf = rt;

            /* Must set status to DONE before calling the callback,
               since the callback might free the leaf node */
            rt->status = IPCOM_ROUTE_STATUS_DONE;
            if (rt == subtree)
                /* Done with the whole subtree */
                done = IP_TRUE;
            else
                rt = rt->parent;

            if (leaf->off == IP_NULL
                && (IP_BIT_ISFALSE(leaf->flags, IPCOM_ROUTE_FLAG_INIT)
                    || include_init_nodes))
            {
                ip_assert(leaf->on == IP_NULL);
                cb(leaf, user);
            }
        }
    } while (!done);
}


/*
 *===========================================================================
 *                    ipcom_route_init_subtree_mask_cb
 *===========================================================================
 * Description: Initialize the internal node mask references on a subtree.
 * Parameters:  subtree - The head of the subtree.
 *              consecutive_mask  - Must be IP_FALSE if the masks can be
 *                                  non-consecutive, this operation is much
 *                                  expensive in this case.
 * Returns:
 *
 */
IP_STATIC void
ipcom_route_init_subtree_mask_cb(Ipcom_route_entry *rt, void *unused)
{
    /* Process the leaf node */
    Ipcom_route_entry *current;

    (void) unused;
    ip_assert(rt->on == IP_NULL);

    if (rt->mask != IP_NULL)
        for (current = rt->parent; current != IP_NULL; current = current->parent)
            if (!IPCOM_IS_BIT_SET(rt->mask, current->bit))
                current->mask = rt->mask;
}


/*
 *===========================================================================
 *                    ipcom_route_init_subtree_mask2
 *===========================================================================
 * Description: Initialize the internal node mask references on a subtree.
 * Parameters:  subtree - The head of the subtree.
 *              consecutive_mask  - Must be IP_FALSE if the masks can be
 *                                  non-consecutive, this operation is much
 *                                  expensive in this case.
 * Returns:
 *
 */
IP_STATIC void
ipcom_route_init_subtree_mask2(Ipcom_route_entry *subtree, Ip_bool consecutive_mask)
{
    ipcom_route_walk_subtree(subtree,
                             consecutive_mask,
                             IP_TRUE,
                             ipcom_route_init_subtree_mask_cb,
                             IP_NULL);
}


/*
 *===========================================================================
 *                    ipcom_route_free_subtree_cb
 *===========================================================================
 * Description: Frees the specified entry.
 * Parameters:  rt - The entry to free.
 * Returns:
 *
 */
IP_STATIC void
ipcom_route_free_subtree_cb(Ipcom_route_entry *rt, Ipcom_route *rtab)
{
    if (IP_BIT_ISSET(rt->flags, IPCOM_ROUTE_FLAG_INTERNAL))
        ipcom_free(rt);
    else
        rtab->notify_func(rtab, rt, IPCOM_ROUTE_CODE_REMOVE);
}


/*
 *===========================================================================
 *                    ipcom_route_prev_entry
 *===========================================================================
 * Description: Returns the previous leaf element in the route table.
 * Parameters:  rt - The current route entry
 * Returns:     The previous route entry or IP_NULL if the passed entry was
 *              the first.
 *
 */
IP_INLINE Ipcom_route_entry *
ipcom_route_prev_entry(Ipcom_route_entry *rt)
{
    while (rt->parent->off == rt)
    {
        rt = rt->parent;
        if (rt->parent == IP_NULL)
            return IP_NULL;
    }
    rt = rt->parent->off;
    while (rt->on != IP_NULL)
        rt = rt->on;
    if (IP_BIT_ISSET(rt->flags, IPCOM_ROUTE_FLAG_INIT))
        return IP_NULL;
    return rt;
}


/*
 *===========================================================================
 *                    ipcom_route_last_entry
 *===========================================================================
 * Description: Returns the last entry in the route table.
 * Parameters:  rtab - The route table for which the last entry should be returned.
 * Returns:     The last route entry or IP_NULL if the route table is empty.
 *
 */
IP_INLINE Ipcom_route_entry *
ipcom_route_last_entry(Ipcom_route *rtab)
{
    Ipcom_route_entry *rt;

    rt = rtab->top;
    while (rt->on != IP_NULL)
        rt = rt->on;
    if (IP_BIT_ISSET(rt->flags, IPCOM_ROUTE_FLAG_INIT))
        rt = ipcom_route_prev_entry(rt);

    return rt;
}


/*
 ****************************************************************************
 * 9                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_route_next_entry
 *===========================================================================
 * Description: Returns the next leaf element in the route table.
 * Parameters:  rt - The current route entry
 * Returns:     The next route entry or IP_NULL if the passed entry was the last.
 *
 */
IP_GLOBAL Ipcom_route_entry *
ipcom_route_next_entry(Ipcom_route_entry *rt)
{
    while (rt->parent->on == rt)
    {
        rt = rt->parent;
        if (rt->parent == IP_NULL)
            return IP_NULL;
    }
    rt = rt->parent->on;
    while (rt->off != IP_NULL)
        rt = rt->off;
    if (IP_BIT_ISSET(rt->flags, IPCOM_ROUTE_FLAG_INIT))
        return IP_NULL;
    return rt;
}


/*
 *===========================================================================
 *                    ipcom_route_first_entry
 *===========================================================================
 * Description: Returns the first entry in the route table.
 * Parameters:  rtab - The route table for which the first entry should be returned.
 * Returns:     The first route entry or IP_NULL if the route table is empty.
 *
 */
IP_GLOBAL Ipcom_route_entry *
ipcom_route_first_entry(Ipcom_route *rtab)
{
    Ipcom_route_entry *rt;

    rt = rtab->top;
    while (rt->off != IP_NULL)
        rt = rt->off;
    if (IP_BIT_ISSET(rt->flags, IPCOM_ROUTE_FLAG_INIT))
        rt = ipcom_route_next_entry(rt);

    return rt;
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_route_lookup
 *===========================================================================
 * Description: Returns the matching entry in the route table.
 * Parameters:  rtab - The route table to search in.
 *              key - The key to use when searching. Must be at least
 *              as long as specified in rtab->bit_size.
 *              perfect_match - Will be set to IP_TRUE if the match was
 *              perfect. Can be IP_NULL if this it not interesting.
 * Returns:     The matching route entry (or IP_NULL if nothing matches).
 *
 */
IP_PUBLIC Ipcom_route_entry *
ipcom_route_lookup(Ipcom_route *rtab, const void *key, Ip_bool *prefect_match)
{
    Ipcom_route_entry *current;
    Ipcom_route_entry *found;

    found = ipcom_patricia_search(rtab->top, key, IP_NULL);
    if (ipcom_route_key_cmp(rtab->bit_count, key, found->key, IP_NULL)
        == IPCOM_ROUTE_KEY_CMP_PREFECT_MATCH)
    {
        if (IP_BIT_ISSET(found->flags, IPCOM_ROUTE_FLAG_INIT))
            /* This entry is one of the two entries added when the tree was created */
            return IP_NULL;
        if (prefect_match != IP_NULL)
            *prefect_match = IP_TRUE;
        return found;
    }

    /* Start backtracking */
    current = found;
    while (ipcom_route_key_cmp(rtab->bit_count, key, found->key, found->mask)
           == IPCOM_ROUTE_KEY_CMP_NO_MATCH)
    {
        if (rtab->consecutive_mask == IP_FALSE
            && current->mask != found->mask)
        {
            found = ipcom_patricia_search(current, key, found->mask);
            if (ipcom_route_key_cmp(rtab->bit_count, key, found->key, found->mask)
                != IPCOM_ROUTE_KEY_CMP_NO_MATCH)
                break;
        }

        do
        {
            /* Go up to the next node with a netmask. */
            current = current->parent;
            if (current == IP_NULL)
                return IP_NULL;
        } while (current->mask == IP_NULL);
        found = ipcom_patricia_search(current, key, current->mask);
    }

    if (IP_BIT_ISSET(found->flags, IPCOM_ROUTE_FLAG_INIT))
        /* This entry is one of the two entries added when the tree was created */
        return IP_NULL;
    if (prefect_match != IP_NULL)
        *prefect_match = IP_FALSE;
    return found;
}


/*
 *===========================================================================
 *                    ipcom_route_new_table
 *===========================================================================
 * Description: Create a empty routing table with 3 nodes. 1 internal node
 *              and 2 leafs (all bits zero + all bits one).
 * Parameters:  addr_bit_size - The bitsize of the addresses in the address
 *              domain in which this routing table will be used.
 * Returns:     Head of the route table.
 *
 */
IP_PUBLIC Ipcom_route *
ipcom_route_new_table(Ip_u16 bit_count,
                      Ipcom_route_notify_func notify_func)
{
    Ip_u16 entry_size;
    Ip_u16 key_octet_count;
    Ipcom_route *rtab;
    Ipcom_route_entry *top;
    Ipcom_route_entry *off;
    Ipcom_route_entry *on;

    ip_assert(notify_func != IP_NULL);

    /* Round the bitsize up to the nearest size that can be divided by 4 */
    key_octet_count = (Ip_u16) (((bit_count + 31) & ~31) / 8);

    /* Allocate memory for all 3 nodes */
    entry_size = (Ip_u16) (sizeof(Ipcom_route_entry) + key_octet_count);
    rtab = ipcom_malloc(sizeof(Ipcom_route) + sizeof(Ipcom_route_entry) + entry_size * 2);
    if (rtab == IP_NULL)
        /* Not enough memory */
        return IP_NULL;
    top = (Ipcom_route_entry *) (rtab + 1);
    off = top + 1;
    on = (Ipcom_route_entry *) ((Ip_u8 *) off + entry_size);

    /* Init the route table */
    rtab->bit_count = bit_count;
    rtab->octet_count = key_octet_count;
    rtab->notify_func = notify_func;
    rtab->top = top;
    rtab->org_off = off;
    rtab->org_on = on;

    /* Init the internal node (head of table) */
    top->parent = IP_NULL;
    top->off = off;
    top->on = on;
    top->bit = 0;
    top->flags = IPCOM_ROUTE_FLAG_INIT | IPCOM_ROUTE_FLAG_INTERNAL;
    top->key = IP_NULL;
    top->mask = IP_NULL;

    /* All zero end mark (default route) */
    off->parent = top;
    off->on = IP_NULL;
    off->off = IP_NULL;
    off->bit = 0xffff;
    off->flags = IPCOM_ROUTE_FLAG_END | IPCOM_ROUTE_FLAG_INIT;
    off->key = (off + 1);
    ipcom_memset(off->key, 0, key_octet_count);
    off->mask = IP_NULL;

    /* All one end mark */
    on->parent = top;
    on->on = IP_NULL;
    on->off = IP_NULL;
    on->bit = 0xffff;
    on->flags = IPCOM_ROUTE_FLAG_END | IPCOM_ROUTE_FLAG_INIT;
    on->key = (on + 1);
    ipcom_memset(on->key, -1, key_octet_count);
    on->mask = IP_NULL;

    rtab->consecutive_mask = IP_TRUE;

    return rtab;
}


/*
 *===========================================================================
 *                    ipcom_route_free_table
 *===========================================================================
 * Description: Frees all entries in the route table including the route
 *              table head pointed to by 'rtab'.
 * Parameters:  rtab - The route table to free.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_route_free_table(Ipcom_route *rtab)
{
    ip_assert(rtab != IP_NULL);
    ipcom_route_walk_subtree(rtab->top,
                             IP_FALSE,
                             IP_FALSE,
                             (Ipcom_route_walk_subtree_cb) ipcom_route_free_subtree_cb,
                             rtab);
    ipcom_free(rtab);
}


/*
 *===========================================================================
 *                    iplite_add_route
 *===========================================================================
 * Description: Adds a route entry to the specified routing tabel.
 * Parameters:  rtab - The route tabel in which the element will be inserted.
 *              elem_to_add - The new node to insert. The following fields must
 *              have valid values:
 *                  flags must contain any user defined flags.
 *                  key must point to the search key
 *                  mask must point to the mask or be IP_NULL if the mask is all 1:s.
 *              all other fields are initialized by this function.
 * Returns:     IPCOM_SUCCESS or ipcom error code.
 *
 */
IP_PUBLIC Ip_err
ipcom_route_add(Ipcom_route *rtab, Ipcom_route_entry *elem_to_add)
{
    Ip_err err;

    ip_assert(elem_to_add->key != IP_NULL);
    elem_to_add->off    = IP_NULL;
    elem_to_add->on     = IP_NULL;
    elem_to_add->flags &= IPCOM_ROUTE_FLAG_MASK;

    ip_assert(rtab->notify_func != IP_NULL);

    err = ipcom_patricia_insert(rtab, elem_to_add);
    if (err == IPCOM_SUCCESS)
        rtab->notify_func(rtab, elem_to_add, IPCOM_ROUTE_CODE_ADD);
    return err;
}


/*
 *===========================================================================
 *                    ipcom_route_remove
 *===========================================================================
 * Description: Removes the specified entry from the route tabel.
 *              NOTE: The entry is only removed, not freed.
 * Parameters:  top - The top node in the route table.
 *              elem_to_remove - The element to remove.
 * Returns:     ipcom error code.
 *
 */
IP_PUBLIC Ip_err
ipcom_route_remove(Ipcom_route *rtab, Ipcom_route_entry *elem_to_remove)
{
    Ipcom_route_entry  *del_internal;
    Ipcom_route_entry  *internal;
    Ipcom_route_entry **pleaf;
    Ip_bool             fix_subtree_mask = IP_FALSE;

    /* The element to remove must be a leaf */
    ip_assert(elem_to_remove->off == IP_NULL);
    ip_assert(elem_to_remove->on == IP_NULL);

    if (elem_to_remove->mask != IP_NULL)
    {
        /* There might be internal node that has a reference to this mask */
        for (internal = elem_to_remove->parent;
             internal != IP_NULL;
             internal = internal->parent)
        {
            if (internal->mask == elem_to_remove->mask)
            {
                internal->mask = IP_NULL;
                fix_subtree_mask = IP_TRUE;
            }
        }
    }

    if (IP_BIT_ISSET(elem_to_remove->flags, IPCOM_ROUTE_FLAG_END))
    {
        /* Put back the orginal end node */
        internal = elem_to_remove->parent;
        if (elem_to_remove->parent->on == elem_to_remove)
        {
            elem_to_remove->parent->on = rtab->org_on;
            rtab->org_on->parent = internal;
        }
        else
        {
            elem_to_remove->parent->off = rtab->org_off;
            rtab->org_off->parent = internal;
        }
    }
    else
    {
        /* Remove the parent (internal node) of the remove entry also */
        del_internal = elem_to_remove->parent;
        ip_assert(del_internal != IP_NULL);
        internal = del_internal->parent;
        ip_assert(internal != IP_NULL);

        if (internal->off == del_internal)
            pleaf = &internal->off;
        else
            pleaf = &internal->on;

        /* Link the other subtree/leaf to the parent of the removed internal node */
        if (del_internal->on == elem_to_remove)
            *pleaf = del_internal->off;
        else
            *pleaf = del_internal->on;
        (*pleaf)->parent = internal;

        ipcom_free(del_internal);
    }

    if (fix_subtree_mask)
        /* The 'internal' variable now points to the nearest parent of deleted entry */
        ipcom_route_init_subtree_mask2(internal, rtab->consecutive_mask);

    elem_to_remove->parent = IP_NULL;
    if (IP_BIT_ISFALSE(elem_to_remove->flags, IPCOM_ROUTE_FLAG_INIT))
        rtab->notify_func(rtab, elem_to_remove, IPCOM_ROUTE_CODE_REMOVE);

    rtab->restart_walk = IP_TRUE;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_route_walk_tree
 *===========================================================================
 * Description: Calls a callback function for each element in the route tree.
 * Parameters:  rtab - The route tree to use
 *              cb - Callback function to that will be called for each route entry.
 *              data - User data that will be passed each time the callback
 *              function is called.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_route_walk_tree(Ipcom_route *rtab,
                      Ipcom_route_walk_cb cb,
                      void *data)
{
    Ipcom_route_entry *current;
    Ipcom_route_entry *next;

    current = ipcom_route_first_entry(rtab);

    rtab->restart_walk = IP_FALSE;
    while (current != IP_NULL)
    {
        next = ipcom_route_next_entry(current);
        if (cb(current, data))
        {
            /* The callback told us to remove this entry */
            (void) ipcom_route_remove(rtab, current);
            rtab->restart_walk = IP_FALSE;
        }
        if (rtab->restart_walk)
        {
            rtab->restart_walk = IP_FALSE;
            next = ipcom_route_first_entry(rtab);
        }
        current = next;
    }
    rtab->restart_walk = IP_TRUE;
}


/*
 *===========================================================================
 *                    ipcom_route_walk_tree_backwards
 *===========================================================================
 * Description: Calls a callback function for each element in the route tree
 *              in backwards order.
 * Parameters:  rtab - The route tree to use
 *              cb - Callback function to that will be called for each route entry.
 *              data - User data that will be passed each time the callback
 *              function is called.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_route_walk_tree_backwards(Ipcom_route *rtab,
                                Ipcom_route_walk_cb cb,
                                void *data)
{
    Ipcom_route_entry *current;
    Ipcom_route_entry *prev;

    current = ipcom_route_last_entry(rtab);

    rtab->restart_walk = IP_FALSE;
    while (current != IP_NULL)
    {
        prev = ipcom_route_prev_entry(current);
        if (cb(current, data))
        {
            /* The callback told us to remove this entry */
            (void) ipcom_route_remove(rtab, current);
            rtab->restart_walk = IP_FALSE;
        }
        if (rtab->restart_walk)
        {
            rtab->restart_walk = IP_FALSE;
            prev = ipcom_route_last_entry(rtab);
        }
        current = prev;
    }
    rtab->restart_walk = IP_TRUE;
}


/*
 *===========================================================================
 *                    ipcom_route_key_cmp
 *===========================================================================
 * Description: Compares 2 keys with or without a mask and returns
 *              the result.
 * Parameters:  key_bit_size - The size of the keys in bits.
 *              key1 - A key.
 *              key2 - Another key.
 *              mask - The mask to use when comparing the keys, a 0 bit means
 *              that this bit is alwasy a match. A value of IP_NULL is
 *              interpreted as a mask of all 1.
 * Returns:     IPCOM_ROUTE_KEY_CMP_PREFECT_MATCH,
 *              IPCOM_ROUTE_KEY_CMP_MASK_MATCH,
 *              IPCOM_ROUTE_KEY_CMP_NO_MATCH
 */
/*lint -efunc(413,ipcom_route_key_cmp) Likely use of null pointer */
IP_PUBLIC int
ipcom_route_key_cmp(Ip_u32 key_bit_size, const void *key1, const void *key2, const void *mask)
{
    int i = 0;

    /* Check if the key bit size is dividable with 16 */
    if ((key_bit_size & 0xf) == 0)
    {
        int key_byte_size;
        int key_short_size = (int)(key_bit_size >> 4);

        /* Check for perfect match first */
        for (; i < key_short_size; i++)
        {
            if (((Ip_u16 *) key1)[i] != ((Ip_u16 *) key2)[i])
                break;
        }
        if (i == key_short_size)
            return IPCOM_ROUTE_KEY_CMP_PREFECT_MATCH;

        /* No perfect match, check for mask match */
        key_byte_size = key_short_size << 1;
        for (i <<= 1; i < key_byte_size; i++)
        {
            if (mask && ((Ip_u8 *) mask)[i] != 0xff)
                break;
            if (((Ip_u8 *) key1)[i] != ((Ip_u8 *) key2)[i])
                return IPCOM_ROUTE_KEY_CMP_NO_MATCH;
        }

        ip_assert(mask != IP_NULL);
        while (i < key_byte_size && ((Ip_u8 *) mask)[i] == 0)
            i++;
    }

    for (i <<= 3; i < (int)key_bit_size; i++)
        if (mask == IP_NULL || IPCOM_IS_BIT_SET(mask, i))
            if (IPCOM_IS_BIT_SET(key1, i) != IPCOM_IS_BIT_SET(key2, i))
                return IPCOM_ROUTE_KEY_CMP_NO_MATCH;

    return IPCOM_ROUTE_KEY_CMP_MASK_MATCH;
}


/*
 *===========================================================================
 *                    ipcom_route_init_subtree_mask
 *===========================================================================
 * Description: Initialize the internal node mask references on a subtree.
 * Parameters:  rt - The head of the subtree.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_route_init_subtree_mask(Ipcom_route_entry *rt)
{
    ipcom_route_init_subtree_mask2(rt, IP_FALSE);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

