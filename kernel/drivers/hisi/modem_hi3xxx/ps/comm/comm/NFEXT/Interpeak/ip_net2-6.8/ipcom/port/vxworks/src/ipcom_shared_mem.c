/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_shared_mem.c,v $ $Revision: 1.2.8.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_shared_mem.c,v $
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
 */

/*
 * This module implements shared memory pools based upon a Fibonacci
 * buddy allocator. It supports allocation and freeing concurrently
 * by different contexts in an AMP system.  Buffer alignment is manual.
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
#include <ipcom_clib.h>

#include <ipcom_shared_mem.h>

#include <vxWorks.h>
#include <stdio.h>

#define SHM_LOG_ALLOC_FAILURES

#ifndef USE_SHM_LOG_ALLOC_FAILURES
#define SHM_LOG(x) do {} while ((0))
#elif defined (_WRS_CONFIG_WRHV_GUEST)
#include <vbi.h>
#define SHM_LOG(x) do {vbiKputs(x);} while ((0))
#else
#include <logLib.h>
#define SHM_LOG(x) do {logMsg(x, 0,0,0,0,0,0);} while ((0))
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifdef _WRS_CONFIG_LP64
#define IPCOM_SHARED_MEM_SHIFT 5UL
#else
#define IPCOM_SHARED_MEM_SHIFT 4UL
#endif

#define IPCOM_SHARED_MEM_GRANULE (1UL << IPCOM_SHARED_MEM_SHIFT)

/* BLOCK_SZ() applies for free blocks, BLOCK_IX for allocated blocks */
#define BLOCK_SZ(fi) ((fi) &~(LR_BIT|INHERIT_BIT|ALLOC_BIT))
#define BLOCK_IX(fi) ((int)(((fi) &~(LR_BIT|INHERIT_BIT|ALLOC_BIT)) >>  \
                            IPCOM_SHARED_MEM_SHIFT))
#define LR_BIT ((Ipcom_shm_free_t)1)            /* 0 right, 1 left */
#define INHERIT_BIT  ((Ipcom_shm_free_t)2)
#define ALLOC_BIT ((Ipcom_shm_free_t)4)

/*
 * Top blocks (with no parent) are always marked as LEFT blocks (with
 * the INHERIT bit clear) and are follwed immediately by a dummy
 * Ipcom_shm_free_t with the ALLOC_BIT set, which stops coalescing from
 * occurring any further.
 */

#define SHM_FIB_LIMIT 41 /*
                          * Maximum allowed Fibonacci index plus 1
                          * In our convention,
                          * F_0 = 1
                          * F_1 = 2
                          * F_2 = 3
                          * F_3 = 5
                          * F_4 = 8
                          * ...
                          * F_40 = 267914296
                          * F_41 = 433494437
                          * ...
                          *
                          * So the maximum allocation we support with
                          * this limit is 267914296 * IPCOM_SHARED_MEM_GRANULE.
                          * With an IPCOM_SHARED_MEM_GRANULE of 16, this becomes
                          * 4286628736 = 0xff80c380.
                          * The largest allocations are probably rarely done,
                          * and the size of the free_list array helps separate
                          * the spin lock from commonly used members also ;-)
                          */



/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef Ip_size_t Ipcom_shm_free_t;

typedef struct free_hdr_struct
{
    Ipcom_shm_free_t free_info;
    Ip_shared_mem_offset next_offset;
    Ip_shared_mem_offset prev_offset;  /* offset of previous offset field */
} free_hdr_t;

typedef struct Ip_shm_free_list_struct
{
    Ip_size_t bsize;    /* block size for this free list */
    Ip_shared_mem_offset head_offset;
} Ip_shm_free_list_t;

struct Ipcom_shared_mem_pool_struct
{
    int high;  /* limit in free_list array */
    Ip_size_t free_count;  /* for debug only -- count of free blocks */
    Ip_shm_free_list_t free_list [SHM_FIB_LIMIT];  /* Variable length actually */
    Ipcom_ipc_spinlock_t lock;
};

#undef SHM_DEBUG
#undef SHM_FREECOUNT
#ifdef SHM_DEBUG
#define SHM_FREECOUNT
int shm_debug = 0;
#define ipcom_ipc_spinlock_take(l) (0)
#define ipcom_ipc_spinlock_give(l, k) do {} while ((0))
#endif

/*
 ****************************************************************************
 * 11                   LOCAL FUNCTIONS
 ****************************************************************************
 */

IP_STATIC __inline__ int
ipcom_shared_mem_min_index(Ip_size_t size)
{
    Ip_size_t a = IPCOM_SHARED_MEM_GRANULE;
    Ip_size_t b = IPCOM_SHARED_MEM_GRANULE;
    Ip_size_t c;
    int ix = 0;

    for (ix = 0; ix < SHM_FIB_LIMIT; ++ix)
    {
        if (size <= b)
            break;

        c = b;
        b = b + a;
        a = c;
    }
    return ix;
}

/*
 ****************************************************************************
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_shared_blk_size
 *===========================================================================
 * Description:   Determine the actual amount of usable space in a buffer
 *                returned by ipcom_shared_mem_alloc().
 * Parameters:    pool - shared memory pool pointer passed to previous
 *                call to ipcom_shared_mem_alloc().
 *                buffer - pointer returned from a previous call to
 *                ipcom_shared_mem_alloc(), and not yet freed.
 *
 * Returns:       The actual amount of usable space in the buffer, which
 *                may be greater than the amount of space originally
 *                requested in the ipcom_shared_mem_alloc() call, but not
 *                less.
 */

IP_PUBLIC Ip_size_t
ipcom_shared_mem_blk_size(Ipcom_shared_mem_pool_t *pool, void *buffer)
{
    Ipcom_shm_free_t *free_info;
    int ix;

    /*
     * We don't absolutely need the 'pool' argument to calculate this,
     * but it makes things a bit simpler and is likely conveniently
     * available right after allocating the buffer.
     */
    if (pool == IP_NULL || buffer == IP_NULL)
        return 0;

    free_info = (Ipcom_shm_free_t *)((char *)buffer -
                                     sizeof (Ipcom_shm_free_t));

    ix = BLOCK_IX(*free_info);
    return pool->free_list[ix].bsize - sizeof (Ipcom_shm_free_t);
}

/*
 *===========================================================================
 *                    ipcom_shared_mem_alloc
 *===========================================================================
 * Description:   Carve memory from a shared memory pool
 * Parameters:    pool - the shared memory pool from which to allocate a block
 *                size - the required usable size within the block
 *
 * Returns:       A pointer to the allocated block, or IP_NULL if the
 *                allocation failed.
 *
 * Note: The actual amount of usable memory available in the block may
 * be determined using the ipcom_shared_mem_blk_size() function, and may
 * be larger than the requested amount. The alignment of the returned
 * buffer is only guaranteed to be that of an Ip_size_t variable.
 */

IP_PUBLIC void *
ipcom_shared_mem_alloc(Ipcom_shared_mem_pool_t *pool, Ip_size_t size)
{
    Ipcom_int_key_t key;
    char *buf;
    Ip_shared_mem_offset offset;
    Ip_shared_mem_offset *head;
    Ip_size_t lsize;
    Ipcom_shm_free_t finfo;
    free_hdr_t * hdr;
    free_hdr_t * nhdr;
    int ix, jx;

    buf = IP_NULL;

    if (pool == IP_NULL)
        goto done;

    if ((lsize = size + sizeof (Ipcom_shm_free_t)) < size)
        goto done;

    size = lsize;

    ix = ipcom_shared_mem_min_index(size);

    if (ix >= pool->high)  /* bigger than largest block size */
        goto done;

    key = ipcom_ipc_spinlock_take (&pool->lock);

    jx = ix;
    while ((offset = pool->free_list[jx].head_offset) == 0)
    {
        if (++jx == pool->high)
            goto unlock;        /* failure */
    }

    buf = (char *)pool + offset;
    hdr = (free_hdr_t *)buf;
    pool->free_list[jx].head_offset = hdr->next_offset;
    if (hdr->next_offset != 0)
    {
        nhdr = (free_hdr_t *)((char *)pool + hdr->next_offset);
        nhdr->prev_offset = hdr->prev_offset;
    }
    finfo = hdr->free_info;

    /*
     * If jx is at least ix + 2, split block into R+L, freeing R,
     * reducing jx by two, and continuing with L, until jx <= ix + 1.
     */
#ifdef SHM_FREECOUNT
    pool->free_count--;
#endif

    while (jx > ix + 1)
    {
        /*
         * Split into LEFT and RIGHT blocks, putting RIGHT block on
         * free list, and continuing with the LEFT block.
         */
        lsize = pool->free_list[jx - 2].bsize;

        /* locate RIGHT block */
        hdr = (free_hdr_t *)(buf + lsize);
        finfo -= lsize; /* Now reflects size of RIGHT block */
        /* keep INHERIT bit in RIGHT sub-block */
        hdr->free_info = finfo & ~(LR_BIT|ALLOC_BIT);
        head = &pool->free_list[jx - 1].head_offset;
        hdr->prev_offset = ((char *)head - (char *)pool);

        if ((hdr->next_offset = *head) != 0)
        {
            nhdr = (free_hdr_t *)((char *)pool + *head);
            nhdr->prev_offset = (char *)&hdr->next_offset - (char *)pool;
        }
        *head = (char *)hdr - (char *)pool;

        /* Now move to consider LEFT sub-block */
        finfo = (finfo << 1U) & INHERIT_BIT; /* save parent's LR bit as INHERIT */
        finfo += lsize + LR_BIT;

        jx -= 2;

#ifdef SHM_FREECOUNT
        pool->free_count++;
#endif
    }

    /*
     * At this point, jx == ix or jx == ix + 1.
     * If jx == ix, we are done splitting.
     * Otherwise split block, take R, L becomes free.
     */
    if (jx > ix)
    {
        jx = ix;
        /*
         * Split the current block, putting the LEFT sub-block on free list
         * and returning the RIGHT sub-block.
         *
         * For the special case ix = 0, corresponding to F_0 = 1,
         * with F_1 = 2, when we split the block both sub-blocks
         * are the same size, in which case the unused LEFT sub-block
         * goes on the list with index ix rather than ix - 1, which
         * is the usual case.
         */
        if (jx > 0)
            --jx;
        lsize = pool->free_list[jx].bsize;
        hdr = (free_hdr_t *)buf;
        hdr->free_info = lsize + LR_BIT + ((finfo << 1U) & INHERIT_BIT);
    
        head = &pool->free_list[jx].head_offset;
        hdr->prev_offset = ((char *)head - (char *)pool);

        if ((hdr->next_offset = *head) != 0)
        {
            nhdr = (free_hdr_t *)((char *)pool + *head);
            nhdr->prev_offset = (char *)&hdr->next_offset - (char *)pool;
        }
        *head = (char *)hdr - (char *)pool;

        /* Now the RIGHT block, which we will return */
        buf += lsize;
        finfo &= ~(LR_BIT);  /* RIGHT sub-block; keep parent's INHERIT bit */
#ifdef SHM_FREECOUNT
        pool->free_count++;
#endif
    }

    /*
     * We store the (shifted) index ix rather than the block size
     * in the free_info for an allocated block. That slightly
     * speeds up the freeing.
     */
    finfo &= (INHERIT_BIT | LR_BIT);
    finfo |= ALLOC_BIT;
    finfo += (ix << IPCOM_SHARED_MEM_SHIFT);

    ((free_hdr_t *)buf)->free_info = finfo;
    buf += sizeof (Ipcom_shm_free_t);

unlock:
    ipcom_ipc_spinlock_give(&pool->lock, key);

done:
#ifdef USE_SHM_LOG_ALLOC_FAILURES
    if (buf == IP_NULL)
        SHM_LOG("ipcom_shared_mem_alloc() failed.\n");
#endif

#ifdef SHM_DEBUG
    if (shm_debug)
        printf ("alloc %p ix=%d freecount=%lu\n",
                buf - sizeof (Ipcom_shm_free_t), ix, pool->free_count);
#endif

    return buf;
}

/*
 *===========================================================================
 *                    ipcom_shared_mem_free
 *===========================================================================
 * Description:   Free memory allocated with ipcom_shared_mem_alloc()
 * Parameters:    pool - the shared memory pool from which
 *                buf - address of buffer returned from earlier call
 *                      to ipcom_shared_mem_alloc()
 *
 * Returns:       N/A
 */

IP_PUBLIC void
ipcom_shared_mem_free(Ipcom_shared_mem_pool_t *pool, void *buffer)
{
    Ipcom_int_key_t key;
    Ipcom_shm_free_t finfo;
    free_hdr_t * hdr;
    free_hdr_t * nhdr;
    Ip_shared_mem_offset *head;
    int ix;
    char * buf = buffer;
    Ip_size_t size;

    if (pool == IP_NULL || buf == IP_NULL)
        return;

#ifdef SHM_FREECOUNT
    pool->free_count++;
#endif
    buf -= sizeof (Ipcom_shm_free_t);

    /* Debug test: check buf for proper alignment */

    hdr = (free_hdr_t *)buf;
    finfo = hdr->free_info;

    /*
     * Debug test: check that finfo has the ALLOC_BIT set and encodes
     * a legal index.
     */

    ix = BLOCK_IX(finfo);

    size = pool->free_list[ix].bsize;
    finfo &= (INHERIT_BIT|LR_BIT);
    finfo += size;

#ifdef SHM_DEBUG
    if (shm_debug)
        printf ("free %p ix=%d finfo=0x%lx\n", buf, ix, finfo);
#endif

    key = ipcom_ipc_spinlock_take (&pool->lock);

    while (IP_TRUE)
    {
        if (finfo & LR_BIT)
        {
            /*
             * We are a LEFT sub-block, look for the free RIGHT buddy.
             * The buddy is free if the ALLOC_BIT is off and the
             * size (and LR_BIT) are correct. To check the size,
             * it's enough to check that it is at least the current
             * block's size, since if it were split, the left-hand
             * sub-part would be smaller.
             */
            
            hdr = (free_hdr_t *)(buf + size);
            if (hdr->free_info & (ALLOC_BIT|LR_BIT))
                break;
            if (BLOCK_SZ(hdr->free_info) < size)
                break;

#ifdef SHM_DEBUG
            if (shm_debug)
                printf ("ix=%d coal w/right %p, finfo=0x%lx\n", ix, hdr, hdr->free_info);
#endif
            /* Remove hdr from its free list */
            head = (Ip_shared_mem_offset *)((char *)pool + hdr->prev_offset);
            if ((*head = hdr->next_offset) != 0)
            {
                nhdr = (free_hdr_t *)((char *)pool + *head);
                nhdr->prev_offset = hdr->prev_offset;
            }

            /* Add in inherit bit and size from right block */
            size += hdr->free_info;
            /* Get LR_BIT from old finfo's INHERIT_BIT */
            finfo = size + ((finfo >> 1U) & LR_BIT);
            /* buf stays the same */
            ix += 2;
        }
        else /* RIGHT sub-block, look for LEFT buddy */
        {
            if (ix != 0)
                size = pool->free_list[ix - 1].bsize;

            /*
             * Check both the ALLOC_BIT and the size to make sure
             * that the buddy is ALL free, not just a left-most sub-block.
             */
            hdr = (free_hdr_t *)(buf - size);
            if (hdr->free_info & ALLOC_BIT)
                break;
            if (BLOCK_SZ(hdr->free_info) != size)
                break;
            
#ifdef SHM_DEBUG
            if (shm_debug)
                printf ("ix=%d coal w/left %p, finfo=0x%lx\n", ix, hdr, hdr->free_info);
#endif
            /* Remove hdr from its free list */
            head = (Ip_shared_mem_offset *)((char *)pool + hdr->prev_offset);
            if ((*head = hdr->next_offset) != 0)
            {
                nhdr = (free_hdr_t *)((char *)pool + *head);
                nhdr->prev_offset = hdr->prev_offset;
            }

            finfo &= ~(LR_BIT|ALLOC_BIT); /* preserve size and INHERIT bit */
            finfo += size + ((hdr->free_info >> 1U) & LR_BIT);
            buf = (char *)hdr;
            ix += 1;
        }
        size = BLOCK_SZ(finfo);
#ifdef SHM_FREECOUNT
        pool->free_count--;
#endif
    }

    hdr = (free_hdr_t *)buf;
    hdr->free_info = finfo;

#ifdef SHM_DEBUG
    if (shm_debug)
        printf ("free final %p ix=%d  finfo=0x%lx freecount=%lu\n",
                buf, ix, finfo, pool->free_count);
#endif

    /* put the maximally coalesced buffer back on its free list */
    head = &pool->free_list[ix].head_offset;
    hdr->prev_offset = ((char *)head - (char *)pool);

    if ((hdr->next_offset = *head) != 0)
    {
        nhdr = (free_hdr_t *)((char *)pool + *head);
        nhdr->prev_offset = (char *)&hdr->next_offset - (char *)pool;
    }
    *head = (char *)hdr - (char *)pool;

    ipcom_ipc_spinlock_give(&pool->lock, key);
}

/*
 *===========================================================================
 *                    ipcom_shared_mem_pool_init
 *===========================================================================
 * Description:   Initialize a shared memory pool
 * Parameters:    pool - A cache-aligned pointer indicating the start
 *                       address of a shared memory block on which to construct
 *                       a shared memory allocation pool.
 *                size - The size of the block on which to construct the
 *                       pool.  The pool control structures take up some
 *                       memory, so the maximum space remaining for allocation
 *                       after initialization of the pool is a bit less.
 *
 * Returns:       0 if successful; -1 if invalid arguments were given.
 *
 */

IP_PUBLIC int
ipcom_shared_mem_pool_init(Ipcom_shared_mem_pool_t *pool, Ip_size_t size)
{
    Ip_size_t min_size;
    Ip_size_t min_align = _CACHE_ALIGN_SIZE;
    Ip_size_t a, b, c;
    Ip_shared_mem_offset offset;
    int ix;
    free_hdr_t * hdr;
    free_hdr_t * nhdr;
    Ip_shared_mem_offset *head;

    if (IPCOM_SHARED_MEM_GRANULE < sizeof(free_hdr_t))
    {
        ipcom_printf("IPCOM_SHARED_MEM_SHIFT is too small.\n");
        return -1;
    }

    if (min_align < IPCOM_SHARED_MEM_GRANULE)
        min_align = IPCOM_SHARED_MEM_GRANULE;

    if (pool == IP_NULL || ((Ip_size_t)pool & (min_align - 1)) != 0)
        return -1;

    min_size = sizeof (Ipcom_shared_mem_pool_t);
    min_size = (min_size + min_align - 1) & ~(min_align - 1);
    /*
     * Verify enough space for at least one minimum-size top-level block,
     * with its dummy Ipcom_shm_free_t word immediately following.
     */

    if (size < min_size + IPCOM_SHARED_MEM_GRANULE + sizeof(Ipcom_shm_free_t))
        return -1;

    size -= min_size;

    a = IPCOM_SHARED_MEM_GRANULE;
    b = IPCOM_SHARED_MEM_GRANULE;
    for (ix = 0; ix < SHM_FIB_LIMIT; ++ix)
    {
        c =  b;
        b = a + b;
        a = c;
        pool->free_list[ix].bsize = a;
        pool->free_list[ix].head_offset = 0;
    }

    /*
     * Now add top-level blocks as large as possible from
     * the space remaining.
     */

    offset = min_size;
    pool->high = -1;
    pool->free_count = 0;

    for (ix = SHM_FIB_LIMIT - 1; ix >= 0; --ix)
    {
        min_size = pool->free_list[ix].bsize;
        min_size += IPCOM_SHARED_MEM_GRANULE;
        head = &pool->free_list[ix].head_offset;

        while (IP_TRUE)
        {
            if (size < min_size)
                break;

            hdr = (free_hdr_t *)((char *)pool + offset);
            hdr->prev_offset = ((char *)head - (char *)pool);

            if ((hdr->next_offset = *head) != 0)
            {
                nhdr = (free_hdr_t *)((char *)pool + *head);
                nhdr->prev_offset = (char *)&hdr->next_offset - (char *)pool;
            }
            *head = (char *)hdr - (char *)pool;

            hdr->free_info = min_size - IPCOM_SHARED_MEM_GRANULE +
                LR_BIT;

            /*
             * The top level block is followed by a dummy allocated
             * block header:
             */
            hdr = (free_hdr_t *)((char *)hdr + min_size -
                                 IPCOM_SHARED_MEM_GRANULE);
            hdr->free_info = ALLOC_BIT; /* don't care about size */

            offset += min_size;
            size -= min_size;

            if (pool->high < 0)
                pool->high = ix + 1;

            pool->free_count++;
        }
    }

    ipcom_ipc_spinlock_init (&pool->lock);

    return 0;
}

#define SHM_TEST

#ifdef SHM_TEST

#include <stdlib.h>
#include <string.h>
#include <memLib.h>

#define ERR_BADIX               1
#define ERR_BADALIGN            2
#define ERR_ALLOCBIT0           3
#define ERR_BADINFOIX           4
#define ERR_BLKSIZE             5

typedef struct buf_link_s
{
    struct buf_link_s *next;
} buf_link;

IP_PUBLIC Ipcom_shared_mem_pool_t *
ipcom_shared_mem_pool_create(Ip_size_t size)
{
    Ipcom_shared_mem_pool_t *pool;
    pool = memalign (256, size);
    if (pool == NULL)
        return IP_NULL;
    bzero ((void *)pool, size);
    if (ipcom_shared_mem_pool_init (pool, size) != 0)
    {
        free(pool);
        return IP_NULL;
    }
    return pool;
}

IP_PUBLIC int
ipcom_shared_mem_test(Ipcom_shared_mem_pool_t *pool, Ip_size_t startsize)
{
    char * buf;
    free_hdr_t *hdr;  /* white box */
    Ip_size_t size;
    Ip_size_t actsize;
    Ip_size_t cumulativeNom;
    Ip_size_t cumulativeAct;
    Ip_size_t nblks;
    Ip_size_t tmp;
    int error;
    Ipcom_shm_free_t finfo;
    buf_link *head;
    buf_link *link;
    int ix;

    if (startsize == 0)
        startsize = 0xffffffffUL;

    if (pool == IP_NULL)
        return -1;

    error = 0;
    cumulativeNom = 0;
    cumulativeAct = 0;
    head = IP_NULL;
    for (size = startsize; size > 0; size >>= 1)
    {
        if (size + sizeof (Ipcom_shm_free_t) < size)
            ix = SHM_FIB_LIMIT;
        else
            ix = ipcom_shared_mem_min_index(size + sizeof (Ipcom_shm_free_t));
        if (ix == SHM_FIB_LIMIT)
            actsize = 0;
        else
            actsize = pool->free_list[ix].bsize;
        for (nblks = 0; ;++nblks)
        {
            buf = ipcom_shared_mem_alloc (pool, size);
            if (buf == IP_NULL)
            {
                break;
            }
            if ((unsigned int)ix >= SHM_FIB_LIMIT)
            {
                printf ("Allocated block with ix=%d\n", ix);
                error = ERR_BADIX;
                goto cleanup;
            }
            hdr = (free_hdr_t *)(buf - sizeof(Ipcom_shm_free_t *));
            if (((Ip_size_t)hdr & (IPCOM_SHARED_MEM_GRANULE - 1)) != 0)
            {
                printf ("Allocated block %p with bad alignment\n", buf);
                error = ERR_BADALIGN;
                goto cleanup;
            }
            finfo = hdr->free_info;
            if ((finfo & ALLOC_BIT) == 0)
            {
                printf ("ALLOC_BIT not set in allocated block %p\n", buf);
                error = ERR_ALLOCBIT0;
                goto cleanup;
            }
            if (BLOCK_IX(finfo) != ix)
            {
                printf ("Allocated block %p indicates bad index %lu\n",
                        buf, BLOCK_IX(finfo));
                error = ERR_BADINFOIX;
                goto cleanup;
            }
            tmp = ipcom_shared_mem_blk_size(pool, buf);
            if (tmp != actsize - sizeof(Ipcom_shm_free_t))
            {
                printf ("ipcom_shared_mem_blk_size(%p, %p) returns %lu\n",
                        pool, buf, tmp);
                error = ERR_BLKSIZE;
                goto cleanup;
            }
            link = (buf_link *)buf;
            link->next = head;
            head = link;

            cumulativeNom += size;
            cumulativeAct += actsize;
        }
        printf ("Allocated %lu blocks of ix=%d size %lu (%lu), cumulative %lu (%lu)\n",
                nblks, ix, size, actsize, cumulativeNom, cumulativeAct);
    }

cleanup:
    for (link = head; link != IP_NULL; link = head)
    {
        head = link->next;
        ipcom_shared_mem_free (pool, link);
    }
    return error;
}
#endif /* SHM_TEST */

/*
 ******************************************************************************
 * End of file
 ******************************************************************************
 */
