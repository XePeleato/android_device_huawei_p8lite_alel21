/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_slab.h,v $ $Revision: 1.6 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_slab.h,v $
 *   $Author: kenneth $ $Date: 2008-12-16 10:34:42 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_SLAB_H
#define IPCOM_SLAB_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Fixed size allocator.
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

#include <ipcom_atomic.h>
#include <ipcom_cstyle.h>
#include <ipcom_list.h>
#include <ipcom_spinlock.h>
#include <ipcom_type.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Slab cache flags */

#define IPCOM_SLAB_F_NO_LOCKING (1u << 0) /* Do not lock/unlock the
                                             slab cache in alloc/free
                                             operations. The caller
                                             must make sure that
                                             alloc/free is only run by
                                             one context at any
                                             specific moment in time */
#define IPCOM_SLAB_F_ZERO       (1u << 1) /* Buffers are cleared before
                                             in ipcom_slab_alloc() */
#define IPCOM_SLAB_F_POISON     (1u << 2) /* Buffer are fill with a
                                             pattern when freed */

#define IPCOM_SLAB_F_AVERAGE    (0u << 30) /* Allocate an average
                                              amount of objects per
                                              slab, this is the
                                              default */
#define IPCOM_SLAB_F_ONE        (1u << 30) /* Allocate one object per
                                              slab, this should only be
                                              used for very big objects
                                              that are used very
                                              seldom */
#define IPCOM_SLAB_F_FEW        (2u << 30) /* Allocate just a few
                                              objects per slab */
#define IPCOM_SLAB_F_MANY       (3u << 30) /* Allocate a large amount
                                              of objects, use this for
                                              objects that are
                                              performance critical and
                                              used in great numbers */



/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_memory_pool_struct
{
    Ipcom_list        list;
    const char       *name;  /* Human readable name of the memory
                                limit */
    const Ip_size_t   limit; /* Maximum number of bytes that can be
                                allocated from this memory pool */
    Ip_size_t         low;   /* The lowest number of bytes left in
                                this pool so far */
    Ipcom_atomic_t    bytes; /* Bytes left in this pool */
    Ipcom_list        memory_caches; /* List of all slab caches using
                                        this pool */
}
Ipcom_memory_pool;


/* Forward declaration */
struct Ipcom_memory_cache_struct;

/*
 *===========================================================================
 *                     Ipcom_slab_ctor
 *===========================================================================
 * Description: Constructor for slab objects. Is run once per object when
 *              objects are added to the memory cache.
 * Parameters:  mc - The slab cache the object is going to be part of.
 *              obj - The object to initialize.
 * Returns:     IP_TRUE - the object was initialized successfully
 *              IP_FALSE - the initialization failed, this will make the
 *                         adding any object to the slab cache fail.
 */
typedef Ip_bool (*Ipcom_slab_ctor)(struct Ipcom_memory_cache_struct *mc, void *obj);

/*
 *===========================================================================
 *                     Ipcom_slab_dtor
 *===========================================================================
 * Description: Destructor for slab objects. Is run once per object when
 *              objects are removed from the memory cache.
 * Parameters:  mc - The slab cache the object was part of.
 *              obj - The object to destroy.
 * Returns:
 */
typedef void (*Ipcom_slab_dtor)(struct Ipcom_memory_cache_struct *mc, void *obj);


/* Top level structure of a slab cache */
typedef struct Ipcom_memory_cache_struct
{
    Ipcom_list      list;         /* List of all memory caches using
                                     the same memory pool */
    Ipcom_spinlock  lock;         /* Lock for this structure */
    Ip_u32          flags;        /* IPCOM_SLAB_F_xxx ipcom_slab_new() */
    Ip_bool         do_garbage_collect; /* This memory cache must free
                                     all slabs in "slabs_empty" as
                                     soon as possible */
    Ipcom_list      slabs_full;   /* List of slabs where all objects
                                     are allocated */
    Ipcom_list      slabs_partial;/* List of slabs that contains both
                                     allocated and free objects */
    Ipcom_list      slabs_empty;  /* List of slabs where all objects
                                     are free */
    const Ip_size_t obj_size;     /* Size of the objects contained
                                     within this slab cache */
    const Ip_size_t alignment;    /* Minimal alignment of all objects */
    Ipcom_memory_pool *memory_pool; /* Memory pool this slab must
                                       allocate memory through */
    Ipcom_slab_ctor ctor;         /* Slab object constructor */
    Ipcom_slab_dtor dtor;         /* Slab object desctructor */
    const char     *name;         /* Human readable name of the memory
                                     cache */
}
Ipcom_memory_cache;


/*
 ****************************************************************************
 * 6                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                       ipcom_memory_pool_new
 *===========================================================================
 * Description: Allocates a new memory pool. The slab caches uses a
 *              specific memory pool will in total never exceed the
 *              limit of the pool
 * Parameters:  name - User define memory pool name.
 *              limit - Maximum number of bytes that can be allocated
 *                       from this pool.
 * Returns:     A newly created memory pool.
 *
 */
IP_PUBLIC Ipcom_memory_pool *
ipcom_memory_pool_new(const char *name, Ip_size_t limit);


/*
 *===========================================================================
 *                       ipcom_slab_new
 *===========================================================================
 * Description: Creates a new memory cache
 * Parameters:  name - User defined name if the cache
 *              flags - IPCOM_SLAB_F_xxx flags.
 *              obj_size - size of the objects stored in this
 *              obj_alignment - minimal alignment of objects returned
 *                       from this slab cache
 *              ctor - Constructor for objects added to this slab.
 *              dtor - Desctructor for objects deleted from this slab.
 *              memory_pool - a memory pool from which this slab cache
 *                       will allocate its memory
 * Returns:     The newly create slab cache object.
 *
 */
IP_PUBLIC Ipcom_memory_cache *
ipcom_slab_new(const char *name,
               Ip_u32 flags,
               Ip_size_t obj_size,
               Ip_size_t obj_alignment,
               Ipcom_slab_ctor ctor,
               Ipcom_slab_dtor dtor,
               Ipcom_memory_pool *memory_pool);


/*
 *===========================================================================
 *                       ipcom_slab_add
 *===========================================================================
 * Description: Allocates a new slab for a memory cache. Returns the new
 *              slab, does not actually add it to the memory cache.
 * Parameters:  mc - memory cache the slab can be added to.
 * Returns:     A pointer to the new slab to be added, or IP_NULL if
 *              allocation fails.
 *
 */
IP_PUBLIC void *
ipcom_slab_add(Ipcom_memory_cache *mc);


/*
 *===========================================================================
 *                       ipcom_slab_alloc_try
 *===========================================================================
 * Description: Allocates an object from the slab cache
 * Parameters:  mc - slab cache to allocate from.
 *              retry - if true, attempt to allocate more memory
 *                      from underlying pool if initial allocation attempt
 *                      fails.
 * Returns:     The newly created object or IP_NULL if out of memory.
 *
 */
IP_PUBLIC void *
ipcom_slab_alloc_try(Ipcom_memory_cache *mc, Ip_bool retry);


/*
 *===========================================================================
 *                       ipcom_slab_alloc
 *===========================================================================
 * Description: Allocates an object from the slab cache, retrying if necessary
 * Parameters:  mc - slab cache to allocate from.
 * Returns:     The newly created object or IP_NULL if out of memory.
 *
 */
IP_INLINE void *
ipcom_slab_alloc(Ipcom_memory_cache *mc)
{
    return ipcom_slab_alloc_try (mc, IP_TRUE);
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
ipcom_slab_alloc_from(Ipcom_memory_cache *mc, void * slab);


/*
 *===========================================================================
 *                       ipcom_slab_free
 *===========================================================================
 * Description: Returns an object to a slab cache
 * Parameters:  mc - slab cache to return the object to
 *              obj - the object to free
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_slab_free(Ipcom_memory_cache *mc, void *obj);


/*
 *===========================================================================
 *                     ipcom_slab_get_objects_per_slab
 *===========================================================================
 * Description: Returns the number of objects per slab in the passed slab
 *              cache.
 * Parameters:  mc - a slab cache
 * Returns:     Number of objects per slab
 *
 */
IP_PUBLIC unsigned
ipcom_slab_get_objects_per_slab(Ipcom_memory_cache *mc);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
