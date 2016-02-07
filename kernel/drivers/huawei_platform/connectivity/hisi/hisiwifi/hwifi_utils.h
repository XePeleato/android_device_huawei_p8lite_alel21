

#ifndef __HWIFI_UTILS_H__
#define __HWIFI_UTILS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/wait.h>         /* wait_queue_head_t */
#include <linux/sched.h>        /* task_struct */
#include <linux/semaphore.h>    /* semaphore */
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/list.h>
#include <linux/moduleparam.h>

#include "common.h"

#ifdef CONFIG_HW_CHR
#include "../hw-chr/chr_user.h"
#else
#define CHR_LOG(prio, tag, fmt...) 
#endif

/*
 * 2 Macro Definition
 */
/*make most use of the kernel API,
never should make one yourself if kernel had,
refer to kernel.h*/
#define MIN(a, b)       min(a, b)
#define MAX(a, b)       max(a, b)

#define MAC2STR(a)      (a)[0], "**", "**", "**", (a)[4], (a)[5]
#define MACFMT          "%02x:%s:%s:%s:%02x:%02x"

/* channel index and frequence */
#define HWIFI_MIN_CHANNEL_2G 1
#define HWIFI_MAX_CHANNEL_2G 14

#define FLOWCTRL_ON                             0       /* can send */
#define FLOWCTRL_OFF                            1       /* can't send */

#ifdef WLAN_PERFORM_OPT
#define HI110X_SDIO_DISPOSE_THREAD_PRIORITY      10
#define HCC_TRANS_THERAD_PRIORITY       10     /*tx相对于协议栈上报 sdiotx是瓶颈*/
#endif
#define HI110X_WATCHDOG_THREAD_PRIORITY          60     /* SCHED_NORMAL policy */
#define HI110X_GPIO_RXDATA_THREAD_PRIORITY       20     /* rx sdio是瓶颈且device需要降成本 */
#define HI110X_GPIO_WAKEUP_THREAD_PRIORITY       50     /* SCHED_FIFO policy */
#define HI110X_GPIO_SLEEP_THREAD_PRIORITY        40     /* SCHED_FIFO policy */


#define HI110X_SDIO_RX_THREAD_POLICY    SCHED_RR
#define HI110X_HCC_TRANS_THREAD_POLICY    SCHED_RR

#define HI110X_SDIO_RX_THREAD_NICE    (-20)
#define HI110X_HCC_TRANS_THREAD_NICE    (-10)

#define HI110X_THREAD_NICE_NONE    (20)

#define HWIFI_MIN_CHANNEL_5G 36
#define HWIFI_MAX_CHANNEL_5G 165

#define HWIFI_MIN_CHANNEL_4_9G 184
#define HWIFI_MAX_CHANNEL_4_9G 196

#define HWIFI_BASE_FREQ_2G   (2412)
#define HWIFI_BASE_FREQ_5G   (5000)
#define HWIFI_BASE_FREQ_4_9G (4000)

#define HWIFI_MIN_FREQ_2G    (2412 + 5*(HWIFI_MIN_CHANNEL_2G - 1))
#define HWIFI_MAX_FREQ_2G    (2484)
#define HWIFI_MIN_FREQ_5G    (5000 + 5*(HWIFI_MIN_CHANNEL_5G))
#define HWIFI_MAX_FREQ_5G    (5000 + 5*(HWIFI_MAX_CHANNEL_5G))

/* channel nums */
#define HWIFI_ROAM_SCAN_CHANNEL_MAX_NUM ((HWIFI_MAX_CHANNEL_2G - HWIFI_MIN_CHANNEL_2G + 1) + (HWIFI_MAX_CHANNEL_4_9G - HWIFI_MIN_CHANNEL_5G + 1))

#ifdef WLAN_PERFORM_DEBUG
enum LOG_LEVEL
{
    LOG_LEVEL_PERFORM = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
};
#else
enum LOG_LEVEL
{
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
};
#endif

 typedef enum _WIFI_LOG_PRINT_VERBOSE_
 {
    PRINT_VERBOSE_SYSTEM_TIME = 1 << 0,
    PRINT_VERBOSE_FUNC_NAME = 1 << 1,
    PRINT_VERBOSE_LINE_NUM = 1 << 2,
    PRINT_VERBOSE_RETURN_ADDR = 1<<3,
 }WIFI_LOG_PRINT_VERBOSE;
 #define WIFI_LOG_PREFIX_BUF_LEN    (100)
  #define WIFI_LOG_POSTFIX_BUF_LEN    (100)

#define TRANS_LOCK  (1)
#define TRANS_UNLOCK  (0)

//#define CONFIG_HI1101_PRINT_EMMC 1
#define DEFAULT_POWER_STATUS  1

//#if CONFIG_HI1101_PRINT_EMMC
/*no serial port, color leacy*/
#if 1
#define COLOR_NONE
#define COLOR_RED
#define COLOR_LIGHT_RED
#define COLOR_GREEN
#define COLOR_LIGHT_GREEN
#define COLOR_BLUE
#define COLOR_LIGHT_BLUE
#define COLOR_DARY_GRAY
#define COLOR_CYAN
#define COLOR_LIGHT_CYAN
#define COLOR_PURPLE
#define COLOR_LIGHT_PURPLE
#define COLOR_BROWN
#define COLOR_YELLOW
#define COLOR_LIGHT_GRAY
#define COLOR_WHITE
#else
#define COLOR_NONE ".33[m"
#define COLOR_RED ".33[0;32;31m"
#define COLOR_LIGHT_RED ".33[1;31m"
#define COLOR_GREEN ".33[0;32;32m"
#define COLOR_LIGHT_GREEN ".33[1;32m"
#define COLOR_BLUE ".33[0;32;34m"
#define COLOR_LIGHT_BLUE ".33[1;34m"
#define COLOR_DARY_GRAY ".33[1;30m"
#define COLOR_CYAN ".33[0;36m"
#define COLOR_LIGHT_CYAN ".33[1;36m"
#define COLOR_PURPLE ".33[0;35m"
#define COLOR_LIGHT_PURPLE ".33[1;35m"
#define COLOR_BROWN ".33[0;33m"
#define COLOR_YELLOW ".33[1;33m"
#define COLOR_LIGHT_GRAY ".33[0;37m"
#define COLOR_WHITE ".33[1;37m"
#endif

#define print_log   printk
#define pr_debug_wifi   pr_debug


extern uint32 loglevel;
extern uint32 g_powermgmt_switch;
extern uint32 print_verbose;

extern int hi_append_system_time(char* buf, size_t len);
extern int hi_append_fun_name(char* buf, size_t len, const char* name);
extern int hi_append_line_num(char* buf, size_t len, int line_num);
extern int hi_append_return_addr(char* buf, size_t len, void* ret_ip);

/*This debug info effect the transfer performance,
must set to 0 in release version*/
#define HWIFI_DEBUG_VERBOSE (0)

#define GET_WIFI_LOG_PREFIX(buf,len_t)\
    do{\
        int len = len_t;\
        int ret  = 0;\
        int buf_ret = 0;\
        memset(buf, 0, 1);\
        hi_append_system_time(buf, len);\
        len -= ret;\
        buf_ret += ret;\
        WARN_ONCE(len < 0,"prefix warning len:%d,ret:%d\n", len, ret);\
    }while(0)


#define GET_WIFI_LOG_POSTFIX(buf,len_t)\
    do{\
        int len = len_t;\
        int ret  = 0;\
        int buf_ret = 0;\
        memset(buf, 0, 1);\
        ret = hi_append_fun_name(buf + buf_ret, len, __FUNCTION__);\
        len -= ret;\
        buf_ret += ret;\
        ret = hi_append_line_num(buf + buf_ret, len, __LINE__);\
        len -= ret;\
        buf_ret += ret;\
        ret = hi_append_return_addr(buf + buf_ret, len, (void*)_RET_IP_);\
        len -= ret;\
        buf_ret += ret;   \
        WARN_ONCE(len < 0,"postfix warning len:%d,ret:%d\n", len, ret);\
    }while(0)    

#define DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)  \
            char buf_pre[WIFI_LOG_PREFIX_BUF_LEN];\
            char buf_post[WIFI_LOG_POSTFIX_BUF_LEN];\
            GET_WIFI_LOG_PREFIX(buf_pre ,WIFI_LOG_PREFIX_BUF_LEN);\
            GET_WIFI_LOG_POSTFIX(buf_post, WIFI_LOG_POSTFIX_BUF_LEN);               

#ifdef WLAN_PERFORM_DEBUG
#define HWIFI_PERFORM(fmt, arg...)      \
    do {                                \
        if (loglevel >= LOG_LEVEL_PERFORM){                     \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_ERR"[WIFI][PERM]%s"fmt"%s\n", buf_pre, ##arg, buf_post);\
        }       \
    } while(0)
#else
#define HWIFI_PERFORM(fmt, arg...)
#endif
#define HWIFI_DEBUG_CONDTION()  (unlikely(loglevel >= LOG_LEVEL_DEBUG))
#define HWIFI_DEBUG(fmt, arg...)    \
    do {                            \
        if (HWIFI_DEBUG_CONDTION()){                                   \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_DEBUG"[WIFI][DBG]%s"fmt"%s\n", buf_pre, ##arg, buf_post);\
            CHR_LOG(CHR_LOG_DEBUG, CHR_LOG_TAG_WIFI, KERN_DEBUG"[WIFI][WARN]%s"fmt"%s\n",buf_pre, ##arg,buf_post); \
        }       \
    } while(0)

#define HWIFI_INFO(fmt, arg...)     \
    do {                            \
        if (loglevel >= LOG_LEVEL_INFO){            \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_DEBUG"[WIFI][INFO]%s"fmt"%s\n", buf_pre, ##arg, buf_post);\
            CHR_LOG(CHR_LOG_INFO, CHR_LOG_TAG_WIFI, KERN_DEBUG"[WIFI][WARN]%s"fmt"%s\n",buf_pre, ##arg,buf_post); \
        }       \
    } while(0)

#define HWIFI_WARNING(fmt, arg...)  \
    do {                            \
        if (loglevel >= LOG_LEVEL_WARNING){             \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_ALERT"[WIFI][WARN]%s"fmt"%s\n", buf_pre, ##arg, buf_post);\
            CHR_LOG(CHR_LOG_WARN, CHR_LOG_TAG_WIFI, KERN_ALERT"[WIFI][WARN]%s"fmt"%s\n",buf_pre, ##arg,buf_post); \
        }       \
    } while(0)


#define HWIFI_WARN_ON(condition)   ({                       \
    int __ret_warn_on = !!(condition);              \
    if (unlikely(__ret_warn_on)){                   \
         HWIFI_WARNING("Hi110x WiFi Warn On happaned!");\
        HWIFI_WARNING("cond:%s",#condition);\
        __WARN();   }                   \
    unlikely(__ret_warn_on);                    \
})

#define HWIFI_WARN(condition, format...)        ({                      \
    int __ret_warn_on = !!(condition);              \
    if (unlikely(__ret_warn_on)){                   \
        HWIFI_WARNING("cond:%s", #condition);\
        HWIFI_WARNING(format);\
        __WARN_printf(format);}                 \
    unlikely(__ret_warn_on);                    \
})

#if HWIFI_DEBUG_VERBOSE
#define HWIFI_VERBOSE(fmt, arg...) \
    do {                            \
        if (loglevel >= LOG_LEVEL_DEBUG){                                     \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_DEBUG"[WIFI][V]%s"fmt"%s\n", buf_pre, ##arg, buf_post);\
        }                                                                       \
    } while(0)
#else
#define HWIFI_VERBOSE(fmt, arg...) do{}while(0)
#endif
#if defined(DEBUG) || defined(CONFIG_DYNAMIC_DEBUG) || defined(CONFIG_HI1101_PRINT_EMMC)
#define HWIFI_PR_DEBUG(fmt, arg...) \
    do {                            \
         /*if (loglevel >= LOG_LEVEL_DEBUG){             */\
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            pr_debug_wifi("[WIFI][PR_DBG]%s"fmt"%s\n", buf_pre, ##arg, buf_post);\
        /*    }    */   \
    } while(0)
#else
#define HWIFI_PR_DEBUG(fmt, arg...)     do{}while(0)
#endif

#define HWIFI_ERROR(fmt, arg...)    \
    do {                            \
        if (loglevel >= LOG_LEVEL_ERROR){       \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_ALERT "[WIFI][ERR]%s"fmt"%s\n", buf_pre, ##arg, buf_post);\
            CHR_LOG(CHR_LOG_ERROR, CHR_LOG_TAG_WIFI, KERN_ALERT"[WIFI][ERR]%s"fmt"%s\n",buf_pre, ##arg,buf_post); \
        }           \
    } while(0)


#define HWIFI_ASSERT(cond)      \
    do {                        \
        if (unlikely(!(cond)))            \
        {                                  \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_ALERT "[WIFI]%s ASSERT Failed!!!cond:%s %s", buf_pre, #cond, buf_post);\
            BUG();              \
        }                       \
    }                           \
    while(0)

/*leacy define, remove some day.*/
#define HWIFI_BUG_ON(cond) \
    do {                        \
        if (unlikely((cond)))            \
        {                                  \
            DEFINE_PREFIX_POST_ARRAY(buf_pre, buf_post)    \
            print_log(KERN_ALERT "[WIFI]%s ASSERT Failed!!!cond:%s%s", buf_pre, #cond, buf_post);\
            BUG();              \
        }                       \
    }                           \
    while(0)

#define HWIFI_PRINT_ONCE(log_type, fmt, arg...)\
    do{\
            static int is_print = 0;\
            if(unlikely(!is_print))\
                {\
                    HWIFI_##log_type(fmt,##arg);\
                    is_print = !is_print;\
                }\
    }while(0)

/*
 * 3 Enum Type Definition
 */

enum return_val
{
    SUCC = 0,
    EFAIL,
};


/*
 * 4 Global Variable Declaring
 */


/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */

struct task_ctrl_struct
{
    struct task_struct     *task;
    wait_queue_head_t       wait_queue;
};
typedef struct _hwifi_log_node_ hwifi_log_node;
typedef struct _hwifi_log_head_ hwifi_log_head;
typedef void (* hwifi_log_info_dump)(hwifi_log_head* head, void* data, int pos);
typedef void (* hwifi_log_info_cb)(hwifi_log_head* head, void* data);
struct _hwifi_log_node_
{
    struct list_head list;
    void* data;    
};

/*wifi debug info struct*/
struct _hwifi_log_head_
{
    uint32    qlen;
    uint32    real_len;
    uint32    max_qlen;
    spinlock_t      lock;
    hwifi_log_info_cb release;
    hwifi_log_info_dump dump;
    char* name;
    struct list_head       head;    
};

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */

/*
 * 10 Function Declare
 */
struct task_struct *hi110x_thread_create(int (*threadfn)(void *data),
                                        void                    *param,
                                        struct semaphore        *sema_sync,
                                        const char               namefmt[],
                                        uint32                   policy,
                                        int32                    prio,
                                        int32                    cpuid);
void hi110x_thread_stop(struct task_struct      *tsk,
                               struct semaphore        *sema_sync);

void hi110x_set_thread_property(struct task_struct *p,int policy,
                                        const struct sched_param *param,
                                        long nice);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_utils.h */
