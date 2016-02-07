/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cache.c,v $ $Revision: 1.5 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cache.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
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
#include "ipcom_cache.h"

#include "ipcom_h.h"
#include "ipcom_clib.h"
#include "ipcom_syslog.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Maxium number of objects that the cache can hold. The definition is
   a bit awkward, but it is done that way so
   sizeof(Ipcom_cache_index)==4 won't cause overflow when the shift
   operation is done.  */
#define IPCOM_CACHE_MAX_OBJ_CNT \
    ((1u << (8 * sizeof(Ipcom_cache_index) - 1)) \
     - 1u \
     + (1u << (8 * sizeof(Ipcom_cache_index) - 1)))

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
 *                         ipcom_cache_obj_order
 *===========================================================================
 * Description: Calculates the order of an object given it size.
 * Parameters:  obj_size - size of the object
 * Returns:     The order of the object.
 *
 */
IP_STATIC unsigned
ipcom_cache_obj_order(unsigned obj_size)
{
    unsigned obj_order = 3;

    while (obj_size > (1 << obj_order))
        ++obj_order;

    return obj_order;
}


/*
 *===========================================================================
 *                         ipcom_cache_obj_index
 *===========================================================================
 * Description: Returns the index of 'obj' in the object array.
 * Parameters:  cache - a cache instance
 *              obj - an object in the cache
 * Returns:
 *
 */
IP_STATIC Ipcom_cache_index
ipcom_cache_obj_index(Ipcom_cache *cache, const void *obj)
{
    Ip_ptrdiff_t obj_base = (Ip_ptrdiff_t) cache->objs;
    Ip_ptrdiff_t obj_val = (Ip_ptrdiff_t) obj;

    ip_assert(((obj_val - obj_base) & ((1 << cache->obj_order) - 1)) == 0);

    return (Ipcom_cache_index) ((obj_val - obj_base) >> cache->obj_order);
}



/*
 *===========================================================================
 *                         ipcom_cache_alloc_obj
 *===========================================================================
 * Description: Allocates a new object from the cache.
 * Parameters:  cache - a cache instance
 * Returns:     A newly allocated object.
 *
 */
IP_STATIC void *
ipcom_cache_alloc_obj(Ipcom_cache *cache)
{
    int hidx;

    if (cache->free_objs != IP_NULL)
    {
        Ipcom_cache_free_obj *obj = cache->free_objs;

        cache->free_objs = obj->next;
        return obj;
    }

    /* Out of free objects, reuse an object */
    IPCOM_CACHE_STATS(++cache->stats.recycles);
    hidx = ipcom_random() % cache->htbl_cnt;
    for (;;)
    {
        Ipcom_cache_index_set *obj_set = &cache->htbl[hidx];

        /* Only check the first set, it will be valid if there is ANY
           valid entry in this bucket. */
        if (obj_set->obj_idx[0] != IPCOM_CACHE_INVALID_INDEX)
        {
            void *obj;

            /* Reuse this object */
            obj = ipcom_cache_index_to_obj(cache,
                                           obj_set->obj_idx[0]);

            /* Move all indexes forward one step and make sure the
               last index is the invalid index */
            ipcom_memmove(&obj_set->obj_idx[0],
                          &obj_set->obj_idx[1],
                          ip_offsetof(Ipcom_cache_index_set, obj_idx[IPCOM_CACHE_SET_N - 1]));
            obj_set->obj_idx[IPCOM_CACHE_SET_N - 1] = IPCOM_CACHE_INVALID_INDEX;

            return obj;
        }
        hidx = (hidx + 1) % cache->htbl_cnt;
    }
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
 *                           ipcom_cache_delete
 *===========================================================================
 * Description: Frees all resouces allocated by the cache
 * Parameters:  cache - the cache to delete
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_cache_delete(Ipcom_cache *cache)
{
    ipcom_free(cache);
}


/*
 *===========================================================================
 *                     ipcom_cache_new_with_bucket_count
 *===========================================================================
 * Description: Creates a new cache with a specific number of objects
 *              in the cache. The amount of memory occupied by the
 *              cache will be the amount required to fit all objects.
 * Parameters:  htbl_cnt - number of buckets the cache will hold
 *              obj_size - the size of each object, this actual object
 *                   size might be larger to satisfy cache line alignment
 *              obj_func - given an object, will return the
 *                   corresponding hash value
 *              key_func - given a search key, will return the
 *                   corresponding hash value
 *              cmp_func - functions that returns if an object and a
 *                   search key match
 * Returns:     A newly created cache or IP_NULL if not enough memory
 *              could be allocated.
 *
 */
IP_PUBLIC Ipcom_cache *
ipcom_cache_new_with_bucket_count(unsigned htbl_cnt,
                                  unsigned obj_size,
                                  Ipcom_hash_obj_func obj_func,
                                  Ipcom_hash_key_func key_func,
                                  Ipcom_hash_cmp_func cmp_func)
{
    Ipcom_cache      *cache;
    unsigned          obj_order;
    unsigned          objs_size;
    unsigned          obj_cnt;
    unsigned          htbl_size;

    obj_cnt = (htbl_cnt
               * IPCOM_CACHE_FILL_RATIO
               * IPCOM_CACHE_SET_N
               / 100);
    obj_cnt = IP_MIN(IPCOM_CACHE_MAX_OBJ_CNT, obj_cnt);

    obj_order = ipcom_cache_obj_order(obj_size);
    objs_size = obj_cnt * (1 << obj_order);

    htbl_size = sizeof(Ipcom_cache_index_set) * htbl_cnt;

    cache = ipcom_calloc(1,
                         sizeof(*cache)
                         + sizeof(void*)
                         + htbl_size
                         + ipcom_cache_line_size()
                         + objs_size);
    if (cache == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_cache: Failed to allocate memory");
        return IP_NULL;
    }

    cache->obj_func = obj_func;
    cache->key_func = key_func;
    cache->cmp_func = cmp_func;

    cache->htbl     = IP_PTR_ALIGN((cache + 1), sizeof(void*));
    cache->htbl_cnt = htbl_cnt;

    cache->objs      = IP_PTR_ALIGN((char*)cache->htbl + htbl_size,
                                    ipcom_cache_line_size());
    cache->obj_cnt   = (Ipcom_cache_index) obj_cnt;
    cache->obj_size  = obj_size;
    cache->obj_order = obj_order;

    ipcom_cache_flush(cache);
    return cache;
}


/*
 *===========================================================================
 *                   ipcom_cache_new_with_memory_limit
 *===========================================================================
 * Description: Creates a new cache that will not occupy more than a
 *              the specified amount of memory. The actualt number of
 *              objects in the cache will be determined by the memory
 *              limit.
 * Parameters:  memory_limit - number of bytes the cache is allowed to
 *                   use
 *              obj_size - the size of each object, this actual object
 *                   size might be larger to satisfy cache line alignment
 *              obj_func - given an object, will return the
 *                   corresponding hash value
 *              key_func - given a search key, will return the
 *                   corresponding hash value
 *              cmp_func - functions that returns if an object and a
 *                   search key match
 * Returns:     A newly created cache or IP_NULL if not enough memory
 *              could be allocated.
 *
 */
IP_PUBLIC Ipcom_cache *
ipcom_cache_new_with_memory_limit(unsigned memory_limit,
                                  unsigned obj_size,
                                  Ipcom_hash_obj_func obj_func,
                                  Ipcom_hash_key_func key_func,
                                  Ipcom_hash_cmp_func cmp_func)
{
    unsigned obj_cnt_hi = 0xfffe;
    unsigned obj_cnt_lo = 1;
    unsigned obj_cnt = (obj_cnt_lo + obj_cnt_hi) / 2;
    unsigned htbl_cnt;
    unsigned actual_obj_size;
    unsigned mem;
    unsigned mem_spill_limit;

    memory_limit -= sizeof(Ipcom_cache);
    memory_limit -= ipcom_cache_line_size();

    actual_obj_size = 1 << ipcom_cache_obj_order(obj_size);

    mem_spill_limit = IP_MAX(ipcom_cache_line_size(), actual_obj_size);

    /* Using iterative way of calculating the object count since
       doing the calculation analytical is subject to rounding errors
       and aliasing effect since it not possible to use floating
       point math. It is also easier to get formula (1) correct this
       way. */
    for (;;)
    {
        obj_cnt = (obj_cnt_lo + obj_cnt_hi) / 2;

        htbl_cnt = obj_cnt * 100 / IPCOM_CACHE_FILL_RATIO / IPCOM_CACHE_SET_N;
        mem = obj_cnt
            * actual_obj_size + htbl_cnt
            * sizeof(Ipcom_cache_index_set);

        if (mem > memory_limit)
            obj_cnt_hi = obj_cnt;
        else if (memory_limit - mem > mem_spill_limit)
            obj_cnt_lo = obj_cnt;
        else
            break;
    }

    return ipcom_cache_new_with_bucket_count(htbl_cnt,
                                             obj_size,
                                             obj_func,
                                             key_func,
                                             cmp_func);
}


/*
 *===========================================================================
 *                           ipcom_cache_flush
 *===========================================================================
 * Description:
 * Parameters:  cache -
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_cache_flush(Ipcom_cache *cache)
{
    Ipcom_cache_index i;
    Ipcom_cache_index first_idx = ipcom_cache_obj_index(cache, cache->objs);

    cache->free_objs = IP_NULL;
    for (i = first_idx + cache->obj_cnt;
         i > first_idx;
         --i)
    {
        Ipcom_cache_free_obj *obj;

        obj = ipcom_cache_index_to_obj(cache, i - 1);
        obj->next = cache->free_objs;
        cache->free_objs = obj;
    }

    ipcom_memset(cache->htbl,
                 IPCOM_CACHE_INVALID_INDEX,
                 cache->htbl_cnt * sizeof(Ipcom_cache_index_set));

}


/*
 *===========================================================================
 *                           ipcom_cache_insert
 *===========================================================================
 * Description: Inserts a new object into the cache.
 * Parameters:  cache - a cache instance
 *              obj - object to insert
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_cache_insert(Ipcom_cache *cache, void *obj)
{
    ipcom_cache_insert_with_index(cache,
				  obj,
				  cache->obj_func(obj) % cache->htbl_cnt);
}


/*
 *===========================================================================
 *                     ipcom_cache_insert_with_index
 *===========================================================================
 * Description: Inserts a new object into the cache, the hash table
 *              index must be pre-calculated.
 * Parameters:  cache - a cache instance
 *              obj - object to insert
 *              hidx - hash
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_cache_insert_with_index(Ipcom_cache *cache,
                              void *obj,
                              unsigned hidx)
{
    unsigned               set_idx;
    void                  *new_obj;
    Ipcom_cache_index_set *obj_set = &cache->htbl[hidx];

    IPCOM_CACHE_STATS(++cache->stats.inserts);
    IPCOM_CACHE_STATS((obj_set->obj_idx[0] == IPCOM_CACHE_INVALID_INDEX
                       ? cache->stats.bucketshares
                       : ++cache->stats.bucketshares));

    for (set_idx = 0; set_idx < IPCOM_CACHE_SET_N; ++set_idx)
    {
        if (obj_set->obj_idx[set_idx] == IPCOM_CACHE_INVALID_INDEX)
        {
            /* Found unused slot, allocate a new object from the
               cache */
            new_obj = ipcom_cache_alloc_obj(cache);

            if (set_idx > 0
                && obj_set->obj_idx[set_idx - 1] == IPCOM_CACHE_INVALID_INDEX)
                /* ipcom_cache_alloc_obj() had to reuse an already
                   allocated object and it was selected from the same
                   bucket as this objects is being added to. So this
                   bucket now has one less valid entry */
                --set_idx;

            obj_set->obj_idx[set_idx] = ipcom_cache_obj_index(cache, new_obj);
            break;
        }
    }

    if (set_idx == IPCOM_CACHE_SET_N)
    {
        /* All sets used, overwrite on of the cached objects in this
           set. */
        IPCOM_CACHE_STATS(++cache->stats.collisions);
        set_idx = ipcom_random() % IPCOM_CACHE_SET_N;
        new_obj = ipcom_cache_index_to_obj(cache, obj_set->obj_idx[set_idx]);
    }

    ipcom_memcpy(new_obj, obj, cache->obj_size);
}


/*
 *===========================================================================
 *                          ipcom_cache_lookup
 *===========================================================================
 * Description: Lookups an object using a pre-calculated hash
 *              value. This function can be when the hash value is
 *              calculated by hardware.
 * Parameters:  cache - cache instance
 *              key - lookup key
 * Returns:     The matching object or IP_NULL if no match was found
 *
 */
IP_PUBLIC void *
ipcom_cache_lookup(Ipcom_cache *cache, const void *key)
{
    return ipcom_cache_lookup_with_index(cache,
                                         key,
                                         cache->key_func(key) % cache->htbl_cnt);
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

