/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cache.h,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_cache.h,v $
 *   $Author: kenneth $ $Date: 2009-06-22 21:22:03 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_CACHE_H
#define IPCOM_CACHE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Implements a N-set associative cache for constant sized
 * objects. Lookup and insert operations are always O(1), flush are
 * always O(H) where H is the size of the cache index hash table(s).
 *
 * The memory gived to the cache is partition in two parts, first part
 * (which is relativly small) is the cache index hash table(s). The
 * cache index consists of N (where N is the set associativity of the
 * cache) hash tables. The information stored in the hash table in an
 * index into the 2:nd part of the cache, which is the object array.
 *
 * This partition of the cache allows for 100% usage of the
 * objects. That is a benefit of having the objects directly in the
 * hash table if obj_size is a lot larger than
 * sizeof(Ipcom_cache_index).
 *
 * The memory is partition as this:
 * (1) total_mem >= obj_cnt * obj_size
 *                  + htbl_cnt * sizeof(Ipcom_cache_index_set)
 * (2) C = htbl_cnt * IPCOM_CACHE_SET_N
 * (3) obj_cnt = C * IPCOM_CACHE_FILL_RATIO / 100
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/*
 * Number of set the cache index should have. This is the number of
 * collisions in the hash table the cache can handle before entries
 * has to be removed from the cache. Increasing this number will
 * increase the efficiency of the cache index. Increasing this number
 * also means that the time it takes to find an object or determine
 * that a lookup fails increases.
 */
#define IPCOM_CACHE_SET_N   16

/*
 * The ratio between the number of objects and the number of buckets
 * in the hash table. This number is in percent. A number of X means
 * that there will be X% as many objects as there is buckets. Higher
 * ration means less cache efficiency, but more of the memory used by
 * the cache are used for storing objects objects. A reasonably range
 * is 40-80.
 */
#define IPCOM_CACHE_FILL_RATIO 50

/*
 * Define if the cache should keep track of statistics.
 * NOTE: keep track of statistics will make the cache slightly
 * slower.
 */
#define IPCOM_CACHE_USE_STATS


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_hash.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 * This index value is used in a bucket that does not refer to any object.
 */
#define IPCOM_CACHE_INVALID_INDEX ((Ipcom_cache_index) ~0)

#ifdef IPCOM_CACHE_USE_STATS
#define IPCOM_CACHE_STATS(x) (x)
#else
#define IPCOM_CACHE_STATS(x)
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 * The definition of Ipcom_cache_index determines the maximum number
 * of objects the cache can hold. Possible definitions are Ip_u8,
 * Ip_u16 or Ip_u32 for a maximum of 2^8-1, 2^16-1 or 2^32-1 objects.
 * NOTE: A higher maximum object count means that the index hash table
 * will take more memory for the SAME amount of hash buckets since the
 * size of a hash bucket is directly proportional to
 * sizeof(Ipcom_cache_index)
 */
typedef Ip_u16 Ipcom_cache_index;


#ifdef IPCOM_CACHE_USE_STATS
/*
 * Statistics counters for the cachex
 */
struct Ipcom_cache_stats {
    unsigned inserts;      /* Number of objects inserted in the cache */
    unsigned bucketshares; /* Number of hash buckets that has more
                              than one entry queued */
    unsigned collisions;   /* Number of objects that had to be
                              deleted due to hash collisions */
    unsigned recycles;     /* Number of times a valid object had to be
                              recycled since there were no free once */
    unsigned lookups;      /* Number of lookups done */
    unsigned misses;       /* Number of unsuccessful lookups */
};
#endif


/*
 * Each cache index entry represents all objects that hash to the same
 * index.
 */
typedef struct Ipcom_cache_index_set_t {
    Ipcom_cache_index obj_idx[IPCOM_CACHE_SET_N];
}
Ipcom_cache_index_set;

/*
 * Representation of a free object.
 */
typedef struct Ipcom_cache_free_obj_t {
    struct Ipcom_cache_free_obj_t *next;
}
Ipcom_cache_free_obj;

/*
 * Represent one cache instance
 */
typedef struct Ipcom_cache_t {
    Ipcom_hash_obj_func obj_func;
    Ipcom_hash_key_func key_func;
    Ipcom_hash_cmp_func cmp_func;
    unsigned            htbl_cnt;  /* Number of buckets in 'htbl' */
    Ipcom_cache_index_set *htbl;   /* Hash table of indexes into the
                                     'objs' array */
    unsigned            obj_size;  /* Space used by each object  */
    unsigned            obj_order; /* Each object occupies (1 << obj_order) */
    Ipcom_cache_index   obj_cnt;   /* Number of objects in 'objs' */
    void               *objs;      /* Pointer to the first object */
    Ipcom_cache_free_obj *free_objs; /* List of free objects */
#ifdef IPCOM_CACHE_USE_STATS
    struct Ipcom_cache_stats stats;
#endif
}
Ipcom_cache;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC void ipcom_cache_delete(Ipcom_cache *cache);
IP_PUBLIC Ipcom_cache * ipcom_cache_new_with_bucket_count(unsigned obj_cnt,
                                                          unsigned obj_size,
                                                          Ipcom_hash_obj_func obj_func,
                                                          Ipcom_hash_key_func key_func,
                                                          Ipcom_hash_cmp_func cmp_func);
IP_PUBLIC Ipcom_cache * ipcom_cache_new_with_memory_limit(unsigned memory_limit,
                                                          unsigned obj_size,
                                                          Ipcom_hash_obj_func obj_func,
                                                          Ipcom_hash_key_func key_func,
                                                          Ipcom_hash_cmp_func cmp_func);
IP_PUBLIC void ipcom_cache_flush(Ipcom_cache *cache);
IP_PUBLIC void ipcom_cache_insert(Ipcom_cache *cache, void *obj);
IP_PUBLIC void ipcom_cache_insert_with_index(Ipcom_cache *cache,
                                             void *obj,
                                             unsigned hidx);
IP_PUBLIC void * ipcom_cache_lookup(Ipcom_cache *cache, const void *key);


/*
 *===========================================================================
 *                        ipcom_cache_index_to_obj
 *===========================================================================
 * Description: Returns the object with the specified index
 * Parameters:  cache - cache instance
 *              idx - object index
 * Returns:
 *
 */
IP_INLINE void *
ipcom_cache_index_to_obj(Ipcom_cache *cache, Ipcom_cache_index idx)
{
    return &(((char *) cache->objs)[idx << cache->obj_order]);
}


/*
 *===========================================================================
 *                     ipcom_cache_lookup_with_index
 *===========================================================================
 * Description: Lookups an object using a pre-calculated hash
 *              value. This function can be when the hash value is
 *              calculated by hardware.
 * Parameters:  cache - cache instance
 *              key - lookup key
 *              hidx - the hash index the lookup key would produce
 * Returns:     The matching object or IP_NULL if no match was found
 *
 */
IP_INLINE void *
ipcom_cache_lookup_with_index(Ipcom_cache *cache,
                              const void *key,
                              unsigned hidx)
{
    unsigned               set_idx;
    void                  *obj;
    Ipcom_cache_index      obj_idx;
    Ipcom_cache_index_set *obj_set = &cache->htbl[hidx];

    IPCOM_CACHE_STATS(++cache->stats.lookups);

    for (set_idx = 0; set_idx < IPCOM_CACHE_SET_N; ++set_idx)
    {
        obj_idx = obj_set->obj_idx[set_idx];

        if (IP_UNLIKELY(obj_idx == IPCOM_CACHE_INVALID_INDEX))
            break;

        obj = ipcom_cache_index_to_obj(cache, obj_idx);
        if (cache->cmp_func(obj, key))
            return obj;
    }

    IPCOM_CACHE_STATS(++cache->stats.misses);
    return IP_NULL;
}



#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
