/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_os.c,v $ $Revision: 1.120.20.4 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_os.c,v $
 *     $Author: dlkrejsa $ $Date: 2010-07-27 18:06:22 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_SKIP_NATIVE_SOCK_API

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <vxWorks.h>
#include <cpuset.h>
#include <private/taskLibP.h>
#include <private/vxCpuLibP.h>
#include <private/taskLibP.h>  /* for taskIdListSort() */
#include <private/taskUtilLibP.h>  /* for taskReservedFieldGet() */
#include <semLib.h>

/* Special Ipcom_ipc trick. */
#include "ipcom_type.h"
typedef struct Ipcom_ipc
{
    Ip_u32     magic;
    Ip_pid_t   peer_pid;
}
Ipcom_ipc; /*lint !e18 */
#define IPCOM_IPC_T_TYPE
#include "ipcom_os.h"

#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>
#include <ipcom_vxworks.h>

#ifdef IPNET
#include "ipnet_h.h"
#endif

#include <drv/timer/timerDev.h>
#include <errnoLib.h>
#include <intLib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taskLib.h>
#include <tickLib.h>
#include <time.h>
#include <vxCpuLib.h>
#include <net/socketvar.h>

#if defined(IPCOM_USE_VXSEM_INLINE) || defined(IPCOM_USE_VXSEM_SCALABLE)

/* IPCOM_VXSEM_MUTEX_OPTS is configured in ipcom_pconfig.h */
#define SEM_M_INLINE_OPTS IPCOM_VXSEM_MUTEX_OPTS

#define MUTEX_CREATE_OPTS (SEM_MUT_OPTIONS_MASK & SEM_M_INLINE_OPTS)

#define SEM_B_INLINE_OPTS \
    (SEM_Q_FIFO | \
     SEM_NO_ID_VALIDATE | \
     SEM_NO_ERROR_CHECK | \
     SEM_NO_SYSTEM_VIEWER | \
     SEM_NO_EVENT_SEND)

#endif

#ifdef IPCOM_USE_VXSEM_INLINE
#include <inline/semLibInline.h>
#elif defined (IPCOM_USE_VXSEM_SCALABLE)
#include <semLib.h>
#else

#include <semLib.h>
#define MUTEX_CREATE_OPTS \
    (SEM_INVERSION_SAFE | SEM_Q_PRIORITY | SEM_DELETE_SAFE)

#endif /* IPCOM_USE_VXSEM_INLINE */

#ifdef IPCOM_USE_SMP
#ifdef IPCOM_USE_SPINLOCK_ISR_ND
#include <private/spinlockIsrNdLibP.h>
#else
#include <spinLockLib.h>
#endif /* IPCOM_USE_SPINLOCK_ISR_ND */
#endif /* IPCOM_USE_SMP */

/* Below functions has been changed to macros in ipcom_os.h: */
#ifdef ipcom_sem_wait
#undef ipcom_sem_wait
IP_FASTTEXT IP_PUBLIC Ip_err ipcom_sem_wait(Ipcom_sem sem);
#endif

#undef ipcom_sem_post
#undef ipcom_sem_interrupt_post
#undef ipcom_interrupt_disable
#undef ipcom_interrupt_enable
#undef ipcom_mutex_lock
#undef ipcom_mutex_unlock

#ifdef ipcom_block_post
#undef ipcom_block_post
IP_PUBLIC void ipcom_block_post(Ipcom_block block);
#endif

#undef ipcom_getpid


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

#undef IPCOM_SYSLOG_PRIORITY
#undef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

#define IPCOM_IPC_MAGIC      0x43218765
#define IPCOM_IPC_MSG_MAGIC  0x56781234

#ifdef IP_ASSERT
#define CHK(x)  chk(x, #x, __FILE__, __LINE__)
#define PCHK(x) pchk(x, #x, __FILE__, __LINE__)
#else
#define CHK(x)  (x)
#define PCHK(x) (x)
#endif

#if IP_PORT_VXWORKS >= 65
#define IPCOM_TCB_MEMBER   pIpcomInfo
#else
#define IPCOM_TCB_MEMBER   reserved1
#endif


#ifdef IPCOM_USE_VXSEM_INLINE

#  define SEM_B_TAKE(sem, timeout) \
    semBTake_inline((SEM_ID)sem, timeout, SEM_B_INLINE_OPTS)
#  define SEM_B_GIVE(sem) \
    semBGive_inline((SEM_ID)sem, SEM_B_INLINE_OPTS)

#  define SEM_M_TAKE(sem, timeout) \
    semMTake_inline((SEM_ID)sem, timeout, SEM_M_INLINE_OPTS)
#  define SEM_M_GIVE(sem) \
    semMGive_inline((SEM_ID)sem, SEM_M_INLINE_OPTS)

#elif defined (IPCOM_USE_VXSEM_SCALABLE)

#  define SEM_B_TAKE(sem, timeout)				\
    semBTakeScalable((SEM_ID)sem, timeout, SEM_B_INLINE_OPTS)
#  define SEM_B_GIVE(sem) \
    semBGiveScalable((SEM_ID)sem, SEM_B_INLINE_OPTS)

#  define SEM_M_TAKE(sem, timeout) \
    semMTakeScalable((SEM_ID)sem, timeout, SEM_M_INLINE_OPTS)
#  define SEM_M_GIVE(sem) \
    semMGiveScalable((SEM_ID)sem, SEM_M_INLINE_OPTS)

#else
# define SEM_B_TAKE(sem, timeout) semTake((SEM_ID)sem, timeout)
# define SEM_B_GIVE(sem)          semGive((SEM_ID)sem)
# define SEM_M_TAKE(sem, timeout) semTake((SEM_ID)sem, timeout)
# define SEM_M_GIVE(sem)          semGive((SEM_ID)sem)
#endif /* IPCOM_USE_VXSEM_INLINE */


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipc_open_q
 *===========================================================================
 */
typedef struct Ipc_open_q_struct
{
    Ipcom_list     list;
    Ipcom_sem      open_queue_sem;
    Ipcom_block	   open_queue_block;
    int            open_queue_size;
    char           ipc_srv_name[IPCOM_IPC_MAX_NAME];
}
Ipc_open_q;


/*
 *===========================================================================
 *                         Ipc_msg
 *===========================================================================
 */
typedef struct Ipc_msg_struct
{
    Ipcom_list   list;
    Ip_u32       magic;
    Ip_u32       size;
    Ip_pid_t     sender_pid;
    char         data[1];
}
Ipc_msg;


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

#ifdef IPCOM_USE_SMP
#ifdef IPCOM_USE_SPINLOCK_ISR_ND
IP_EXTERN spinlockIsrNd_t ipcom_spinlock;  /* global IPCOM spinlock */
#else
IP_EXTERN spinlockIsr_t ipcom_spinlock;  /* global IPCOM spinlock */
#endif
#endif


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_ipc_msg_from_buf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipc_msg *
ipcom_ipc_msg_from_buf(void *buf)
{
    return (Ipc_msg *) ((Ip_u8 *) buf - ip_offsetof(Ipc_msg, data));
}


/*
 *===========================================================================
 *                    millisec2tick
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
millisec2tick(int ms)
{
    int ticks;

    ip_assert(ms >= -1);

    switch(ms)
    {
    case IPC_TIMEOUT_POLL:
    case IPC_TIMEOUT_INFINITE:
        ticks = ms;
        break;
    default:
        ticks = (ms*sysClkRateGet())/1000;
        if (ticks == 0)
            ticks++; /* Wait at least one tick, if ms > 0 */
    }

    return ticks;
}


/*
 *===========================================================================
 *                    find_open_q
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipc_open_q *
find_open_q(const char *ipc_name)
{
    Ipc_open_q *open_q;

    for (open_q = (Ipc_open_q *)ipcom_list_first(&ipcom_port.open_q_head);
         open_q;
         open_q = (Ipc_open_q *)ipcom_list_next(&(open_q->list)))
    {
        if (ipcom_strcmp(ipc_name, open_q->ipc_srv_name) == 0)
            return open_q;
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    delete_open_q
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
delete_open_q(Ipc_open_q *entry)
{
    Ip_err rc = IPCOM_SUCCESS;

    if (entry == IP_NULL)
        return IPCOM_ERR_FAILED;

    /* Remove the QUEUE item - will make it 'unreachable' */
    ipcom_list_remove(&(entry->list));

    /* Release the master LOCK. All entries that are waiting for this
     * item will now fall through and post their references */
    ipcom_block_post(entry->open_queue_block);

    /* Fetch all references; make certain that NO entries are left
     * that may access destroyed data */
    while(entry->open_queue_size-- > 0)
    {
        /* Consume all references to this QUEUE item. Will make certain
         * that all TASKS waiting for this QUEUE item has been safely
         * allowed to exit */
        if (ipcom_sem_wait(entry->open_queue_sem) != IPCOM_SUCCESS)
            IPCOM_LOG1(ERR, "ipcom_ipc_install :: ipcom_sem_post failed, errno = %d", ipcom_errno);
    }

    /* Reclaim the mutex; ensure that all waiting TASKS have
     * exited the 'dangerous' area */
    ipcom_block_wait(entry->open_queue_block, -1);

    /* Destroy the synchronizaton items; all TASKS are out
     * and their purpose is completed */
    if (ipcom_block_delete(&entry->open_queue_block) != IPCOM_SUCCESS)
	rc = IPCOM_ERR_FAILED;
    if (ipcom_sem_delete(&entry->open_queue_sem) != IPCOM_SUCCESS)
	rc = IPCOM_ERR_FAILED;

    ipcom_free(entry);

    return rc;
}


/*
 *===========================================================================
 *                    delete_open_q_2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
delete_open_q_2(const char *ipc_name)
{
    return delete_open_q(find_open_q(ipc_name));
}


/*
 *===========================================================================
 *                    ipcom_ipc_free_inqueue_msgs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipcom_ipc_free_inqueue_msgs(Ipcom_proc *proc)
{
    Ipc_msg *ipc_msg;

    /* Free in-queue messages */
    for (ipc_msg = (Ipc_msg *)ipcom_list_first(&(proc->pproc->ipc_in_queue));
         ipc_msg;
         ipc_msg = (Ipc_msg *)ipcom_list_first(&(proc->pproc->ipc_in_queue)))
    {
        ip_assert(ipc_msg->magic == IPCOM_IPC_MSG_MAGIC);

        ipcom_list_remove(&(ipc_msg->list));
        ipcom_free(ipc_msg);
    }
}


/*
 *===========================================================================
 *                    chk
 *===========================================================================
 * Description:	Check system call.
 * Parameters:	Status.
 * Returns:	Status.
 *
 */
#ifdef IP_ASSERT
IP_STATIC STATUS
chk(int status, char *msg, char *file, int line)
{
    if (status == ERROR)
    {
        IPCOM_LOG4(ERR, "VxWorks returned ERROR at %s(%d): %s, errno:%d\n",
                   file, line, msg, errno);
    }

    return status;
}
#endif


/*
 *===========================================================================
 *                    pchk
 *===========================================================================
 * Description:	Check system call.
 * Parameters:	Status.
 * Returns:	Status.
 *
 */
#ifdef IP_ASSERT
IP_STATIC void *
pchk(void *status, char *msg, char *file, int line)
{
    if (status == NULL)
    {
        IPCOM_LOG4(ERR, "VxWorks returned NULL at %s(%d): %s, errno:%d\n",
                   file, line, msg, errno);
    }

    return status;
}
#endif


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_sleep
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC unsigned int
ipcom_sleep(unsigned int seconds)
{
    return taskDelay(sysClkRateGet() * seconds);
}


/*
 *===========================================================================
 *                    ipcom_millisleep
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC unsigned int
ipcom_millisleep(unsigned int milli_seconds)
{
#if IP_PORT_VXWORKS == 54
    int ticks = sysClkRateGet();
#else
    uint64_t ticks = sysClkRateGet();
#endif
    ticks = ticks * milli_seconds / 1000;
    return taskDelay((int)ticks ? (int)ticks : 1);
}

/*
 *===========================================================================
 *                    ipcom_getpid
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_pid_t
ipcom_getpid(void)
{
    return (Ip_pid_t)taskIdSelf();
}


/*
 *===========================================================================
 *                           ipcom_is_pid_valid
 *===========================================================================
 * Description: Checks if a pid matching any running task.
 * Parameters:  pid - PID to check
 * Returns:
 *
 */
IP_PUBLIC Ip_bool
ipcom_is_pid_valid(Ip_pid_t pid)
{
    return taskIdVerify(pid) == OK;
}


/*
 *===========================================================================
 *                    ipcom_interrupt_disable
 *===========================================================================
 * Description:	Disable all interrupts on the current CPU.
 * Parameters:	None.
 * Returns:	    Value to be used in subsequent ipcom_interrupt_enable().
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_u32
ipcom_interrupt_disable(void)
{
#ifdef IPCOM_USE_SMP

#ifdef IPCOM_USE_SPINLOCK_ISR_ND
    return spinlockIsrNdTake(&ipcom_spinlock);
#else
    SPIN_LOCK_ISR_TAKE(&ipcom_spinlock);
    return IPCOM_SUCCESS;   /* always */
#endif

#else
    return intLock();
#endif
}


/*
 *===========================================================================
 *                    ipcom_interrupt_enable
 *===========================================================================
 * Description:	Enable all interrupts to a previous level on the current CPU.
 * Parameters:	Old interrupt level/ID/whatever.
 * Returns:	Nothing.
 *
 */
IP_FASTTEXT IP_PUBLIC void
ipcom_interrupt_enable(Ip_u32 key)
{
#ifdef IPCOM_USE_SMP

#ifdef IPCOM_USE_SPINLOCK_ISR_ND
    spinlockIsrNdGive(&ipcom_spinlock, (int)key);
#else
    IPCOM_UNUSED_ARG(key);
    SPIN_LOCK_ISR_GIVE(&ipcom_spinlock);
#endif

#else
    (void)intUnlock(key);
#endif
}


/*
 *===========================================================================
 *                    ipcom_preempt_disable
 *===========================================================================
 * Description:	Disable task preemption on the current CPU.
 *              In VxWorks, we currently implement this by locking
 *              interrupts, since locking and unlocking interrupts
 *              is faster than taskCpuLock() followed by taskCpuUnlock().
 * Parameters:	None.
 * Returns:	Value to be used in subsequent ipcom_preempt_enable().
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_preempt_key
ipcom_preempt_disable(void)
{
    return (Ip_preempt_key)(INT_CPU_LOCK ());
}


/*
 *===========================================================================
 *                    ipcom_preempt_enable
 *===========================================================================
 * Description:	Enable previous task preemption lock level on the current CPU.
 *              On VxWorks this reenables the previous interrupt locking mask.
 * Parameters:	Value returned by previous ipcom_preempt_disable() call.
 * Returns:	Nothing.
 *
 */
IP_FASTTEXT IP_PUBLIC void
ipcom_preempt_enable(Ip_preempt_key key)
{
    INT_CPU_UNLOCK (((int)key));
}


/*
 *===========================================================================
 *                    ipcom_block_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_block_create(Ipcom_block *block)
{
    *block = (Ipcom_block)PCHK(semBCreate(SEM_Q_FIFO, SEM_EMPTY));
    if (IP_NULL != *block)
    {
#if defined(_WRS_CONFIG_OBJECT_OWNERSHIP)
        /* Set kernel as owner to disable possible RTP cleanup */
        if (objOwnerSet(*block, kernelId) == OK)
            return IPCOM_SUCCESS;

        semDelete((SEM_ID)*block);
#else
        return IPCOM_SUCCESS;
#endif
    }

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_block_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_block_delete(Ipcom_block *block)
{
    Ip_err rc;

    ip_assert(*block != IPCOM_BLOCK_INVALID);

    rc = (CHK(semDelete((SEM_ID)*block)) == OK ? IPCOM_SUCCESS : IPCOM_ERR_FAILED);
    *block = IPCOM_BLOCK_INVALID;
    return rc;
}


/*
 *===========================================================================
 *                    ipcom_block_wait
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_block_wait(Ipcom_block block, Ip_s32 milli_seconds)
{
    Ip_s64 tot_timeout;
    STATUS rc;

    ip_assert(milli_seconds >= -1);

    tot_timeout = 0;
    switch(milli_seconds)
    {
    case 0: /* Poll */
    case -1: /* WAIT_FOREVER */
        rc = SEM_B_TAKE(block, milli_seconds);
        break;
    default:
        tot_timeout = milli_seconds;
        tot_timeout *=  sysClkRateGet();
        tot_timeout /= 1000;
        if (!tot_timeout)
            tot_timeout = 1;

        ip_assert(tot_timeout > 0);

        do
        {
            int timeout;

            timeout = (int)IP_MIN(0x7fffffff, tot_timeout);
            tot_timeout -= timeout;

            ip_assert(timeout > 0);

            rc = SEM_B_TAKE(block, timeout);

            if (rc == OK)
                break;

            if (errno != S_objLib_OBJ_TIMEOUT)
                break;


        } while (tot_timeout != 0);
        break;
    }

    if (rc == OK)
        return IPCOM_SUCCESS;

    switch(errno)
    {
    case S_objLib_OBJ_TIMEOUT:
        errnoSet(0);
        return IPCOM_ERR_TIMEOUT;
    default:
        return IPCOM_ERR_FAILED;
    }
}


/*
 *===========================================================================
 *                    ipcom_block_post
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_block_post(Ipcom_block block)
{
    (void)SEM_B_GIVE(block);
}


/*
 *===========================================================================
 *                    ipcom_mutex_create
 *===========================================================================
 * Description:	Create mutex with undefined queue order.
 * Parameters:	Pointer to mutex descriptor.
 * Returns:	IPCOM_SUCCESS if success.
 *
 */
IP_PUBLIC Ip_err
ipcom_mutex_create(Ipcom_mutex *mutex)
{
    *mutex = (Ipcom_mutex)PCHK(semMCreate(MUTEX_CREATE_OPTS));
    if (*mutex != NULL)
    {
#if defined(_WRS_CONFIG_OBJECT_OWNERSHIP)
        /* Set kernel as owner to disable possible RTP cleanup */
        if (objOwnerSet(*mutex, kernelId) == OK)
            return IPCOM_SUCCESS;

        semDelete((SEM_ID)*mutex);
#else
        return IPCOM_SUCCESS;
#endif
    }

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_mutex_delete
 *===========================================================================
 * Description:	Delete mutex. The mutex must be empty,
 *		i.e. have no pending processes.
 * Parameters:	Mutex descriptor.
 * Returns:	0 if success.
 *
 */
IP_PUBLIC Ip_err
ipcom_mutex_delete(Ipcom_mutex *mutex)
{
    STATUS   ret;

    ip_assert(*mutex != IPCOM_MUTEX_INVALID);

    ret = CHK(semDelete((SEM_ID)*mutex));
    *mutex = IPCOM_MUTEX_INVALID;

    return (ret == OK) ? IPCOM_SUCCESS : IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_mutex_lock
 *===========================================================================
 * Description:	Lock mutex. Recursive calls by the same process
 *		are not allowed.
 * Parameters:	Mutex descriptor.
 * Returns:	Nothing.
 *
 */
IP_FASTTEXT IP_PUBLIC void
ipcom_mutex_lock(Ipcom_mutex mutex)
{
    CHK(SEM_M_TAKE(mutex, WAIT_FOREVER));
}


/*
 *===========================================================================
 *                    ipcom_mutex_unlock
 *===========================================================================
 * Description:	Unlock mutex. Only the process that owns
 *		the mutex may unlock it.
 * Parameters:	Mutex descriptor.
 * Returns:	Nothing.
 *
 */
IP_FASTTEXT IP_PUBLIC void
ipcom_mutex_unlock(Ipcom_mutex mutex)
{
    CHK(SEM_M_GIVE(mutex));
}


/*
 *===========================================================================
 *                    ipcom_sem_create
 *===========================================================================
 * Description:	Create counting semaphore mutex with undefined queue order.
 * Parameters:	Semaphore descriptor.
 * Returns:	0 if success.
 *
 */
IP_PUBLIC Ip_err
ipcom_sem_create(Ipcom_sem *sem, Ip_s32 value)
{
    *sem = (Ipcom_sem)PCHK(semCCreate(SEM_Q_FIFO, value));
    if (*sem != NULL)
    {
#if defined(_WRS_CONFIG_OBJECT_OWNERSHIP)
        /* Set kernel as owner to disable possible RTP cleanup */
        if (objOwnerSet(*sem, kernelId) == OK)
            return IPCOM_SUCCESS;

        semDelete((SEM_ID)*sem);
#else
        return IPCOM_SUCCESS;
#endif
    }

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_sem_delete
 *===========================================================================
 * Description:	Delete semaphore. The semaphore must be empty,
 *		i.e. have no pending processes.
 * Parameters:	Semaphore descriptor.
 * Returns:	0 if success.
 *
 */
IP_PUBLIC Ip_err
ipcom_sem_delete(Ipcom_sem *sem)
{
    STATUS   ret;

    ip_assert(*sem != IPCOM_SEM_INVALID);

    ret = CHK(semDelete((SEM_ID)*sem));
    *sem = IPCOM_SEM_INVALID;

    return (ret == OK) ? IPCOM_SUCCESS : IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_sem_wait
 *===========================================================================
 * Description:	Wait on semaphore.
 * Parameters:	Semaphore descriptor.
 * Returns:	Nothing.
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_err
ipcom_sem_wait(Ipcom_sem sem)
{
    return CHK(semTake((SEM_ID)sem, WAIT_FOREVER));
}


/*
 *===========================================================================
 *                    ipcom_sem_post
 *===========================================================================
 * Description:	Post to semaphore.
 * Parameters:	Semaphore descriptor.
 * Returns:	Nothing.
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_err
ipcom_sem_post(Ipcom_sem sem)
{
    return CHK(semGive((SEM_ID)sem));
}


/*
 *===========================================================================
 *                    ipcom_sem_interrupt_post
 *===========================================================================
 * Description: Post to semaphore in interrupt context
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_err
ipcom_sem_interrupt_post(Ipcom_sem sem)
{
    return CHK(semGive((SEM_ID)sem));
}


/*
 *===========================================================================
 *                    ipcom_sem_flush
 *===========================================================================
 * Description:	Flush the Binary or Counting semaphore.
 * Parameters:	Semaphore descriptor.
 * Returns:	Nothing.
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_err
ipcom_sem_flush(Ipcom_sem sem)
{
	return CHK(semFlush((SEM_ID)sem));
}


/*
 *===========================================================================
 *                    ipcom_sem_interrupt_flush
 *===========================================================================
 * Description: Flush the Binary or Counting semaphore in interrupt context
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_err
ipcom_sem_interrupt_flush(Ipcom_sem sem)
{
	return CHK(semFlush((SEM_ID)sem));
}


/*
 *===========================================================================
 *                    ipcom_sem_getvalue
 *===========================================================================
 * Description:	Get value of semaphore.
 * Parameters:	Semaphore descriptor.
 * Returns:
 *
 */
IP_PUBLIC Ip_s32
ipcom_sem_getvalue(Ipcom_sem sem)
{
    int id_list[16];
    int len;

    len = semInfo((SEM_ID)sem, id_list, 16);
    ip_assert(len < 16);

    return len;
}


/*
 *===========================================================================
 *                    ipcom_proc_acreate
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_proc_acreate(const char *name, void (*entry)(void), const Ipcom_proc_attr *attr, Ip_pid_t *pidp)
{
    int     stacksize;
    Ip_u16  priority;
    int opt = 0;
    int pid;
    Ip_u32 flags;
    Ip_cpu_set_t cpuset;
#if IPCOM_VR_MAX > 1
    char procname[IPCOM_PROC_NAME_MAX+1];
#endif

    if (attr != IP_NULL)
    {
        stacksize = (int)attr->stacksize;
        priority  = attr->priority;
        flags     = attr->flags;
        ipcom_memcpy(&cpuset, &attr->cpuset, sizeof(cpuset));
    }
    else
    {
        stacksize = IPCOM_PROC_STACK_DEFAULT;
        priority  = IPCOM_PRIORITY_DEFAULT;
        flags     = 0;
        IP_CPU_ZERO(&cpuset);
    }

    if (stacksize == IPCOM_PROC_STACK_INHERIT)
    {
#if IP_PORT_VXWORKS >= 65
        if (taskStackSizeGet(ipcom_getpid(), &stacksize) != OK)
            return IPCOM_ERR_FAILED;
#else
        TASK_DESC task_desc;
        if (taskInfoGet(ipcom_getpid(), &task_desc) != OK)
            return IPCOM_ERR_FAILED;
        stacksize = (int)task_desc.td_stackSize; /*lint !e734*/
#endif
    }

    if (priority == IPCOM_PROC_PRIO_INHERIT)
        priority = (Ip_u16)ipcom_proc_getprio(ipcom_getpid());

    if (priority > IPCOM_PRIORITY_MAX)
        return IPCOM_ERR_INVALID_ARG;

    /* Change priority into OS priority. */
    priority = ipcom_priority_map[priority];

#if IPCOM_USE_ENV == IPCOM_ENV_NATIVE
    opt |= VX_PRIVATE_ENV;
#endif
    if (flags & IPCOM_PROC_FLAG_FP)
        opt |= VX_FP_TASK;

#if IPCOM_VR_MAX > 1
    {
        int  vr = ipcom_proc_vr_get();
        if (vr != 0)
        {
            ipcom_sprintf(procname, "%s#%d" ,name, vr);
            name = procname;
        }
    }
#endif /* IPCOM_VR_MAX */

    /* Create the process. */
    pid = CHK(taskSpawn((char *)name, priority, opt, stacksize, (FUNCPTR) entry,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    if (pid == ERROR)
        return IPCOM_ERR_FAILED;

    /* Set the 'process' CPU affinity */
    if (ipcom_proc_cpu_affinity_set(pid, (Ip_cpu_set_t *)&cpuset) != IPCOM_SUCCESS)
    {
        IP_PANIC();
    }

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    {
        Ipcom_proc *proc;
#error NO_PROC_IS_CREATED_IN_PROC_FIND___RACE_CONDITION
        proc = ipcom_proc_find(pid, IP_TRUE);
        ipcom_proc_copyenv(proc);
    }
#endif

    if (pidp != IP_NULL)
        *pidp = (Ip_pid_t)pid;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_proc_create
 *===========================================================================
 * Description:	Create process. All processes reside in the same
 *		address space which means that normal memory in
 *		conjunction with mutexes and semaphores can be
 *		used to implement IPC. The create call will also
 *		start the new process immediately. No assumptions
 *		can be made about the inheritance of descriptors.
 * Parameters:	Pointer to ID, name, entrypoint.
 * Returns:	0 if success.
 *
 */
IP_PUBLIC Ip_err
ipcom_proc_create(const char *name, void (*entry)(void), Ip_u32 stacksz, Ip_pid_t *pidp)
{
    Ipcom_proc_attr  attr;

    ipcom_proc_attr_init(&attr);
    attr.priority = IPCOM_PROC_PRIO_INHERIT;
    attr.stacksize = (Ip_u16)stacksz;

    return ipcom_proc_acreate(name, entry, &attr, pidp);
}


/*
 *===========================================================================
 *                    ipcom_proc_exit
 *===========================================================================
 * Description:	Terminate the calling process.
 * Parameters:	Process ID.
 * Returns:	Nothing.
 *
 */
IP_PUBLIC void
ipcom_proc_exit(void)
{
    WIND_TCB  *tcb;


    tcb = taskTcb(taskIdSelf());
    ip_assert(tcb);

#if defined(IP_DEBUG) && IP_PORT_VXWORKS >= 65
    {
        TASK_DESC task_desc;
        if (taskInfoGet(taskIdSelf(), &task_desc) == OK)
        {
            if ((task_desc.td_options & VX_NO_STACK_FILL) == 0)
            {
                ip_assert(task_desc.td_stackMargin > 0);
                ip_assert(task_desc.td_excStackMargin > 0);
            }
            else
            {
                IPCOM_LOG0(ERR, "Stack fill not available");
            }
        }
        else
        {
            IPCOM_LOG0(ERR, "Failed to retrieve task information");
        }
    }
#endif

    if (tcb->IPCOM_TCB_MEMBER)
    {
        (void)ipcom_vxworks_proc_free((Ipcom_proc *)tcb->IPCOM_TCB_MEMBER);
        tcb->IPCOM_TCB_MEMBER = IP_NULL;
    }
    CHK(taskDelete((int) taskIdSelf())); /*lint !e534*/

    IPCOM_LOG0(ERR, "Resumed execution after ipcom_proc_exit(), entering loop");

    /*lint -e550*/
    for (;;)
    {
        static int loop;
        ipcom_millisleep(1000);
        loop++;
    }
}


/*
 *===========================================================================
 *                    ipcom_vxworks_proc_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_vxworks_proc_free(Ipcom_proc *proc)
{
    if (proc == IP_NULL)
        return IPCOM_ERR_FAILED;

    ipcom_mutex_lock(ipcom_port.code_mutex);

    ipcom_ipc_free_inqueue_msgs(proc);     /* Free in-queue messages */

    /* Delete IPC semaphore */
    (void)ipcom_sem_delete(&(proc->pproc->in_queue_sem));

    ipcom_proc_free(proc);

    ipcom_mutex_unlock(ipcom_port.code_mutex);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_proc_find
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_proc *
ipcom_proc_find(Ip_pid_t pid, Ip_bool create)
{
    WIND_TCB *tcb;

    tcb = taskTcb(pid);

    ip_assert(tcb);
    ip_assert(tcb->IPCOM_TCB_MEMBER && !create); /* We should not create a Ipcom_proc here */

    return (Ipcom_proc *)tcb->IPCOM_TCB_MEMBER;
}


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
ipcom_proc_attr_init(Ipcom_proc_attr *attr)
{
    attr->priority = IPCOM_PRIORITY_DEFAULT;
    attr->stacksize = IPCOM_PROC_STACK_DEFAULT;
    attr->flags = 0;
    IP_CPU_ZERO(&attr->cpuset);
}


/*
 *===========================================================================
 *                    ipcom_proc_init
 *===========================================================================
 * Description:	Called when IPCOM process is started.
 * Parameters:	None.
 * Returns:	Nothing.
 *
 */
IP_PUBLIC void
ipcom_proc_init(void)
{
    (void)ipcom_proc_self();
}


/*
 *===========================================================================
 *                    ipcom_proc_self
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipcom_proc *
ipcom_proc_self(void)
{
    Ip_pid_t    pid;
    WIND_TCB   *tcb;
    Ipcom_proc *proc;

    tcb = taskTcb(0);
    pid = (Ip_pid_t)tcb;

    ip_assert(tcb);

    proc = (Ipcom_proc *)tcb->IPCOM_TCB_MEMBER;

    if (IP_UNLIKELY(proc == IP_NULL))
    {
        proc = ipcom_proc_malloc(pid, sizeof(Ipcom_pproc));
        if (proc == IP_NULL)
            return IP_NULL;
        ipcom_sem_create(&(proc->pproc->in_queue_sem), 0); /*lint !e534*/
        ipcom_list_init(&(proc->pproc->ipc_in_queue));
#if IP_PORT_VXWORKS >= 65
        tcb->IPCOM_TCB_MEMBER = (void *)proc;
#else
        tcb->IPCOM_TCB_MEMBER = (int)proc;
#endif
    }

    return proc;
}


/*
 *===========================================================================
 *                    ipcom_proc_yield(void)
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_proc_yield(void)
{
    if (taskDelay(0) == OK)
        return IPCOM_SUCCESS;
    else
        return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_vxworks_task_del_hook
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_vxworks_task_del_hook(WIND_TCB *tcb)
{
    Ipcom_proc *proc;

    IPCOM_STATS(task_del_hook++);

    if (tcb->IPCOM_TCB_MEMBER)
    {
        IPCOM_STATS(task_del_hook_proc_found++);
        proc = (Ipcom_proc *)tcb->IPCOM_TCB_MEMBER;
        ipcom_vxworks_proc_free(proc); /*lint !e534*/
        tcb->IPCOM_TCB_MEMBER = IP_NULL;
    }

#if defined(_WRS_CONFIG_RTP) && defined(IPNET)
    /*
     * Decrease reference count if an RTP task is deleted while being
     * blocked on a socket. This is indicated by a non-zero reserved
     * field in the WIND_TCB. Note that the reserved field is set in
     * the RTP socket library, ahead of incrementing the reference
     * count in the stack. We therefore check that the reference count
     * isn't decremented below 1, which happens when the task is killed
     * while in the lib, but before/after the actual block.
     * Note that there is a potential problem when another task is
     * accessing the same socket while this task is in limbo as indi-
     * cated above. This situation would erroneously decrement the ref
     * count, which probably affects operations on the socket.
     */
    {
        int fd;
#ifdef RESERVE_OLDSTYLE
        fd = tcb->reserved2;
#else
        fd = taskReservedFieldGet((int)tcb,
                                  ipcom_rtp_tcb_index_get_fd());
#endif
        if (fd >= IPNET_SOCKET_STARTFD && fd < ipnet_conf_max_sockets)
        {
            Ipnet_socket *sock = ipnet->sockets[fd];

            if (ipcom_atomic_get(&sock->ref_count) > 1)
            {
                ipcom_atomic_dec(&sock->ref_count);
            }
        }
    }
#endif
}


/*
 *===========================================================================
 *                    ipcom_getprio
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u16
ipcom_proc_getprio(Ip_pid_t pid)
{
    int  prio;
    int  i;

    (void)taskPriorityGet((int)pid, &prio);

    /* Find 'best match' priority. */
    for (i = 0; i <= IPCOM_PRIORITY_MAX; i++)
        if (prio >= (int)ipcom_priority_map[i])
            return (Ip_u16)i;

    /* Highest priority. */
    return IPCOM_PRIORITY_MAX;
}


/*
 *===========================================================================
 *                    ipcom_setprio
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_proc_setprio(Ip_pid_t pid, Ip_u16 prio)
{
    if (prio < IPCOM_PRIORITY_MIN || prio > IPCOM_PRIORITY_MAX)
        return IPCOM_ERR_FAILED;

    (void)taskPrioritySet((int)pid, (int)ipcom_priority_map[prio]);  /*!!check retval. */

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                            ipcom_get_cpuid
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_get_cpuid(void)
{
    return vxCpuIndexGet();
}


/*
 *===========================================================================
 *                    ipcom_num_configured_cpus
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_num_configured_cpus(void)
{
    return vxCpuConfiguredGet();
}


/*
 *===========================================================================
 *                    ipcom_num_online_cpus
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_num_online_cpus(void)
{
    cpuset_t cpuset_native;
    int      i, num = 0;

    cpuset_native = vxCpuEnabledGet();
    for (i=0; i<IP_CPU_SETSIZE; i++)
        if (CPUSET_ISSET(cpuset_native, i))
            num++;
    return num;
}


/*
 *===========================================================================
 *                    ipcom_cpu_is_online
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_bool
ipcom_cpu_is_online(int cpu)
{
    cpuset_t cpuset_native;

    cpuset_native = vxCpuEnabledGet();

    if (CPUSET_ISSET(cpuset_native, cpu))
        return IP_TRUE;
    else
        return IP_FALSE;
}

/*
 *===========================================================================
 *                    ipcom_cpu_reserved_alloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_cpu_reserved_alloc(void)
{
#ifdef IPCOM_USE_SMP
    cpuset_t cpuset_native;

    CPUSET_ZERO(cpuset_native);
    if (vxCpuReserve(cpuset_native, &cpuset_native) == OK)
    {
        return CPUSET_FIRST_INDEX(cpuset_native);
    }
#endif
    return -1;
}


/*
 *===========================================================================
 *                    ipcom_cpu_reserved_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_cpu_reserved_free(int cpu)
{
#ifdef IPCOM_USE_SMP
    cpuset_t cpuset_native;

    CPUSET_ZERO(cpuset_native);
    CPUSET_SET(cpuset_native, cpu);
    if (vxCpuUnreserve(cpuset_native) == OK)
        return IPCOM_SUCCESS;
#endif
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_proc_cpu_affinity_clr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_proc_cpu_affinity_clr(Ip_pid_t pid)
{
    Ip_cpu_set_t cpuset;

    IP_CPU_ZERO(&cpuset);
    return ipcom_proc_cpu_affinity_set(pid, &cpuset);
}


/*
 *===========================================================================
 *                    ipcom_proc_cpu_affinity_set
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_proc_cpu_affinity_set(Ip_pid_t pid, Ip_cpu_set_t *cpuset)
{
    cpuset_t cpuset_native;
    int      i;

    CPUSET_ZERO(cpuset_native);
    for (i=0; i<IP_CPU_SETSIZE; i++)
    {
        if (IP_CPU_ISSET(i, cpuset))
        {
            /* Check that CPUs in the set are online */
            if (ipcom_cpu_is_online(i) == IP_FALSE)
            {
                return IPCOM_ERR_FAILED;
            }
            CPUSET_SET(cpuset_native, i);
        }
    }

    if (pid == 0)
        pid = ipcom_getpid();

    if (taskCpuAffinitySet (pid, cpuset_native) == ERROR)
    {
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_proc_cpu_affinity_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_proc_cpu_affinity_get(Ip_pid_t pid, Ip_cpu_set_t *cpuset)
{
    cpuset_t cpuset_native;
    int      i;

    if (pid == 0)
        pid = ipcom_getpid();
    if (taskCpuAffinityGet (pid, &cpuset_native) == ERROR)
        return IPCOM_ERR_FAILED;

    IP_CPU_ZERO(cpuset);
    for (i=0; i<IP_CPU_SETSIZE; i++)
        if (CPUSET_ISSET(cpuset_native, i))
            IP_CPU_SET(i, cpuset);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_proc_cpu_affinity_set_all
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
#define IPCOM_MAX_TASKS_ALL 256
IP_PUBLIC Ip_err
ipcom_proc_cpu_affinity_set_all(Ip_cpu_set_t *cpuset)
{
    int i, num, *task_ids;
    TASK_DESC td;

    task_ids = ipcom_malloc(IPCOM_MAX_TASKS_ALL * sizeof(*task_ids));
    if (task_ids == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;

    num = taskIdListGet(task_ids, IPCOM_MAX_TASKS_ALL);
    taskIdListSort(task_ids, num);
    for (i=0; i<num; i++)
    {
	    if (taskInfoGet(task_ids[i], &td) == OK)
        {
            /* Skip idle tasks */
            if (ipcom_strncmp(td.td_name, "tIdleTask", ipcom_strlen("tIdleTask")) == 0)
                continue;
            if (ipcom_proc_cpu_affinity_set(task_ids[i], cpuset) != IPCOM_SUCCESS)
            {
                IPCOM_LOG2(ERR,
                           "ipcom_proc_cpu_affinity_set_all() :: failed to set affinity to 0x%x for task %s",
                           cpuset->set[0], td.td_name);
            }
            else
            {
                IPCOM_LOG2(DEBUG,
                           "ipcom_proc_cpu_affinity_set_all() :: affinity is set to 0x%x for task %s",
                           cpuset->set[0], td.td_name);
            }
		}
    }

    ipcom_free(task_ids);
    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipcom_mb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_mb(void)
{
#ifdef IPCOM_USE_SMP
    VX_MEM_BARRIER_RW();
#endif
}


/*
 *===========================================================================
 *                    ipcom_rmb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_rmb(void)
{
#ifdef IPCOM_USE_SMP
    VX_MEM_BARRIER_R();
#endif
}


/*
 *===========================================================================
 *                    ipcom_wmb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_wmb(void)
{
#ifdef IPCOM_USE_SMP
    VX_MEM_BARRIER_W();
#endif
}


/*
 *===========================================================================
 *                    ipcom_ipc_install
 *===========================================================================
 * Description:  Install the server end of an IPC channel.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipc_install(void)
{
    Ipcom_proc *proc;
    char *task_name;

    ipcom_mutex_lock(ipcom_port.code_mutex);

    IPCOM_STATS(ipc_install++);

    /* Tasks using IPC must have an Ipcom_proc entry */
    proc = ipcom_proc_self();
    ip_assert(proc);
    if (proc == IP_NULL)
    {
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_ERR_FAILED;
    }

    if (proc->pproc->ipc_installed == IP_TRUE)
    {
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_ERR_ALREADY_INSTALLED;
    }

    task_name = taskName(taskIdSelf());
    ip_assert(task_name);

    /* Check if we have any IPC clients waiting for this server */
    delete_open_q_2(task_name);
    proc->pproc->ipc_installed = IP_TRUE;

    ipcom_mutex_unlock(ipcom_port.code_mutex);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_ipc_uninstall
 *===========================================================================
 * Description:  Uninstall the server end of an IPC channel.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipc_uninstall(void)
{
    Ipcom_proc *proc;
    char       *task_name;
    Ip_err rc = IPCOM_SUCCESS;

    ipcom_mutex_lock(ipcom_port.code_mutex);

    IPCOM_STATS(ipc_uninstall++);

    proc = ipcom_proc_self();
    ip_assert(proc);
    if (proc == IP_NULL)
    {
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_ERR_FAILED;
    }

    if (proc->pproc->ipc_installed == IP_FALSE)
    {
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_ERR_NOT_INSTALLED;
    }

    ipcom_ipc_free_inqueue_msgs(proc);
    task_name = taskName(taskIdSelf());
    ip_assert(task_name);
    if (delete_open_q_2(task_name) != IPCOM_SUCCESS)
        rc = IPCOM_ERR_FAILED;

    proc->pproc->ipc_installed = IP_FALSE;

    ipcom_mutex_unlock(ipcom_port.code_mutex);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_ipc_open
 *===========================================================================
 * Description:  Open the client end of an IPC channel.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipc_open(Ipcom_ipc *ipc, const char *name, Ip_s32 ms)
{ /*lint !e18 !e516*/
    Ipcom_proc   *proc;
    Ipc_open_q   *open_q;
    WIND_TCB     *tcb;
    int           task_id;
    const char   *orgname = name;
#if IPCOM_VR_MAX > 1
    int  vr = ipcom_proc_vr_get();
    char procname[IPCOM_PROC_NAME_MAX+1];

    if (vr != 0)
    {
        ipcom_sprintf(procname, "%s#%d" ,name, vr);
        name = procname;
    }
#endif /* IPCOM_VR_MAX */

    ip_assert(ms >= -1);
    ip_assert(ipcom_strlen(name) <= IPCOM_IPC_MAX_NAME-1);

    ipcom_mutex_lock(ipcom_port.code_mutex);

    IPCOM_STATS(ipc_open++);

    /* Tasks using IPC must have an Ipcom_proc entry */
    proc = ipcom_proc_self();
    ip_assert(proc);
    if (proc == IP_NULL)
    {
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_ERR_FAILED;
    }

    task_id = taskNameToId((char *)name);
    if (task_id == ERROR)
        proc = IP_NULL;
    else
    {
        tcb = taskTcb(task_id);
        ip_assert(tcb);
        proc = (Ipcom_proc *)tcb->IPCOM_TCB_MEMBER;
    }

    if (proc == IP_NULL)
    {
        int ret;
        /* IPC server has not been installed yet. Create Ipc_open_q entry and wait for
         * IPC server, it will post the semaphore once it has been installed */

        IPCOM_STATS(ipc_open_before_install++);

        open_q = find_open_q(name);
        if (open_q == IP_NULL)
        {
            open_q = ipcom_malloc(sizeof(Ipc_open_q));
            if (open_q == IP_NULL)
            {
                ipcom_mutex_unlock(ipcom_port.code_mutex);
                return IPCOM_ERR_FAILED;
            }
            ipcom_memset(open_q, 0, sizeof(Ipc_open_q));

            if (ipcom_block_create(&open_q->open_queue_block) != IPCOM_SUCCESS)
            {
                ipcom_free(open_q);
                ipcom_mutex_unlock(ipcom_port.code_mutex);
                return IPCOM_ERR_FAILED;
            }

            if (ipcom_sem_create(&(open_q->open_queue_sem), 0) != IPCOM_SUCCESS)
            {
                ipcom_block_delete(open_q->open_queue_block);
                ipcom_free(open_q);
                ipcom_mutex_unlock(ipcom_port.code_mutex);
                return IPCOM_ERR_FAILED;
            }

            ipcom_strcpy(open_q->ipc_srv_name, name);
            ipcom_list_insert_last(&ipcom_port.open_q_head, (Ipcom_list *)open_q);
        }

        /* Up the reference count, one more item has referenced this QUEUE */
        open_q->open_queue_size++;
        ipcom_mutex_unlock(ipcom_port.code_mutex);

        /* Take the master LOCK. We wont be allowed to do this until the
         * QUEUE item is to be destroyed */
        ret = ipcom_block_wait(open_q->open_queue_block, ms);

        /* Indicate that we're out of any volatile data */
        ipcom_sem_post(open_q->open_queue_sem);

        /* If we claimed the mutex, return it. This is the
         * last synchronization required; the 'deleting'
         * task will claim this mutex prior to deleting
         * it; ensuring that we're all out of the code
         * referencing 'to be deleted' data */
        if (ret == IPCOM_SUCCESS)
            ipcom_block_post(open_q->open_queue_block);

        /* If it all went OK, do open the IPC channel */
        if (ret == OK)
            return ipcom_ipc_open(ipc, orgname, ms);

        /* Failure */
        return IPCOM_ERR_FAILED;
    }

    ipc->magic = IPCOM_IPC_MAGIC;
    ipc->peer_pid = proc->pid;
    ipcom_mutex_unlock(ipcom_port.code_mutex);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_ipc_close
 *===========================================================================
 * Description:	 Close IPC channel opened by ipcom_ipc_open() or implicitly
 *               opened by ipcom_ipc_receive().
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipc_close(Ipcom_ipc *ipc) /*lint !e18*/
{ /*lint !e18 !e516*/
    IPCOM_STATS(ipc_close++);

    ipc->magic = 0;
    ipc->peer_pid = 0xffffffff;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_ipc_isopen
 *===========================================================================
 * Description:	 Check if 'ipc' is valid.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_bool
ipcom_ipc_isopen(Ipcom_ipc *ipc)
{/*lint !e18 !e516*/
    return (ipc->magic == IPCOM_IPC_MAGIC);
}


/*
 *===========================================================================
 *                    ipcom_ipc_malloc
 *===========================================================================
 * Description:	 Allocate an IPC buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_ipc_malloc(Ip_size_t size)
{
    Ipc_msg *ipc_msg;

    ipc_msg = ipcom_malloc(size + ip_offsetof(Ipc_msg, data));
    if (ipc_msg == IP_NULL)
        return IP_NULL;

    ipc_msg->magic = IPCOM_IPC_MSG_MAGIC;
    ipc_msg->size  = size;
    ipc_msg->sender_pid = (Ip_pid_t)-1; /*lint !e570*/

    return ipc_msg->data;
}


/*
 *===========================================================================
 *                    ipcom_ipc_free
 *===========================================================================
 * Description:	 Free an IPC buffer.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_ipc_free(void *buf)
{
    Ipc_msg *ipc_msg;

    if (buf == IP_NULL)
        return;

    ipc_msg = ipcom_ipc_msg_from_buf(buf);

    ip_assert(ipc_msg->magic == IPCOM_IPC_MSG_MAGIC);

    ipc_msg->size  = 0;
    ipc_msg->magic = 0xdeadbeef;

    ipcom_free(ipc_msg);
}


/*
 *===========================================================================
 *                    ipcom_ipc_size
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_ipc_size(void *buf)
{
    Ipc_msg *ipc_msg;

    if (buf == IP_NULL)
        return (Ip_size_t)-1; /*lint !e570*/

    ipc_msg = ipcom_ipc_msg_from_buf(buf);

    ip_assert(ipc_msg->magic == IPCOM_IPC_MSG_MAGIC);

    return ipc_msg->size;
}


/*
 *===========================================================================
 *                    ipcom_ipc_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipc_send(Ipcom_ipc *ipc, void *buf)
{ /*lint !e18 !e516*/
    Ipc_msg *ipc_msg;
    Ipcom_proc *proc;
    Ipcom_ipc tmp_ipc;
    Ip_err rc;

    if (buf == IP_NULL)
        return IPCOM_ERR_INVALID_ARG;

    ipc_msg = ipcom_ipc_msg_from_buf(buf);

    if (ipc == IPCOM_IPC_SENDER)
    {
        ipc = &tmp_ipc;
        ipc->magic = IPCOM_IPC_MAGIC;
        ipc->peer_pid = ipc_msg->sender_pid;
    }

    ip_assert(ipc->magic == IPCOM_IPC_MAGIC);
    ip_assert(ipc_msg->magic == IPCOM_IPC_MSG_MAGIC);

    ipcom_mutex_lock(ipcom_port.code_mutex);

    IPCOM_STATS(ipc_send++);

    proc = ipcom_proc_find(ipc->peer_pid, IP_FALSE);
    if (proc == IP_NULL)
    {
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_ERR_FAILED;
    }

    ipc_msg->sender_pid = ipcom_getpid();
    ipcom_list_insert_last(&(proc->pproc->ipc_in_queue), (Ipcom_list *)ipc_msg);
    rc = ipcom_sem_post(proc->pproc->in_queue_sem);

    ipcom_mutex_unlock(ipcom_port.code_mutex);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_ipc_receive
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipc_receive(Ipcom_ipc *ipc, void *buf, Ip_s32 ms)
{ /*lint !e18 !e516*/
    Ipc_msg *ipc_msg;
    Ipcom_proc *proc;
    int ticks;

    ipcom_mutex_lock(ipcom_port.code_mutex);

    IPCOM_STATS(ipc_receive++);

    proc = ipcom_proc_find(ipcom_getpid(), IP_FALSE);
    ip_assert(proc);
    if (proc == IP_NULL)
    {
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_ERR_FAILED;
    }

    ticks = millisec2tick(ms);

    ipcom_mutex_unlock(ipcom_port.code_mutex);
    if (semTake(proc->pproc->in_queue_sem, ticks) == OK)
    {
        ipcom_mutex_lock(ipcom_port.code_mutex);
        ipc_msg = (Ipc_msg *)ipcom_list_first(&(proc->pproc->ipc_in_queue));
        ip_assert(ipc_msg);
        ip_assert(ipc_msg->magic == IPCOM_IPC_MSG_MAGIC);
        ipcom_list_remove(&(ipc_msg->list));
        *(void **)buf = ipc_msg->data;
        if (ipc)
        {
            ipc->peer_pid = ipc_msg->sender_pid;
            ipc->magic = IPCOM_IPC_MAGIC;
        }
        ipcom_mutex_unlock(ipcom_port.code_mutex);
        return IPCOM_SUCCESS;
    }
    else /* Timeout */
        return IPCOM_ERR_TIMEOUT;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
