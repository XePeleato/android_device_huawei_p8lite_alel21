/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pipe.c,v $ $Revision: 1.4.18.1 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pipe.c,v $
 *   $Author: ulf $
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
 * Implements single sender (always the IPNET daemon) and multiple
 * readers pipe.
 *
 * This pipe is used by the IPNET network daemon to send back
 * responses to user tasks that are using the public socket API.
 * A reader is added to the "readers" hash table when it needs to wait
 * for a message for the first time or when the network daemon needs
 * to send a message to a new PID for the first time.
 * A garbage collector timer is running in the network daemon, its
 * handler will check if all PID:s in the "readers" table is still
 * alive and remove the entry if the reader has been killed.
 *
 * The pipe implementation can handle the case where a task is killed
 * while it is waiting for a response from the network daemon in a
 * public socket API call.
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
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_pipe.h>
#include <ipcom_type.h>
#include <ipcom_hash.h>

#include "ipnet_netif.h"
#include "ipnet_signal.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 * Delay (in milliseconds) between garbage collection runs. The GC
 * timer will be started when a message is queued to a reader. The GC
 * timer will be left untouched if it already running when a message
 * is queued.
 */
#define IPNET_PIPE_GC_DELAY 1000

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                       Ipnet_pipe_reader
 *===========================================================================
 * Represent one possible reader. Each reader is distinguished by its PID.
 */
typedef struct Ipnet_pipe_reader_struct
{
    Ip_pid_t    pid; /* PID of the receiver */
    Ipcom_block sem; /* Semaphore that will be increasead each time
                        "sig" is assigned and decreased each time a
                        signal is removed by a reader */
    Ipnet_sig  *sig; /* Queued signal, can only be one queued signal
                        per PID */
}
Ipnet_pipe_reader;


/*
 *===========================================================================
 *                         Ipcom_pipe_generic
 *===========================================================================
 * Generic pipe
 */
typedef struct Ipnet_pipe
{
    Ipcom_mutex    lock;    /* Protects access to 'readers' */
    Ipcom_hash    *readers; /* Hash table of "Ipnet_pipe_readers". PID
                              is used as search key */
    Ipnet_timeout *gc_tmo;   /* Timer for the next garbage collect
                                cycle */
}
Ipnet_pipe;


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

Ip_err ipnet_pipe_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);

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
 *                          ipnet_pipe_hash_key
 *===========================================================================
 * Description: Returns a hash value for "pid"
 * Parameters:  pid - a PID
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_pipe_hash_key(Ip_pid_t *pid)
{
    return (unsigned) *pid;
}


/*
 *===========================================================================
 *                          ipnet_pipe_hash_obj
 *===========================================================================
 * Description: Returns a hash value for a reader
 * Parameters:  reader - a reader instance
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_pipe_hash_obj(Ipnet_pipe_reader *reader)
{
    return ipnet_pipe_hash_key(&reader->pid);
}


/*
 *===========================================================================
 *                          ipnet_pipe_hash_cmp
 *===========================================================================
 * Description: Checks if "reader" matches "pid"
 * Parameters:  reader - a reader instance
 *              pid - a PID
 * Returns:     IP_TRUE if they match, IP_FALSE otherwise.
 *
 */
IP_STATIC unsigned
ipnet_pipe_hash_cmp(Ipnet_pipe_reader *reader, Ip_pid_t *pid)
{
    return reader->pid == *pid;
}


/*
 *===========================================================================
 *                       ipnet_pipe_free_queued_sig
 *===========================================================================
 * Description: Frees any queued signals.
 * Parameters:  reader - the reader to free signals for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pipe_free_queued_sig(Ipnet_pipe_reader *reader)
{
    if (reader->sig)
        ipnet_sig_free(reader->sig);
}


/*
 *===========================================================================
 *                           ipnet_pipe_gc_pid
 *===========================================================================
 * Description: Verifies that a specific PID is still valid. The
 *              "reader" entry is deleted if the PID is no longer valid.
 * Parameters:  reader -
 *              htbl -
 * Returns:
 *
 */
IP_STATIC void
ipnet_pipe_gc_pid(Ipnet_pipe_reader *reader, Ipcom_hash *htbl)
{
    if (!ipcom_is_pid_valid(reader->pid))
    {
        ipnet_pipe_free_queued_sig(reader);
        ipcom_hash_remove(htbl, reader);
        ipcom_block_delete(&reader->sem);
        ipcom_free(reader);
    }
}


/*
 *===========================================================================
 *                             ipnet_pipe_gc
 *===========================================================================
 * Description: Verifies that all PID:s in the "readers" hash table is
 *              still valid. Invalid (killed tasks) are removed as reader.
 * Parameters:  pipe - the pipe to perform GC on.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pipe_gc(Ipnet_pipe *pipe)
{
    ipcom_mutex_lock(pipe->lock);

    ipcom_hash_for_each(pipe->readers,
                        (Ipcom_hash_foreach_cb_func) ipnet_pipe_gc_pid,
                        pipe->readers);

    ipcom_mutex_unlock(pipe->lock);
}


/*
 *===========================================================================
 *                         ipnet_pipe_get_reader
 *===========================================================================
 * Description: Gets the reader instance matching "pid".
 * Parameters:  pipe - a pipe instance
 *              pid - PID to get the reader instance for.
 * Returns:     The reader instane for "pid".
 *
 */
IP_STATIC Ipnet_pipe_reader *
ipnet_pipe_get_reader(Ipnet_pipe *pipe, Ip_pid_t pid)
{
    Ipnet_pipe_reader *reader;

    reader = ipcom_hash_get(pipe->readers, &pid);
    if (reader == IP_NULL)
    {
        reader = ipcom_calloc(1, sizeof(*reader));
        if (reader == IP_NULL)
            IP_PANIC();

        reader->pid = pid;
        ipcom_block_create(&reader->sem);
        ipcom_hash_add(pipe->readers, reader);
    }

    return reader;
}


/*
 *===========================================================================
 *                    ipnet_pipe_dtor
 *===========================================================================
 * Description: Destroys a pipe. This operation is not supported
 * Parameters:  pipe - the pipe do destroy
 * Returns:
 *
 */
IP_STATIC void
ipnet_pipe_dtor(Ipcom_pipe *pipe)
{
    IPCOM_UNUSED_ARG(pipe);
    /* The network daemon may never die in the current implementation */
    IP_PANIC();
}


/*
 *===========================================================================
 *                        ipnet_pipe_send
 *===========================================================================
 * Description: Sends one message to the specified pipe.
 * Parameters:  pipe - a pipe
 *              msg - the message to send
 * Returns:
 *
 */
IP_STATIC void
ipnet_pipe_send(Ipcom_pipe *pipe, void *msg)
{
    Ipnet_pipe        *p = pipe->p;
    Ipnet_sig         *sig = msg;
    Ipnet_pipe_reader *reader;

    ipcom_mutex_lock(p->lock);

    reader = ipnet_pipe_get_reader(p, sig->receiver_pid);

    ipnet_pipe_free_queued_sig(reader);
    reader->sig = msg;
    ipcom_block_post(reader->sem);

    ipcom_mutex_unlock(p->lock);

    if (p->gc_tmo == IP_NULL)
        ipnet_timeout_schedule(IPNET_PIPE_GC_DELAY,
                               (Ipnet_timeout_handler) ipnet_pipe_gc,
                               p,
                               &p->gc_tmo);
}


/*
 *===========================================================================
 *                      ipnet_pipe_try_send
 *===========================================================================
 * Description: Tries to send one message to the specified pipe.
 * Parameters:  pipe - a pipe
 *              msg - the message to send
 * Returns:     IP_TRUE since the IPNET pipe send cannot fail or block.
 *
 */
IP_STATIC Ip_bool
ipnet_pipe_try_send(Ipcom_pipe *pipe, void *msg)
{
    ipnet_pipe_send(pipe, msg);
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_pipe_recv_with_tmo
 *===========================================================================
 * Description: Receives one message from a pipe, the operation will
 *              be aborted if nothing is received within a specific
 *              duration.
 * Parameters:  pipe - the pipe to receive from
 *              psmg - pointer to where a pointer to the message
 *                    should be stored
 *              tv - maximum duration to wait for a message, IP_NULL
 *                    means wait forever.
 * Returns:     IPCOM_SUCCESS - message was received
 *              IPCOM_ERR_TIMEOUT - the specified duration elapsed
 *                    without any message being received.
 *              IPCOM_ERR_INTERRUPT - operation was aborted before any message
 *                    was received.
 *
 */
IP_STATIC int
ipnet_pipe_recv_with_tmo(Ipcom_pipe *pipe, void **pmsg, struct Ip_timeval *tv)
{
    Ipnet_pipe        *p = pipe->p;
    Ip_u32             msec;
    int                ret = IPCOM_SUCCESS;
    Ipnet_pipe_reader *reader;

    if (tv)
        msec = tv->tv_sec * 1000 + tv->tv_usec / 1000;
    else
        msec = IPC_TIMEOUT_INFINITE;

    ipcom_mutex_lock(p->lock);

    reader = ipnet_pipe_get_reader(p, ipcom_getpid());

    while (ret == IPCOM_SUCCESS && reader->sig == IP_NULL)
    {
        ipcom_mutex_unlock(p->lock);
        /* Unlocking here is OK since the only thing that can make
           "reader" invalid is if this task gets killed. A killed
           task will not dereference any pointers ;) */
        ret = ipcom_block_wait(reader->sem, msec);
        ipcom_mutex_lock(p->lock);
    }

    *pmsg = reader->sig;
    reader->sig = IP_NULL;

    ipcom_mutex_unlock(p->lock);

    return *pmsg ? IPCOM_SUCCESS : ret;
}


/*
 *===========================================================================
 *                    ipnet_pipe_recv
 *===========================================================================
 * Description: Receives one message from the specified pipe.
 * Parameters:  pipe - a pipe
 *              pmsg - pointer to where a pointer to the received
 *                    message should be stored
 *
 */
IP_STATIC int
ipnet_pipe_recv(Ipcom_pipe *pipe, void **pmsg)
{
    int ret;

    do
    {
        ret = ipnet_pipe_recv_with_tmo(pipe, pmsg, IP_NULL);
    } while (ret == IPCOM_ERR_TIMEOUT);

    return ret;
}



/*
 *===========================================================================
 *                             ipnet_pipe_new
 *===========================================================================
 * Description: Creates a new IPNET pipe.
 * Parameters:
 * Returns:     The new pipe of IP_NULL if out of memory
 *
 */
IP_STATIC Ipnet_pipe *
ipnet_pipe_new(void)
{
    Ipnet_pipe *pipe;

    pipe = ipcom_calloc(1, sizeof(*pipe));
    if (pipe != IP_NULL)
    {
        pipe->readers
            = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_pipe_hash_obj,
                             (Ipcom_hash_key_func) ipnet_pipe_hash_key,
                             (Ipcom_hash_cmp_func) ipnet_pipe_hash_cmp);
        ipcom_mutex_create(&pipe->lock);
    }

    return pipe;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_pipe_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_pipe_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr)
{
    static Ipcom_pipe_ops ipnet_pipe_ops = {
        ipnet_pipe_dtor,
        ipnet_pipe_send,
        ipnet_pipe_try_send,
        ipnet_pipe_recv,
        ipnet_pipe_recv_with_tmo,
    };

    IPCOM_UNUSED_ARG(attr);

    pipe->p = ipnet_pipe_new();
    if (pipe->p == IP_NULL)
        return IPCOM_ERR_FAILED;

    pipe->ops = &ipnet_pipe_ops;
    return IPCOM_SUCCESS;
}



/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

