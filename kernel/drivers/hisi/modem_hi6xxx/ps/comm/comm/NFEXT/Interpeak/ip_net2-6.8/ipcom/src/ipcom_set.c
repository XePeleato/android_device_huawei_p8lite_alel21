/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_set.c,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_set.c,v $
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
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_set.h>
#include <ipcom_type.h>
#include <ipcom_err.h>

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
 *===========================================================================
 *                    ipcom_set_index
 *===========================================================================
 * Description: Returns the index the specified object has.
 * Parameters:  set - A set pointer.
 *              obj - The object to find the index for.
 * Returns:     [0..set->obj_count-1] the index of the element
 *              set->obj_count if the element was not found.
 *
 */
IP_STATIC Ip_size_t
ipcom_set_index(Ipcom_set *set, IP_CONST void *obj)
{
    Ip_size_t i;

    for (i = 0; i < set->obj_count; i++)
        if (ipcom_memcmp(obj, &set->objs[i * set->obj_len], set->obj_len) == 0)
            break;
    return i;
}

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
 *                    ipcom_set_new
 *===========================================================================
 * Description: Create a new empty set.
 * Parameters:  obj_len - The length of each element.
 * Returns:     A newly created set.
 *
 */
IP_PUBLIC Ipcom_set *
ipcom_set_new(Ip_size_t obj_len)
{
    Ipcom_set *set;

    set = ipcom_malloc(sizeof(Ipcom_set));
    if (set == IP_NULL)
        return IP_NULL;

    set->obj_maxcount = 0;
    set->obj_count    = 0;
    set->obj_len      = obj_len;
    set->objs         = IP_NULL;
    return set;
}


/*
 *===========================================================================
 *                    ipcom_set_delete
 *===========================================================================
 * Description: Frees the resources allocated by the set.
 * Parameters:  set - The set to delete.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_set_delete(Ipcom_set *set)
{
    if (set == IP_NULL)
        return;
    ipcom_free(set->objs);
    ipcom_free(set);
}


/*
 *===========================================================================
 *                    ipcom_set_clone
 *===========================================================================
 * Description: Returns a set with the same members as the passed set.
 * Parameters:  set - The set to clone.
 * Returns:     The cloned set or IP_NULL if out of memory.
 *
 */
IP_PUBLIC Ipcom_set *
ipcom_set_clone(Ipcom_set *set)
{
    Ipcom_set *clone;

    clone = ipcom_set_new(set->obj_len);
    if (clone == IP_NULL)
        return IP_NULL;

    *clone = *set;
    if (clone->obj_maxcount)
    {
        /* Make a deep copy of all elements */
        clone->objs = ipcom_malloc(clone->obj_maxcount * clone->obj_len);
        if (clone->objs == IP_NULL)
        {
            ipcom_free(clone);
            return IP_NULL;
        }
        ipcom_memcpy(clone->objs, set->objs, clone->obj_count * clone->obj_len);
    }
    return clone;
}


/*
 *===========================================================================
 *                    ipcom_set_add
 *===========================================================================
 * Description: Adds an object to the set, a deep copy is made of obj.
 * Parameters:  set - A set pointer.
 *              obj - The object to add.
 * Returns:     IPCOM_SUCCESS if the object was added
 *              IPCOM_ERR_ALREADY_INSTALLED if a matching (or same) object
 *              already exists
 *              IPCOM_ERR_NO_MEMORY if the object could not be added due to
 *              insufficient memory
 *
 */
IP_PUBLIC Ip_err
ipcom_set_add(Ipcom_set *set, void *obj)
{
    if (ipcom_set_contains(set, obj))
        return IPCOM_ERR_ALREADY_INSTALLED;

    if (set->obj_maxcount == set->obj_count)
    {
        /* Must grow the set */
        void     *new_objs;
        Ip_size_t new_maxcount = (set->obj_maxcount ? set->obj_maxcount << 1 : 1);

        new_objs = ipcom_realloc(set->objs, new_maxcount * set->obj_len);
        if (new_objs == IP_NULL)
            return IPCOM_ERR_NO_MEMORY;

        set->objs         = new_objs;
        set->obj_maxcount = new_maxcount;
    }

    ipcom_memcpy(&set->objs[set->obj_len * set->obj_count],
                 obj,
                 set->obj_len);
    ++set->obj_count;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_set_remove
 *===========================================================================
 * Description: Removes an object from the set.
 * Parameters:  set - A set pointer.
 *              obj - The object to delete.
 * Returns:     IPCOM_SUCCESS if the object was removed.
 *              IPCOM_ERR_NOT_FOUND the object was not found in the set.
 *
 */
IP_PUBLIC Ip_err
ipcom_set_remove(Ipcom_set *set, void *obj)
{
    Ip_size_t i;

    i = ipcom_set_index(set, obj);
    if (i == set->obj_count)
        return IPCOM_ERR_NOT_FOUND;

    --set->obj_count;

    while (i++ < set->obj_count)
    {
        ipcom_memcpy(&set->objs[(i - 1) * set->obj_len],
                     &set->objs[i * set->obj_len],
                     set->obj_len);
    }

    if (set->obj_count < (set->obj_maxcount >> 1))
    {
        /* Time to shrink the set */
        void     *new_objs;
        Ip_size_t new_maxcount = set->obj_maxcount >> 1;

        new_objs = ipcom_realloc(set->objs, new_maxcount * set->obj_len);
        if (new_objs != IP_NULL)
        {
            set->objs         = new_objs;
            set->obj_maxcount = new_maxcount;
        }
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_set_remove_all
 *===========================================================================
 * Description: Removes all object from the set.
 * Parameters:  set - A set pointer.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_set_remove_all(Ipcom_set *set)
{
    ipcom_free(set->objs);
    set->objs         = IP_NULL;
    set->obj_maxcount = 0;
    set->obj_count    = 0;
}


/*
 *===========================================================================
 *                    ipcom_set_contains
 *===========================================================================
 * Description: Returns if a specific object is already in the set.
 * Parameters:  set - A set pointer.
 *              obj - The object to check if it is part of the set.
 * Returns:     IP_TRUE if the object is part of the set.
 *              IP_FALSE otherwise
 *
 */
IP_PUBLIC Ip_bool
ipcom_set_contains(Ipcom_set *set, IP_CONST void *obj)
{
    return ipcom_set_index(set, obj) < set->obj_count;
}


/*
 *===========================================================================
 *                    ipcom_set_equal
 *===========================================================================
 * Description: Returns if 'a' contains the same members as 'b'
 * Parameters:  a - A set pointer.
 *              b - Another set pointer.
 * Returns:     IP_TRUE if they contains the same members.
 *              IP_FALSE otherwise
 *
 */
IP_PUBLIC Ip_bool
ipcom_set_equal(Ipcom_set *a, Ipcom_set *b)
{
    Ip_size_t i;

    if (b == IP_NULL)
        return a->obj_count == 0;

    if (a->obj_count != b->obj_count)
        return IP_FALSE;

    for (i = 0; i < a->obj_count; i++)
        if (ipcom_set_contains(b, (char *)a->objs + i * a->obj_len) == IP_FALSE)
            return IP_FALSE;

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipcom_set_subtract
 *===========================================================================
 * Description: Removes all members of set 'b' from set 'a' and stores
 *              the result in 'a'
 * Parameters:  a - a set.
 *              b - a set of members to remove from 'a'
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_set_subtract(Ipcom_set *a, Ipcom_set *b)
{
    Ip_size_t i;

    ip_assert(a->obj_len == b->obj_len);

    for (i = 0; i < b->obj_count; i++)
        (void)ipcom_set_remove(a, (char *)b->objs + i * b->obj_len);
}


/*
 *===========================================================================
 *                    ipcom_set_union
 *===========================================================================
 * Description: Returns the union of two sets and stores the result in 'a'.
 * Parameters:  a - A set.
 *              b - Another set.
 * Returns:     IPCOM_SUCCESS if the object was added
 *              IPCOM_ERR_NO_MEMORY if an object could not be added due to
 *              insufficient memory
 *
 */
IP_PUBLIC Ip_err
ipcom_set_union(Ipcom_set *a, Ipcom_set *b)
{
    Ip_size_t i;

    ip_assert(a->obj_len == b->obj_len);

    for (i = 0; i < b->obj_count; i++)
        if (ipcom_set_add(a, (char *)b->objs + i * b->obj_len) == IPCOM_ERR_NO_MEMORY)
            return IPCOM_ERR_NO_MEMORY;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_set_intersection
 *===========================================================================
 * Description: Returns the intersection of two sets, 'a' will contain
 *              all members that was part of both 'a' and 'b'.
 * Parameters:  a - A set.
 *              b - Another set.
 * Returns:     The intersection of the two sets.
 *
 */
IP_PUBLIC void
ipcom_set_intersection(Ipcom_set *a, Ipcom_set *b)
{
    Ip_size_t i = 0;
    void     *obj;

    ip_assert(a->obj_len == b->obj_len);

    while (i < a->obj_count)
    {
        obj = (char *)a->objs + i * a->obj_len;
        if (ipcom_set_contains(b, obj))
            i++;
        else
            (void)ipcom_set_remove(a, obj);
    }
}


/*
 *===========================================================================
 *                      ipcom_set_for_each
 *===========================================================================
 * Description: Calls the suppled function for each object in the set.
 * Parameters:  set - The set to travers.
 *              cb_func - Function to call for each element, with 'obj'
 *              set to the current object and 'data' is the values passed
 *              as 3:rd argument to this function.
 *              data - User defined data.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_set_for_each(Ipcom_set *set,
                   Ipcom_set_foreach_cb_func cb_func,
                   void *data)
{
    Ip_size_t i;

    for (i = set->obj_count; i > 0; i--)
        cb_func(&set->objs[(i - 1) * set->obj_len], data);
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

