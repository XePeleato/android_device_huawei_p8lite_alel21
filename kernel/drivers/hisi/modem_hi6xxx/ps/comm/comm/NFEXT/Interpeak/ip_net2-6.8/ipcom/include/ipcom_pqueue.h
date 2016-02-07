/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pqueue.h,v $ $Revision: 1.11 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_pqueue.h,v $
 *   $Author: kenneth $ $Date: 2007-05-25 21:29:05 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_PQUEUE_H
#define IPCOM_PQUEUE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Priority queue implemented using a binary heap structure */


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

#if !defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)
#define ipcom_pqueue_new ipcom_pqueue_new_usr
#define ipcom_pqueue_delete ipcom_pqueue_delete_usr
#define ipcom_pqueue_insert ipcom_pqueue_insert_usr
#define ipcom_pqueue_remove ipcom_pqueue_remove_usr
#define ipcom_pqueue_remove_next ipcom_pqueue_remove_next_usr
#define ipcom_pqueue_get ipcom_pqueue_get_usr
#define ipcom_pqueue_get_next ipcom_pqueue_get_next_usr
#define ipcom_pqueue_for_each ipcom_pqueue_for_each_usr
#define ipcom_pqueue_size ipcom_pqueue_size_usr
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipcom_pqueue_cmp_func
 *===========================================================================
 * Description: Compares two objects in the queue.
 * Parameters:  obj1 - An object in the queue.
 *              obj2 - Another object in the queue.
 * Returns:     <0 if obj1 should be removed from the queue after obj2
 *               0 if obj1 and obj2 should be removed at the same time
 *              >0 if obj1 should be removed from the queue before obj2
 */
typedef int (*Ipcom_pqueue_cmp_func)(IP_CONST void *obj1, IP_CONST void *obj2);


/*
 *===========================================================================
 *                    Ipcom_pqueue_store_index
 *===========================================================================
 * Description: Gives the index where the specified object is stored in the
 *              heap.
 * Parameters:  obj - An object in the queue.
 *              oindex - The index the object is stored at.
 * Returns:
 */
typedef void (*Ipcom_pqueue_store_index)(void *obj, int indx);


/*
 *===========================================================================
 *                         Ipcom_pqueue
 *===========================================================================
 * List head and entry structure.
 */
typedef struct Ipcom_pqueue_struct
{
    /* Function to compare the priority of two objects */
    Ipcom_pqueue_cmp_func      cmp_func;

    /* Pointer to a function that creates a hash value from a search key */
    Ipcom_pqueue_store_index   store_index_func;

    int         num_elem; /* The number of elements currently in the heap */
    int         capacity; /* The maximum size of the heap */
    void      **heap;     /* The heap array */
}
Ipcom_pqueue;


/*
 *===========================================================================
 *                    Ipcom_pqueue_forech_cb_func
 *===========================================================================
 * Description: Callback function that is called for each element in the
 *              priority queue when using ipcom_pqueue_for_each().*
 * Parameters:  obj - A object in the queue.
 *              data - User defined data passed to ipcom_pqueue_for_each().
 * Returns:
 *
 */
typedef void (*Ipcom_pqueue_forech_cb_func)(void *obj, void *data);


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_pqueue_new
 *===========================================================================
 * Description: Creates a empty priority queue.
 * Parameters:  cmp_func - A function that returns the relative priority
 *              between two objects.
 *              store_index_func - A function that will tell the user which
 *              index a object is stored at.
 * Returns:     A pointer to a new priority queue or IP_NULL if out of memory.
 *
 */
IP_PUBLIC Ipcom_pqueue *
ipcom_pqueue_new(Ipcom_pqueue_cmp_func    cmp_func,
                 Ipcom_pqueue_store_index store_index_func);


/*
 *===========================================================================
 *                    ipcom_pqueue_delete
 *===========================================================================
 * Description: Deletes a priority queue. NOTE: any remaining elements will
 *              NOT be freed.
 * Parameters:  pqueue - A priority queue.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pqueue_delete(Ipcom_pqueue *pqueue);


/*
 *===========================================================================
 *                    ipcom_pqueue_insert
 *===========================================================================
 * Description: Inserts a new object into the priority queue. O(log(N)).
 * Parameters:  pqueue - A priority queue.
 *              obj - The object to insert.
 * Returns:     IPCOM_SUCCESS if successful
 *              IPCOM_ERR_NO_MEMORY if out of memory.
 *
 */
IP_PUBLIC Ip_err
ipcom_pqueue_insert(Ipcom_pqueue *pqueue, void *obj);


/*
 *===========================================================================
 *                    ipcom_pqueue_remove
 *===========================================================================
 * Description: Removes the object located at the specified index. O(log(N))
 * Parameters:  pqueue - A priority queue.
 *              oindex - The index of the object to remove.
 * Returns:     The removed object or IP_NULL if no such object exist.
 *
 */
IP_PUBLIC void *
ipcom_pqueue_remove(Ipcom_pqueue *pqueue, int indx);


/*
 *===========================================================================
 *                    ipcom_pqueue_remove_next
 *===========================================================================
 * Description: Returns the element that will be dequeued next. O(log(N)).
 * Parameters:  pqueue - A priority queue.
 * Returns:     The element with highest priority or IP_NULL if the queue is
 *              empty.
 *
 */
IP_PUBLIC void *
ipcom_pqueue_remove_next(Ipcom_pqueue *pqueue);


/*
 *===========================================================================
 *                    ipcom_pqueue_get_next
 *===========================================================================
 * Description: Returns the element that will be dequeued next. O(1).
 * Parameters:  pqueue - A priority queue.
 * Returns:     The element with highest priority.
 *
 */
IP_PUBLIC void *
ipcom_pqueue_get_next(Ipcom_pqueue *pqueue);


/*
 *===========================================================================
 *                    ipcom_pqueue_get
 *===========================================================================
 * Description: Returns the element with index pindex. O(1).
 * Parameters:  pqueue - A priority queue.
 *              pindex - Index to the object to get.
 * Returns:     The element with highest priority.
 *
 */
IP_FASTTEXT IP_PUBLIC void *
ipcom_pqueue_get(Ipcom_pqueue *pqueue, int pindx);


/*
 *===========================================================================
 *                    ipcom_pqueue_size
 *===========================================================================
 * Description: Returns the number of elements in the queue. O(1).
 * Parameters:  pqueue - A priority queue.
 * Returns:     Number of elements in the queue.
 *
 */
IP_PUBLIC int
ipcom_pqueue_size(Ipcom_pqueue *pqueue);


/*
 *===========================================================================
 *                    ipcom_pqueue_for_each
 *===========================================================================
 * Description: Calls the suppled function for each object in the priority
 *              queue.
 *
 * Parameters:  pqueue - A priority queue.
 *              cb_func - Function to call for each element, with 'obj'
 *              set to the current object and 'data' is the values passed
 *              as 3:rd argument to this function.
 *              data - User defined data.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pqueue_for_each(Ipcom_pqueue *pqueue, Ipcom_pqueue_forech_cb_func cb_func, void *data);


/*
 *===========================================================================
 *                    ipcom_pqueue_nop_store_index
 *===========================================================================
 * Description: No-operation store index function.
 * Parameters:  obj - An object in the queue.
 *              indx - The index the object is stored at.
 * Returns:
 */
IP_PUBLIC void
ipcom_pqueue_nop_store_index(void *obj, int indx);


#ifdef IPCOM_PQUEUE_LOG_DUMP
/*
 *===========================================================================
 *                    ipcom_pqueue_log
 *===========================================================================
 * Description: For debug only: dumps the content of the priority queue
 *              to the syslog.
 * Parameters:  pqueue - A priority queue.
 *              print_func - Function that is called for each entry in the
 *              priority queue. obj is the object to print, buf is the buffer
 *              where the information to be printed should be written and
 *              index is the location the object has in the queue.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pqueue_log(Ipcom_pqueue *pqueue, void (*print_func)(void *obj, char *buf, int indx));
#endif /* IPCOM_PQUEUE_LOG_DUMP */

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
