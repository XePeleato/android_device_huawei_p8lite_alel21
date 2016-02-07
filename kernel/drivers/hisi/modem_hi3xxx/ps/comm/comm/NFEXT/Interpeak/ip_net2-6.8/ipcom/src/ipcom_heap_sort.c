/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_heap_sort.c,v $ $Revision: 1.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_heap_sort.c,v $
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
#include "ipcom_heap_sort.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_HEAP_SORT_GET_ELEM(ptr, indx, siz) ((Ip_u8*) (ptr) + (((indx) - 1) * (siz)))

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
 *                    ipcom_heap_sort_downheap
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_heap_sort_downheap(void *base, Ip_size_t elem_size, int root, int last,
                         Ipcom_heap_sort_cmp_func cmp_func, void *tmp)
{
    int new_r;
    int max_r = last >> 1;

    ipcom_memcpy(tmp,
                 IPCOM_HEAP_SORT_GET_ELEM(base, root, elem_size),
                 elem_size);

    while (root <= max_r)
    {
        new_r = root << 1;
        if (new_r < last
            && cmp_func(IPCOM_HEAP_SORT_GET_ELEM(base, new_r, elem_size),
                        IPCOM_HEAP_SORT_GET_ELEM(base, new_r + 1, elem_size)) < 0)
        {
            new_r++;
        }

        if (cmp_func(tmp, IPCOM_HEAP_SORT_GET_ELEM(base, new_r, elem_size)) >= 0)
            goto done;

        ipcom_memcpy(IPCOM_HEAP_SORT_GET_ELEM(base, root, elem_size),
                     IPCOM_HEAP_SORT_GET_ELEM(base, new_r, elem_size),
                     elem_size);
        root = new_r;
    }

 done:
    ipcom_memcpy(IPCOM_HEAP_SORT_GET_ELEM(base, root, elem_size),
                 tmp,
                 elem_size);
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

IP_PUBLIC void
ipcom_heap_sort(void *base, Ip_size_t num_members, Ip_size_t elem_size,
                Ipcom_heap_sort_cmp_func cmp_func, void *tmp)
{
    int i;

    for (i = num_members >> 1; i >= 1; i--)
        ipcom_heap_sort_downheap(base, elem_size, i, num_members, cmp_func, tmp);

    for (i = num_members; i > 1; i--)
    {
        ipcom_memcpy(tmp,
                     IPCOM_HEAP_SORT_GET_ELEM(base, i, elem_size),
                     elem_size);
        ipcom_memcpy(IPCOM_HEAP_SORT_GET_ELEM(base, i, elem_size),
                     IPCOM_HEAP_SORT_GET_ELEM(base, 1, elem_size),
                     elem_size);
        ipcom_memcpy(IPCOM_HEAP_SORT_GET_ELEM(base, 1, elem_size),
                     tmp,
                     elem_size);
        ipcom_heap_sort_downheap(base, elem_size, 1, i - 1, cmp_func, tmp);
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

