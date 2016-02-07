/* ipcom_os.h - API of IPCOM OS Abstraction Layer */

/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_os.h,v $ $Revision: 1.84 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_os.h,v $
*     $Author: kenneth $ $Date: 2009-07-02 11:16:47 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*
******************************************************************************
*/
#ifndef IPCOM_OS_H
#define IPCOM_OS_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for IPCOM Abstraction layer
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

#include "ipcom_cstyle.h"
#include "ipcom_pipe.h"
#include "ipcom_type.h"
#include "ipcom_wait_queue.h"
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
#include "ipcom_hash.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    PROCESS - PROC
 *===========================================================================
 */

/* Max process name length */
#define IPCOM_PROC_NAME_MAX        64

/* Priorities: */
#define IPCOM_PRIORITY_BACKGROUND  0
#define IPCOM_PRIORITY_MIN         1
#define IPCOM_PRIORITY_DEFAULT     4
#define IPCOM_PRIORITY_MAX         7

/* Priority map prototype: */
IP_EXTERN IP_CONST Ip_s16 ipcom_priority_map[IPCOM_PRIORITY_MAX+1];

/* CPU Affinity - must be less than or equal to CPU_SETSIZE in the OS */
#define IP_CPU_SETSIZE             32

/* Access macros for Ip_cpu_set_t */
#define IP_CPU_ZERO(cpusetp)        ipcom_memset(cpusetp, 0, sizeof(Ip_cpu_set_t));
#define IP_CPU_CLR(cpu, cpusetp)    ((cpusetp)->set[(cpu) >> 5] &= ~(1 << ((cpu) & 31)))
#define IP_CPU_SET(cpu, cpusetp)    ((cpusetp)->set[(cpu) >> 5] |= 1 << ((cpu) & 31))
#define IP_CPU_ISSET(cpu, cpusetp)  (0 != ((cpusetp)->set[(cpu) >> 5] & (1 << ((cpu) & 31))))


/*
 *===========================================================================
 *                    SEM/MUTEX/BLOCK INVALID
 *===========================================================================
 */
#define IPCOM_MUTEX_INVALID       IP_NULL
#define IPCOM_SEM_INVALID         IP_NULL
#define IPCOM_BLOCK_INVALID       IP_NULL


/*
 *===========================================================================
 *                    IPCOM_ONCE_INIT
 *===========================================================================
 */
#define IPCOM_ONCE_INIT {IP_NULL, IPCOM_ONCE_STATE_UNINITIALIZED}


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Mutex type */
typedef void * Ipcom_mutex;

/* Semaphore type */
typedef void * Ipcom_sem;

/* Block type  */
typedef void * Ipcom_block;


/* Generic IPC type */
#ifndef IPCOM_IPC_TYPE_SIZE
#error IPCOM_IPC_TYPE_SIZE_must_be_set_to_size_of_port_Ipcom_ipc_in_number_of_32bits
#endif
#ifndef IPCOM_IPC_T_TYPE
#define IPCOM_IPC_T_TYPE
typedef struct Ipcom_ipc_struct
{
    /* Force alignment */
    union {void *v; Ip_u32 l;} align[IPCOM_IPC_TYPE_SIZE];
}
Ipcom_ipc;
#endif

/* Process function pointer type. */
#ifdef IP_PORT_LKM
typedef int (*Ipcom_proc_func)(void* d);
#else
typedef void (*Ipcom_proc_func)(void);
#endif

/* preemption locking key */

typedef Ip_u32 Ip_preempt_key;

/*
 *===========================================================================
 *                         Ipcom_env_entry
 *===========================================================================
 */
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
typedef struct Ipcom_env_entry_struct
{
    const char  *name;
    const char  *value;
    /* Memory for 'name' and 'value' follows. */
}
Ipcom_env_entry;
#endif /* IPCOM_USE_ENV == IPCOM_ENV_IPCOM */


/*
 *===========================================================================
 *                         Ipcom_once_t
 *===========================================================================
 */
enum Ipcom_once_state {
    IPCOM_ONCE_STATE_UNINITIALIZED,
    IPCOM_ONCE_STATE_RUNNING,
    IPCOM_ONCE_STATE_DONE
};

typedef struct Ipcom_once_struct
{
    Ipcom_wait_queue     *wq;
    enum Ipcom_once_state state;
}
Ipcom_once_t;


/*
 *===========================================================================
 *                         Ip_cpu_set_t
 *===========================================================================
 */
typedef struct Ip_cpu_set_t_struct
{
    Ip_u32 set[(IP_CPU_SETSIZE + sizeof(Ip_u32) * 8 - 1) / (sizeof(Ip_u32) * 8)];
}
Ip_cpu_set_t;


/*
 *===========================================================================
 *                         Ipcom_proc
 *===========================================================================
 */
struct Ipcom_pproc_struct;  /* dummy */
#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
struct Ipcom_file_proc_struct;  /* dummy */
#endif

typedef struct Ipcom_proc_struct
{
#ifndef IP_PORT_LKM
    /* Process ID - PID */
    Ip_pid_t        pid;         /* process ID */

    Ipcom_block     wakeup_signal; /* Signal that this process will sleep on
                                      when put to sleep on a wait queue */
#ifdef IP_DEBUG
    void           *wq;          /* The wait queue this process is sleeping on
                                      or IP_NULL if its not in a wait queue a.t.m */
#endif

    /* IPCOM shell */
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM || IPCOM_USE_SHELL == IPCOM_SHELL_MINI
    Ip_fd           shell_fd;    /* Shell command stdio socket */
#endif
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    Ip_pid_t        shell_ppid;  /* Shell parent pid. */
    Ip_u32          shell_seqno;  /* Shell parent sequence number. */
#endif

    /* IPCOM file system */
#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
    struct Ipcom_file_proc_struct   *fileinfo;
#endif

    /* Environment variables */
#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
    Ipcom_once_t                env_once;
    Ipcom_hash                 *env_tree;
    Ipcom_mutex                 env_mutex;
#endif

    /* Used to port programs with global variables to IPCOM */
    void                       *globals;

    /* Communication pipe for this process. Will be IP_NULL for
       process that does not listen on a pipe */
    Ipcom_pipe                 *pipe;

    /* OS port specific variables. If IP_NULL, none exist. */
    struct Ipcom_pproc_struct  *pproc;
#else
    /* Proc parameters used by the LKM port */

    /* Communication pipe for this process. Will be IP_NULL for
       process that does not listen on a pipe */
    Ipcom_pipe                 *pipe;

#endif /* IP_PORT_LKM */
}
Ipcom_proc;


/*
 *===========================================================================
 *                         Ipcom_proc_attr
 *===========================================================================
 */
typedef struct Ipcom_proc_attr_struct
{
    Ip_s32       priority;		/* Process priority. See ipcom_pconfig.h */
    Ip_u32       stacksize;		/* Stacksize. See ipcom_pconfig.h */
    Ip_u32       flags;
    Ip_cpu_set_t cpuset;        /* CPU affinity */
}
Ipcom_proc_attr;

/* Inherit attributes from creator. */
#define IPCOM_PROC_PRIO_INHERIT    (IPCOM_PRIORITY_MAX + 1)
#define IPCOM_PROC_STACK_INHERIT   0

#define IPCOM_PROC_FLAG_FP       (1)       /* Set if task will use floating point operations */


/*
 ****************************************************************************
 * X                    ISR
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_interrupt_disable - disable all interrupts
*
* This routine disables all interrupts. It returns the current interrupt status
* register to pass to the ipcom_interrupt_enable() routine in order to restore
* the system to its previous state.
*
* RETURNS: The current interrupt status register.
*
* ERRNO: None.
*/
#if defined(IP_DEBUG) && (defined(IP_PORT_UNIX) || defined(IP_PORT_OSE5) || defined(IP_PORT_OSE))
IP_PUBLIC Ip_u32 ipcom_dinterrupt_disable
(
    const char *file,
    const char *function,
    Ip_u32 line
);
#define ipcom_interrupt_disable() ipcom_dinterrupt_disable(__FILE__, __FUNCTION__, __LINE__)
#else
IP_FASTTEXT IP_PUBLIC Ip_u32 ipcom_interrupt_disable
(
    void
);
#endif /* IP_DEBUG && */


/*******************************************************************************
*
* ipcom_interrupt_enable - enable interrupts
*
* This routine enables interrupts.
*
* The return value from the ipcom_interrupt_disable() routine must be passed to
* ipcom_interrupt_enable() in order to restore the system to its previous state.
*
* Parameter:
* \is
* \i <old>
* The interrupt status register returned from a previous call to the
* ipcom_interrupt_disable() routine.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC void ipcom_interrupt_enable
(
    Ip_u32 old
);


/*
 ****************************************************************************
 *                        PREEMPTION LOCKING
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_preempt_disable - disable preemption
*
* This routine disables preemption of the current kernel thread by
* other threads.  It does not prevent interrupts for all IPCOM ports.
* It returns a value to be passed to the ipcom_preempt_enable() routine in
* order in order to restore the system to its previous state.  (Not all ports
* will need to make use of the value.)
*
* After calling ipcom_preempt_disable(), the stack will always call
* ipcom_preempt_enable() before making another call to ipcom_preempt_disable(),
* so the port implementation need not support nested use of these routines.
*
* RETURNS: An opaque value to be passed to ipcom_preempt_enable().
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC Ip_preempt_key ipcom_preempt_disable
(
    void
);


/*******************************************************************************
*
* ipcom_preempt_enable - enable interrupts
*
* This routine re-enables preemption for the current kernel thread after
* a call to ipcom_preempt_disable().  The return value from the
* ipcom_preempt_disable() routine, called in the same thread, must be passed
* to ipcom_preempt_enable() in order to restore the system to its previous state.
* Since the network stack does not make use of nested calls to these routines,
* the previous state may be assumed to be the state with preemption enabled.
*
* Parameter:
* \is
* \i <old>
* The value returned from a previous call to the ipcom_preempt_disable() routine.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC void ipcom_preempt_enable
(
    Ip_preempt_key old
);


/*
 ****************************************************************************
 * X                    MUTEX
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_mutex_create - create and initialize an IPCOM mutex
*
* This routine creates and initializes an IPCOM mutex. If the system call is
* successful, <mutex> points to an initialized, port-specific mutex handle for
* use in mutex system calls to identify a specific mutex.
*
* The mutex handle should be released by calling ipcom_mutex_delete() when no
* longer used.
*
* Parameter:
* \is
* \i <mutex>
* Pointer to <mutex> handle.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_mutex_create
(
    Ipcom_mutex *mutex
);
#if defined(IPCOM_USE_DMALLOC) && defined(IP_PORT_UNIX)
IP_PUBLIC Ip_err ipcom_dmutex_create(Ipcom_mutex *mutex, const char *file, const char *function, Ip_u32 line);
#define ipcom_mutex_create(m)  ipcom_dmutex_create(m,__FILE__, __FUNCTION__, __LINE__)
#endif


/*******************************************************************************
*
* ipcom_mutex_delete - delete a mutex
*
* This routine deletes a mutex created by ipcom_mutex_create().
*
* Parameter:
* \is
* \i <mutex>
* Pointer to <mutex> handle.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_mutex_delete
(
    Ipcom_mutex *mutex
);


/*******************************************************************************
*
* ipcom_mutex_lock - lock a mutex
*
* This routine locks a mutex, which must not be owned by the calling process.
*
* Parameter:
* \is
* \i <mutex>
* <Mutex> handle.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC void ipcom_mutex_lock
(
    Ipcom_mutex mutex
);


/*******************************************************************************
*
* ipcom_mutex_unlock - unlock a mutex
*
* This routine unlocks a mutex, which must be owned by the calling process.
*
* Parameter:
* \is
* \i <mutex>
* <Mutex> handle.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC void ipcom_mutex_unlock
(
    Ipcom_mutex mutex
);


/*
 ****************************************************************************
 * X                    SEMAPHORE
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_sem_create - create a semaphore and initialize its counter
*
* This routine creates a semaphore and initializes its counter to <value>. A
* semaphore handle is obtained in the location pointed to by <sem>.
*
* Note that it is invalid to initialize the semaphore to a negative value. The
* only way for the counter to become negative is when processes are blocked on
* the semaphore.
*
* Parameters:
* \is
* \i <sem>
* Pointer to a location to store the semaphore handle in.
* \i <value>
* Initial <value> of the semaphore.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_sem_create
(
    Ipcom_sem *sem,
    Ip_s32 value
);
#if defined(IPCOM_USE_DMALLOC) && defined(IP_PORT_UNIX)
IP_PUBLIC Ip_err ipcom_dsem_create(Ipcom_sem *sem, Ip_s32 value, const char *file, const char *function, Ip_u32 line);
#define ipcom_sem_create(s,v)   ipcom_dsem_create(s,v,__FILE__, __FUNCTION__, __LINE__)
#endif


/*******************************************************************************
*
* ipcom_sem_delete - delete a semaphore
*
* This routine deletes a semaphore.
*
* Parameter:
* \is
* \i <sem>
* Pointer to semaphore handle.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_sem_delete
(
    Ipcom_sem *sem
);


/*******************************************************************************
*
* ipcom_sem_getvalue - get the value of a semaphore counter
*
* This routine gets the value of a semaphore counter. A negative value indicates
* the number of processes blocked on the semaphore.
*
* Parameter:
* \is
* \i <sem>
* Semaphore handle.
* \ie
*
* RETURNS: The value of the semaphore.
*
* ERRNO: None.
*/
IP_PUBLIC Ip_s32 ipcom_sem_getvalue
(
    Ipcom_sem sem
);


/*******************************************************************************
*
* ipcom_sem_wait - decrement the semaphore sem
*
* This routine decrements the semaphore <sem> and blocks, if the value of <sem>
* becomes negative.
*
* Parameter:
* \is
* \i <sem>
* Semaphore handle.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC Ip_err ipcom_sem_wait
(
    Ipcom_sem sem
);


/*******************************************************************************
*
* ipcom_sem_post - increment the semaphore sem
*
* This routine increments the semaphore sem. It can only be called in a
* non-interrupt context. To increment a semaphore in an interrupt context, call
* ipcom_sem_interrupt_post.
*
* If any processes are blocked on the semaphore when ipcom_sem_post() is called,
* one of the processes is allowed to return from ipcom_sem_wait(). The order in
* which processes are released is implementation defined. Priority queuing
* generally provides the best real-time behavior, but FIFO queuing often has the
* least overhead.
*
* Parameter:
* \is
* \i <sem>
* Semaphore handle.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC Ip_err ipcom_sem_post
(
    Ipcom_sem sem
);


/*******************************************************************************
*
* ipcom_sem_interrupt_post - increment the semaphore sem
*
* This routine increments the semaphore sem. It can only be called in an
* interrupt context. To increment a semaphore in a non-interrupt context, call
* ipcom_sem_post().
*
* If any processes are blocked on the semaphore when ipcom_sem_interrupt_post()
* is called, one of the processes is allowed to return from ipcom_sem_wait().
* The order in which processes are released is implementation defined. Priority
* queuing generally provides the best real-time behavior, but FIFO queuing often
* has the least overhead.
*
* Parameter:
* \is
* \i <sem>
* Semaphore handle.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO: None.
*/
IP_FASTTEXT IP_PUBLIC Ip_err ipcom_sem_interrupt_post
(
    Ipcom_sem sem
);

IP_FASTTEXT IP_PUBLIC Ip_err ipcom_sem_flush
(
    Ipcom_sem sem
);

IP_FASTTEXT IP_PUBLIC Ip_err ipcom_sem_interrupt_flush
(
    Ipcom_sem sem
);


/*
 ****************************************************************************
 * X                    BLOCK
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_block_create - create a block object and store its handle
*
* This routine creates a block object, and stores its handle in the <block>
* parameter.
*
* Parameter:
* \is
* \i <block>
* Pointer to <block> handle.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS' or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_block_create
(
    Ipcom_block *block
);

#if defined(IPCOM_USE_DMALLOC) && defined(IP_PORT_UNIX)
IP_PUBLIC Ip_err ipcom_dblock_create(Ipcom_block *block, const char *file, const char *function, Ip_u32 line);
#define ipcom_block_create(b) ipcom_dblock_create(b, __FILE__, __FUNCTION__, __LINE__)
#endif


/*******************************************************************************
*
* ipcom_block_delete - delete a block object
*
* This routine deletes a block object.
*
* Parameter:
* \is
* \i <block>
* Pointer to <block> handle.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS' or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_block_delete
(
    Ipcom_block *block
);


/*******************************************************************************
*
* ipcom_block_wait - wait for a post operation
*
* Wait for a post operation on the block object. If there already has been a
* post operation, the calling process returns immediately. Otherwise, the
* calling process waits for the number of milliseconds specified in the <msec>
* parameter.
*
* Parameters:
* \is
* \i <block>
* Block handle.
* \i <msec>
* Timeout, in milliseconds, to wait for a post operation. The macro
* 'IPC_TIMEOUT_POLL' causes an immediate return from the function; the macro
* 'IPC_TIMEOUT_INFINITE' makes the function wait forever.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS,' if system call is successful, otherise, an error
* value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_TIMEOUT','IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_block_wait
(
    Ipcom_block block,
    Ip_s32 milli_seconds
);


/*******************************************************************************
*
* ipcom_block_post - perform a post operation on the block
*
* This routine performs a post operation on the block. This means that if there
* are processes pending on the block, one of them is released. If there are no
* pending processes, the next call to ipcom_block_wait() returns immediately.
*
* The order in which processes are released from the block object is
* unspecified.
*
* Parameter:
* \is
* \i <block>
* Block handle.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS,' or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC void ipcom_block_post
(
    Ipcom_block block
);


/*
 ****************************************************************************
 * X                    PROC
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_proc_init - initialize a process to use IPCOM services
*
* This routine initializes a process to be able to use IPCOM services. It must
* be the first call in the process code. The routine ipcom_proc_exit() must be
* used to terminate the process.
*
* Parameters:
*
* None.
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_PUBLIC void ipcom_proc_init
(
    void
);


/*******************************************************************************
*
* ipcom_proc_exit - terminate a process and release its resources
*
* This routine terminates a process that has been initialized with
* ipcom_proc_init() and release all resources associated with it. The call does
* not return.
*
* Parameters:
*
* None.
*
* RETURNS: N/A
*
* ERRNO: None.
*/
#if !defined(IP_PORT_INTEGRITY) || !defined(INT_178B)
IP_PUBLIC void ipcom_proc_exit
(
    void
);
#endif


/*******************************************************************************
*
* ipcom_proc_create - create an IPCOM process
*
* This routine creates an IPCOM process. Its replacement routine,
* ipcom_proc_acreate(), should be used in all future programs. [* But
* ipcom_proc_create() can still be used in current programs?]
*
* Parameters:
* \is
* \i <name>
* Process name.
* \i <entry>
* Process entrypoint.
* \i <stacksz>
* Stack size.
* \i <pidp>
* Pointer to a location that gets the ID of the new process.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_proc_create
(
    const char *name,
#ifdef IP_PORT_LKM
    int (*entry)(void* data),
#else
    void (*entry)(void),
#endif
    Ip_u32 stacksz,
    Ip_pid_t *pidp
);


/*******************************************************************************
*
* ipcom_proc_acreate - create a new IPCOM process
*
* This routine creates a new IPCOM process with the characteristics specified in
* the attribute object <attr>. The process starts execution immediately and
* competes for execution time according to the scheduling rules of the
* underlying operating system.
*
* See ipcom_proc_attr_init() for information on how to set process attributes.
*
* Parameters:
* \is
* \i <name>
* The name of the new process.
* \i <entry>
* Entrypoint of the new process.
* \i <attr>
* Process attributes.
* \i <pidp>
* Pointer to a location which will get the ID of the new process.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_proc_acreate
(
    const char *name,
#ifdef IP_PORT_LKM
    int (*entry)(void* data),
#else
    void (*entry)(void),
#endif
    const Ipcom_proc_attr *attr,
    Ip_pid_t *pidp
);


/*******************************************************************************
*
* ipcom_proc_attr_init - initialize a process attribute object
*
* This routine initializes a process attribute object with default values. This
* must always be the first step in creating an attribute object for the
* ipcom_proc_acreate() call. Using this call ensures that future process
* attributes can be implemented without any IPCOM client source code
* modifications.
*
* Individual attributes can be set after this routine is called. The current set
* of attributes is:
* \is
* \i 'attr.priority'
* Process priority. IPCOM supports seven priorities, 0-7, where 0 is the lowest
* and 7 the highest. The priority range is centered around the priority of the
* IP stack, which is defined to have priority 4. This means that an IPCOM
* process with priority 4 always will be at the same priority level as the
* stack. The following macros should be used to make client code more readable:
*
* IPCOM_PRIORITY_BACKGROUND (0)
*
* IPCOM_PRIORITY_MIN (1)
*
* IPCOM_PRIORITY_DEFAULT (4)
*
* IPCOM_PRIORITY_MA (7)
* \i 'attr.stacksize'
* Process stacksize. IPCOM supports five stack sizes, defined to different
* values depending on OS, compiler and target hardware. This addresses the
* problem of writing generic code for different environments. The values are:
*
* IPCOM_PROC_STACK_MIN (256 for PPC)
*
* IPCOM_PROC_STACK_SMALL (2048 for PPC)
*
* IPCOM_PROC_STACK_DEFAULT (4096 for PPC)
*
* IPCOM_PROC_STACK_LARGE (8192 for PPC)
*
* IPCOM_PROC_STACK_MAX (16384 for PPC)
* \ie
*
* Parameter:
* \is
* \i <ap>
* Pointer to attribute object.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_PUBLIC void ipcom_proc_attr_init
(
    Ipcom_proc_attr *attr
);


/*
 *===========================================================================
 *                    ipcom_proc_yield
 *===========================================================================
 * Description:  Yield the processor voluntarily to another process. The
 *               calling process will then be moved to the end of scheduling
 *               queue for its priority. If there are no other processes with
 *               the same priority, the calling process will continue to
 *               execute.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_proc_yield(void);


/*******************************************************************************
*
* ipcom_proc_self - obtain a pointer to a set of process-specific variables
*
* This routine obtains a pointer to the set of process-specific variables for
* the calling process. The variables are used by IPCOM to store data associated
* only with one particular process. RTOS ports may also use this set of data for
* port-specific issues.
*
* Parameters:
*
* None.
*
* RETURNS: A pointer to the set of process-specific variables.
*
* ERRNO: None.
*/
IP_PUBLIC Ipcom_proc *ipcom_proc_self
(
    void
);


/*******************************************************************************
*
* ipcom_getpid - get the ID of the calling process
*
* This routine gets the ID of the calling process.
*
* Parameters:
*
* None.
*
* RETURNS: Process ID.
*
* ERRNO: None.
*/
IP_PUBLIC Ip_pid_t ipcom_getpid
(
    void
);


/*******************************************************************************
*
* ipcom_is_pid_valid - check if a PID referes to a valid process/task.
*
* This routine checks if a PID referes to a valid process/task.
*
* Parameters:
* \is
* \i <pid>
* Process ID to check.
* \ie
*
* None.
*
* RETURNS: IP_TRUE if the PID is valid, IP_FALSE otherwise.
*
* ERRNO: None.
*/
IP_PUBLIC Ip_bool ipcom_is_pid_valid
(
     Ip_pid_t pid
);

/*
 ****************************************************************************
 * X                    MISC
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_sleep - delay execution of the current process
*
* This routine delays execution of the current process for a specified number of
* seconds but allows other processes to run.
*
* Parameter:
* \is
* \i <seconds>
* Number of <seconds> to sleep.
* \ie
*
* RETURNS: 0 [* on success?--other unsigned int on failure?]
*
* ERRNO: None.
*/
IP_PUBLIC unsigned int ipcom_sleep
(
    unsigned int seconds
);


/*******************************************************************************
*
* ipcom_millisleep - delay execution of the current process
*
* This routine delays execution of the current process for a specified number of
* milliseconds but allows other processes to run.
*
* Parameter:
* \is
* \i <milli_seconds>
* Number of milliseconds to sleep.
* \ie
*
* RETURNS: 0 [* on success?--other unsigned int on failure?]
*
* ERRNO: None.
*/
IP_PUBLIC unsigned int ipcom_millisleep
(
    unsigned int milli_seconds
);


/*
 ****************************************************************************
 * X                    IPC
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_ipc_install - install the calling process
*
* This routine installs the calling process in the IPC name service. This means
* that the process is now available to receive IPC messages. The process name
* (chosen when the process was created) is used as the identifier, and should be
* entered in subsequent calls to ipcom_ipc_open() by other processes.
*
* Parameters:
*
* None.
*
* RETURNS: 'IPCOM_SUCCESS', if the system call is successful, otherwise, an
* error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_ipc_install
(
    void
);


/*******************************************************************************
*
* ipcom_ipc_uninstall - uninstall the calling process
*
* This routine uninstalls the calling process from the IPC name service. Note
* that this only means that all name service resources are released. It may
* still be possible, in some implementations, both to open the IPC channel and
* to send IPC messages to the calling process.
*
* Parameters:
*
* None.
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_ipc_uninstall
(
    void
);


/*******************************************************************************
*
* ipcom_ipc_open - open an IPC channel
*
* This routine opens an IPC channel to another process. Clients must use this
* call before sending IPC messages to a server. When the client has finished
* using the IPC channel, it must be closed with the ipcom_ipc_close() call.
*
* Parameters:
* \is
* \i <ipc>
* Pointer to an IPC structure which should be used at subsequent send
* operations.
* \i <name>
* Name of the process to open an IPC channel to. This is the same name as
* entered in the ipcom_proc_create() call.
* \i <ms>
* Timeout, in milliseconds, to wait for the IPC channel to become available.
* This may be used when servers not yet have been able to announce their
* existence. The macro 'IPC_TIMEOUT_POLL' causes an immediate return from the
* function; the macro 'IPC_TIMEOUT_INFINITE' makes the function wait forever.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_ipc_open
(
    Ipcom_ipc *ipc,
    const char *name,
    Ip_s32 ms
);


/*******************************************************************************
*
* ipcom_ipc_close - close an IPC channel
*
* This routine closes an IPC channel, previously opened with ipcom_ipc_open() or
* implicitly opened by ipcom_ipc_receive(). Closing an IPC channel means that
* all resources that are associated with the channel are released.
*
* Parameter:
* \is
* \i <ipc>
* Pointer to the IPC structure to close.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_ipc_close
(
    Ipcom_ipc *ipc
);


/*******************************************************************************
*
* ipcom_ipc_isopen - check for an open IPC channel
*
* This routine checks if an IPC channel is opened or not. Note that this does
* not give any information about the peer process; it only checks the local end
* of the IPC channel.
*
* Parameter:
* \is
* \i <ipc>
* Pointer to IPC structure to check.
* \ie
*
* RETURNS: '1' if the IPC channel is open, '0' otherwise.
*
* ERRNO: None.
*/
IP_PUBLIC Ip_bool ipcom_ipc_isopen
(
    Ipcom_ipc *ipc
);


/*******************************************************************************
*
* ipcom_ipc_malloc - allocate an IPC message buffer for a send operation
*
* This routine allocates an IPC message buffer for use in a subsequent send
* operation. This is the only buffer type that may be used for IPC
* transmissions. An allocated IPC buffer may either be sent to another process
* with ipcom_ipc_send() or released with ipcom_ipc_free().
*
* Parameter:
* \is
* \i <size>
* Buffer size.
* \ie
*
* RETURNS: A buffer pointer, on success, 'IP_NULL' otherwise.
*
* ERRNO: None.
*/
IP_PUBLIC void *ipcom_ipc_malloc
(
    Ip_size_t size
);
#if defined(IPCOM_USE_DMALLOC) && defined(IP_PORT_UNIX)
IP_PUBLIC void *ipcom_dipc_malloc(Ip_size_t size, const char *file, const char *function, Ip_u32 line);
#define ipcom_ipc_malloc(s) ipcom_dipc_malloc(s, __FILE__, __FUNCTION__, __LINE__)
#endif


/*******************************************************************************
*
* ipcom_ipc_free - free an IPC buffer
*
* This routine frees an IPC buffer that either is allocated with an
* ipcom_ipc_malloc() call or obtained in an ipcom_ipc_receive() call.
*
* Parameter:
* \is
* \i <buf>
* Buffer pointer.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_PUBLIC void ipcom_ipc_free
(
    void *buf
);


/*******************************************************************************
*
* ipcom_ipc_size - get the IPC buffer size
*
* This routine gets the size of an IPC buffer obtained from ipcom_ipc_malloc()
* or ipcom_ipc_receive().
*
* Parameter:
* \is
* \i <buf>
* Buffer pointer.
* \ie
*
* RETURNS: Buffer size.
*
* ERRNO: None.
*/
IP_PUBLIC Ip_size_t ipcom_ipc_size
(
    void *buf
);


/*******************************************************************************
*
* ipcom_ipc_send - send a message on an IPC channel
*
* This routine sends a message on an IPC channel, which means that the message
* is entered in the input queue of the associated process. The IPC channel must
* have been opened with a previous ipcom_ipc_open() call. The message buffer
* must either have been allocated with ipcom_ipc_malloc() or obtained in an
* ipcom_ipc_receive() call. No other buffer types are valid in this call.
*
* Note that when this call returns with successful status, the buffer is
* transferred to the receiving process and may not be accessed by the calling
* process again. Unsuccessful returns do, however, indicate that the buffer has
* not been sent and must be released by the calling process.
*
* If a message buffer is obtained with ipcom_ipc_receive(), the buffer may be
* returned to the sending process by specifying 'IPCOM_IPC_SENDER' as the
* destination. 'IPCOM_IPC_SENDER' can not be used in combination with a buffer
* allocated by ipcom_ipc_malloc().
*
* Since the IPC channel may be implemented on top of synchronous communication
* primitives, messages cannot be sent to the IPC channel of the calling process.
*
* Parameters:
* \is
* \i <ipc>
* Pointer to the destination IPC, or to 'IPCOM_IPC_SENDER', if replying to a
* buffer obtained with ipcom_ipc_receive().
* \i <buf>
* Buffer to send.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_ipc_send
(
    Ipcom_ipc *ipc,
    void *buf
);

/* Use ID of sender as destination of IPC send operation */
#define IPCOM_IPC_SENDER ((Ipcom_ipc *)1)


/*******************************************************************************
*
* ipcom_ipc_receive - wait for an IPC message
*
* This routine waits for an IPC message to arrive. A pointer to the received
* message is obtained in the location pointed to by <buf>. Note that the <buf>
* parameter should point to a pointer variable that contains the received
* message.  If the <buf> parameter is 'IP_NULL', the received message is
* implicitly released.
*
* The received buffer must either be sent to another process with the
* ipcom_ipc_send() call or released with ipcom_ipc_free().
*
* If the <ipc> parameter is non-zero, an IPC channel to the sender of the
* received buffer is implicitly opened and has to be closed with
* ipcom_ipc_close(). Replies to the sender can conveniently be sent using this
* channel. Set <ipc> to 'IP_NULL', if this is not desired.
*
* Parameters:
* \is
* \i <ipc>
* Pointer to an IPC structure which, upon return, contains an open channel to
* the sender of the received message. Set to 'IP_NULL' to disable this feature.
* \i <buf>
* Pointer to a pointer which, upon return, contains the received message.
* \i <ms>
* Timeout, in milliseconds, to wait for a message to become available. The macro
* 'IPC_TIMEOUT_POLL' causes an immediate return from the function; the macro
* 'IPC_TIMEOUT_INFINITE' makes the function wait forever.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_ipc_receive
(
    Ipcom_ipc *ipc,
    void *buf,
    Ip_s32 ms
);

#define IPC_TIMEOUT_POLL       0
#define IPC_TIMEOUT_INFINITE  -1


/*
 ****************************************************************************
 * X                  public non-documented functions
 ****************************************************************************
 */

IP_PUBLIC Ip_err  ipcom_proc_setprio(Ip_pid_t pid, Ip_u16 prio);
IP_PUBLIC Ip_u16  ipcom_proc_getprio(Ip_pid_t pid);
IP_PUBLIC Ip_err  ipcom_proc_cpu_affinity_clr(Ip_pid_t pid);
IP_PUBLIC Ip_err  ipcom_proc_cpu_affinity_set(Ip_pid_t pid, Ip_cpu_set_t *cpuset);
IP_PUBLIC Ip_err  ipcom_proc_cpu_affinity_get(Ip_pid_t pid, Ip_cpu_set_t *cpuset);
IP_PUBLIC Ip_err  ipcom_proc_cpu_affinity_set_all(Ip_cpu_set_t *cpuset);
IP_PUBLIC int     ipcom_cpu_reserved_alloc(void);
IP_PUBLIC Ip_err  ipcom_cpu_reserved_free(int cpu);
IP_PUBLIC Ip_bool ipcom_cpu_is_online(int cpu);
IP_PUBLIC int     ipcom_num_online_cpus(void);
IP_PUBLIC int     ipcom_num_configured_cpus(void);
IP_PUBLIC int     ipcom_get_cpuid(void);
IP_PUBLIC void    ipcom_mb(void);
IP_PUBLIC void    ipcom_rmb(void);
IP_PUBLIC void    ipcom_wmb(void);


/*
 *===========================================================================
 *                    ipcom_once
 *===========================================================================
 * Description: Run init() function once, wait until initialized.
 *              The init() function must not call ipcom_once(),
 *              failure to do so will result in a deadlock.
 * Parameters:  once - handle that has been statically initialized to
 *                    IPCOM_ONCE_INIT
 *              init - function to run exactly once
 *              param - user defined argument passed to the init() function
 * Returns:     IPCOM error code.
 */
IP_PUBLIC Ip_err ipcom_once(Ipcom_once_t *once, Ip_err (*init)(void *param), void *param);


/*
 ****************************************************************************
 * X                  VIRTUAL ROUTING
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcom_proc_vr_get
 *===========================================================================
 * Description:  Get VR id for this process.
 * Parameters:   -
 * Returns:      VR
 */
IP_PUBLIC int ipcom_proc_vr_get(void);


/*
 *===========================================================================
 *                         ipcom_proc_vr_set
 *===========================================================================
 * Description:  Set VR id for this process.
 * Parameters:   VR id
 * Returns:      -
 */
IP_PUBLIC void ipcom_proc_vr_set(int vr);


/*
 ****************************************************************************
 * X                  INTERNAL IPCOM OS Functions
 ****************************************************************************
 */

IP_PUBLIC Ipcom_proc *ipcom_proc_malloc(Ip_pid_t pid, Ip_size_t pproc_size);
IP_PUBLIC void        ipcom_proc_free(Ipcom_proc *proc);

#if IPCOM_USE_ENV == IPCOM_ENV_IPCOM
IP_GLOBAL void ipcom_proc_copyenv(Ipcom_proc *proc_c);
#endif


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
