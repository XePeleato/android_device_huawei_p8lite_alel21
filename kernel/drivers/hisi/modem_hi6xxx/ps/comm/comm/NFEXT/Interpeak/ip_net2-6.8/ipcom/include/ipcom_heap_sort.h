/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_heap_sort.h,v $ $Revision: 1.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_heap_sort.h,v $
 *   $Author: kenneth $ $Date: 2005-10-31 21:17:39 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_HEAP_SORT_H
#define IPCOM_HEAP_SORT_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Heap sort of an array. Heap sort is the slower than quick sort and
   merge sort, but can run only very small stack since it is not recursive */


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

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipcom_heap_sort_cmp_func
 *===========================================================================
 * Description: Compares two objects in the queue.
 * Parameters:  obj1 - An object in the queue.
 *              obj2 - Another object in the queue.
 * Returns:     <0 if obj1 should come before obj2
 *               0 if it does not matter if obj1 or obj2 is first.
 *              >0 if obj1 should come after obj2
 */
typedef int (*Ipcom_heap_sort_cmp_func)(IP_CONST void *obj1, IP_CONST void *obj2);


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipcom_heap_sort
 *===========================================================================
 * Description: Sorts an array of elements using heap sort.
 * Parameters:  base - Pointer to the first element.
 *              num_members - Number of members in the array.
 *              elem_size - Size of each element.
 *              cmp_func - Comparison function.
 *              tmp - Must point to a temporary buffer of size 'elem_size'
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_heap_sort(void *base, Ip_size_t num_members, Ip_size_t elem_size,
                Ipcom_heap_sort_cmp_func cmp_func, void *tmp);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
