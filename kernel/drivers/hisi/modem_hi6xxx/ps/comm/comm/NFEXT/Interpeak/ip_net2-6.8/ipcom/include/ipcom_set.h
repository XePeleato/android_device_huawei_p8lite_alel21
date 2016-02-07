/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_set.h,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_set.h,v $
 *   $Author: kenneth $ $Date: 2006-05-08 14:15:43 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_SET_H
#define IPCOM_SET_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Minimalistic unorder set implementation.
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

#include <ipcom_type.h>
#include <ipcom_cstyle.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Helper constants for the "set->user" variable, the set implementation
   does not used the variable for anything */
#define IPCOM_SET_IS_INCLUDE 1
#define IPCOM_SET_IS_EXCLUDE 2

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipcom_set_foreach_cb_func
 *===========================================================================
 * Description: Callback function that is called for each element in the set
 *              when using ipcom_set_for_each().
 * Parameters:  obj - A object in the set.
 *              data - User defined data passed to ipcom_set_for_each().
 * Returns:
 *
 */
typedef void (*Ipcom_set_foreach_cb_func)(void *obj, void *data);

typedef struct Ipcom_set_struct
{
    Ip_size_t  obj_maxcount; /* The current maximum number of elements */
    Ip_size_t  obj_count;    /* Number of elements in the set */
    Ip_size_t  obj_len;      /* Size of each element */
    char      *objs;         /* Buffer where all elements are stored */
    int        user;         /* Use defined data associated with this set */
}
Ipcom_set;

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_set_new
 *===========================================================================
 * Description: Create a new empty set.
 * Parameters:  obj_len - The length of each element.
 *              key to an element.
 * Returns:     A newly created set.
 *
 */
IP_PUBLIC Ipcom_set *
ipcom_set_new(Ip_size_t obj_len);

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
ipcom_set_delete(Ipcom_set *set);

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
ipcom_set_clone(Ipcom_set *set);

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
ipcom_set_add(Ipcom_set *set, void *obj);

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
ipcom_set_remove(Ipcom_set *set, void *obj);


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
ipcom_set_remove_all(Ipcom_set *set);

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
ipcom_set_contains(Ipcom_set *set, IP_CONST void *obj);

/*
 *===========================================================================
 *                    ipcom_set_equal
 *===========================================================================
 * Description: Returns if 'a' contains the same members as 'b'
 * Parameters:  a - A set pointer, must not be IP_NULL.
 *              b - Another set pointer, may be IP_NULL which is treated like
 *                  an empty set.
 * Returns:     IP_TRUE if they contains the same members.
 *              IP_FALSE otherwise
 *
 */
IP_PUBLIC Ip_bool
ipcom_set_equal(Ipcom_set *a, Ipcom_set *b);

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
ipcom_set_subtract(Ipcom_set *a, Ipcom_set *b);

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
ipcom_set_union(Ipcom_set *a, Ipcom_set *b);

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
ipcom_set_intersection(Ipcom_set *a, Ipcom_set *b);

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
                   void *data);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
