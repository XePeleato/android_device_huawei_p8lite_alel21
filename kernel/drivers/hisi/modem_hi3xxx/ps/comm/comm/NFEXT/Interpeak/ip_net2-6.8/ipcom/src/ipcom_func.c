/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_func.c,v $ $Revision: 1.25 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_func.c,v $
 *   $Author: uolofsso $
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

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_sock.h"
#include "ipcom_type.h"
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
#include "ipcom_tree.h"
#endif


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

IP_GLOBAL void ipcom_file_proc_exit(Ipcom_proc *proc);

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
IP_GLOBAL void ipcom_proc_clearenv(Ipcom_proc *proc);
#endif


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

/* ipcom port priority map. */
IP_GLOBAL IP_CONST Ip_s16 ipcom_priority_map[IPCOM_PRIORITY_MAX+1] =
{
    (Ip_s16)IPCOM_PROC_PRIO_BACKGROUND,
    (Ip_s16)IPCOM_PROC_PRIO_1,
    (Ip_s16)IPCOM_PROC_PRIO_2,
    (Ip_s16)IPCOM_PROC_PRIO_3,
    (Ip_s16)IPCOM_PROC_PRIO_4,
    (Ip_s16)IPCOM_PROC_PRIO_5,
    (Ip_s16)IPCOM_PROC_PRIO_6,
    (Ip_s16)IPCOM_PROC_PRIO_7
};


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_proc_malloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_proc *
ipcom_proc_malloc(Ip_pid_t pid, Ip_size_t pproc_size)
{
    Ipcom_proc *proc;

    proc = ipcom_calloc(1, sizeof(*proc) + pproc_size);
    if (proc == IP_NULL)
        return IP_NULL;

    if(pproc_size > 0)
        proc->pproc = (struct Ipcom_pproc_struct *)(proc + 1);

    proc->pid = pid;
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    proc->shell_fd = IP_INVALID_SOCKET;
#endif

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    ip_assert(proc->env_once.wq == IP_NULL);
    ip_assert(proc->env_once.state == IPCOM_ONCE_STATE_UNINITIALIZED);
#endif

    if (ipcom_block_create(&proc->wakeup_signal) != IPCOM_SUCCESS)
    {
        ipcom_proc_free(proc);
        return IP_NULL;
    }

    return proc;
}


/*
 *===========================================================================
 *                    ipcom_proc_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_proc_free(Ipcom_proc *proc)
{
    ip_assert(proc != IP_NULL);

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if(proc->shell_fd != IP_INVALID_SOCKET)
        ipcom_socketclose(proc->shell_fd);
#endif

#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
    ipcom_file_proc_exit(proc);
#endif

    (void) ipcom_block_delete(&proc->wakeup_signal);

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    if (proc->env_tree != IP_NULL)
    {
        ipcom_proc_clearenv(proc);

        ipcom_mutex_lock(proc->env_mutex);
        ipcom_hash_delete(proc->env_tree);
        ipcom_mutex_unlock(proc->env_mutex);

        (void)ipcom_mutex_delete(&proc->env_mutex);
    }
#endif /* IPCOM_USE_ENV == IPCOM_ENV_IPCOM */
#ifndef IP_PORT_LAS
    ipcom_pipe_free(proc->pipe);
#endif
    ipcom_free(proc);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


