/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks_nehalem_forwarder_common.c,v $ $Revision: 1.2.6.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_vxworks_nehalem_forwarder_common.c,v $
 *     $Author: dlkrejsa $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This is the port specific part of the 'ipcom_forwarder'. It is only valid
 * if IPCOM_USE_NEHALEM_FORWARDER is defined (typically in
 * ipcom/config/ipcom_config.h). When IPCOM_USE_NEHALEM_FORWARDER is not
 * defined, the contents of this file is "ifdefed out".
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/* Refer to description above*/
#ifdef IPCOM_USE_NEHALEM_FORWARDER

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_vxworks.h>

#include <ipcom_forwarder.h>
#include <ipcom_vxworks_nehalem_forwarder.h>

#ifdef IPCOM_FORWARDER_AMP
#include <ipcom_shared_mem.h>
#endif

#include <string.h>
#include <stdio.h>
#include <memLib.h>

#undef BSP_VERSION

#ifdef IPCOM_FW_DEBUG

#include <fioLib.h>

int fw_log_ix = 0;
char fw_log [IPCOM_FW_LOG_SIZE + 1];

IP_STATIC int
ipcom_fw_log_cb(char *buf, int nbytes, int unused)
{
    int ix = fw_log_ix;

    if (nbytes > IPCOM_FW_LOG_SIZE - ix)
	return -1;

    fw_log_ix = ix + nbytes;

    memcpy (&fw_log[ix], buf, nbytes);
    return 0;
}

IP_PUBLIC int
ipcom_fw_logmsg(char * fmt, ...)
{
    va_list ap;
    int nchars;

    va_start(ap, fmt);
    nchars = fioFormatV(fmt, ap, ipcom_fw_log_cb, 1);
    va_end(ap);

    return nchars;
}

IP_PUBLIC void
ipcom_fw_log_dump(void)
{
    printf("%s\n", fw_log);
}

#endif /* IPCOM_FW_DEBUG */

/* DATA & CTRL -- add flags to indicate whether we want:
   - shared memory
   - dma-accessible memory
*/
/*
 *===========================================================================
 *                    ipcom_fw_cpu_mem_alloc
 *===========================================================================
 * Context:        called from any task
 * Description:    This routine allocates aligned memory from a NUMA node
 *                 local to the specified CPU. The returned buffer is at
 *                 an address that is a multiple of the specified alignment,
 *                 which must be a power of two.  The size is also rounded
 *                 up to a multiple of the alignment.
 *                 The memory block is virtually contiguous, but might
 *                 not be physically contiguous, or usable for DMA,
 *                 unless the IPCOM_FW_CPU_MEM_ALLOC_DMA flag is specified.
 *                 For Nehalem, the allocation request is increased by 2 cache
 *                 lines, and the buffer start address increased by 1 cache line,
 *                 to ensure that the cache line before and the cache line
 *                 after the returned block are never in use by a different
 *                 cpu.
 * Parameters:     cpu - index of the logical CPU for which to allocate memory
 *                 size - number of bytes to allocate
 *                 align - alignment of returned buffer; power of two.
 *                 flags - Bitwise OR of any combination of
 *                   IPCOM_FW_CPU_MEM_ALLOC_SHARED -- allocate from shared memory pool
 *                    (Note: shared memory will also be allocated implicitly
 *                     if 'cpu' is not the caller's own logical CPU number.)
 *                   IPCOM_FW_CPU_MEM_ALLOC_DMA    -- allocate DMA-visible memory
 * Returns:        Pointer to the allocated memory block, or IP_NULL.
 *
 */
IP_PUBLIC void *
ipcom_fw_cpu_mem_alloc(unsigned int cpu, Ip_size_t size, Ip_size_t align, unsigned int flags)
{
    char * buf;
    char * usrbuf;
    size_t * word;
#ifdef IPCOM_FORWARDER_AMP
    Ipcom_shared_mem_pool_t * pool;
    unsigned int my_cpu = ipcom_forwarder_current_core(); /* volatile! */
#endif

    if (align < _CACHE_ALIGN_SIZE)
        align = _CACHE_ALIGN_SIZE;  /* require at least cache-line alignment */

    size = (size + align - 1) & ~(align - 1);

    /* for unused cache line preceding and following */
    size += 2 * _CACHE_ALIGN_SIZE;

    size += align;  /* for alignment */

#ifdef IPCOM_FORWARDER_AMP
    if ((flags & IPCOM_FW_CPU_MEM_ALLOC_SHARED) || cpu != my_cpu)
    {
        flags |= IPCOM_FW_CPU_MEM_ALLOC_SHARED;
        pool = ipcom_shared_mem_pool_for_core(cpu);
        buf = ipcom_shared_mem_alloc(pool, size);
    }
    else
#endif
    {
        /* Eventually consider NUMA for SMP also. */
        buf = malloc (size);
    }

    if (buf == IP_NULL)
        return IP_NULL;

    usrbuf = buf + _CACHE_ALIGN_SIZE;
    usrbuf = (char *)(((size_t)usrbuf + align - 1) & ~(align - 1));

    word = (size_t *)usrbuf;
    word[-1] = (size_t)buf;
    word[-2] = cpu;
    word[-3] = flags;

    return (void *)usrbuf;
}

/*
 *===========================================================================
 *                    ipcom_fw_cpu_mem_free
 *===========================================================================
 * Context:        called from any task
 * Description:    This routine frees a memory block previously allocated
 *                 using ipcom_fw_cpu_mem_alloc(), for the same CPU.
 * Parameters:     ptr - pointer to block of memory to be returned. This
 *                       must be the value returned by an earlier call
 *                       to ipcom_fw_cpu_mem_alloc().
 * Returns:        N/A
 *
 */
IP_PUBLIC void
ipcom_fw_cpu_mem_free(void * ptr)
{
    size_t * word;

    if (ptr == NULL)
        return;

    word = (size_t *)ptr;
    ptr = (void *)word[-1];

#ifdef IPCOM_FORWARDER_AMP
    if (word[-3] & IPCOM_FW_CPU_MEM_ALLOC_SHARED)
    {
        Ipcom_shared_mem_pool_t * pool;
        unsigned int cpu = word[-2];

        pool = ipcom_shared_mem_pool_for_core(cpu);

        ipcom_shared_mem_free (pool, ptr);
    }
    else
#endif
    {
        free (ptr);
    }
}

#else
char ipcom_vxworks_nehalem_forwarder_common_empty_file;
#endif /* IPCOM_USE_NEHALEM_FORWARDER */

/*
 ******************************************************************************
 * End of file
 ******************************************************************************
 */
