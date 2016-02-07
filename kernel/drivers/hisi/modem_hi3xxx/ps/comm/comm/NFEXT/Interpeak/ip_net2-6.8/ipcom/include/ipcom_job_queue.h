/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_job_queue.h,v $ $Revision: 1.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_job_queue.h,v $
 *     $Author: kenneth $ $Date: 2008-08-15 16:29:25 $
 *     $State: Exp $ $Locker:  $
 *
 * INTERPEAK_COPYRIGHT_STRING
 * Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_NETJOB_H
#define IPCOM_NETJOB_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
API for accessing the IPCOM job daemon.
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

#ifdef __cplusplus
extern "C" {
#endif

#include "ipcom_slab.h"

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

enum Ipcom_job_prio {
    IPCOM_JOB_PRIO_LOW,
    IPCOM_JOB_PRIO_NORMAL,
    IPCOM_JOB_PRIO_HIGH
};

typedef void(*Ipcom_job_action)(void);
typedef void* Ipcom_singleton_job_t;

/*
 ****************************************************************************
 * 6                 PUBLIC FUNCTIONS
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
IP_PUBLIC int ipcom_job_queue_init(Ipcom_memory_pool *memory_pool);


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
                                                              enum Ipcom_job_prio prio);


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
IP_PUBLIC void ipcom_job_queue_singleton_delete(Ipcom_singleton_job_t sjob);


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
IP_PUBLIC void ipcom_job_queue_schedule_singleton(Ipcom_singleton_job_t sjob);


/*
 *===========================================================================
 *                    ipcom_job_queue_schedule_delayed
 *===========================================================================
 * Description: Schedule a job to be run in the future.
 * Parameters:  sjob - singleton job created with
 *                     ipcom_job_queue_singleton_new()
 *              delay - number of milliseconds to delay the job
 * Returns:
 *
 */
IP_PUBLIC void ipcom_job_queue_schedule_singleton_delayed(Ipcom_singleton_job_t sjob,
                                                          Ip_u32 delay);



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
IP_PUBLIC void ipcom_job_queue_schedule(Ipcom_job_action action,
                                        enum Ipcom_job_prio prio);

#ifdef __cplusplus
}
#endif

#endif /* IPCOM_SPINLOCK_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

