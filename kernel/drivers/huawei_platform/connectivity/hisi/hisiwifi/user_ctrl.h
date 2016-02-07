

#ifndef __USER_CTRL_H__
#define __USER_CTRL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#include <linux/list.h>
#include <linux/printk.h>
#include "hi110x.h"
/*
 * 1 Other Header File Including
 */


/*
 * 2 Macro Definition
 */


/*
 * 3 Enum Type Definition
 */


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
#define HWIFI_UNION_PRINT(log,fmt,arg...)   \
            do{printk("%s "fmt"",log->loglevel?:"",##arg);}while(0)
typedef  struct _hwifi_union_log_   hwifi_union_log; 
typedef int (* hwifi_union_log_cb)(hwifi_union_log* log);
struct _hwifi_union_log_
{
    /*the log module name*/
    char* module;
    char* loglevel;
    hwifi_union_log_cb    cb;
    void* data;
    struct list_head list;
};
static inline void HWIFI_UNION_LOG_INIT(hwifi_union_log* log,char* loglevel)
{
    INIT_LIST_HEAD(&log->list);
    log->module = log->module?:"unkown";
    log->loglevel = loglevel;
}
void hwifi_union_log_register(hwifi_union_log* log, void* data);
extern void hwifi_union_log_dump(char* print_loglevel);
/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
typedef enum  _debug_item_
{
    DEBUG_POWER_OFF = 0,
    DEBUG_EXCEPTION_SUMBIT,
    DEBUG_DEVICE_MEM,
    DEBUG_DEVICE_MEM_FROCE,
    DEBUG_DEVICE_FLOWCTRL,
    DEBUG_NET_WORK_EXCETPION,
    DEBUG_ITEM_COUNT
}debug_item;
typedef void (* user_ctrl_debug_cb)(void);
typedef struct _user_ctrl_debug_
{
    char* name;
    char* usage;
    user_ctrl_debug_cb func;
}user_ctrl_debug;
extern int32 userctrl_init(void);
extern void  userctrl_exit(void);
struct work_struct *hwifi_union_get_worker(void);

extern int32  hsdio_test_init_module(void);
extern void  hsdio_test_exit_module(void);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of user_ctrl.h */
