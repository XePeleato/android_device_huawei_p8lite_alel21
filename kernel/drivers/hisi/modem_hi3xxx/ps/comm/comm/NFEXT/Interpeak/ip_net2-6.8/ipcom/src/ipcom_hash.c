/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_hash.c,v $ $Revision: 1.37 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_hash.c,v $
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

/* IPCOM hash table implementation */

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
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_hash.h"
#include "ipcom_syslog.h"
#include "ipcom_type.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

 /* The hash table will shrink when less populated that this value (in percent) */
#define IPCOM_HASH_LOW_WATER_MARK  2

 /* The hash table will be doubled in size when more populated than this value
   (in percent) */
#define IPCOM_HASH_HIGH_WATER_MARK  25

/* Size of the hash table when it is initially created. */
#define IPCOM_HASH_INITIAL_SIZE     29


#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

/* Default memory allocation routines (static memory) */
#ifndef IPCOM_MALLOC_STATIC
#define IPCOM_MALLOC_STATIC ipcom_malloc
#endif
#ifndef IPCOM_CALLOC_STATIC
#define IPCOM_CALLOC_STATIC ipcom_calloc
#endif
#ifndef IPCOM_FREE_STATIC
#define IPCOM_FREE_STATIC   ipcom_free
#endif

/* Default memory allocation routines (dynamic memory) */
#ifndef IPCOM_MALLOC_DYNAMIC
#define IPCOM_MALLOC_DYNAMIC ipcom_malloc
#endif
#ifndef IPCOM_FREE_DYNAMIC
#define IPCOM_FREE_DYNAMIC   ipcom_free
#endif

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
 *                    ipcom_hash_resize
 *===========================================================================
 * Description:   Doubles the size or cuts the size in half.
 * Parameters:    head - The head of the hash table.
 *                increase - Set to IP_TRUE to double the size, IP_FALSE
 *                to half the size.
 * Returns:       IPCOM_SUCCESS or IPCOM_ERR_NO_MEMORY if not enough memory
 *                could be allocated.
 *
 */
IP_STATIC Ip_err
ipcom_hash_resize(Ipcom_hash *head, Ip_bool increase)
{
    void **old_hash_table;
    int    old_size;
    int    i;
    Ip_err err;

    old_hash_table = head->table;
    old_size = head->size;

    /* Keep the size odd since that will increase the chance of the high
       bits of the hash value to affect the low bits which is used as
       index in the hash table */
    if (increase)
        head->size = (head->size << 1) + 3;
    else
        head->size = (head->size >> 1) - 1;

    head->table = IPCOM_CALLOC_STATIC(head->size, sizeof(*head->table));
    if (head->table == IP_NULL)
    {
        IPCOM_LOG2(ERR, "ipcom_hash_resize :: ipcom_calloc(%d, %d) failed",
                   head->size,
                   sizeof(*head->table));
        head->size = old_size;
        head->table = old_hash_table;
        return IPCOM_ERR_NO_MEMORY;
    }

    head->elem = 0;
    for (i = 0; i < old_size; i++)
        if (old_hash_table[i] != IP_NULL)
        {
            err = ipcom_hash_add(head, old_hash_table[i]);
            ip_assert(err == IPCOM_SUCCESS);
            (void)err;
        }

    IPCOM_FREE_STATIC(old_hash_table);
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_hash_can_move_obj
 *===========================================================================
 * Description:   Returns if the object with the specified ideal index can
 *                be move to the specified new index.
 * Parameters:    ideal_index - The index the object would have the table had
 *                no collisions.
 *                new_index - The new index this object would be moved to.
 *                size - The size of the current hash array.
 * Returns:       IP_TRUE if the object can be moved.
 *
 */
IP_STATIC Ip_bool
ipcom_hash_can_move_obj(unsigned ideal_index, unsigned new_index, unsigned size)
{
    int half_size = size >> 1;

#if IPCOM_HASH_HIGH_WATER_MARK > 50
    /* NOTE: this algorithm will break if IPCOM_HASH_HIGH_WATER_MARK
       is 50 or higher */
    IP_PANIC();
#endif

    if (IP_ABS((int) (ideal_index - new_index)) > half_size)
    {
        ideal_index = (ideal_index + half_size) % size;
        new_index = (new_index + half_size) % size;
    }

    return ideal_index <= new_index;
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
 *                    ipcom_hash_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_hash *
ipcom_hash_new(Ipcom_hash_obj_func obj_hash_func,
               Ipcom_hash_key_func key_hash_func,
               Ipcom_hash_cmp_func obj_key_cmp)
{
    Ipcom_hash *head;

    ip_assert(obj_hash_func != IP_NULL);
    ip_assert(key_hash_func != IP_NULL);
    ip_assert(obj_key_cmp != IP_NULL);

    head = IPCOM_MALLOC_STATIC(sizeof(*head));
    if (head == IP_NULL)
        return IP_NULL;
    head->table = IPCOM_CALLOC_STATIC(IPCOM_HASH_INITIAL_SIZE, sizeof(*head->table));
    if (head->table == IP_NULL)
    {
        IPCOM_FREE_STATIC(head);
        return IP_NULL;
    }

    head->obj_hash_func = obj_hash_func;
    head->key_hash_func = key_hash_func;
    head->obj_key_cmp = obj_key_cmp;
    head->size = IPCOM_HASH_INITIAL_SIZE;
    head->elem = 0;
    return head;
}


/*
 *===========================================================================
 *                    ipcom_hash_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_hash_delete(Ipcom_hash *head)
{
    if (head == IP_NULL)
        return;
    head->size = 0;
    IPCOM_FREE_STATIC(head->table);
    IPCOM_FREE_STATIC(head);
}


/*
 *===========================================================================
 *                    ipcom_hash_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_hash_add(Ipcom_hash *head, void *obj)
{
    unsigned hash_index;
    Ip_err   err;

    ip_assert(head->size > 0);
    if ((head->elem + 1) * 100 / head->size > IPCOM_HASH_HIGH_WATER_MARK)
    {
        err = ipcom_hash_resize(head, IP_TRUE);
        if (err != IPCOM_SUCCESS)
            return err;
    }

    if (head->size / 2 <= head->elem)
        /* The size of the hash table is locked and it has reached
           the capacity limit */
        return IPCOM_ERR_REACHED_MAX;

    hash_index = head->obj_hash_func(obj) % head->size;
    /* Find an empty slot at the hash index or after it if another
       object hash:ed to the same value */
    while (head->table[hash_index % head->size] != IP_NULL)
    {
        if (head->table[hash_index % head->size] == obj)
            return IPCOM_ERR_DUPLICATE;
        hash_index++;
    }
    head->table[hash_index % head->size] = obj;
    head->elem++;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_hash_remove
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_hash_remove(Ipcom_hash *head, void *obj)
{
    void    *cobj;
    unsigned hash_index;
    unsigned new_index;

    hash_index = head->obj_hash_func(obj) % head->size;
    while (head->table[hash_index] != obj)
    {
        if (head->table[hash_index] == IP_NULL)
            /* The object does not exist */
            return IPCOM_ERR_NOT_FOUND;
        hash_index++;
        hash_index = hash_index % head->size;
    }
    /* Object found, remove it */
    head->table[hash_index] = IP_NULL;
    head->elem--;

    if (head->size > IPCOM_HASH_INITIAL_SIZE &&
        head->elem * 100 / head->size < IPCOM_HASH_LOW_WATER_MARK)
    {
        /* Half the size of the hash table */
        if (ipcom_hash_resize(head, IP_FALSE) == IPCOM_SUCCESS)
            /* No need to fix collisions since the hash table was regenerated */
            return IPCOM_SUCCESS;
    }

    new_index = hash_index;
    /* The following object(s) can be out-of-place due to collisions, check them */
    while ((cobj = head->table[++hash_index % head->size]) != IP_NULL)
    {
        unsigned ideal_index = head->obj_hash_func(cobj) % head->size;

        hash_index %= head->size;
        if (ipcom_hash_can_move_obj(ideal_index, new_index, head->size))
        {
            /* Move this object to the hole from the removed or last moved object */
            head->table[new_index] = cobj;
            head->table[hash_index] = IP_NULL;
            new_index = hash_index;
        }
        /* else: the object is already at the correct place */
    }
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_hash_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_hash_get(Ipcom_hash *head, const void *key)
{
    unsigned hash_index;
    void    *obj;

    hash_index = head->key_hash_func(key) % head->size;
    while ((obj = head->table[hash_index]) != IP_NULL)
    {
        if (head->obj_key_cmp(obj, key))
            return obj;
        hash_index = (hash_index + 1) % head->size;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_hash_for_each
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_hash_for_each(Ipcom_hash *head, Ipcom_hash_foreach_cb_func cb_func, void *data)
{
    unsigned  i;
    unsigned  x;
    void    **elems;

    if (head->elem == 0)
        return;

    elems = IPCOM_MALLOC_DYNAMIC(head->elem * sizeof(void*)); /*lint !e647 */
    if (elems == IP_NULL)
    {
        IP_PANIC();
        return;
    }

    for (i = 0, x = 0; i < head->size; i++)
        if (head->table[i] != IP_NULL)
            elems[x++] = head->table[i];

    ip_assert(x == head->elem);
    for (i = 0; i < x; i++)
        cb_func(elems[i], data);

    IPCOM_FREE_DYNAMIC(elems);
}


/*
 *===========================================================================
 *                    ipcom_hash_update
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
/*lint -efunc(616,ipcom_hash_update) Control flows into case/default */
IP_PUBLIC unsigned
ipcom_hash_update(const void *key_param, int key_len, unsigned initval)
{
   unsigned a;
   unsigned len;
   Ip_u8 *key = (Ip_u8 *) key_param;

   /* Set up the internal state */
   len = (unsigned) key_len;
   a = initval + len;

   while (len >= 4)
   {
       a += key[0] + (key[1] << 8) + (key[2] << 16) + (key[3] << 24);
       a += ~(a << 15);
       a ^=  (a >> 10);
       a +=  (a << 3);
       a ^=  (a >> 6);
       a += ~(a << 11);
       a ^=  (a >> 16);
       key += 4; len -= 4;
   }

   /* All the case statements fall through */
   switch (len)
   {
   case 3 : a += key[2] << 16;
   case 2 : a ^= key[1] << 8;
   case 1 : a += key[0];
   default: break;
   }

   return a;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

