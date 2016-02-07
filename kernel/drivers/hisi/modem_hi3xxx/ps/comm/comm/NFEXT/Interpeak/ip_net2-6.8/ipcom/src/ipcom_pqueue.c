/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pqueue.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_pqueue.c,v $
 *   $Author: kenneth $
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
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_pqueue.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* The initial size of the heap */
#define IPCOM_PQUEUE_INITIAL_SIZE          32


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
 *                    ipcom_pqueue_upheap
 *===========================================================================
 * Description: Walks upwards in the heap and restores the heap property.
 * Parameters:  pqueue - The queue to fix the heap property for.
 *              i - The index to start the walk from.
 * Returns:     The index the last operation was performed on.
 *
 */
IP_STATIC int
ipcom_pqueue_upheap(Ipcom_pqueue *pqueue, int i)
{
    int *obj;
    int new_i;

    obj = pqueue->heap[i];
    for (;;)
    {
        new_i = i >> 1;
        if (new_i == 0 || pqueue->cmp_func(pqueue->heap[new_i], obj) > 0)
            break;

        pqueue->heap[i] = pqueue->heap[new_i];
        pqueue->store_index_func(pqueue->heap[i], i);
        i = new_i;
    }
    pqueue->heap[i] = obj;
    pqueue->store_index_func(obj, i);
    return i;
}


/*
 *===========================================================================
 *                    ipcom_pqueue_downheap
 *===========================================================================
 * Description: Walks downwards in the heap and restores the heap property.
 * Parameters:  pqueue - The queue to fix the heap property for.
 *              i - The index to start the walk from.
 * Returns:
 *
 */
IP_STATIC void
ipcom_pqueue_downheap(Ipcom_pqueue *pqueue, int i)
{
    int *obj;
    int new_i;
    int max_i = pqueue->num_elem >> 1;

    obj = pqueue->heap[i];
    while (i <= max_i)
    {
        new_i = i << 1;
        if (new_i < pqueue->num_elem
            && pqueue->cmp_func(pqueue->heap[new_i], pqueue->heap[new_i + 1]) < 0)  /*lint !e679 */
        {
            new_i++;
        }

        if (pqueue->cmp_func(obj, pqueue->heap[new_i]) >= 0)
            goto done;

        pqueue->heap[i] = pqueue->heap[new_i];
        pqueue->store_index_func(pqueue->heap[i], i);
        i = new_i;
    }

 done:
    pqueue->heap[i] = obj;
    pqueue->store_index_func(obj, i);
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
 *                    ipcom_pqueue_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_pqueue *
ipcom_pqueue_new(Ipcom_pqueue_cmp_func    cmp_func,
                 Ipcom_pqueue_store_index store_index_func)
{
    Ipcom_pqueue *pqueue;

    pqueue = ipcom_calloc(1, sizeof(Ipcom_pqueue));
    if (pqueue != IP_NULL)
    {
        pqueue->heap = ipcom_malloc((IPCOM_PQUEUE_INITIAL_SIZE + 1) * sizeof(void*));
        if (pqueue->heap == IP_NULL)
        {
            ipcom_free(pqueue);
            return IP_NULL;
        }
        pqueue->capacity         = IPCOM_PQUEUE_INITIAL_SIZE;
        pqueue->cmp_func         = cmp_func;
        pqueue->store_index_func = store_index_func;
    }
    return pqueue;
}


/*
 *===========================================================================
 *                    ipcom_pqueue_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pqueue_delete(Ipcom_pqueue *pqueue)
{
    ipcom_free(pqueue->heap);
    ipcom_free(pqueue);
}


/*
 *===========================================================================
 *                    ipcom_pqueue_insert
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_pqueue_insert(Ipcom_pqueue *pqueue, void *obj)
{
    if (pqueue->num_elem + 1 > pqueue->capacity)
    {
        void *heap_ptr = ipcom_realloc(pqueue->heap, ((pqueue->capacity << 1) + 1) * sizeof(void*)); /*lint !e647 */
        if (heap_ptr == IP_NULL)
            return IPCOM_ERR_NO_MEMORY;
        pqueue->heap = heap_ptr;
        pqueue->capacity <<= 1;
    }

    pqueue->num_elem++;
    pqueue->heap[pqueue->num_elem] = obj;
    (void)ipcom_pqueue_upheap(pqueue, pqueue->num_elem);
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_pqueue_remove
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_pqueue_remove(Ipcom_pqueue *pqueue, int pindex)
{
    void *obj;

    if (pindex > pqueue->num_elem)
        return IP_NULL;

    /* Set the index for the entry that is beeing removed */
    obj = pqueue->heap[pindex];
    pqueue->store_index_func(obj, 0);
    if (pindex < pqueue->num_elem--)
    {
        /* Swap the last entry and the entry to remove */
        pqueue->heap[pindex] = pqueue->heap[pqueue->num_elem + 1];  /*lint !e679 */
        pqueue->store_index_func(pqueue->heap[pindex], pindex);
        ipcom_pqueue_downheap(pqueue, ipcom_pqueue_upheap(pqueue, pindex));
    }
    return obj;
}


/*
 *===========================================================================
 *                    ipcom_pqueue_remove_next
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_pqueue_remove_next(Ipcom_pqueue *pqueue)
{
    return ipcom_pqueue_remove(pqueue, 1);
}


/*
 *===========================================================================
 *                    ipcom_pqueue_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_PUBLIC void *
ipcom_pqueue_get(Ipcom_pqueue *pqueue, int pindex)
{
    void *obj;

    if (pindex > pqueue->num_elem)
        return IP_NULL;
    obj = pqueue->heap[pindex];
    return obj;
}


/*
 *===========================================================================
 *                    ipcom_pqueue_get_next
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_pqueue_get_next(Ipcom_pqueue *pqueue)
{
    if (pqueue->num_elem == 0)
        return IP_NULL;
    return pqueue->heap[1];
}


/*
 *===========================================================================
 *                    ipcom_pqueue_for_each
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pqueue_for_each(Ipcom_pqueue *pqueue, Ipcom_pqueue_forech_cb_func cb_func, void *data)
{
    int i;

    for (i = 1; i <= pqueue->num_elem; i++)
        cb_func(pqueue->heap[i], data);
}


/*
 *===========================================================================
 *                    ipcom_pqueue_size
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_pqueue_size(Ipcom_pqueue *pqueue)
{
    return pqueue->num_elem;
}


/*
 *===========================================================================
 *                    ipcom_pqueue_nop_store_index
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pqueue_nop_store_index(void *obj, int indx)
{
    (void) obj;
    (void) indx;
}


#ifdef IPCOM_PQUEUE_LOG_DUMP
/*
 *===========================================================================
 *                    ipcom_pqueue_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_pqueue_print(Ipcom_pqueue *pqueue, int index, int tabs, void (*f)(void *obj, char *buf, int index), char *buf)
{
    int t;

    if ((index << 1) <= pqueue->num_elem)
        buf = ipcom_pqueue_print(pqueue, index << 1, tabs + 1, f, buf);

    for (t = 0; t < tabs; t++)
        sprintf(buf++, "\t");
    f(pqueue->heap[index], buf, index);
    buf += ipcom_strlen(buf);

    if ((index << 1) + 1 <= pqueue->num_elem)
        buf = ipcom_pqueue_print(pqueue, (index << 1) + 1, tabs + 1, f, buf);
    return buf;
}


/*
 *===========================================================================
 *                    ipcom_pqueue_log
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pqueue_log(Ipcom_pqueue *pqueue, void (*f)(void *obj, char *buf, int index))
{
    char buf[10000];
    if (pqueue->num_elem)
    {
        ipcom_memset(buf, 0, sizeof(buf));
        ipcom_pqueue_print(pqueue, 1, 0, f, buf);
        IPCOM_LOG1(DEBUG2, "*** PQ DUMP ***\n%s", buf);
    }
}
#endif /* IPCOM_PQUEUE_LOG_DUMP */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

