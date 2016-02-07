/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sysvar.c,v $ $Revision: 1.60.24.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_sysvar.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by kenneth jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * A system variable is a configuration paramter that can be modified while
 * the system is running.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
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
#include "ipcom_os.h"
#include "ipcom_pqueue.h"
#include "ipcom_sysvar.h"
#include "ipcom_type.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         code lock
 *===========================================================================
 */
#if defined(IP_PORT_LKM) || defined(IP_PORT_LAS)
/* The IPCOM lock is not needed (and will infact break things) since IPCOM
   is only used in the context of IPNET (or related products) so the IPNET
   code lock is already taken */
#define IPCOM_CODE_ISLOCKED_ASSERT()    IP_NOOP
#define IPCOM_CODE_LOCK()               IP_NOOP
#define IPCOM_CODE_UNLOCK()             IP_NOOP

/* The sysvar table in the kernel does not need to be per VR on the LKM. */
#undef IPCOM_VR_MAX
#define IPCOM_VR_MAX 1

#else /* !IP_PORT_LKM */
#ifdef IP_ASSERT
#define IPCOM_CODE_ISLOCKED_ASSERT()    ip_assert(ipcom_sysvar.lock_count == 1)
#define IPCOM_CODE_LOCK()   do { ip_assert(ipcom_sysvar.lock_owner != ipcom_getpid()); \
                                  ip_assert(ipcom_sysvar.lock != IP_NULL); \
                                  ipcom_mutex_lock(ipcom_sysvar.lock); \
                                  ipcom_sysvar.lock_owner = ipcom_getpid(); \
                                  ip_assert(ipcom_sysvar.lock_count == 0); \
                                  ipcom_sysvar.lock_count++; \
                             } while((0))
#define IPCOM_CODE_UNLOCK() do { ip_assert(ipcom_sysvar.lock_count == 1); \
                                  ipcom_sysvar.lock_count--; \
                                  ipcom_sysvar.lock_owner = 0; \
				                  ipcom_mutex_unlock(ipcom_sysvar.lock); \
                             } while((0))
#else /* !IP_ASSERT */
#define IPCOM_CODE_ISLOCKED_ASSERT()    IP_NOOP
#define IPCOM_CODE_LOCK()               ipcom_mutex_lock(ipcom_sysvar.lock)
#define IPCOM_CODE_UNLOCK()             ipcom_mutex_unlock(ipcom_sysvar.lock)
#endif /* IP_ASSERT */
#endif /* IP_PORT_LKM */


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                   Global ipcom_sysvar data
 *===========================================================================
 * Internal global Ipcom_sysvar data.
 */
typedef struct Ipcom_sysvar_data_struct
{
    Ipcom_mutex   lock;    /* Lock that protects access to "tree"
                              and its members */
    Ipcom_sysvar_tree tree[IPCOM_VR_MAX];

#ifdef IP_ASSERT
    int           lock_count;
    Ip_pid_t      lock_owner;
#endif
}
Ipcom_sysvar_data;

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IP_PORT_OSE5
IP_PUBLIC char *
ipcom_sysvar_get_ose5(const char *name, char *value, Ip_size_t *value_size);

IP_PUBLIC Ip_err
ipcom_sysvar_for_each_ose5(const char *name,
                           int (*cb_func)(const char *name, const char *value, int flags, void *cookie),
                           void *cookie);

IP_PUBLIC char *ipcom_strdup_usr(const char *s1);
#endif

#ifdef IP_PORT_LAS
IP_GLOBAL void ipcom_sysvar_tree_initialize(Ipcom_sysvar_tree *tree);
IP_GLOBAL Ip_bool ipcom_sysvar_tree_finalize(Ipcom_sysvar_tree *tree);
#else
#define ipcom_sysvar_tree_initialize(t)
#define ipcom_sysvar_tree_finalize(t)    IP_FALSE
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_err ipcom_sysvar_init(void *ununsed);

IP_GLOBAL char *
ipcom_sysvar_getvr(const char *name, char *value, Ip_size_t *value_size, Ip_bool usr, int vr);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_sysvar_data   ipcom_sysvar;

IP_STATIC Ipcom_once_t   ipcom_sysvar_once = IPCOM_ONCE_INIT;


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_sysvar_hash_key
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC unsigned
ipcom_sysvar_hash_key(const char *key)
{
    return ipcom_hash_update(key, ipcom_strlen(key), 0);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_hash_obj
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC unsigned
ipcom_sysvar_hash_obj(Ipcom_sysvar_entry *s)
{
    return ipcom_sysvar_hash_key(s->name);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_hash_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_bool
ipcom_sysvar_hash_cmp(Ipcom_sysvar_entry *s, char *key)
{
    return ipcom_strcmp(s->name, key) == 0;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_pqueue_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_bool
ipcom_sysvar_pqueue_cmp(Ipcom_sysvar_entry *s1, Ipcom_sysvar_entry *s2)
{
    return ipcom_strcmp(s2->name, s1->name);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_pqueue_insert_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_sysvar_pqueue_insert_cb(Ipcom_sysvar_entry *s, Ipcom_pqueue *pq)
{
    s->refcount++;
    ipcom_pqueue_insert(pq, s);
}

/*
 *===========================================================================
 *                    ipcom_sysvar_release
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_sysvar_free(Ipcom_sysvar_entry *sysvar, Ipcom_sysvar_tree *tree)
{
    /* Remove the sys entry */
    ipcom_hash_remove(tree->sysvars, sysvar);
    ipcom_free(sysvar);
}

/*
 *===========================================================================
 *                    ipcom_sysvar_tree_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ipcom_sysvar_tree *
ipcom_sysvar_tree_get(int vr)
{
#if IPCOM_VR_MAX > 1
    if (vr == -1)
        vr = ipcom_proc_vr_get();

    if (vr >= IPCOM_VR_MAX)
        return IP_NULL;
#else
    vr = 0;
#endif

    if (ipcom_sysvar.tree[vr].sysvars)
        ipcom_sysvar.tree[vr].referenced++;
    else
    {
        ipcom_sysvar.tree[vr].sysvars
            = ipcom_hash_new((Ipcom_hash_obj_func) ipcom_sysvar_hash_obj,
                             (Ipcom_hash_key_func) ipcom_sysvar_hash_key,
                             (Ipcom_hash_cmp_func) ipcom_sysvar_hash_cmp);

        if (ipcom_sysvar.tree[vr].sysvars == IP_NULL)
            return IP_NULL;

        ipcom_sysvar.tree[vr].vr = vr;
        ipcom_sysvar.tree[vr].referenced = 1;
        ipcom_sysvar_tree_initialize(&ipcom_sysvar.tree[vr]);
    }

    return &ipcom_sysvar.tree[vr];
}


/*
 *===========================================================================
 *                    ipcom_sysvar_tree
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_sysvar_tree_done(Ipcom_sysvar_tree    *tree)
{
    if (tree == IP_NULL)
        return;

    /* If we're killing the last reference, to deal finalization */
    if (tree->referenced == 1)
    {
        /* Did it kill the tree? */
        if (ipcom_sysvar_tree_finalize(tree))
        {
            ipcom_hash_for_each(tree->sysvars,
                                (Ipcom_hash_foreach_cb_func)ipcom_sysvar_free,
                                tree);

            ipcom_hash_delete(tree->sysvars);
            tree->sysvars = IP_NULL;
        }
    }

    /* Decrease AFTER, since finalize may invoke sysvar calls as
       well */
    tree->referenced--;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_err
ipcom_sysvar_init(void *ununsed)
{
    IPCOM_UNUSED_ARG(ununsed);

    ipcom_memset(&ipcom_sysvar, 0, sizeof(Ipcom_sysvar_data));
    return ipcom_mutex_create(&ipcom_sysvar.lock);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_release
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_sysvar_release(Ipcom_sysvar_tree *tree, Ipcom_sysvar_entry *sysvar)
{
    if (--sysvar->refcount == 0)
    {
        tree->modified = IP_TRUE;
        ipcom_hash_remove(tree->sysvars, sysvar);
        ipcom_free(sysvar);
    }
}


IP_STATIC Ip_err
ipcom_sysvar_setvr(const char *name, const char *value, int flags, int vr)
{
    Ip_err retval;
    Ipcom_sysvar_tree   *tree;


    if (value == IP_NULL)
        return IPCOM_SUCCESS;  /* simply ignore it silently */

#ifdef IP_PORT_OSE5
    if (IP_BIT_ISFALSE(flags, IPCOM_SYSVAR_FLAG_OVERWRITE))
    {
        Ip_size_t test_size = 1;
        char test[1];

        if (ipcom_sysvar_get_ose5(name, test, &test_size) || test_size > 1)
            return IPCOM_ERR_DUPLICATE;
    }
#endif /* IP_PORT_OSE5 */


    retval = ipcom_once(&ipcom_sysvar_once, ipcom_sysvar_init, IP_NULL);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return retval;
    }

    IPCOM_CODE_LOCK();

    tree = ipcom_sysvar_tree_get(vr);
    if (tree == IP_NULL)
        retval = IPCOM_ERR_NO_MEMORY;
    else
        retval =  ipcom_sysvar_set_tree(name, value, flags, tree);
    ipcom_sysvar_tree_done(tree);

    IPCOM_CODE_UNLOCK();

    return retval;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_getvr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL char *
ipcom_sysvar_getvr(const char *name, char *value, Ip_size_t *value_size, Ip_bool usr, int vr)
{
    Ipcom_sysvar_entry  *sysvar;
    Ip_size_t           value_length;
    char                *ret_value = IP_NULL;
    Ip_err              retval;
    Ipcom_sysvar_tree   *tree;

    if (name == IP_NULL)
        return IP_NULL;

    retval = ipcom_once(&ipcom_sysvar_once, ipcom_sysvar_init, IP_NULL);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return IP_NULL;
    }

    IPCOM_CODE_LOCK();

    tree = ipcom_sysvar_tree_get(vr);
    if (tree == IP_NULL)
        goto leave;

    sysvar = ipcom_hash_get(tree->sysvars, name);
    if (sysvar)
    {
        if (value == IP_NULL)
        {
#ifdef IP_PORT_OSE5
            if (usr)
                ret_value = ipcom_strdup_usr(sysvar->value);
            else
#else
                (void)usr;
#endif
                ret_value = ipcom_strdup(sysvar->value);
        }
        else
        {
            value_length = ipcom_strlen(sysvar->value) + 1;
            if (value_length > *value_size)
            {
                ret_value = IP_NULL;
                *value_size = value_length;
            }
            else
            {
                ret_value = value;
                ipcom_memcpy(ret_value, sysvar->value, value_length);
                *value_size = value_length - 1;
            }
        }
    }
#ifdef IP_PORT_OSE5
    else
    {
        ret_value = ipcom_sysvar_get_ose5(name, value, value_size);
    }
#endif /* IP_PORT_OSE5 */

 leave:
    ipcom_sysvar_tree_done(tree);
    IPCOM_CODE_UNLOCK();
    return ret_value;
}

/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */




/*
 *===========================================================================
 *                    ipcom_sysvar_set_internal
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL Ip_err
ipcom_sysvar_set_tree(const char *name, const char *value, int flags, Ipcom_sysvar_tree *tree)
{
    Ip_size_t           name_length;
    Ip_size_t           value_length;
    Ipcom_sysvar_entry  *sysvar;
    Ip_err              retval = IPCOM_SUCCESS;

    /* Check for duplicate. */
    sysvar = ipcom_hash_get(tree->sysvars, name);
    if (sysvar)
    {
        if (IP_BIT_ISSET(sysvar->flags, IPCOM_SYSVAR_FLAG_READONLY))
        {
            retval = IPCOM_ERR_READONLY;
            goto leave;
        }
        else if (IP_BIT_ISFALSE(flags, IPCOM_SYSVAR_FLAG_OVERWRITE))
        {
            retval = IPCOM_ERR_DUPLICATE;
            goto leave;
        }
        else
        {
            flags |= sysvar->flags & IPCOM_SYSVAR_FLAG_KERNEL;
            ipcom_hash_remove(tree->sysvars, sysvar);
            ipcom_free(sysvar);
            goto set;
        }
    }

    /* No current entry found and not allowed to create a new entry. */
    if(IP_BIT_ISSET(flags, IPCOM_SYSVAR_FLAG_NOCREATE))
    {
        retval = IPCOM_ERR_FAILED;
        goto leave;
    }

    /* Add the new sysvar. */
 set:
     name_length = ipcom_strlen(name) + 1;
     value_length = ipcom_strlen(value) + 1;
     sysvar = ipcom_malloc(sizeof(Ipcom_sysvar_entry) + name_length + value_length);

     if(sysvar == IP_NULL)
     {
         retval = IPCOM_ERR_NO_MEMORY;
         goto leave;
     }

     sysvar->refcount = 1;
     sysvar->flags    = flags;
     if (IP_BIT_ISFALSE(flags, IPCOM_SYSVAR_FLAG_INIT))
     {
         tree->modified = IP_TRUE;
         sysvar->flags |= IPCOM_SYSVAR_FLAG_MODIFIED;
     }
     sysvar->name     = (const char *)sysvar + sizeof(Ipcom_sysvar_entry);
     ipcom_memcpy((void *)sysvar->name, name, name_length);
     sysvar->value    = sysvar->name + name_length;
     ipcom_memcpy((void *)sysvar->value, value, value_length);

     retval = ipcom_hash_add(tree->sysvars, sysvar);

 leave:
    return retval; /*lint !e429 Custodial pointer 'sysvar' has not been freed or returned */
}


/*
 ****************************************************************************
 * 11                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_sysvar_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC char *
ipcom_sysvar_get(const char *name, char *value, Ip_size_t *value_size)
{
    return ipcom_sysvar_getvr(name, value, value_size, IP_FALSE, -1);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_get0
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC char *
ipcom_sysvar_get0(const char *name, char *value, Ip_size_t *value_size)
{
    return ipcom_sysvar_getvr(name, value, value_size, IP_FALSE, 0);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_get_as_int0
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_sysvar_get_as_int0(const char *name, int default_value)
{
    int val;
    (void)ipcom_sysvar_get_descriptive_int0(name, default_value, &val, IP_NULL);
    return val;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_get_descriptive_int0
 *===========================================================================
 * Description: Returns the value of a specific sysvar as an integer.
 * Parameters:  name - name of the sysvar
 *              default_value - the value that will be loaded into "value"
 *                              if sysvar "name" does not exists.
 *              value - points to the location where the integer will be
 *                      stored
 *              map - mapping from a name to an integer value or IP_NULL
 * Returns:     -1 = sysvar "name" does not exist
 *               0 = value was given by mapping name to integer via "map"
 *               1 = value was given by transforming the string value to
 *                   its corresponding integer value via ipcom_atoi().
 */
IP_PUBLIC int
ipcom_sysvar_get_descriptive_int0(const char *name,
                                  int default_value,
                                  int *value,
                                  const Ipcom_cmd_int_str_map *map)
{
    char      strval[32];
    Ip_size_t strval_len = sizeof(strval);

    *value = default_value;
    if (ipcom_sysvar_get0(name, strval, &strval_len) == IP_NULL)
        return -1;

    if (map != IP_NULL)
        if (ipcom_cmd_str_to_key2(map, strval, value) == 0)
            return 0;

    *value = ipcom_atoi(strval);
    return 1;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_set
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_sysvar_set(const char *name, const char *value, int flags)
{
    return ipcom_sysvar_setvr(name, value, flags, -1);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_set0
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_sysvar_set0(const char *name, const char *value, int flags)
{
    return ipcom_sysvar_setvr(name, value, flags, 0);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_unset_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_sysvar_unset_cb(const char *name, const char *value, int flags, void *cookie)
{
    (void)value;
    (void)flags;
    (void)cookie;

    (void)ipcom_sysvar_unset(name);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_unset
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_sysvar_unset(const char *name)
{
    Ipcom_sysvar_entry *sysvar;
    Ip_err              retval;
    Ip_size_t           namelen;
    Ipcom_sysvar_tree   *tree;

    retval = ipcom_once(&ipcom_sysvar_once, ipcom_sysvar_init, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        return retval;

    namelen = ipcom_strlen(name);
    if (name[namelen-1] == '*')
        return ipcom_sysvar_for_each(name, ipcom_sysvar_unset_cb, IP_NULL);

    IPCOM_CODE_LOCK();

    tree = ipcom_sysvar_tree_get(-1);
    if (tree == IP_NULL)
    {
        retval = IPCOM_ERR_NO_MEMORY;
        goto leave;
    }

    retval = IPCOM_ERR_NOT_FOUND;
    sysvar = ipcom_hash_get(tree->sysvars, name);
    if (sysvar)
    {
        if(IP_BIT_ISSET(sysvar->flags, IPCOM_SYSVAR_FLAG_READONLY))
            retval = IPCOM_ERR_READONLY;
        else
        {
            ipcom_sysvar_release(tree, sysvar);
            retval = IPCOM_SUCCESS;
        }
    }

 leave:
    ipcom_sysvar_tree_done(tree);
    IPCOM_CODE_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_for_each
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_sysvar_for_each(const char            *name,
                      Ipcom_sysvar_for_each cb_func,
                      void                  *cookie)
{
    Ipcom_sysvar_entry *sysvar;
    Ipcom_sysvar_tree  *tree;
    Ip_size_t           namelen = 0;
    Ip_err              retval;
    Ipcom_pqueue       *pq;

    retval = ipcom_once(&ipcom_sysvar_once, ipcom_sysvar_init, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        return retval;

    if (name)
    {
        namelen = ipcom_strlen(name);
        if (name[namelen-1] == '*')
            namelen--; /* ignore trailing wildcard sign */
    }

    pq = ipcom_pqueue_new((Ipcom_pqueue_cmp_func)ipcom_sysvar_pqueue_cmp,
                          ipcom_pqueue_nop_store_index);
    if (pq == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;

    IPCOM_CODE_LOCK();

    tree = ipcom_sysvar_tree_get(-1);
    if (tree == IP_NULL)
    {
        ipcom_pqueue_delete(pq);
        IPCOM_CODE_UNLOCK();
        return IPCOM_ERR_NO_MEMORY;
    }

    /* Add all sysvars to the priority queue so they can be extracted
       sorted in lexicographical order */
    ipcom_hash_for_each(tree->sysvars,
                        (Ipcom_hash_foreach_cb_func)ipcom_sysvar_pqueue_insert_cb,
                        pq);

    while (IP_NULL != (sysvar = ipcom_pqueue_remove_next(pq)))
    {
        if (name == IP_NULL
            || ipcom_strncmp(sysvar->name, name, namelen) == 0)
        {
            IPCOM_CODE_UNLOCK();
            (void)cb_func(sysvar->name, sysvar->value, sysvar->flags, cookie);
            IPCOM_CODE_LOCK();
        }
        ipcom_sysvar_release(tree, sysvar);
    }

    ipcom_pqueue_delete(pq);
    ipcom_sysvar_tree_done(tree);

    IPCOM_CODE_UNLOCK();

#ifdef IP_PORT_OSE5_DISABLED
    return ipcom_sysvar_for_each_ose5(name, cb_func, cookie);
#else
    return IPCOM_SUCCESS;
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

