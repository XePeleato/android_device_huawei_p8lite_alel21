/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_generic_slab.c,v $ $Revision: 1.9.30.1.4.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_generic_slab.c,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:20:56 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/*
   Implementation of the IPCOM slab cache API using only other the
   public IPCOM API. This implementation will run unmodified on all OS:es
   supported by IPCOM.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */


#define IPCOM_KERNEL
#include "ipcom_config.h"

#define IPCOM_USE_SLAB_CMD

/*
 * Defines the maximum number of empty slabs that will be kept around
 * before the memory they occupies is released back to the memory
 * pool.
 */
#define IPNET_GENERIC_SLAB_AUTO_GC_THRESH 1

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"
#include "ipcom_slab.h"

#ifdef IPCOM_USE_SLAB_CMD
#include "ipcom_getopt.h"
#endif

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

#define IPCOM_SLAB_SIZEOF(data_type) ((Ip_size_t) (Ip_ptrdiff_t) (((data_type*)IP_NULL) + 1))
#define IPCOM_SLAB_ALIGN(size, align) ((Ip_size_t) (Ip_ptrdiff_t) IP_PTR_ALIGN(size, align))

/* Minimum number of objects that will be put into each slab. The
   actual number will be rouded up to fit an integer
   IPCOM_SLAB_PAGE_SIZE as close as possible */
#define IPCOM_SLAB_FEW_OBJS      10
#define IPCOM_SLAB_AVERAGE_OBJS  30
#define IPCOM_SLAB_MANY_OBJS     100

/* Memory will always be allocated in multiple of this size */
#define IPCOM_SLAB_PAGE_SIZE     4000

#define IPCOM_SLAB_LOCK(mc)                                     \
    do {                                                        \
        if (IP_BIT_ISFALSE(mc->flags, IPCOM_SLAB_F_NO_LOCKING)) \
            ipcom_spinlock_lock(mc->lock);                      \
    } while (0)

#define IPCOM_SLAB_UNLOCK(mc)                                   \
    do {                                                        \
        if (IP_BIT_ISFALSE(mc->flags, IPCOM_SLAB_F_NO_LOCKING)) \
            ipcom_spinlock_unlock(mc->lock);                    \
    } while (0)


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

/* Bottom level structure of a slab cache, this is the memory blocks
   being returned to the caller */
typedef struct Ipcom_slab_obj_struct
{
    struct Ipcom_slab_obj_struct *next;
}
Ipcom_slab_obj;


typedef struct Ipcom_slab_struct
{
    Ipcom_list      list;          /* List of slabs of this type */
    const unsigned  total_objs;    /* Total number of objects in this
                                      slab */
    unsigned        allocated_objs;/* Number of allocated objects
                                      from this slab */
    Ipcom_slab_obj *free_objs;     /* List of free objects in this
                                      slab */
}
Ipcom_slab;


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

Ipcom_once_t ipcom_slab_once = IPCOM_ONCE_INIT;
Ipcom_list   ipcom_slab_memory_pool_list;

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                       ipcom_memory_pool_free
 *===========================================================================
 * Description: Return a buffer allocated with
 *                       ipcom_memory_pool_alloc() back to the pool.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_memory_pool_free(Ipcom_memory_pool *mp, Ip_size_t size, void *buf)
{
    ipcom_free(buf);
    ipcom_atomic_add(&mp->bytes, size);
}


/*
 *===========================================================================
 *                       ipcom_memory_pool_alloc
 *===========================================================================
 * Description: Allocates memory from the specified memory pool.
 * Parameters:  mp - the memory pool to allocate from
 *              size - number of bytes to allocate
 * Returns:     Pointer to a memory buffer of the specified size or
 *                       IP_NULL if the allocation failed.
 *
 */
IP_STATIC void *
ipcom_memory_pool_alloc(Ipcom_memory_pool *mp, Ip_size_t size)
{
    void *buf;

    if (ipcom_atomic_sub_and_return(&mp->bytes, size) < 0)
        goto failed;

    buf = ipcom_calloc(1, size);
    if (buf == IP_NULL)
    {
        IP_PANIC();
        goto failed;
    }

    if (ipcom_atomic_get(&mp->bytes) < (int) mp->low)
        mp->low = ipcom_atomic_get(&mp->bytes);

    return buf;

 failed:
    ipcom_memory_pool_free(mp, size, IP_NULL);
    return IP_NULL;
}


/*
 *===========================================================================
 *                     ipcom_slab_get_size
 *===========================================================================
 * Description: Calculates the total, per object size and number of objects.
 * Parameters:  mc - memory cache to do garbage collection on.
 *              aligned_obj_size_ptr - Will contain the size each
 *                     object takes in memory, can be IP_NULL
 *              num_objs_ptr - Will contain the total number of
 *                     objects, can be IP_NULL
 * Returns:     The total amount of memory used by this slab.
 *
 */
IP_STATIC Ip_size_t
ipcom_slab_get_size(Ipcom_memory_cache *mc,
                    Ip_size_t *aligned_obj_size_ptr,
                    unsigned *num_objs_ptr)
{
    Ip_size_t aligned_obj_size;
    unsigned  num_objs;
    Ip_size_t slab_size = IPCOM_SLAB_PAGE_SIZE;
    Ip_size_t calculated_size;

    aligned_obj_size = IPCOM_SLAB_ALIGN(mc->obj_size, sizeof(void*));
    aligned_obj_size = IPCOM_SLAB_ALIGN(aligned_obj_size + sizeof(void*), mc->alignment);

    switch (mc->flags & IPCOM_SLAB_F_MANY)
    {
    case IPCOM_SLAB_F_ONE:
        num_objs = 1;
        break;
    case IPCOM_SLAB_F_FEW:
        num_objs = IPCOM_SLAB_FEW_OBJS;
        break;
    case IPCOM_SLAB_F_MANY:
        num_objs = IPCOM_SLAB_MANY_OBJS;
        break;
    default:
        num_objs = IPCOM_SLAB_AVERAGE_OBJS;
        break;
    }

    calculated_size = (IPCOM_SLAB_SIZEOF(Ipcom_slab)
                       + mc->alignment
                       - sizeof(void*)
                       + num_objs * aligned_obj_size);

    if (slab_size <= calculated_size)
        /* Figure out how many pages that are needed */
        slab_size *= (calculated_size + slab_size - 1) / slab_size;
    else
    {
        /* One slab page is enough, calculate how many objects that
           actual fit into the allocated memory */
        num_objs = (slab_size - (IPCOM_SLAB_SIZEOF(Ipcom_slab)
                                 + mc->alignment
                                 - sizeof(void*))) / aligned_obj_size;
    }

    if (aligned_obj_size_ptr)
        *aligned_obj_size_ptr = aligned_obj_size;
    if (num_objs_ptr)
        *num_objs_ptr = num_objs;

    return slab_size;
}


/*
 *===========================================================================
 *                     ipcom_slab_ptr_from_obj
 *===========================================================================
 * Description: Returns a pointer to where the slab structure the
 *                     object belongs to
 * Parameters:  mc - A pointer to the memory cache structure the slab
 *                   is located in
 *              obj - An object allocated from the passed memory cache.
 * Returns:
 *
 */
IP_INLINE Ipcom_slab **
ipcom_slab_ptr_from_obj(Ipcom_memory_cache *mc, void *obj)
{
    return (Ipcom_slab **) ((char*) obj + IPCOM_SLAB_ALIGN(mc->obj_size, sizeof(void*)));
}


/*
 *===========================================================================
 *                     ipcom_slab_dtor_objs
 *===========================================================================
 * Description: Frees all unused slabs within the specified slab cache.
 * Parameters:  mc - memory cache to do garbage collection on.
 * Returns:
 *
 */
IP_STATIC void
ipcom_slab_dtor_objs(Ipcom_memory_cache *mc, Ipcom_slab *slab)
{
    if (mc->dtor != IP_NULL)
    {
        Ipcom_slab_obj *obj;
        Ipcom_slab_obj *next_obj = slab->free_objs;

        while (IP_NULL != (obj = next_obj))
        {
            next_obj = obj->next;
            mc->dtor(mc, obj);
        }
    }
}


/*
 *===========================================================================
 *                     ipcom_slab_garbage_collect
 *===========================================================================
 * Description: Frees all unused slabs within the specified slab cache.
 * Parameters:  mc - memory cache to do garbage collection on.
 * Returns:
 *
 */
IP_STATIC void
ipcom_slab_garbage_collect(Ipcom_memory_cache *mc)
{
    Ipcom_slab *slab;

    IPCOM_SLAB_LOCK(mc);

    while (IP_NULL != (slab = IPCOM_LIST_FIRST(&mc->slabs_empty)))
    {
        ipcom_list_remove(&slab->list);
        ipcom_slab_dtor_objs(mc, slab);
        ipcom_memory_pool_free(mc->memory_pool,
                               ipcom_slab_get_size(mc, IP_NULL, IP_NULL),
                               slab);
    }

    IPCOM_SLAB_UNLOCK(mc);
}


/*
 *===========================================================================
 *                     ipcom_memory_pool_garbage_collect
 *===========================================================================
 * Description: Frees all unused memory in all the slabs using the
 *                     specified memory pool
 * Parameters:  mp - memory pool to do garbage collection on.
 * Returns:
 *
 */
IP_STATIC void
ipcom_memory_pool_garbage_collect(Ipcom_memory_pool *mp)
{
    Ipcom_memory_cache *mc;

    for (mc = IPCOM_LIST_FIRST(&mp->memory_caches);
         mc != IP_NULL;
         mc = IPCOM_LIST_NEXT(&mc->list))
    {
        if (IP_BIT_ISSET(mc->flags, IPCOM_SLAB_F_NO_LOCKING))
            mc->do_garbage_collect = IP_TRUE;
        else
            ipcom_slab_garbage_collect(mc);
    }
}


#ifdef IPCOM_USE_SLAB_CMD
/*
 *===========================================================================
 *                       ipcom_cmd_slab
 *===========================================================================
 * Description: Slab information shell command
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_slab(int argc, char **argv)
{
    Ipcom_memory_pool  *mp;
    Ipcom_memory_cache *mc;
    Ipcom_slab         *slab;
    Ipcom_getopt        opt;
    int                 i;
    Ip_bool             garbage_collect = IP_FALSE;

    ipcom_getopt_clear_r(&opt);
    while ((i = ipcom_getopt_r(argc, argv, "g", &opt)) != -1)
    {
        switch (i)
        {
        case 'g':
            garbage_collect = IP_TRUE;
            break;
        default:
            break;
        }
    }

    for (mp = IPCOM_LIST_FIRST(&ipcom_slab_memory_pool_list);
         mp != IP_NULL;
         mp = IPCOM_LIST_NEXT(&mp->list))
    {
        ipcom_printf("Memory pool: %s has %d bytes allocated, alloc high is %d, limit is %d"IP_LF,
                     mp->name,
                     mp->limit - ipcom_atomic_get(&mp->bytes),
                     mp->limit - mp->low,
                     mp->limit);
        for (mc = IPCOM_LIST_FIRST(&mp->memory_caches);
             mc != IP_NULL;
             mc = IPCOM_LIST_NEXT(&mc->list))
        {
            if (garbage_collect)
                ipcom_slab_garbage_collect(mc);

            ipcom_printf("  Slab: %s, object size %d, alignment %d"IP_LF,
                         mc->name, mc->obj_size, mc->alignment);

            ipcom_printf("    Full:    %d"IP_LF, mc->slabs_full.size);
            for (slab = IPCOM_LIST_FIRST(&mc->slabs_full);
                 slab != IP_NULL;
                 slab = IPCOM_LIST_NEXT(&slab->list))
                ipcom_printf("      Objects - total: %d, allocated %d"IP_LF,
                             slab->total_objs,
                             slab->allocated_objs);

            ipcom_printf("    Partial: %d"IP_LF, mc->slabs_partial.size);
            for (slab = IPCOM_LIST_FIRST(&mc->slabs_partial);
                 slab != IP_NULL;
                 slab = IPCOM_LIST_NEXT(&slab->list))
                ipcom_printf("      Objects - total: %d, allocated %d"IP_LF,
                             slab->total_objs,
                             slab->allocated_objs);

            ipcom_printf("    Empty:   %d"IP_LF, mc->slabs_empty.size);
            for (slab = IPCOM_LIST_FIRST(&mc->slabs_empty);
                 slab != IP_NULL;
                 slab = IPCOM_LIST_NEXT(&slab->list))
                ipcom_printf("      Objects - total: %d, allocated %d"IP_LF,
                             slab->total_objs,
                             slab->allocated_objs);
        }
    }
    return 0;
}
#endif /* IPCOM_USE_SLAB_CMD */


/*
 *===========================================================================
 *                       ipcom_slab_init
 *===========================================================================
 * Description: Initializes the slab module.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_slab_init(void *unused)
{
    IPCOM_UNUSED_ARG(unused);

    ipcom_list_init(&ipcom_slab_memory_pool_list);
#ifdef IPCOM_USE_SLAB_CMD
    ipcom_shell_add_cmd("slab", "slab", "Print slab cache information",
                        ipcom_cmd_slab,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif /* IPCOM_USE_SLAB_CMD */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                       ipcom_memory_pool_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_memory_pool *
ipcom_memory_pool_new(const char *name, Ip_size_t limit)
{
    Ipcom_memory_pool *mp;

    if (ipcom_once(&ipcom_slab_once, ipcom_slab_init, IP_NULL)
        != IPCOM_SUCCESS)
        return IP_NULL;

    mp = ipcom_malloc(IPCOM_SLAB_SIZEOF(Ipcom_memory_pool) + ipcom_strlen(name) + 1);
    if (mp != IP_NULL)
    {
        mp->name = (char *) (mp + 1);
        ipcom_strcpy((char *) mp->name, name);
        ipcom_atomic_set(&mp->bytes, limit);
        *(Ip_size_t *) &mp->limit = limit;
        mp->low = limit;
        ipcom_list_init(&mp->memory_caches);
        ipcom_list_insert_last(&ipcom_slab_memory_pool_list, &mp->list);
    }
    return mp;
}


/*
 *===========================================================================
 *                       ipcom_slab_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_memory_cache *
ipcom_slab_new(const char *name,
               Ip_u32 flags,
               Ip_size_t obj_size,
               Ip_size_t obj_alignment,
               Ipcom_slab_ctor ctor,
               Ipcom_slab_dtor dtor,
               Ipcom_memory_pool *memory_pool)
{
    Ipcom_memory_cache *mc;

    mc = ipcom_memory_pool_alloc(memory_pool,
                                 IPCOM_SLAB_SIZEOF(Ipcom_memory_cache) + ipcom_strlen(name) + 1);
    if (mc != IP_NULL)
    {
        mc->name = (char *) (mc + 1);
        ipcom_strcpy((char *) mc->name, name);
        mc->flags = flags;
        mc->ctor = ctor;
        mc->dtor = dtor;
        *(Ip_size_t *) &mc->obj_size = obj_size;
        *(Ip_size_t *) &mc->alignment = IP_MAX(obj_alignment, sizeof(void*));

        ipcom_list_init(&mc->slabs_full);
        ipcom_list_init(&mc->slabs_partial);
        ipcom_list_init(&mc->slabs_empty);

        mc->memory_pool = memory_pool;
        ipcom_list_insert_last(&memory_pool->memory_caches, &mc->list);

        if (IP_BIT_ISFALSE(flags, IPCOM_SLAB_F_NO_LOCKING))
            if (ipcom_spinlock_create(&mc->lock) != IPCOM_SUCCESS)
                IP_PANIC();
    }

    return mc;
}


/*
 *===========================================================================
 *                       ipcom_slab_add
 *===========================================================================
 * Description: Allocates a new slab for a memory cache. Returns the new
 *              slab, does not actually add it to the memory cache.
 * Parameters:  mc - memory cache the slab will be added to.
 * Returns:     A pointer to the new slab to be added, or IP_NULL if
 *              allocation fails.
 *
 */
IP_PUBLIC void *
ipcom_slab_add(Ipcom_memory_cache *mc)
{
    Ipcom_slab     *slab;
    Ipcom_slab_obj *obj;
    Ip_size_t       aligned_obj_size;
    unsigned        added_objs;
    unsigned        total_objs;
    Ip_bool         first_try = IP_TRUE;
    Ip_size_t       slab_size;

    slab_size = ipcom_slab_get_size(mc,
                                    &aligned_obj_size,
                                    &total_objs);
 retry:
    slab = ipcom_memory_pool_alloc(mc->memory_pool, slab_size);
    if (slab == IP_NULL)
    {
        if (first_try)
        {
            /* Try to free up enough resource to satisfy this request */
            ipcom_memory_pool_garbage_collect(mc->memory_pool);
            first_try = IP_FALSE;
            goto retry;
        }
        return IP_FALSE;
    }

    *(unsigned *) &slab->total_objs = total_objs;
    slab->allocated_objs = 0;

    slab->free_objs = IP_NULL;
    obj = IP_PTR_ALIGN(slab + 1, mc->alignment);

    for (added_objs = 0; added_objs < slab->total_objs; added_objs++)
    {
        if (mc->ctor != IP_NULL)
            if (mc->ctor(mc, obj) == IP_FALSE)
            {
                /* Ctor failed, garbage collect slabs to see if that
                   frees up the necessary resources */
                ipcom_memory_pool_garbage_collect(mc->memory_pool);
                if (mc->ctor(mc, obj) == IP_FALSE)
                {
                    /* Ctor still failed, give up */
                    ipcom_slab_dtor_objs(mc, slab);
                    ipcom_memory_pool_free(mc->memory_pool,
                                           slab_size,
                                           slab);
                    return IP_NULL;
                }
            }

        obj->next = slab->free_objs;
        slab->free_objs = obj;
        *ipcom_slab_ptr_from_obj(mc, obj) = slab;
        obj = (Ipcom_slab_obj *) ((char *)obj + aligned_obj_size);
    }

    return slab;
}


/*
 *===========================================================================
 *                       ipcom_slab_alloc_try
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_slab_alloc_try(Ipcom_memory_cache *mc, Ip_bool retry)
{
    Ipcom_slab *slab;
    void       *obj;

    IPCOM_SLAB_LOCK(mc);
    slab = IPCOM_LIST_FIRST(&mc->slabs_partial);
    if (slab == IP_NULL)
    {
        slab = IPCOM_LIST_FIRST(&mc->slabs_empty);
        if (slab == IP_NULL)
        {
            IPCOM_SLAB_UNLOCK(mc);
            if (!retry || (slab = ipcom_slab_add(mc)) == IP_NULL)
                return IP_NULL;
            IPCOM_SLAB_LOCK (mc);
            goto insert;
        }
    }

    if (slab->allocated_objs == 0)
    {
        ipcom_list_remove(&slab->list);
 insert:
        ipcom_list_insert_last(&mc->slabs_partial, &slab->list);
    }

    if (++slab->allocated_objs == slab->total_objs)
    {
        ipcom_list_remove(&slab->list);
        ipcom_list_insert_last(&mc->slabs_full, &slab->list);
    }
    obj = slab->free_objs;
    slab->free_objs = slab->free_objs->next;

    IPCOM_SLAB_UNLOCK(mc);

    if (IP_BIT_ISSET(mc->flags, IPCOM_SLAB_F_ZERO))
        ipcom_memset(obj, 0, mc->obj_size);

    return obj;
}


/*
 *===========================================================================
 *                       ipcom_slab_alloc_from
 *===========================================================================
 * Description: Allocate an object from new slab, add slab to memory cache.
 *              Do not use slab subsequently, it has been added to the cache.
 * Parameters: mc - memory cache to add slab to (minus one object)
 *             slab - new slab returned from ipcom_slab_add()
 * Returns: A pointer to the object allocated out of slab.
 */
IP_PUBLIC void *
ipcom_slab_alloc_from(Ipcom_memory_cache *mc, void * slab)
{
    Ipcom_slab * s = (Ipcom_slab *) slab;
    void * obj;

    ip_assert (s != IP_NULL);
    ip_assert(s->allocated_objs == 0);

    s->allocated_objs = 1;
    obj = s->free_objs;
    s->free_objs = s->free_objs->next;

    IPCOM_SLAB_LOCK(mc);
    if (s->total_objs == 1)
        ipcom_list_insert_last(&mc->slabs_full, &s->list);
    else
        ipcom_list_insert_last(&mc->slabs_partial, &s->list);
    IPCOM_SLAB_UNLOCK(mc);

    if (IP_BIT_ISSET(mc->flags, IPCOM_SLAB_F_ZERO))
        ipcom_memset(obj, 0, mc->obj_size);

    return obj;
}

/*
 *===========================================================================
 *                       ipcom_slab_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_slab_free(Ipcom_memory_cache *mc, void *obj)
{
    Ipcom_slab *slab;

    slab = *ipcom_slab_ptr_from_obj(mc, obj);
    if (IP_BIT_ISSET(mc->flags, IPCOM_SLAB_F_POISON))
        ipcom_memset(obj, 0xab, mc->obj_size);

    IPCOM_SLAB_LOCK(mc);

    if (slab->allocated_objs-- == slab->total_objs)
    {
        ipcom_list_remove(&slab->list);
        ipcom_list_insert_first(&mc->slabs_partial, &slab->list);
    }

    if (slab->allocated_objs == 0)
    {
        ipcom_list_remove(&slab->list);
        ipcom_list_insert_first(&mc->slabs_empty, &slab->list);
        /* Release the memory if too many objects are unused in this
           slab */
        if (mc->slabs_empty.size > IPNET_GENERIC_SLAB_AUTO_GC_THRESH)
            mc->do_garbage_collect = IP_TRUE;
    }

    ((Ipcom_slab_obj *) obj)->next = slab->free_objs;
    slab->free_objs = obj;

    IPCOM_SLAB_UNLOCK(mc);

    if (IP_UNLIKELY(mc->do_garbage_collect))
    {
        /* do_garbage_collect is modified outside of any lock. This is
           OK since the worst think that could happen is that we run
           the garbage collect function more times than needed, which
           would not cause any corruption of the slab. */
        mc->do_garbage_collect = IP_FALSE;
        ipcom_slab_garbage_collect(mc);
    }
}


/*
 *===========================================================================
 *                     ipcom_slab_get_objects_per_slab
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC unsigned
ipcom_slab_get_objects_per_slab(Ipcom_memory_cache *mc)
{
    unsigned objs_per_slab;

    (void) ipcom_slab_get_size(mc, IP_NULL, &objs_per_slab);
    return objs_per_slab;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
