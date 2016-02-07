

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/syscalls.h>
//#include "drv_global.h"
#include "drv_mailbox.h"
#include "drv_mailbox_gut.h"
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_test_linux.h"
#include "drv_mailbox_platform.h"
#include "drv_mailbox_debug.h"
#include "bsp_mailbox.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
#undef  _MAILBOX_FILE_
#define _MAILBOX_FILE_   "tm"
typedef int (*FUNCPTR)(void);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*****************************************************************************
MSG 功能 测试区
*****************************************************************************/
static const unsigned char g_test_mailbox_data_str[] = {"Use this page to search the product documentation and Microsoft Partner sites for information. To access this page, choose Search from the Help menu.\
Security Note Microsoft does not guarantee or endorse search results provided by third-parties, such as Codezone Community participants. Use caution when downloading and installing components from online sources.\
<search box>Enter a word or phrase to search all text or select a previous search string from the drop-down list.\
SearchStarts the search based on the search expression you provided. By default, the search expression is applied to all of the text within all of the topics that are included in the scope of the current filter. Searches return a maximum of 500 matches.\
<filter area>Allows you to narrow your search results by selecting options in the Language, Technology, and Topic Type categories. \
Note Topics that do not use these categories to define their topics are excluded from the search even if the content in those topics does match your search query. Choose Unfiltered to search all topics.\
If your product includes pre-defined search filters, additional Help might appear below.\
Click the minus (-) sign to hide the filter-related UI to provide a larger viewing space for search results.\
LanguageLists the programming languages, such as C#, available to filter your search.\
TechnologyLists technologies, such as .NET Framework and Office Development, available to filter your search.\
Topic TypeLists content types for topics, such as Contains Code and How-Tos, available to filter your search.\
Search ResultsUse this section to review the list of topics, technical articles, and other information returned from a search. For each result, an abstract, source, and other information is displayed.Tip  \
You can choose not to display abstracts for search results in General, Help, Options Dialog Box.\
Searched for: <query>A list of results that meet your search criteria found in the selected Help source, such as Local Help or Questions. Sort by\
Specifies the criteria that results are sorted by. The default options available might not apply to all types of results. Available sorting criteria for a result is listed below the abstract for each item.\
For example, search results from Local Help do not include Topic Date or Rating, so selecting either of these options will not affect the order in which results are displayed for Local Help.\
Descending or AscendingDescending sorts the results from A-Z for the Sort by criteria selected. Ascending sorts the results from Z-A for the Sort by criteria selected.\
Source The physical source of the results, such as Local Help and MSDN Online. Choosing a source displays the search results for that source. You can change the sources searched and the order in which source results are displayed in Online, Help, Options Dialog Box.\
"};

struct tm_mb_handle g_test_mailbox_handle = {0};

/*****************************************************************************
  3 函数定义
*****************************************************************************/

/*PC模拟板侧ST的收邮件通知函数*/
extern void st_mailbox_notify(unsigned int MailCode);

MAILBOX_LOCAL unsigned int test_mailbox_get_bound_width(
                unsigned int data_size,
                unsigned int time_diff)
{
    unsigned int temp_value;
    unsigned int bound_width = 0;

    if ((fls(data_size) + fls(Second_To_Millisecond)) < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
        temp_value = data_size * Second_To_Millisecond;
        if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
            temp_value = temp_value * Second_To_Millisecond;
            bound_width = temp_value / TM_MAILBOX_BYTE_PER_KB / time_diff;
        } else {
            temp_value = temp_value / TM_MAILBOX_BYTE_PER_KB;
            if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
                temp_value = temp_value * Second_To_Millisecond;
                bound_width = temp_value /  time_diff;
            } else {
                temp_value = temp_value / time_diff;
                bound_width = temp_value * Second_To_Millisecond;
            }
        }
    } else {
        temp_value = data_size / TM_MAILBOX_BYTE_PER_KB;
        if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
            temp_value = temp_value * Second_To_Millisecond;
            if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
                temp_value = temp_value * Second_To_Millisecond;
                bound_width = temp_value / time_diff;
            } else {
                temp_value = temp_value / time_diff;
                bound_width = temp_value * Second_To_Millisecond;
            }
        } else {
            temp_value = temp_value / time_diff;
            bound_width = bound_width *  Second_To_Millisecond * Second_To_Millisecond;
        }
    }
    return bound_width;
}
MAILBOX_EXTERN unsigned int test_mailbox_get_time_stamp(void)
{
    static unsigned int time_us = 0;

#ifndef _DRV_LLT_
    time_us = mailbox_get_timestamp();
    //(void)BSP_TIMER_GetRestTime(TIMER_ALL_SLICE_ID, TIMER_UNIT_US, (BSP_U32*)&time_us);
#else
    time_us--;
#endif

    return  time_us;
}


char* test_mailbox_msg_pack(unsigned int recv_code, unsigned int size)
{
    struct mb_st_msg *usr_msg = MAILBOX_NULL;
    int pack_size = (size + sizeof(struct mb_st_msg));

    if((pack_size > (unsigned int)sizeof(g_test_mailbox_data_str)) ||
        (pack_size > MAILBOX_MCU_TEST_BUFF_SIZE)) {
        printk("error test_mailbox_msg_pack ,size too large(0x%x),(%d)\n", (unsigned int)size, _MAILBOX_LINE_);
        return MAILBOX_NULL;
    }

    usr_msg = (struct mb_st_msg *)kcalloc(pack_size, 1, GFP_KERNEL);
    if (MAILBOX_NULL == usr_msg) {
        printk("error test_mailbox_msg_pack ,kcalloc line(%d)\n",_MAILBOX_LINE_);
        return MAILBOX_NULL;
    }

    usr_msg->protect = MAILBOX_BOARDST_USER_PROTECT1;
    usr_msg->length = size;
    usr_msg->back_code = recv_code;
    usr_msg->test_id = MAILBOX_BOARDST_ID_LOOP_SEND;

    memcpy((usr_msg + 1), &g_test_mailbox_data_str[0], size);

    return (char*)usr_msg;
}


void test_mailbox_msg_discard(void * buff)
{
    kfree(buff);
}


void test_mailbox_msg_cb(
                void                   *usr_handle,
                void                   *mail_handle,
                unsigned int           mail_len)
{
    unsigned char* buff = (unsigned char*)kcalloc(mail_len, 1, GFP_KERNEL);
    struct mb_st_msg *st_msg = 0;
    unsigned char* dst_buff = 0;
    struct tm_mb_cb* pCbHandle = usr_handle;
    const unsigned char* SrcBuff = 0;

    if (MAILBOX_NULL == buff) {
        printk("error test_mailbox_msg_cb ,kcalloc line(%d)\n",_MAILBOX_LINE_);
        return;
    }

    mailbox_read_msg_data(mail_handle, buff, &mail_len);

    st_msg = (struct mb_st_msg *)buff;

    if (MAILBOX_BOARDST_USER_PROTECT1 !=  st_msg->protect) {
        /*无效的消息*/
        printk("error test_mailbox_msg_cb: PROTECT \n");
        if ((MAILBOX_NULL != pCbHandle) && (MAILBOX_NULL != pCbHandle->finish_sema)) {
            up(pCbHandle->finish_sema);
        }
        return ;
    }

#ifdef _DRV_LLT_
    /*PC 验证自发自收，不存在真正的 MAILBOX_BOARDST_ID_LOOP_SEND 阶段*/
    st_msg->test_id = MAILBOX_BOARDST_ID_LOOP_BACK;
#endif

    switch (st_msg->test_id)
    {
        case MAILBOX_BOARDST_ID_LOOP_SEND:
            /*接收到发送过来测试消息，通过消息中定义的返回通道返回同样的消息*/
            st_msg->test_id = MAILBOX_BOARDST_ID_LOOP_BACK;
            mailbox_send_msg(st_msg->back_code, (void*)(st_msg), mail_len);
            break;
        case MAILBOX_BOARDST_ID_LOOP_BACK:
            if(MAILBOX_NULL == pCbHandle) 
            {
                printk("error test_mailbox_msg_cb ,user_handle is NULL line(%d)\n", _MAILBOX_LINE_);
                break;
            }
            pCbHandle->back_slice = pCbHandle->back_slice ? pCbHandle->back_slice : test_mailbox_get_time_stamp();
            /*回环过来的消息，验证消息的正确性*/
            dst_buff = (unsigned char*)(st_msg + 1);
            SrcBuff = (g_test_mailbox_data_str);

            /*compare data to check validity*/
            if (0 !=  memcmp((const void*)dst_buff, SrcBuff, st_msg->length ))
            {
                pCbHandle->check_ret = MAILBOX_ERRO;
            }
            else
            {
                pCbHandle->check_ret = MAILBOX_OK;
            }

            pCbHandle->msg_count--;

            if (0 == pCbHandle->msg_count)
            {

                printk("test_mailbox_msg_cb: MsgCount goto 0\n");

                if (MAILBOX_NULL != pCbHandle->finish_sema)
                {
                    up(pCbHandle->finish_sema);
                }
            }
            break ;
        default:
            printk("eror test_mailbox_msg_cb: TestID is Wrong\n");
            if ((MAILBOX_NULL != pCbHandle) && (MAILBOX_NULL != pCbHandle->finish_sema))
            {
                up(pCbHandle->finish_sema);
            }
            break;
    }

    if (MAILBOX_NULL != buff) {
        kfree(buff);
    }
}
void test_mailbox_msg_send_entry(void* _proc)
{
    struct test_mailbox_proc *mb_proc = _proc;
    if (MAILBOX_NULL == mb_proc) {
        printk("test_mailbox_msg_send_entry mb_proc is NULL\n ");
        return ;
    }

    if (MAILBOX_NULL == mb_proc->proc_cb) {
        printk("test_mailbox_msg_send_entry EntryPtr is NULL\n ");
        kfree(mb_proc);
        return ;
    }

    mb_proc->proc_cb(mb_proc->mail_code,
                          mb_proc->recv_code,
                          mb_proc->send_size,
                          mb_proc->msg_num,
                          mb_proc->delay_tick,
                          mb_proc->delay_num,
                          mb_proc->try_times,
                          mb_proc->mb_cb);

    kfree(mb_proc);
}


void test_mailbox_msg_create_task(FUNCPTR entryPtr,
                unsigned int TaskNum,
                unsigned int Priority,
                unsigned int SendMailCode,
                unsigned int RecvMailCode,
                unsigned int SendSize,
                unsigned int SendNum,
                unsigned int DelayTick,
                unsigned int DelayNum,
                unsigned int TryTimes,
                struct tm_mb_cb * pCbHandle
)
{
    struct test_mailbox_proc *pEntryParam = kmalloc(sizeof(struct test_mailbox_proc), GFP_KERNEL);
    char TaskName[255];

    pEntryParam->proc_cb  = (test_mailbox_send_proc_func)entryPtr;
    pEntryParam->mail_code  = SendMailCode;
    pEntryParam->recv_code = RecvMailCode;
    pEntryParam->send_size  = SendSize;
    pEntryParam->msg_num    = SendNum;
    pEntryParam->delay_tick = DelayTick;
    pEntryParam->delay_num  = DelayNum;
    pEntryParam->try_times  = TryTimes;
    pEntryParam->mb_cb = pCbHandle;

    sprintf(TaskName,"test_mailbox_msg_multi_send(%d)",(int)TaskNum);

    kthread_run((int (*)(void *))test_mailbox_msg_send_entry, (void*)pEntryParam, TaskName);
}


void test_mailbox_msg_process(
                unsigned int mail_code,
                unsigned int recv_code,
                unsigned int send_size,
                unsigned int msg_num,
                unsigned int delay_tick,
                unsigned int delay_num,
                unsigned int try_times,
                struct tm_mb_cb * mb_cb)
{
    char* pData = 0;
    unsigned int RetVal = 0;
    unsigned int FullWaitTimes = try_times;

    mb_cb->task_count-- ;

    if (0 == mb_cb->task_count) {
        if (MAILBOX_NULL != mb_cb->sync_sema) {
            mb_cb->start_slice = test_mailbox_get_time_stamp();
            up(mb_cb->sync_sema);
        }
    }

    /*进程同步*/
    if (MAILBOX_NULL != mb_cb->sync_sema) {
        down(mb_cb->sync_sema);
        up(mb_cb->sync_sema);
    }

    while (msg_num) {

        if(MAILBOX_NULL == mb_cb->finish_sema) {
            break;
        }

        pData = test_mailbox_msg_pack(recv_code, send_size);
        if (MAILBOX_NULL == pData) {
            up(mb_cb->finish_sema);
            return;
        }

        /*call test object API*/
        mb_cb->prob_slice = mb_cb->prob_slice ? mb_cb->prob_slice : test_mailbox_get_time_stamp();
        RetVal =  mailbox_send_msg(
            mail_code, pData, send_size + sizeof(struct mb_st_msg));

        if (MAILBOX_FULL == RetVal) {
            msleep(100);
            FullWaitTimes--;

            if (0 !=  FullWaitTimes) {
                continue;
            } else {
                printk("error test_mailbox_msg_process full too many times\n");
                test_mailbox_msg_discard(pData);
                if(MAILBOX_NULL !=mb_cb->finish_sema) {
                    up(mb_cb->finish_sema);
                }
                return;
            }
        } else if (MAILBOX_OK != RetVal ) {
            test_mailbox_msg_discard(pData);
            if (MAILBOX_NULL !=mb_cb->finish_sema) {
                up(mb_cb->finish_sema);
            }
            return;
        }

        if ((0 != delay_tick)  && (0 != delay_num) && (0 == msg_num%delay_num)) {
            msleep(delay_tick);
        }

        msg_num--;

        test_mailbox_msg_discard(pData);
    }

}


unsigned int test_mailbox_msg_get_code(
                unsigned int dst_id,
                unsigned int carrier_id,
                unsigned int *send_code,
                unsigned int *recv_code)
{
    unsigned int src_id = MAILBOX_LOCAL_CPUID;

    *send_code = MAILBOX_MAILCODE_CHANNEL(src_id, dst_id, carrier_id);
    *recv_code = MAILBOX_MAILCODE_CHANNEL(dst_id, src_id, carrier_id);

    return MAILBOX_OK;

}


unsigned int test_mailbox_msg_multi_send(
                unsigned int dst_id,
                unsigned int carrier_id,
                unsigned int task_num,
                unsigned int msg_len,
                unsigned int msg_num,
                unsigned int priority,
                unsigned int delay_tick,
                unsigned int delay_num,
                unsigned int try_times
)
{
    unsigned int ret_val = MAILBOX_OK;
    unsigned int send_code;
    unsigned int recv_code;
    unsigned int bound_width;
    unsigned int data_size;
    unsigned int time_diff;
    struct tm_mb_cb * mb_cb = 0;
    struct tm_mb_handle *mb_handle = &g_test_mailbox_handle;

    if (MAILBOX_OK != test_mailbox_msg_get_code(dst_id, carrier_id, &send_code, &recv_code) ) {
        return MAILBOX_ERRO;
    }

    if ((0 == msg_len) || (0 == msg_num)) {
        printk("error test_mailbox_msg_multi_send input param,line(%d)\n",_MAILBOX_LINE_);
        return MAILBOX_ERRO;
    }

    if (0 == priority) {
        priority = 1;
    }

    /*kthread_run 的线程是异步创建，需要使用信号量和全局变量来保证所有任务的同步 */
    if(mb_handle->mutex) {
        down(mb_handle->mutex);
    } else {
        printk("error test_mailbox_msg_multi_send g_test_MAILBOX_Sem is NULL\n");
        return MAILBOX_ERRO;
    }

    /*创建回调用户句柄*/
    mb_cb = kcalloc(sizeof(struct tm_mb_cb), 1, GFP_KERNEL);
    if (!mb_cb) {
        printk("error test_mailbox_msg_multi_send kcalloc,line(%d)\n",_MAILBOX_LINE_);
        return MAILBOX_ERRO;
    }

    mb_cb->finish_sema = (struct semaphore*)kmalloc(sizeof(struct semaphore), GFP_KERNEL);
    mb_cb->sync_sema = (struct semaphore*)kmalloc(sizeof(struct semaphore), GFP_KERNEL);
    sema_init(mb_cb->finish_sema, 0);
    sema_init(mb_cb->sync_sema, 0);
    mb_cb->check_ret = MAILBOX_CRIT_RET_START;

    /*注册消息响应接收回调*/
    mailbox_reg_msg_cb(recv_code, test_mailbox_msg_cb, mb_cb);

    mb_cb->task_count = task_num;

    data_size = task_num * msg_num * (msg_len + task_num/2);

    /*创建诺干个任务，任务入口函数为本核往其他核发送消息的函数*/
    while (task_num) {
        mb_cb->msg_count += msg_num;
       printk("test_mailbox_msg_multi_send(%d) MailCode(0x%08x), RecevCode(0x%08x), \n\r\
        msg_len(%d), \n\r\
        msg_num(%d),\n\r\
        delay_tick(%d),\n\r\
        delay_num(%d), \n\r\
        priority(%d), \n\r\
        try_times(%d)\n",
       (int)task_num, (unsigned int)send_code, (unsigned int)recv_code, (int)msg_len, (int)msg_num,
       (int)delay_tick, (int)delay_num, (int)priority, (int)try_times);

        test_mailbox_msg_create_task((FUNCPTR)test_mailbox_msg_process,
                     task_num,
                     priority,
                     send_code,
                     recv_code,
                     msg_len,
                     msg_num,
                     delay_tick,
                     delay_num,
                     try_times,
                     mb_cb
                    );

        task_num--;
        msg_len++;
    }

    down(mb_cb->finish_sema);
    kfree(mb_cb->finish_sema);
    mb_cb->finish_sema = MAILBOX_NULL;
    kfree(mb_cb->sync_sema);
    mb_cb->sync_sema = MAILBOX_NULL;

    mb_cb->end_slice= test_mailbox_get_time_stamp();

    time_diff = mailbox_get_slice_diff(mb_cb->start_slice, mb_cb->end_slice);

    bound_width = test_mailbox_get_bound_width(data_size, time_diff);

    printk("total:%d(Byte), bindwidth: %d(KB/Sec), latency: %d (us)\n",
              (int)data_size, (int)bound_width, (int)time_diff);

    if(MAILBOX_OK != mb_cb->check_ret) {
        printk("test_mailbox_msg_multi_send: CheckRet error\n");
        ret_val = MAILBOX_ERRO;
    } else if(0 != mb_cb->msg_count) {
        printk("test_mailbox_msg_multi_send: MsgCount(%d) error\n", (int)mb_cb->msg_count);
        ret_val = MAILBOX_ERRO;
    } else {
        printk("test_mailbox_msg_multi_send: Success!\n");
    }

    kfree(mb_cb);

    if(mb_handle->mutex) {
        up(mb_handle->mutex);
    }
    return ret_val;

}


unsigned int test_mailbox_msg_single_test(
                unsigned int DstId,
                unsigned int MsgLen,
                unsigned int MsgNum,
                unsigned int DelayNum )
{
    unsigned int DelayTick = 2;
    unsigned int Priority  = 100;
    unsigned int TaskNum   = 1;
    unsigned int CarrierID = 1;
    unsigned int TryTimes = 3;

    return test_mailbox_msg_multi_send(
             DstId,
             CarrierID,
             TaskNum,
             MsgLen,
             MsgNum,
             Priority,
             DelayTick,
             DelayNum,
             TryTimes
            );
}


unsigned int test_mailbox_msg_multi_test(
                unsigned int dst_id,
                unsigned int task_num,
                unsigned int msg_num,
                unsigned int delay_num)
{
    unsigned int msg_len = 9;
    unsigned int delay_tick = 1;
    unsigned int priority = 100;
    unsigned int carrier_id = 1;
    unsigned int try_times  = 10;

    return test_mailbox_msg_multi_send(
             dst_id,
             carrier_id,
             task_num,
             msg_len,
             msg_num,
             priority,
             delay_tick,
             delay_num,
             try_times
            );
}


void test_mailbox_init(void)
{
    struct tm_mb_handle *mb_handle = &g_test_mailbox_handle;

    if (TEST_MAILBOX_INIT_FLAG == mb_handle->init_flag) {
        return;
    }
     mb_handle->mutex = (struct semaphore*)kmalloc(sizeof(struct semaphore), GFP_KERNEL);

     sema_init(mb_handle->mutex, 1);

    mb_handle->init_flag = TEST_MAILBOX_INIT_FLAG;
}


void test_mailbox_msg_reg(unsigned int channel_id)
{
    /*通道中的第一个mail_code为保留测试号，这个mail_code和此通道的channel_id一致*/
    unsigned int recv_code = channel_id;

    test_mailbox_init();

    /*注册消息响应接收回调*/
    mailbox_reg_msg_cb(recv_code, test_mailbox_msg_cb, MAILBOX_NULL);

}


/*****************************************************************************
IFC 功能 测试区
*****************************************************************************/
#ifndef _DRV_LLT_

int file_sflags(char *  mode,int *  optr, int* appendflag)
{
    int ret = 1;
    int m;
    int o;

    switch (*mode++)
    {
    case 'r':               /* open for reading */
        ret = 1;
        m = O_RDONLY;
        o = 0;
        break;

    case 'w':               /* open for writing */
        ret = 1;
        m = O_WRONLY;
        o = O_CREAT | O_TRUNC;
        break;

    case 'a':               /* open for appending */
        ret = 1;
        m = O_WRONLY;
        o = O_CREAT | O_APPEND;
        *appendflag = 1;
        break;

    default:                /* illegal mode */
        return (0);
    }

    /* [rwa]\+ or [rwa]b\+ means read and write */

    if ((*mode == '+') || (*mode == 'b' && mode[1] == '+'))
    {
    ret = 1;
    m = O_RDWR;
    }

    *optr = m | o;

    return (ret);
}

int acore_print_p0(const char *str, int len)
{
    printk("%s", str);
    return 0;
}

int acore_fopen(const char *filename, int name_len, char *mode, int mode_len)
{
    int     oflags;
    int     ret = 0;
    int     appendflag = 0;
    unsigned long fs;

    printk("acore_fopen:filename(%s), name_len(%d), mode(%s), mode_len(%d)\n", filename, name_len, mode, mode_len);

    /*通过mode，转换为相应十六进制码*/
    if (file_sflags (mode, &oflags, &appendflag) == 0) {
        return 0;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_open(filename, oflags, S_IRWXU);

    set_fs(fs);

    return ret;

}

int acore_fclose(int fp)
{
    int ret = 0;
    unsigned long fs;
    printk("acore_fclose:fp(0x%x)\n", (unsigned int)fp);

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_close(fp);
    set_fs(fs);

    return ret;
}

int acore_fread( void *ptr, unsigned int count, int fp)
{
    int ret = 0;
    unsigned long fs;

    printk("acore_fread:ptr(0x%x), count(0x%x), fp(0x%x)\n",
                (unsigned int)ptr, (unsigned int)count, (unsigned int)fp);

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_read(fp, (char*)ptr, count);

    set_fs(fs);

    return ret;
}

int acore_fwrite( void *buf, unsigned int count, int fp)
{
    int ret = 0;
    unsigned long fs;

    printk("acore_fwrite:buf(0x%x), count(0x%x), fp(0x%x)\n", (unsigned int)buf, count, fp);

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_write(fp, buf, count);

    set_fs(fs);

    return ret;
}

int acore_fseek(int fp, int offset, int whence )
{
    int ret = 0;
    unsigned long fs;

    printk("acore_fseek:fp(0x%x), offset(0x%x), whence(0x%x)\n", (unsigned int)fp, (unsigned int)offset, whence);

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret= sys_lseek(fp, offset, whence);
    set_fs(fs);

    return ret;
}

int acore_remove(const char *pathname , int path_len)
{
    int ret = 0;
    unsigned long fs = 0;
    fs = get_fs();
    set_fs(KERNEL_DS);

    printk("acore_fseek:pathname(%s), path_len(0x%x)\n", pathname, (unsigned int)path_len);

    ret = sys_unlink(pathname);

    set_fs(fs);

    return ret;
}

int acore_ftell(int fp)
{
    int ret = 0;
    unsigned long fs;

    printk("acore_ftell:fp(0x%x)\n", fp);

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_lseek(fp, 0, 1);
    set_fs(fs);

    return ret;
}

int acore_rename( const char * oldname, int oldlen, const char * newname, int newlen )
{
    int ret = 0;
    unsigned long fs = 0;

    printk("acore_rename:oldname(%s), oldlen(0x%x),newname(%s), newlen(0x%x)\n", oldname, oldlen, newname, newlen );

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_rename(oldname,newname);

    set_fs(fs);

    return ret;
}

int acore_access( const char *filename, int namelen, int amode)
{
    int ret = 0;
    unsigned long fs = 0;

    printk("acore_access:filename(%s), namelen(0x%x), amode(0x%x)\n", filename, namelen, amode );

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_access(filename, amode);

    set_fs(fs);

    return ret;
}

int acore_mkdir( const char *pathname, int pathlen)
{
    int ret = 0;
    unsigned long fs = 0;

    printk("acore_mkdir:pathname(%s), pathlen(0x%x)\n", pathname, pathlen);

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_mkdir(pathname, 0777);

    set_fs(fs);

    return ret;
}

int acore_rmdir( const char *pathname, int pathlen)
{
    int ret = 0;
    unsigned long fs = 0;

    printk("acore_rmdir:pathname(%s), pathlen(0x%x)\n", pathname, pathlen);

    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_rmdir(pathname);
    set_fs(fs);

    return ret;
}


/*A核宏生成函数*/
IFC_GEN_EXEC2( acore_print_p0,
                 IFC_INVAR, const char *  , string , 0,
                 IFC_INCNT, int, strlen, 0
                )

IFC_GEN_EXEC4(acore_fopen, IFC_INVAR, const char *, filename, 0,
                IFC_INCNT, int , name_len , 0,
                IFC_INVAR, char *, mode, 0,
                IFC_INCNT, int , mode_len , 0 )

IFC_GEN_EXEC1(acore_fclose, IFC_INCNT, int , fp , 0 )


IFC_GEN_EXEC3( acore_fwrite,
               IFC_INVAR, void*, filename, 0,
               IFC_INCNT, unsigned int  , count , 0,
               IFC_INCNT, int  , fp , 0 )

IFC_GEN_EXEC3( acore_fread,
                 IFC_OUTVAR, void *, filename, 0,
                 IFC_INCNT, unsigned int, count, 0,
                  IFC_INCNT, int  , fp , 0)

IFC_GEN_EXEC3( acore_fseek,
                 IFC_INCNT, int  , fp , 0,
                 IFC_INCNT, int, offset, 0,
                 IFC_INCNT,  int  , whence , 0)

IFC_GEN_EXEC2( acore_remove,
                 IFC_INVAR, const char *  , pathname , 0,
                 IFC_INCNT, int, path_len, 0)

IFC_GEN_EXEC1( acore_ftell,
                      IFC_INCNT, int , fp , 0)

IFC_GEN_EXEC4( acore_rename,
                IFC_INVAR, const char *, oldname, 0,
                IFC_INCNT, int , oldlen , 0,
                IFC_INVAR, const char *, newname, 0,
                IFC_INCNT, int , newlen , 0)

IFC_GEN_EXEC3( acore_access,
                 IFC_INVAR, const char *  , filename , 0,
                 IFC_INCNT, int, namelen, 0,
                 IFC_INCNT,  int  , amode , 0)

IFC_GEN_EXEC2( acore_mkdir,
                 IFC_INVAR, const char *  , pathname , 0,
                 IFC_INCNT, int, path_len, 0)

IFC_GEN_EXEC2( acore_rmdir,
                 IFC_INVAR, const char *  , pathname , 0,
                 IFC_INCNT, int, path_len, 0)

IFC_GEN_CALL4(MAILBOX_IFC_ACPU_TO_CCPU_FOPEN, ccore_fopen,
                IFC_INVAR, const char *, filename, 0,
                IFC_INCNT, int , name_len , 0,
                IFC_INVAR, const char *, mode, 0,
                IFC_INCNT, int , mode_len , 0
                    )

int acore_memcpy(void * dest,  int dest_len, const void * src,  int src_len)
{
    return (int)memcpy(dest, src, dest_len);
}

//s00227525 add
int acore_memcmp(const void * src, int src_len, const void * dest, int dest_len)
{
    return (int)memcmp(src, dest, dest_len);
}
//s00227525 end

IFC_GEN_EXEC4(acore_memcpy,
                IFC_OUTVAR, void *, dst, 0,
                IFC_INCNT, int, dst_len, 0,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int , src_len , 0 )

//s00227525 add
IFC_GEN_EXEC4(acore_memcmp,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int, src_len, 0,
                IFC_INVAR, const void *, dst, 0,
                IFC_INCNT, int , dst_len , 0)
//s00227525 end

IFC_GEN_CALL4(MAILBOX_IFC_ACPU_TO_CCPU_TEST, ccore_memcpy,
                IFC_OUTVAR, void *, dst, 0,
                IFC_INCNT, int, dst_len, 0,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int , src_len , 0 )

IFC_GEN_CALL4(MAILBOX_IFC_ACPU_TO_MCU_TEST, mcu_memcpy,
                IFC_OUTVAR, void *, dst, 0,
                IFC_INCNT, int, dst_len, 0,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int , src_len , 0 )

//s00227525 add
IFC_GEN_CALL4(MAILBOX_IFC_ACPU_TO_CCPU_TEST_CMP, ccore_memcmp,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int, src_len, 0,
                IFC_INVAR, const void *, dst, 0,
                IFC_INCNT, int , dst_len , 0)

IFC_GEN_CALL4(MAILBOX_IFC_ACPU_TO_MCU_TEST_CMP, mcu_memcmp,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int, src_len, 0,
                IFC_INVAR, const void *, dst, 0,
                IFC_INCNT, int , dst_len , 0)
//s00227525 end

/*******************************************************
*s00227525 start
********************************************************/
IFC_GEN_CALL3(MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST1, ccpu_ifc_base_test1,
                IFC_INFIX, const int *, a, sizeof(int),
                IFC_OUTFIX, int *, b, sizeof(int),
                IFC_BIFIX, int *, c, sizeof(int))

IFC_GEN_CALL6(MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST2, ccpu_ifc_base_test2,
                IFC_INVAR, const int *, a, 0,
                IFC_INCNT, int, len1, 0,
                IFC_OUTVAR, int *, b, 0,
                IFC_INCNT, int, len2, 0,
                IFC_BIVAR, int *, c, 0,
                IFC_INCNT, int, len3, 0)
/*******************************************************
*s00227525 end
********************************************************/

void mailbox_ifc_test_init(void)
{
    //s00227525 add
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_TEST_CMP, acore_memcmp);
    mailbox_ifc_register(MAILBOX_IFC_MCU_TO_ACPU_TEST_CMP, acore_memcmp);

    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_TEST,  acore_memcpy);
    mailbox_ifc_register(MAILBOX_IFC_MCU_TO_ACPU_TEST,  acore_memcpy);

    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_PRINT,  acore_print_p0);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_FOPEN,  acore_fopen);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_FCLOSE, acore_fclose);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_FREAD,  acore_fread);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_FWRITE, acore_fwrite);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_FSEEK,  acore_fseek);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_REMOVE, acore_remove);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_FTELL,  acore_ftell);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_RENAME, acore_rename);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_ACCESS, acore_access);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_MKDIR,  acore_mkdir);
    mailbox_ifc_register(MAILBOX_IFC_CCPU_TO_ACPU_RMDIR,  acore_rmdir);
}


IFC_GEN_CALL1(MAILBOX_IFC_ACPU_TO_CCPU_PRINT, ccore_print_num,
                 IFC_INCNT, int, num, 0)



/*****************************************
 *s00227525 start
 *****************************************/
struct acpu_ifc_test_data{
    unsigned int l;
    unsigned int n;
};

struct acpu_ifc_test_data aitd = {0, 0};

typedef int (*TASK_IFC_FUNC)(struct acpu_ifc_test_data *para);

int acpu_ifc_test_ccpu_entry(unsigned int len, unsigned int loop_num)
{
    unsigned char *src = NULL;
    unsigned char *dst = NULL;
    unsigned int i = 0;
    unsigned int flag = 0;
    int result = 0;

    if(len == 0){
        printk("Error: acpu_ifc_test_ccpu: para must greater than 0!\n");
        return 1;
    }

    src = (char *)kmalloc(len, GFP_KERNEL);
    if(NULL == src){
        printk("Error: acpu_ifc_test_ccpu: src buff malloc fail!\n");
        return 1;
    }

    dst = (char *)kmalloc(len, GFP_KERNEL);
    if(NULL == dst){
        printk("Error: acpu_ifc_test_ccpu: dst buff malloc fail!\n");
        kfree(src);
        return 1;
    }

    if(loop_num == 0){
        flag = 1;
    }
    while((flag == 1) || (i < loop_num)){
        memset(src, 0x5a, len);
        memset(dst, 0, len);

        ccore_memcpy(dst, len, src, len, IFC_WAIT_FOREVER);

        result = ccore_memcmp(src, len, dst, len, IFC_WAIT_FOREVER);

        if(result == 0){
            i++;
            continue;
        }
        else{
            printk("Error: acpu_ifc_test_ccpu: test fail\n");
            printk("loop num = %d\n", i);
            return 1;
        }
    }
    printk("acpu_ifc_test_ccpu: test pass\n");
    kfree(src);
    kfree(dst);
    return 0;
}

int acpu_ifc_test_ccpu(struct acpu_ifc_test_data *para)
{
    unsigned int len = para->l;
    unsigned int loop_num = para->n;
    int result = acpu_ifc_test_ccpu_entry(len, loop_num);
    return result;
}

int acpu_ifc_test_mcu_entry(unsigned int len, unsigned int loop_num)
{
    unsigned char *src = NULL;
    unsigned char *dst = NULL;
    unsigned int i = 0;
    unsigned int flag = 0;
    int result = 0;

    if(len == 0){
        printk("Error: acpu_ifc_test_mcu: para must greater than 0!\n");
        return 1;
    }

    src = (char *)kmalloc(len, GFP_KERNEL);
    if(NULL == src){
        printk("Error: acpu_ifc_test_mcu: src buff malloc fail!\n");
        return 1;
    }

    dst = (char *)kmalloc(len, GFP_KERNEL);
    if(NULL == dst){
        printk("Error: acpu_ifc_test_mcu: dst buff malloc fail!\n");
        kfree(src);
        return 1;
    }

    if(loop_num == 0){
        flag = 1;
    }
    while((flag == 1) || (i < loop_num)){
        memset(src, 0xaa, len);
        memset(dst, 0, len);

        mcu_memcpy(dst, len, src, len, IFC_WAIT_FOREVER);

        result = mcu_memcmp(src, len, dst, len, IFC_WAIT_FOREVER);

        if(result == 0){
            i++;
            continue;
        }
        else{
            printk("Error: acpu_ifc_test_mcu: test fail\n");
            printk("loop num = %d\n", i);
            return 1;
        }
    }
    printk("acpu_ifc_test_mcu: test pass\n");
    kfree(src);
    kfree(dst);
    return 0;
}

int acpu_ifc_test_mcu(struct acpu_ifc_test_data *para)
{
    unsigned int len = para->l;
    unsigned int loop_num = para->n;
    int result = acpu_ifc_test_mcu_entry(len, loop_num);
    return result;
}

 int acpu_ifc_test_main(unsigned int len, unsigned int loop_num)
 {
    struct task_struct *task_ccpu = NULL;
    struct task_struct *task_mcu = NULL;
    char TaskName[255] = {0, 0};

    printk("acpu_ifc_test_main start:\n");
    aitd.l = len;
    aitd.n = loop_num;
    sprintf(TaskName, "acpu_ifc_test_ccpu");
    task_ccpu = kthread_run((int (*)(void *))acpu_ifc_test_ccpu, (void*)&aitd, TaskName);
    if(IS_ERR(task_ccpu)) {
        printk("Error: acpu_ifc_test_ccpu start fail!\n");
        return 1;
    }
    else{
        printk("acpu_ifc_test_ccpu start.\n");
    }

    sprintf(TaskName, "acpu_ifc_test_mcu");
    task_mcu = kthread_run((int (*)(void *))acpu_ifc_test_mcu, (void*)&aitd, TaskName);
    if(IS_ERR(task_mcu)) {
        printk("Error: acpu_ifc_test_mcu start fail!\n");
        return 1;
    }
    else{
        printk("acpu_ifc_test_mcu start.\n");
    }
    printk("acpu_ifc_test_main end.\n");
    return 0;
}

int acpu_ifc_base_test1_ccpu(int aa, int bb, int cc)
{
    int a = aa, b = bb, c = cc;
    ccpu_ifc_base_test1(&a, &b, &c, IFC_WAIT_FOREVER);
    if(aa == b && c == cc + aa){
        printk("test pass\n");
        return 0;
    }
    else{
        printk("Error: test fail!\n");
        printk("a = %d, b = %d, c = %d\n", a, b, c);
        return 1;
    }
}

int acpu_ifc_base_test2_ccpu(int count)
{
    int *a, *b, *c;
    int i = 0;
    int size = count * sizeof(int);
    a = (int *)kmalloc(size, GFP_KERNEL);
    b = (int *)kmalloc(size, GFP_KERNEL);
    c = (int *)kmalloc(size, GFP_KERNEL);

    if(a == NULL || b == NULL || c == NULL){
        printk("Error: malloc fail\n");
        return 1;
    }

    for(i=0; i<count; i++){
        a[i] = 1;
        b[i] = 0;
        c[i] = 8;
    }
    ccpu_ifc_base_test2(a, size, b, size, c, size, IFC_WAIT_FOREVER);

    for(i=0; i<count; i++){
        if(a[i] != b[i] || c[i] != 9){
            printk("Error: test fail!\n");
            printk("pos = %d, b[%d] = %d, c[%d] = %d\n", i, i, b[i], i, c[i]);
            return 1;
        }
    }
    printk("test pass\n");
    kfree(a);
    kfree(b);
    kfree(c);
    return 0;
}
 /*****************************************
 *s00227525 end
 *****************************************/
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

