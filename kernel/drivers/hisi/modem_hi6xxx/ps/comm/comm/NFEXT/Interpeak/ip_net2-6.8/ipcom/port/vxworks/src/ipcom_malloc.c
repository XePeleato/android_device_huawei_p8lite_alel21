/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_malloc.c,v $ $Revision: 1.15 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_malloc.c,v $
 *     $Author: kenneth $ $Date: 2007-01-31 10:40:17 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *        Design and implementation by Lennart Bang <lob@interpeak.se>
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

#include <vxWorks.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipcom_clib.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#ifdef ipcom_malloc
#undef ipcom_malloc
#endif
#ifdef ipcom_calloc
#undef ipcom_calloc
#endif
#ifdef ipcom_realloc
#undef ipcom_realloc
#endif
#ifdef ipcom_free
#undef ipcom_free
#endif


/*
 ****************************************************************************
 * 6                    TYPES
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

#ifdef IP_DEBUG
IP_STATIC Ip_s32 no_of_bufs = 0;
IP_STATIC Ip_s32 tot_no_of_bufs = 0;
#endif


/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_malloc_stat
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IP_DEBUG
IP_PUBLIC void
ipcom_malloc_stat(void)
{
    ipcom_printf("Current no of bufs: %d\n", no_of_bufs);
    ipcom_printf("Total no of bufs: %d\n", tot_no_of_bufs);
}
#endif


/*
 *===========================================================================
 *                         ipcom_free
 *===========================================================================
 * Description:   Free the dynamic memory pointer to by 'ptr'. If 'ptr'
 *                is IP_NULL, no operation is performed.
 * Parameters:    ptr - Pointer to the dynamic memory to free.
 * Returns:       .
 *
 */
IP_PUBLIC void
ipcom_free(void *ptr)
{
    if(ptr != IP_NULL)
    {
#ifdef IP_DEBUG
        no_of_bufs--;
#endif
        free(ptr);
    }
}


/*
 *===========================================================================
 *                    ipcom_calloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_calloc(Ip_size_t nelem, Ip_size_t elsize)
{
#ifdef IP_DEBUG
    no_of_bufs++;
    tot_no_of_bufs++;
#endif

    return calloc(nelem, elsize);
}


/*
 *===========================================================================
 *                    ipcom_realloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_realloc(void *ptr, Ip_size_t size)
{
    return realloc(ptr, (size_t)size);
}


/*
 *===========================================================================
 *                    ipcom_malloc
 *===========================================================================
 * Description:   Allocate 'size' bytes of consecutive dynamic memory which
 *                is uninitialized. The allocated memory must later be freed
 *                by ipcom_free() or ipcom_freep().
 * Parameters:    size - The number of bytes to allocate.
 * Returns:       Pointer to the allocated dynamic memory.
 *
 */
IP_PUBLIC void *
ipcom_malloc(Ip_size_t size)
{
    Ip_u8  *buf;

    buf = malloc(size);

#ifdef IP_DEBUG
    no_of_bufs++;
    tot_no_of_bufs++;
#endif

    return buf;
}

/*
 *===========================================================================
 *                    ipcom_cache_line_size
 *===========================================================================
 * Description:   Get the system cache line size in bytes.
 * Parameters:    N/A
 * Returns:       Number of bytes in a cache line.
 *
 */
IP_PUBLIC Ip_u32
ipcom_cache_line_size( void )
{
	return _CACHE_ALIGN_SIZE;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


