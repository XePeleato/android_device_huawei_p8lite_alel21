


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include <linux/moduleparam.h>
#include <linux/stat.h>   /* for S_IRUGO|S_IWUSR */
#include "hwifi_utils.h"

/*
 * 2 Global Variable Definition
 */

/*Wifi Log Prefix, config by module init*/
uint32 print_verbose = (PRINT_VERBOSE_SYSTEM_TIME  | PRINT_VERBOSE_FUNC_NAME | PRINT_VERBOSE_LINE_NUM);
//uint32 print_verbose = (PRINT_VERBOSE_FUNC_NAME | PRINT_VERBOSE_LINE_NUM );
module_param(print_verbose, uint, S_IRUGO|S_IWUSR);

uint32 loglevel = LOG_LEVEL_INFO;
module_param(loglevel, uint, S_IRUGO|S_IWUSR);

#if (HWIFI_DEBUG_VERBOSE)
#warning This debug info effect the transfer performance, set this macro 0 when release
#endif

#ifdef WLAN_PERFORM_OPT
#if (HI110X_GPIO_RXDATA_THREAD_PRIORITY <= HI110X_SDIO_DISPOSE_THREAD_PRIORITY)
/*If rx thread is faster than dipose thread,
  skb alloc maybe failed, this will be never permitted*/
#warning dispose thread prority must higher than rx thread
#warning protect the kernel skb buffer pool is plentiful to hcc sg transfer
#endif
#endif
/*
 * 3 Function Definition
 */


struct task_struct *hi110x_thread_create(int (*threadfn)(void *data),
                                        void                    *data,
                                        struct semaphore        *sema_sync,
                                        const char               namefmt[],
                                        uint32                   policy,
                                        int32                    prio,
                                        int32                    cpuid)
{
    struct task_struct      *tsk;
    struct sched_param       param;
    int                      ret_bind;
    int ret = 0;

    /* create thread for gpio rx data in interrupt handler*/
    if(sema_sync)
        sema_init(sema_sync, 0);
    tsk = kthread_run(threadfn, data, namefmt);
    if (IS_ERR(tsk))
    {
        HWIFI_ERROR("Failed to run theread:%s", namefmt);
        return NULL;
    }

    /* set thread priority and schedule policy */
    param.sched_priority = prio;
    ret = sched_setscheduler(tsk, policy, &param);
    if(HWIFI_WARN(ret, "%s setscheduler failed! ret=%d ",namefmt , ret))
    {
        HWIFI_WARNING("%s sched_setscheduler failed! ret =%d, prio=%d",namefmt, ret, prio);
    }
    if(cpuid >= 0)
    {
        /*设置线程的cpu绑定*/
        ret_bind    = set_cpus_allowed_ptr(tsk, &cpumask_of_cpu(cpuid));
        if (0 == ret_bind)
        {
            tsk->flags |= PF_NO_SETAFFINITY;
            return tsk;
        }
        else
        {
            HWIFI_ERROR("thread %s can't bind cpuid %d, error code is %d",namefmt, cpuid, ret_bind);
            return NULL;
        }
    }
    else 
    {
        HWIFI_INFO("did not bind cpu...");
        return tsk;
    }
}
void hi110x_thread_stop(struct task_struct      *tsk,
                               struct semaphore        *sema_sync)
{
        /*sema_sync can be NULL*/
    /* create thread for gpio rx data in interrupt handler*/
    send_sig(SIGTERM, tsk, 1);
    kthread_stop(tsk);
}

/*
 * Prototype    : create_task
 * Description  : create kernel thread
 * Input        : struct *task_ctrl_struct
                int32 prio
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/11
 *     Author       :
 *     Modification : Created function
 *
 */
int32 create_task(struct task_ctrl_struct *task_ctrl,
                     int (*threadfn)(void *data),
                     void *data,
                     const char* name)
{
    init_waitqueue_head(&task_ctrl->wait_queue);

    task_ctrl->task = kthread_run(threadfn, data, name);
    if (IS_ERR(task_ctrl->task))
    {
        HWIFI_ERROR("create task %s error:%d", name, (int32)PTR_ERR(task_ctrl->task));
        task_ctrl->task = NULL;
        return -EFAIL;
    }

    return SUCC;
}

/*
 * Prototype    : stop_task
 * Description  : stop task
 * Input        : struct task_ctrl_struct *task_ctrl
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/3/12
 *     Author       :
 *     Modification : Created function
 *
 */
void stop_task(struct task_ctrl_struct *task_ctrl)
{
    if (task_ctrl->task != NULL)
    {
        kthread_stop(task_ctrl->task);
        task_ctrl->task = NULL;

    }
}


void hi110x_set_thread_property(struct task_struct *p,int policy,
                                        const struct sched_param *param,
                                        long nice)
{
    HWIFI_BUG_ON(!p);
    HWIFI_BUG_ON(!param);
    HWIFI_INFO("set thread scheduler policy %d", policy);
    
    if(sched_setscheduler(p, policy, param))
    {
        HWIFI_ERROR("set scheduler failed! %d", policy);
    }
    
    if(policy != SCHED_FIFO && policy != SCHED_RR)
    {
        HWIFI_INFO("set thread scheduler nice %ld", nice);
        set_user_nice(p, nice);
    }
}

int hi_append_system_time(char* buf, size_t len)
{
    int ret = 0;
    struct timeval  tv;
    struct rtc_time tm;
    if(!(print_verbose & PRINT_VERBOSE_SYSTEM_TIME)) 
        return ret;
    if(len<=0)
        return ret;
    do_gettimeofday(&tv); 
    tv.tv_sec -= sys_tz.tz_minuteswest * 60;
    rtc_time_to_tm(tv.tv_sec, &tm); 
    ret = snprintf(buf, len, "[%02d:%02d:%02d.%06d]", tm.tm_hour, tm.tm_min, tm.tm_sec, (int)tv.tv_usec);
    return ret;
}

int hi_append_fun_name(char* buf, size_t len, const char* name)
{
    int ret = 0;
    if(!(print_verbose & PRINT_VERBOSE_FUNC_NAME)) 
        return ret;
    if(len<=0)
        return ret;
    ret = snprintf(buf, len, "[%s]", name);
    return ret;
}

int hi_append_line_num(char* buf, size_t len, int line_num)
{
    int ret = 0;
    if(!(print_verbose & PRINT_VERBOSE_LINE_NUM)) 
        return ret;
    if(len<=0)
        return ret;
    ret = snprintf(buf, len, "[%d]", line_num);
    return ret;
}

int hi_append_return_addr(char* buf, size_t len, void* ret_ip)
{
    int ret = 0;
    if(!(print_verbose & PRINT_VERBOSE_RETURN_ADDR)) 
        return ret;
    if(len<=0)
        return ret;
    ret = snprintf(buf, len, "[<==%pF]", ret_ip); 
    return ret;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

