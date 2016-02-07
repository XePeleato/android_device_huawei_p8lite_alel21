/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_job_queue.c,v $ $Revision: 1.6 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_job_queue.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * VxWorks specific version of the IPCOM net-job API.
 * NOTE: this implementation does only implement enough features to
 * support the IPNET stack. The most important restriction is that
 * only one delayed job can be in flight at any point in time.
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


#if (defined(IPNET) || defined(IPLITE)) && defined(IPCOM_USE_JOB_QUEUE)

#include <vxWorks.h>
#include <jobQueueLib.h>
#include <netLib.h>
#include <wdLib.h>
#include <vxAtomicLib.h>

#include "ipcom_job_queue.h"
#include "ipcom_clib.h"
#include "ipcom_errno.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Maximum number of ticks that a job can be delayed. The 2:nd
   argument of wdStart() takes number of ticks as an "int", so the
   biggest possible value is 0x7fffffff. Ticks is calculated as
   ticks = (delay * tick_per_sec + 999) / 1000
*/
#define IPCOM_JOB_MAX_TICKS_FOR_DELAYED_JOB (0x7fffffff - 999)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_job_struct {
    QJOB                vxjob;       /* VxWorks job structure */
    Ipcom_job_action    action;      /* Handler for this job */
    atomic_t            pending;     /* IS job is pending? */
    WDOG_ID             watchdog_id; /* Watchdog for this job or
                                        IP_NULL if this job has never
                                        been delayed */
    int                 watchdog_ticks; /* Number of ticks the current
                                           watchdog is scheduled */
}
Ipcom_job_t;

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int sysClkRateGet(void);

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

/* Memory cache for IPNET job entries */
static Ipcom_memory_cache   *ipcom_job_slab;
/* ID of the job queue used by all IPNET jobs */
static JOB_QUEUE_ID          ipcom_job_queue_id;
/* Number of system ticks per second */
static int                   ipcom_system_ticks_per_sec;
/* The longest delay (in seconds) that can can be translated to ticks
   without causing integer overflow. It is calculated as
   ipcom_max_delay = IPCOM_JOB_MAX_TICKS_FOR_DELAYED_JOB / ipcom_system_ticks_per_sec */
static int                   ipcom_max_delay;


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipcom_job_action_auto_clean
 *===========================================================================
 * Description: VxWorks job handler that will run the specified action
 *              and then free the job structure.
 * Parameters:  job - contains the action to perform
 * Returns:
 *
 */
IP_STATIC void
ipcom_job_action_auto_clean(Ipcom_job_t *job)
{
    job->action();
    ipcom_slab_free(ipcom_job_slab, job);
}


/*
 *===========================================================================
 *                        ipcom_sigleton_job_action
 *===========================================================================
 * Description: VxWorks job handler for jobs that may only be present no
 *              more than one time on the job queue.
 * Parameters:  job - contains the action to perform
 * Returns:
 *
 */
IP_STATIC void
ipcom_singleton_job_action(Ipcom_job_t *job)
{
    job->watchdog_ticks = 0;
    vxAtomicSet(&job->pending, 0);
    job->action();
}


/*
 *===========================================================================
 *                   ipcom_job_queue_prio_to_vxjob_prio
 *===========================================================================
 * Description: Converts an IPCOM job priority to the equvivalent VxWorks
 *              job priority.
 * Parameters:  prio - an IPCOM job priority.
 * Returns:     A VxWorks job priority.
 *
 */
IP_STATIC int
ipcom_job_queue_prio_to_vxjob_prio(enum Ipcom_job_prio prio)
{
    switch (prio)
    {
    case IPCOM_JOB_PRIO_LOW:
        return NET_TASK_QJOB_PRI - 1;
    case IPCOM_JOB_PRIO_NORMAL:
        return NET_TASK_QJOB_PRI;
    case IPCOM_JOB_PRIO_HIGH:
        return NET_TASK_QJOB_PRI + 1;
    default:
        IP_PANIC();
    }
    return 0;
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
 *                          ipcom_job_queue_init
 *===========================================================================
 * Description: Initializes the job queue.
 * Parameters:  memory_pool - the memory pool the job queue has to allocate
 *                            memory from.
 * Returns:    0 = sucess, <0 = error code.
 *
 */
IP_PUBLIC int
ipcom_job_queue_init(Ipcom_memory_pool *memory_pool)
{
    ipcom_job_queue_id = netDaemonQ(0);
    ipcom_system_ticks_per_sec = sysClkRateGet();
    ipcom_max_delay = IPCOM_JOB_MAX_TICKS_FOR_DELAYED_JOB / ipcom_system_ticks_per_sec;
    ipcom_job_slab = ipcom_slab_new("IPCOM network job",
                                    0,
                                    sizeof(Ipcom_job_t),
                                    ipcom_cache_line_size(),
                                    IP_NULL,
                                    IP_NULL,
                                    memory_pool);
    return (ipcom_job_slab ? 0 : -IP_ERRNO_ENOMEM);
}


/*
 *===========================================================================
 *                     ipcom_job_queue_singleton_new
 *===========================================================================
 * Description: Creates a job that is guaranteed to be present at the most
 *              one time in the queue of pending jobs.
 * Parameters:  action - function to call in order to run the job
 *              prio - priority of the job
 * Returns:     A new singleton job.
 *
 */
IP_PUBLIC Ipcom_singleton_job_t ipcom_job_queue_singleton_new(Ipcom_job_action action,
                                                              enum Ipcom_job_prio prio)
{
    Ipcom_job_t *job;

    job = ipcom_slab_alloc(ipcom_job_slab);
    if (IP_LIKELY(job != IP_NULL))
    {
        job->vxjob.priInfo = ipcom_job_queue_prio_to_vxjob_prio(prio);
        job->vxjob.func    = (QJOB_FUNC) ipcom_singleton_job_action;
        job->action        = action;
        job->watchdog_id   = IP_NULL;
        vxAtomicSet(&job->pending, 0);
    }
    return job;
}


/*
 *===========================================================================
 *                   ipcom_job_queue_singleton_delete
 *===========================================================================
 * Description: Frees all resources allocated by
 *              ipcom_job_queue_singleton_new()
 * Parameters:  sjob - singleton job created with
 *                     ipcom_job_queue_singleton_new()
 * Returns:
 *
 */
IP_PUBLIC void ipcom_job_queue_singleton_delete(Ipcom_singleton_job_t sjob)
{
    Ipcom_job_t *job = (Ipcom_job_t *) sjob;

    ip_assert(vxAtomicGet(&job->pending) == 0);
    if (job->watchdog_id)
        wdDelete(job->watchdog_id);
    ipcom_slab_free(ipcom_job_slab, sjob);
}


/*
 *===========================================================================
 *                    ipcom_job_queue_schedule_singleton
 *===========================================================================
 * Description: Schedule a singleton job to be run as soon as possible.
 * Parameters:  sjob - singleton job created with
 *                     ipcom_job_queue_singleton_new()
 * Returns:
 *
 */
IP_PUBLIC void ipcom_job_queue_schedule_singleton(Ipcom_singleton_job_t sjob)
{
    Ipcom_job_t *job = (Ipcom_job_t *) sjob;

    if (vxAtomicSet(&job->pending, 1) == 0)
        jobQueuePost(ipcom_job_queue_id, &job->vxjob);
    /* else: job is already scheduled, but has not be run yet. No need
       to schedule another job to VxWorks since IPNET will process all
       pending timeouts in the 'action' passed */
}


/*
 *===========================================================================
 *               ipcom_job_queue_schedule_singleton_delayed
 *===========================================================================
 * Description: Schedule a job to be run in the future.
 * Parameters:  sjob - job to run in the future
 *              delay - number of milliseconds to delay the job
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_job_queue_schedule_singleton_delayed(Ipcom_singleton_job_t sjob,
                                           Ip_u32 delay)
{
    Ipcom_job_t *job = (Ipcom_job_t *) sjob;

    if (delay == 0)
        ipcom_job_queue_schedule_singleton(sjob);
    else
    {
        int ticks;

        if (job->watchdog_id == IP_NULL)
        {
            /* Lazy init of the watchdog timer */
            job->watchdog_id = wdCreate();
            if (job->watchdog_id == NULL)
            {
                /* The stack timeout handling will not work without a
                   watchdog timer. */
                IP_PANIC();
                return;
            }
        }

        if (delay >= ipcom_max_delay)
            /* The "ticks" variable would overflow if "delay" is
               translated to ticks in this case. The stack has no
               problem with receiving a call to the timeout machinery
               too early though, it will just schedule a new timeout
               with "delay" set to the timeout to the next timeout. */
            ticks = IPCOM_JOB_MAX_TICKS_FOR_DELAYED_JOB;
        else
            ticks = (delay * ipcom_system_ticks_per_sec + 999) / 1000;

        if (job->watchdog_ticks == 0 || job->watchdog_ticks > ticks)
        {
            job->watchdog_ticks = ticks;
            wdStart(job->watchdog_id,
                    ticks,
                    (FUNCPTR) ipcom_job_queue_schedule_singleton,
                    (int) job);
        }
        /* else: the watchdog is already schedule with the same or
           shorter time, let the running timer expire before the
           current one. IPNET has no problem handling too early
           exparation of this timer. */
    }
}


/*
 *===========================================================================
 *                        ipcom_job_queue_schedule
 *===========================================================================
 * Description: Schedule a job to be run as soon as possible.
 * Parameters:  action - function to call in order to run the job
 *              prio - priority of the job
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_job_queue_schedule(Ipcom_job_action action,
                         enum Ipcom_job_prio prio)
{
    Ipcom_job_t *job;

    job = ipcom_slab_alloc(ipcom_job_slab);
    if (IP_UNLIKELY(job == IP_NULL))
        return;

    job->vxjob.priInfo = ipcom_job_queue_prio_to_vxjob_prio(prio);
    job->vxjob.func    = (QJOB_FUNC) ipcom_job_action_auto_clean;
    job->action        = action;

    jobQueuePost(ipcom_job_queue_id, &job->vxjob);
}
#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
