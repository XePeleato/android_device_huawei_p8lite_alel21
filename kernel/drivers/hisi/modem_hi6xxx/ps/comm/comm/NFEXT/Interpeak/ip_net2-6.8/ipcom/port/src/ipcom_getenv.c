/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getenv.c,v $ $Revision: 1.26 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_getenv.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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

#if IPCOM_USE_ENV == IPCOM_ENV_NATIVE
#include <stdlib.h>
#include <stdio.h>
#ifdef IP_PORT_VXWORKS
#include <vxWorks.h>
#include <envLib.h>
#endif
#else
#define IPCOM_USE_CLIB_PROTO
#endif

#if IPCOM_USE_ENV == IPCOM_ENV_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5))
#include <ose.h>
#include <string.h>
#endif

#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_file.h"
#include "ipcom_hash.h"
#include "ipcom_os.h"
#include "ipcom_sysvar.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

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

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
IP_STATIC Ip_err ipcom_env_init(void *procptr);

IP_GLOBAL void ipcom_proc_clearenv(Ipcom_proc *proc);
IP_GLOBAL void ipcom_proc_copyenv(Ipcom_proc *proc);
#endif


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

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
/*
 *===========================================================================
 *                    ipcom_env_hash_key
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC unsigned
ipcom_env_hash_key(const char *key)
{
    return ipcom_hash_update(key, ipcom_strlen(key), 0);
}


/*
 *===========================================================================
 *                    ipcom_env_hash_obj
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC unsigned
ipcom_env_hash_obj(const Ipcom_env_entry *s)
{
    return ipcom_env_hash_key(s->name);
}


/*
 *===========================================================================
 *                    ipcom_env_hash_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_bool
ipcom_env_hash_cmp(const Ipcom_env_entry *s, const char *key)
{
    return ipcom_strcmp(s->name, key) == 0;
}


/*
 *===========================================================================
 *                    ipcom_env_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ipcom_env_entry *
ipcom_env_create(const char *name, const char *value)
{
    Ip_size_t        name_length;
    Ip_size_t        value_length;
    Ipcom_env_entry *env;

    /* Create environment variable */
    name_length = ipcom_strlen(name) + 1;
    value_length = ipcom_strlen(value) + 1;
    env = ipcom_malloc(sizeof(Ipcom_env_entry) + name_length + value_length);
    if(env == IP_NULL)
        return IP_NULL;

    /* Init environment variable */
    env->name  = (const char *)env + sizeof(Ipcom_env_entry);
    ipcom_memcpy((void *)env->name, name, name_length);
    env->value = env->name + name_length;
    ipcom_memcpy((void *)env->value, value, value_length);

    return env;
}


/*
 *===========================================================================
 *                    ipcom_env_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_err
ipcom_env_init(void *procptr)
{
    Ipcom_proc   *proc = procptr;

    ip_assert(proc != IP_NULL);

    (void)ipcom_mutex_create(&proc->env_mutex);

    proc->env_tree = ipcom_hash_new((Ipcom_hash_obj_func)ipcom_env_hash_obj,
                                    (Ipcom_hash_key_func)ipcom_env_hash_key,
                                    (Ipcom_hash_cmp_func)ipcom_env_hash_cmp);
    ip_assert(proc->env_tree != IP_NULL);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                           ipcom_env_delete
 *===========================================================================
 * Description: Removes the element from its hash table and frees memory for
 *              the entry.
 * Parameters:  env - entry to delete
 *              hash - hash table the entry is assigned to
 * Returns:
 *
 */
IP_STATIC void
ipcom_env_delete(Ipcom_env_entry *env, Ipcom_hash *hash)
{
    ipcom_hash_remove(hash, env);
    ipcom_free(env);
}


/*
 *===========================================================================
 *                           ipcom_env_clone
 *===========================================================================
 * Description: Clones an env entry and adds it to the specified hash table.
 * Parameters:  env - entry to clone
 *              hash - hash table the cloned entry should be added to.
 * Returns:
 *
 */
IP_STATIC void
ipcom_env_clone(Ipcom_env_entry *env, Ipcom_hash *hash)
{
    Ipcom_env_entry *clone = ipcom_env_create(env->name, env->value);
    ipcom_hash_add(hash, clone);
}
#endif /* #if IPCOM_USE_ENV == IPCOM_ENV_IPCOM */


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
/*
 *===========================================================================
 *                    ipcom_clearenv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_proc_clearenv(Ipcom_proc *proc)
{
    ip_assert(proc != IP_NULL);

    ipcom_mutex_lock(proc->env_mutex);

    /* Remove all environment variable entries */
    ipcom_hash_for_each(proc->env_tree,
                        (Ipcom_hash_foreach_cb_func) ipcom_env_delete,
                        proc->env_tree);

    ipcom_mutex_unlock(proc->env_mutex);
}


/*
 *===========================================================================
 *                    ipcom_proc_copyenv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_proc_copyenv(Ipcom_proc *proc_c)
{
    Ipcom_proc         *proc_p;
    Ip_err              retval;

    proc_p = ipcom_proc_self();
    ip_assert(proc_p != IP_NULL);
    ip_assert(proc_c != IP_NULL);

    if (proc_p->env_tree == IP_NULL)
        /* Parent process has no environment variables */
        return;

    /* Create child environment semaphore and tree */
    retval = ipcom_once(&proc_c->env_once, ipcom_env_init, proc_c);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return;
    }

    ipcom_mutex_lock(proc_p->env_mutex);

    ipcom_hash_for_each(proc_p->env_tree,
                        (Ipcom_hash_foreach_cb_func) ipcom_env_clone,
                        proc_c->env_tree);

    ipcom_mutex_unlock(proc_p->env_mutex);
}
#endif /* #if IPCOM_USE_ENV == IPCOM_ENV_IPCOM */


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_getenv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_getenv(const char *name)
{
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    Ip_err           retval;
    Ipcom_proc      *proc;
    Ipcom_env_entry *env;
    char            *value = IP_NULL;

    proc = ipcom_proc_self();
    ip_assert(proc != IP_NULL);

    retval = ipcom_once(&proc->env_once, ipcom_env_init, proc);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return IP_NULL;
    }

    ipcom_mutex_lock(proc->env_mutex);

    env = ipcom_hash_get(proc->env_tree, name);
    if (env != IP_NULL)
        value = (char *)env->value;

    ipcom_mutex_unlock(proc->env_mutex);

    return value;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5))
    char *env;
    static char buf[256];

    env = get_env(current_process(), name);
    if (env != IP_NULL)
    {
        ipcom_strncpy(buf, env, sizeof(buf));
        free_buf((union SIGNAL **)&env);
        return (char *)buf;
    }
    return IP_NULL;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE
    return getenv(name);

#elif defined(IPCOM_USE_SYSVAR) && IPCOM_VR_MAX == 1
    static char buf[256];
    Ip_size_t  buf_size = sizeof(buf);

    return ipcom_sysvar_get(name, buf, &buf_size);

#else
    return IP_NULL;

#endif /* #if IPCOM_USE_ENV == IPCOM_ENV_IPCOM */
}


/*
 *===========================================================================
 *                    ipcom_setenv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_setenv(const char *name, const char *value, int rewrite)
{
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    Ip_err           retval;
    Ipcom_proc      *proc;
    Ipcom_env_entry *env;

    proc = ipcom_proc_self();
    ip_assert(proc != IP_NULL);

    retval = ipcom_once(&proc->env_once, ipcom_env_init, proc);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return -1;
    }

    ipcom_mutex_lock(proc->env_mutex);

    /* Check for duplicate. */
    env = ipcom_hash_get(proc->env_tree, name);
    if (env != IP_NULL)
    {
        if (rewrite == 0)
            goto leave;
        ipcom_env_delete(env, proc->env_tree);
    }

    /* Create environment variable. */
    env = ipcom_env_create(name, value);
    if (env == IP_NULL)
    {
        ipcom_mutex_unlock(proc->env_mutex);
        return -1;
    }
    (void)ipcom_hash_add(proc->env_tree, env);

 leave:
    ipcom_mutex_unlock(proc->env_mutex);
    return 0;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5))
    (void)rewrite;

    set_env(current_process(), name, value);
    return 0;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE && defined(IP_PORT_VXWORKS)
    char buf[256];
    int  ret;

    (void)rewrite;

    ipcom_snprintf(buf, sizeof(buf), "%s=%s", name, value);
    ret = (int)putenv(buf);
    return ret == 0 ? 0 : -1;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE
    return setenv(name, value, rewrite);

#elif defined(IPCOM_USE_SYSVAR) && IPCOM_VR_MAX == 1
    (void)ipcom_sysvar_set(name, value, rewrite ? IPCOM_SYSVAR_FLAG_OVERWRITE : 0);
    return 0;

#else
    (void)name;
    (void)value;
    (void)rewrite;
    return -1;

#endif /* #if IPCOM_USE_ENV == IPCOM_ENV_IPCOM */
}


/*
 *===========================================================================
 *                    ipcom_unsetenv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_unsetenv(const char *name)
{
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    Ip_err           retval;
    Ipcom_proc      *proc;
    Ipcom_env_entry *env;

    proc = ipcom_proc_self();
    ip_assert(proc != IP_NULL);

    retval = ipcom_once(&proc->env_once, ipcom_env_init, proc);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return -1;
    }

    ipcom_mutex_lock(proc->env_mutex);

    env = ipcom_hash_get(proc->env_tree, name);
    if (env != IP_NULL)
        ipcom_env_delete(env, proc->env_tree);

    ipcom_mutex_unlock(proc->env_mutex);

    return 0;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5))
    set_env(current_process(), name, NULL);
    return 0;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE && defined(IP_PORT_VXWORKS)
    {
        char buf[256];
        int  ret;
        ipcom_snprintf(buf, sizeof(buf), "%s=", name);
        ret = (int)putenv(buf);
        return ret == 0 ? 0 : -1;
    }

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE
    return unsetenv(name);

#elif defined(IPCOM_USE_SYSVAR) && IPCOM_VR_MAX == 1
    (void)ipcom_sysvar_unset(name);
    return 0;

#else
    return 0;

#endif /* #if IPCOM_USE_ENV == IPCOM_ENV_IPCOM */
}


/*
 *===========================================================================
 *                    ipcom_clearenv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_clearenv(void)
{
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    Ip_err      retval;
    Ipcom_proc *proc;

    proc = ipcom_proc_self();
    ip_assert(proc != IP_NULL);

    retval = ipcom_once(&proc->env_once, ipcom_env_init, proc);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return -1;
    }

    ipcom_proc_clearenv(proc);
    return 0;

#elif IPCOM_USE_ENV == IPCOM_ENV_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5))
    /*!! Are all environment variables cleaned up by OS automatically? */
    return 0;

#elif defined(IPCOM_USE_SYSVAR) && IPCOM_VR_MAX == 1
    /*! Hmmmm */
    return -1;

#else
    return -1;

#endif /* #if IPCOM_USE_ENV == IPCOM_ENV_IPCOM */
}


/*
 *===========================================================================
 *                    ipcom_getenv_as_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getenv_as_int(const char *name, int default_value)
{
    char *env;

    env = ipcom_getenv(name);
    if (env != IP_NULL)
        return ipcom_atoi(env);
    else
        return default_value;
}


/*
 *===========================================================================
 *                    ipcom_setenv_as_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_setenv_as_int(const char *name, int value, int rewrite)
{
    char buf[16];

    ipcom_sprintf(buf, "%d", value);
    return ipcom_setenv(name, buf, rewrite);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

