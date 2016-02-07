/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pipe.c,v $ $Revision: 1.8.24.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_pipe.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus.carlstedt@windriver.com>
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
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_pipe.h>

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

IP_EXTERN Ip_err ipcom_pipe_generic_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);
IP_EXTERN Ip_err ipcom_pipe_lockless_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);
IP_EXTERN Ip_err ipcom_pipe_job_queue_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);
IP_EXTERN Ip_err ipnet_pipe_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_err ipcom_pipe_register_type_int(int *type,
                                              Ipcom_pipe_create create);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_mutex ipcom_pipe_mutex;
IP_STATIC Ipcom_pipe_create ipcom_pipe_types[IPCOM_PIPE_MAX+1];
IP_STATIC Ipcom_once_t ipcom_pipe_once = IPCOM_ONCE_INIT;

struct Ip_timeval ipcom_pipe_tv_zero = { 0, 0 };


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_pipe_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_pipe_init(void *unused)
{
    Ip_err err;
    int type;

    (void)unused;

    err = ipcom_mutex_create(&ipcom_pipe_mutex);
    if (err != IPCOM_SUCCESS)
        return err;

    ipcom_memset(ipcom_pipe_types, 0, sizeof(ipcom_pipe_types));
    type = IPCOM_PIPE_GENERIC;
    err = ipcom_pipe_register_type_int(&type, ipcom_pipe_generic_create);
#ifndef IP_PORT_OSE5
    type = IPCOM_PIPE_LOCKLESS;
    err = ipcom_pipe_register_type_int(&type, ipcom_pipe_lockless_create);
#endif
#ifdef IPCOM_USE_JOB_QUEUE
    type = IPCOM_PIPE_JOB_QUEUE;
    err = ipcom_pipe_register_type_int(&type, ipcom_pipe_job_queue_create);
#endif
#ifdef IPNET
    type = IPCOM_PIPE_IPNET;
    err = ipcom_pipe_register_type_int(&type, ipnet_pipe_create);
#endif
    return err;
}


/*
 *===========================================================================
 *                    ipcom_pipe_register_type_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_pipe_register_type_int(int *type, Ipcom_pipe_create create)
{
    Ip_err err = IPCOM_SUCCESS;
    int    i   = 0;

    ipcom_mutex_lock(ipcom_pipe_mutex);
    if (*type == IPCOM_PIPE_NEXT)
    {
        while(i < (IPCOM_PIPE_MAX+1) && ipcom_pipe_types[i] != IP_NULL)
            i++;
        if (i > IPCOM_PIPE_MAX)
        {
            err = IPCOM_ERR_FAILED;
            goto done;
        }
        else
            *type = i;
    }
    else if (*type > IPCOM_PIPE_MAX || ipcom_pipe_types[*type] != IP_NULL)
    {
        err = IPCOM_ERR_FAILED;
        goto done;
    }
    ipcom_pipe_types[*type] = create;

done:
    ipcom_mutex_unlock(ipcom_pipe_mutex);
    return err;
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
 *                    ipcom_proc_attr_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pipe_attr_init(Ipcom_pipe_attr *attr)
{
    attr->msg_limit = IPCOM_PIPE_MSG_LIMIT_FEW;
}


/*
 *===========================================================================
 *                    ipcom_pipe_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_pipe *
ipcom_pipe_new(int type, Ipcom_pipe_attr *attr)
{
    Ipcom_pipe *pipe;
    Ip_err      err;

    err = ipcom_once(&ipcom_pipe_once, ipcom_pipe_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return IPCOM_PIPE_INVALID;

    pipe = ipcom_malloc(sizeof(*pipe));
    if (pipe == IP_NULL)
        return IPCOM_PIPE_INVALID;
    ipcom_memset(pipe, 0, sizeof(*pipe));
    pipe->type  = type;
    pipe->owner = ipcom_getpid();
    if (attr)
        ipcom_memcpy(&pipe->attr, attr, sizeof(*attr));

    ipcom_mutex_lock(ipcom_pipe_mutex);
    if (type > IPCOM_PIPE_MAX || ipcom_pipe_types[type] == IP_NULL)
    {
        err = IPCOM_ERR_FAILED;
        goto done;
    }
    err = ipcom_pipe_types[type](pipe, attr);

done:
    ipcom_mutex_unlock(ipcom_pipe_mutex);
    if (err != IPCOM_SUCCESS)
    {
        ipcom_free(pipe);
        return IPCOM_PIPE_INVALID;
    }
    return pipe;
}


/*
 *===========================================================================
 *                    ipcom_pipe_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_pipe_free(Ipcom_pipe *pipe)
{
    if (pipe != IP_NULL)
    {
        pipe->ops->dtor(pipe);
        ipcom_free(pipe);
    }
}


/*
 *===========================================================================
 *                    ipcom_pipe_register_type
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_pipe_register_type(int *type, Ipcom_pipe_create create)
{
    Ip_err err = IPCOM_SUCCESS;

    err = ipcom_once(&ipcom_pipe_once, ipcom_pipe_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    return ipcom_pipe_register_type_int(type, create);
}



/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
