

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_debug.h"
#include "drv_mailbox_gut.h"
#include "excDrv.h"
#include "../../ipc_hi6xxx/bsp_drv_ipc.h"
#include <linux/hisi/hi6xxx_mntn.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
#undef  _MAILBOX_FILE_
#define _MAILBOX_FILE_   "dbg"

#define MAILBOX_LOG_PATH               ("/data/hisi_logs/ap_log/mailbox")
#define MAILBOX_LOG_FILE               ("mailbox_mntn")
#define MAILBOX_LOG_FILE_PATH_SIZE     (512)
#define MAILBOX_LOG_FILE_NAME_SIZE     (40)
#define MAILBOX_LOG_FILE_SIZE      	   (0x100000)
#define MAILBOX_LOG_FILE_CNT           (20)/*保存文件个数*/
#define MAILBOX_LOG_NOT_DEPEND_CNT     (0)
#define MAILBOX_LOG_DEPEND_CNT         (1)

struct mntn {
    unsigned long buff;
    unsigned int size;
};

struct work_mntn {
        struct work_struct work;
        struct mntn mntn_mb;
        struct mntn mntn_ipc;
        unsigned int mailcode;
};

/*****************************************************************************
  2 函数引用申明
*****************************************************************************/

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
static unsigned int mailbox_log_work_cnt;
static struct workqueue_struct *mailbox_log_wq;
#if (MAILBOX_LOG_LEVEL != MAILBOX_LOG_NONE)
static unsigned int mailbox_log_assert_flag = 0;
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if (MAILBOX_LOG_LEVEL != MAILBOX_LOG_NONE)
void mailbox_log_assert_set(unsigned int flag)
{
    mailbox_log_assert_flag = flag;
}


MAILBOX_EXTERN int mailbox_log_erro(
                unsigned int   err_no,
                unsigned long   param1,
                unsigned long   param2,
                unsigned int   line_no,
                char*           file_name)
{
    struct mb *mb      = &g_mailbox_handle;
    struct mb_log  *record = &mb->log_array[0];
    unsigned int log_out = MAILBOX_FALSE;
    char * erro_str = MAILBOX_NULL;

    /*1.记录最近错误轨迹*/
    record[mb->log_prob].erro_num = err_no;
    record[mb->log_prob].line    = line_no;
    record[mb->log_prob].param1  = param1;
    record[mb->log_prob].param2  = param2;
    record[mb->log_prob].file = (const char* )file_name;
    mb->log_prob = ((MAILBOX_ERRO_ARRAY_NUM - 1) == mb->log_prob) ? (0) :
                                                            (mb->log_prob + 1);

    /*2.根据告警等级打印。*/
#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_CRITICAL)
    if ((err_no > MAILBOX_CRIT_RET_START) && (err_no < MAILBOX_CRIT_RET_END)) {
        erro_str = "mb Critical!";
        log_out = MAILBOX_TRUE;
        mailbox_assert(err_no);
    }
#endif

#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_CRITICAL)
    if((err_no > MAILBOX_ERR_RET_START) && (err_no < MAILBOX_ERR_RET_END)) {
        erro_str = "mb error!";
        log_out = MAILBOX_TRUE;
    }
#endif

#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_WARNING)
    if((err_no > MAILBOX_WARNING_RET_START) && (err_no < MAILBOX_WARNING_RET_END)) {
        erro_str = "mb warning!";
        log_out = MAILBOX_TRUE;
    }
#endif

#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_INFO)
    if((err_no > MAILBOX_INFO_RET_START) && (err_no < MAILBOX_INFO_RET_END)) {
        erro_str = "mb info!";
        log_out = MAILBOX_TRUE;
    }
#endif

    if (MAILBOX_FULL == err_no) {
        mailbox_out((KERN_ERR "mb(%lu) full !"RT, param1));
        log_out = MAILBOX_TRUE;
    } else if (MAILBOX_NOT_READY == err_no) {
        mailbox_out((KERN_ERR "remote mb(%lu) not ready!"RT, param1));
        log_out = MAILBOX_TRUE;
    }

    if (MAILBOX_TRUE == log_out) {
        mailbox_out((KERN_INFO "%s:0x%08x, param1:%lu, param2:%lu, (line:%d),(file:%s)"RT,
            erro_str, err_no,  param1,  param2, (unsigned int)line_no, file_name));/*printk set info, not error*/
    }

    /*如果邮箱fifo 空闲大小低于水线，则主动发起复位*/
    if((MAILBOX_ERR_GUT_MAILBOX_RECEIVE_FULL == err_no) &&  (1 == mailbox_log_assert_flag))
    {
        systemError(BSP_MODU_MAILBOX, EXCH_S_MAILBOX, err_no, 0, 0);
    }

    return (int)err_no;
}
#endif

#ifdef MAILBOX_OPEN_MNTN
static unsigned int g_mb_trans_time_limit = MAILBOX_MAIL_TRANS_TIME_LIMIT;
static unsigned int g_mb_deal_time_limit = MAILBOX_MAIL_DEAL_TIME_LIMIT;
static unsigned int g_mb_sche_limit = MAILBOX_MAIL_SCHE_TIME_LIMIT;

/*打开文件  调用者不能访问返回值值的成员变量*/
static int mailbox_log_dopen(const char *dirname)
{
	int fd;

	fd = sys_open(dirname, O_DIRECTORY, 0);
	if (fd < 0) { /* if dir is not exist,then create new dir */
		fd = sys_mkdir(dirname, 0774);
		if (fd < 0) {
		    mailbox_out((KERN_INFO "mailbox_log_dopen:mkdir fd is 0x%x\n", fd));;
			return fd;
		}
	}
    (void)sys_close((unsigned int)fd);
	return 0;
}

static int mailbox_log_fopen( const char *filename, int flags )
{
    int     fd = 0;
    int len = 0, ret=0;
    unsigned long fs;

    if (NULL == filename) {
        mailbox_out((KERN_ERR "mailbox_log_fopen:the input para is error \n"));
        return MAILBOX_ERRO;
    }

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    /*对于V9，ret 实际类型为FILE*，当前传输地址*/
    fd = sys_open((const char __user *)filename, flags, S_IRWXU);
    if (fd < 0) {
        mailbox_out((KERN_ERR "mailbox_log_fopen:sys_open op 1st fail, filename:%s, fd is %x \n", \
                                    filename, fd));
        goto error;
    }
    /*check the file's size, if big than 1M, delete it*/
    len = sys_lseek(fd, 0, SEEK_END);
    if (len < 0) {
    mailbox_out((KERN_ERR "mailbox_log_fopen:seek failed, filename:%s, fd is %x \n", \
                                    filename, fd));
    sys_close(fd);                                    
     goto error;
    }
    if(MAILBOX_LOG_FILE_SIZE <= len)
    {
	sys_close(fd);
	ret = sys_unlink(filename);
	if (0 != ret) {
           mailbox_out((KERN_ERR "mailbox_log_fopen:sys_unlink failed, filename:%s, fd is %x \n", \
                                    filename, fd));
    	goto error;
	}
    /*recreate log file*/
    fd = sys_open((const char __user *)filename, flags, S_IRWXU);
    if (fd < 0) {
    	mailbox_out((KERN_ERR "mailbox_log_fopen:sys_open op 2nd  failed, filename:%s, fd is %x \n", \
                                        filename, fd));
        goto error;
        }        
    }

    set_fs(fs);
    return fd;
    
error:
    set_fs(fs);
    return MAILBOX_ERRO;
}

/*关闭文件*/
static int mailbox_log_fclose(unsigned int fd)
{
    int ret = 0;
    unsigned long fs;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_close(fd);
    if (ret < 0) {
        mailbox_out((KERN_ERR "mailbox_log_fclose:the sys_close ops fail, the ret is %x\n",ret));
        return MAILBOX_ERRO;
    }

    set_fs(fs);

    return MAILBOX_OK;
}

/*写文件*/
static int mailbox_log_fwrite( void *buf, unsigned int count, unsigned int fd )
{
    int ret = 0;
    unsigned long fs;

    if ((NULL == buf) || (0 == count) || (MAILBOX_LOG_FILE_SIZE <= count)) {
        mailbox_out((KERN_ERR "mailbox_log_fwrite:the input para is error\n"));
        return MAILBOX_ERRO;
    }

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_write(fd, buf, count);
    if (ret < 0) {
        mailbox_out((KERN_ERR "mailbox_log_fwrite:the sys_write ops fail,the ret is %x\n", ret));
        return MAILBOX_ERRO;
    }
    else {
        ret = sys_fsync(fd);
        if (ret < 0) {
            mailbox_out((KERN_ERR "mailbox_log_fwrite:sys_fsync fail,the ret is %x\n", ret));
            return MAILBOX_ERRO;
        }
    }

    set_fs(fs);

    return MAILBOX_OK;
}

/*log文件保存函数*/
static int mailbox_log_fsave(char *dir, char *file, void *data, int length)
{
    int ret = 0;
    int ret_t = 0;
    int fd = 0;
    char *filename = (char *)NULL;

    if((strlen(dir) + strlen("/") + strlen(file)) >= MAILBOX_LOG_FILE_PATH_SIZE) {
        return -1;
    }

    ret= mailbox_log_dopen(dir);
    if(ret < 0) {
        return -1;
    }

    filename = (char *)kmalloc(MAILBOX_LOG_FILE_PATH_SIZE, GFP_KERNEL);
    if(NULL == filename) {
        return -1;
    }
    memset(filename, 0, MAILBOX_LOG_FILE_PATH_SIZE);
    strncpy(filename, dir, strlen(dir));
    *(filename + strlen(dir)) = '\0';
    strncat(filename, "/", strlen("/"));
    strncat(filename, file, strlen(file));
    fd = mailbox_log_fopen(filename, (O_RDWR | O_CREAT | O_APPEND | O_CLOEXEC));
    if (MAILBOX_ERRO == fd) {
        mailbox_out((KERN_ERR "mailbox_log_fopen error\n"));
        ret_t = -1;
        goto err2;
    }
    ret = mailbox_log_fwrite(data, length, fd);
    if (MAILBOX_OK != ret) {
        mailbox_out((KERN_ERR "mailbox_log_fwrite error\n"));
        ret_t = -1;
        goto err1;
    }

err1:
    ret = mailbox_log_fclose(fd);
    if (MAILBOX_OK != ret) {
        mailbox_out((KERN_ERR "mailbox_log_fclose error\n"));
    }

err2:
    if(NULL != filename) {
        kfree(filename);
    }

    return ret_t;
}

static void mailbox_log_fprintf(char* fname, unsigned int flen, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsnprintf(fname, flen-1, fmt, args);/* [false alarm]:误报 */
    va_end(args);

    fname[flen - 1] = '\0';
}

static void mailbox_log_work_proc(struct work_struct *log_work)
{
    char file_name[MAILBOX_LOG_FILE_NAME_SIZE];
    struct work_mntn *log_work_mntn = container_of(log_work, struct work_mntn, work);

    /*save mntn info to file*/
    memset((void*)file_name, 0x0, sizeof(file_name));

    mailbox_log_fprintf(file_name, MAILBOX_LOG_FILE_NAME_SIZE, \
                    "%s_%08x.bin", MAILBOX_LOG_FILE, log_work_mntn->mailcode);

    mailbox_log_fsave(MAILBOX_LOG_PATH, file_name, \
                    (void*)(log_work_mntn->mntn_mb.buff), log_work_mntn->mntn_mb.size);
    kfree((void*)(log_work_mntn->mntn_mb.buff));

    mailbox_log_fsave(MAILBOX_LOG_PATH, file_name, \
                    (void*)(log_work_mntn->mntn_ipc.buff), log_work_mntn->mntn_ipc.size);
                    
    kfree((void*)log_work_mntn);

    return;
}

static int mailbox_log_work_submit(unsigned int mailcode, int flag)
{
    struct mb   *mb   = MAILBOX_NULL;
    struct mb_buff  *mbuff = MAILBOX_NULL;
    struct work_mntn *log_work_mntn = MAILBOX_NULL;
    char *mntn_mb_buf = NULL;
    char *mntn_ipc_buf = NULL;
    unsigned long mntn_mb_addr = 0;
    unsigned int mntn_mb_size = 0;
    unsigned long mntn_ipc_addr = 0;
    unsigned int mntn_ipc_size = 0;

    /*is not called durning int context*/
    /*if (mailbox_int_context()) {
        mailbox_out((KERN_ERR "mailbox_log_work_submit:is called durning int context\r\n"));
        return -1;
    }*/

    /*do nothing when file count overflow*/
    if((MAILBOX_LOG_DEPEND_CNT == flag)  \
        &&(mailbox_log_work_cnt >= MAILBOX_LOG_FILE_CNT)) {
        return 0;
    }
    /*file count*/
    mailbox_log_work_cnt++;

    /*do nothing when log's work is pending or work*/
    /*ret_val = work_busy(&log_work_mntn->work);
    if (0 != ret_val) {
        mailbox_out((KERN_ERR "mailbox_log_work_submit:log work stat[0x%x], wait a moment\r\n", ret_val));
        return 0;
    }*/

    /*get mailbox handle*/
    mb =  mailbox_get_mb();
    if (MAILBOX_NULL == mb) {
        mailbox_out((KERN_ERR "mailbox_log_work_submit:fail to get mailbox handle\r\n"));
        return -1;
    }

    /*get mailbox buffer*/
    mbuff = mailbox_get_channel_handle(mb, mailcode);
    if (MAILBOX_NULL == mbuff) {
        mailbox_out((KERN_ERR "mailbox_log_work_submit:fail to get mailbox buffer\r\n"));
        return -1;
    }

    log_work_mntn = kmalloc(sizeof(struct work_mntn), GFP_ATOMIC);
    if (MAILBOX_NULL == log_work_mntn) {
        mailbox_out((KERN_ERR "mailbox_log_work_submit:fail to malloc for work-mntn\r\n"));
        return -1;
    }

    /*malloc memory and copy mailbox mntn info*/
    mntn_mb_addr = (unsigned long)(&(mbuff->mntn));
    mntn_mb_size = sizeof(struct mb_mntn);
    mntn_mb_buf = kmalloc(mntn_mb_size, GFP_ATOMIC);
    if (MAILBOX_NULL == mntn_mb_buf) {
        mailbox_out((KERN_ERR "mailbox_log_work_submit:fail to malloc for mailbox mntn\r\n"));
        kfree(log_work_mntn);
        return -1;
    }

    memcpy((void*)mntn_mb_buf, (void*)mntn_mb_addr, mntn_mb_size);
    log_work_mntn->mntn_mb.buff = (unsigned long)mntn_mb_buf;
    log_work_mntn->mntn_mb.size = mntn_mb_size;

    /*copy ipc mntn info directly through  g_stIpcIntDbgInfo instead of  kmalloc then memcpy in case kmalloc fail */
    ipc_debug_int_info(&mntn_ipc_addr, &mntn_ipc_size);
    log_work_mntn->mntn_ipc.buff = (unsigned long)mntn_ipc_addr;
    log_work_mntn->mntn_ipc.size = mntn_ipc_size;

    /*fill mailcode*/
    log_work_mntn->mailcode = mailcode;

    /*init the work of mailbox log*/
    INIT_WORK(&(log_work_mntn->work), mailbox_log_work_proc);

    /*schdule work of workqueue*/
    queue_work(mailbox_log_wq, &(log_work_mntn->work));


    return 0;
}

void mailbox_log_init(void)
{
    /*create the work-queue of mailbox log*/
    mailbox_log_wq = create_singlethread_workqueue("mailbox_log_workqueue");
    BUG_ON(!mailbox_log_wq);
}

/*对总时间和最大单次时间进行统计*/
static void mailbox_statistic_slice(
                struct mb_slice* slice,
                unsigned int mailcode,
                unsigned int threslhold,
                unsigned int erro_code
                )
{
    unsigned int slice_diff;
    unsigned int slice_end = (unsigned int)mailbox_get_timestamp();
    unsigned int slice_start = slice->start;

    slice_diff = mailbox_get_slice_diff(slice_start, slice_end);
    /*if (slice_diff < 0) {
        return;
    }*/

    /*记录回调历史总耗时*/
    slice_end = slice->total;
    slice->total += slice_diff;
    if (slice_end > slice->total) {
        slice->overflow = MAILBOX_TRUE; /*记录计时溢出，数据无效*/
    }

    /*记录最大回调耗时*/
    if (slice_diff > slice->max) {
        slice->max = slice_diff;
        slice->code = mailcode;
    }

    /*记录耗时值到最近记录里*/
    slice->rdiff[slice->rdiff_index] = slice_diff;
    slice->rdiff_index++;
    slice->rdiff_index = (MAILBOX_RECORD_RECENT_NUM == slice->rdiff_index) ? 0 : slice->rdiff_index;

    if (slice_diff >= threslhold) {
        #ifndef _DRV_LLT_
        mailbox_logerro_p2(erro_code, slice_diff, mailcode);
        #endif
        if(MAILBOX_WARNING_TRANSPORT_TIME_OUT == erro_code \
            && check_himntn(HIMNTN_MBX_ERRLOG_SAVE)) {
            (void)mailbox_log_work_submit(mailcode ,MAILBOX_LOG_DEPEND_CNT);
        }
    }
}

/*记录中断响应，调度处理之前的时间点(IPC中断开始响应的时刻点)*/
void mailbox_record_sche_send(void *priv)
{
    struct mb_buff *mbuf = (struct mb_buff *)priv;
    mbuf->mntn.sche.start = (unsigned long)mailbox_get_timestamp();
}

/*记录中断响应后，调度处理的时间点，并进行统计(MB线程得到调度的时刻点)*/
void mailbox_record_sche_recv(void *priv)
{
    struct mb_buff *mbuf = (struct mb_buff *)priv;

    /*开始调度时需要将上轮邮箱记录的处理邮件序号清0*/
    mbuf->mntn.per_sche_mail_cnt = 0;

    mailbox_statistic_slice(&mbuf->mntn.sche, mbuf->channel_id,
                    g_mb_sche_limit,
                    MAILBOX_WARNING_SCHE_TIME_OUT);
}

/*可维可测: 记录剩余空间最小值*/
void mailbox_record_send(
                struct mb_mntn* mntn,
                unsigned int mailcode,
                unsigned int time_stamp,
                unsigned long mail_addr)
{
    struct mb_queue *m_queue = &mntn->mbuff->mail_queue;
    unsigned int size_left = (unsigned int)mailbox_queue_left(m_queue->rear, m_queue->front, m_queue->length);

    /*可维可测: 记录剩余空间最小值*/
    if (size_left < mntn->peak_traffic_left){
        mntn->peak_traffic_left = size_left;
    }
    /*更新可维可测信息. 写通道只有发送时间和Use ID*/
    mntn->track_array[mntn->track_prob].send_slice = time_stamp;
    mntn->track_array[mntn->track_prob].mail_addr  = mail_addr;
    mntn->track_array[mntn->track_prob].use_id     = mailbox_get_use_id(mailcode);
    mntn->track_prob = ((MAILBOX_RECORD_USEID_NUM - 1) == mntn->track_prob) ?
                                                   (0) : (mntn->track_prob + 1);
}

/*记录发送核IPC中断触发到接受核中断响应之间的时间(邮件开始读取的时刻点)*/
void mailbox_record_transport(
                struct mb_mntn* mntn,
                unsigned int mailcode,
                unsigned int write_slice,
                unsigned int read_slice,
                unsigned long mail_addr)
{
    struct mb_queue *m_queue = &mntn->mbuff->mail_queue;
    unsigned int  size_left = (unsigned int)mailbox_queue_left(m_queue->rear, m_queue->front, m_queue->length);

    /*本轮调度后邮箱处理邮件个数加1*/
    mntn->per_sche_mail_cnt++;

    /*可维可测: 记录剩余空间最小值*/
    if (size_left < mntn->peak_traffic_left){
        mntn->peak_traffic_left = size_left;
    }

    if (size_left < (m_queue->length >> 3) ){
        mailbox_logerro_p2(MAILBOX_ERR_GUT_MAILBOX_RECEIVE_FULL, size_left, mailcode);
        if (check_himntn(HIMNTN_MBX_ERRLOG_SAVE)){
            (void)mailbox_log_work_submit(mailcode, MAILBOX_LOG_NOT_DEPEND_CNT);
        }
    }

    /*记录收到的邮件use id*/
    mntn->track_array[mntn->track_prob].use_id        =
                                   (unsigned short)mailbox_get_use_id(mailcode);
    mntn->track_array[mntn->track_prob].send_slice    =  write_slice;
    mntn->track_array[mntn->track_prob].recv_slice =  read_slice;
    mntn->track_array[mntn->track_prob].mail_addr = mail_addr;
    mntn->track_prob = ((MAILBOX_RECORD_USEID_NUM - 1)==mntn->track_prob) ?
                                                   (0) : (mntn->track_prob + 1);

    mntn->trans.start = write_slice;
    mailbox_statistic_slice(&mntn->trans, mailcode,
                g_mb_trans_time_limit,
                MAILBOX_WARNING_TRANSPORT_TIME_OUT);

}

/*记录响应核邮箱用户回调函数执行时间(邮件对应回调的时刻点)*/
void mailbox_record_receive(
                struct mb_mntn* mntn,
                unsigned int mailcode,
                unsigned int slice_start)
{
    mntn->deal.start = slice_start;
    mailbox_statistic_slice(&mntn->deal, mailcode,
                    g_mb_deal_time_limit,
                    MAILBOX_WARNING_RECEIVE_TIME_OUT);
}

/*清除某个邮箱通道的可维可测信息*/
static void mailbox_clear_mntn( struct mb_mntn *mntn, int clear)
{
    struct mb_buff   *mbuff;
    if (clear) {
        mbuff = mntn->mbuff;
        mailbox_memset(mntn, 0x00, sizeof(struct mb_mntn));
        mntn->mbuff = mbuff;
        mntn->peak_traffic_left = MAILBOX_QUEUE_LEFT_INVALID;
    }
}

/*****************************************************************************/
/*以下是可维可测函数*/

MAILBOX_LOCAL void mailbox_show_general(struct mb_cfg *cfg)
{
    struct mb_head   *pBoxHead   =  (struct mb_head*)(cfg->head_addr);

    /*总体信息*/
    mailbox_out((KERN_ERR "Max Id,      HeadAddr,    DataAddr,     DataSize,   IntSrcId"RT));
    mailbox_out((KERN_ERR "0x%08x,  0x%08x,  0x%08x,   0x%08x, %04d"RT,(unsigned int)cfg->butt_id,
                    (unsigned int)cfg->head_addr, (unsigned int)cfg->data_addr,
                    (unsigned int)cfg->data_size, (unsigned int)cfg->int_src));
    mailbox_out((KERN_ERR "Head information:"RT));

    /*打印此邮箱的邮箱头内容。*/
    mailbox_out((KERN_ERR "Head Front: 0x%x (0x%08x)"RT,  (unsigned int)pBoxHead->ulFront,
                (unsigned int)(cfg->data_addr + (pBoxHead->ulFront * sizeof(unsigned long)))));
    mailbox_out((KERN_ERR "Head Rear: 0x%x (0x%08x)"RT, (unsigned int)pBoxHead->ulRear,
                (unsigned int)(cfg->data_addr + (pBoxHead->ulRear * sizeof(unsigned long)))));
    mailbox_out((KERN_ERR "Head Frontslice: 0x%x"RT, (unsigned int)pBoxHead->ulFrontslice));
    mailbox_out((KERN_ERR "Head Rearslice: 0x%x"RT,  (unsigned int)pBoxHead->ulRearslice));
    mailbox_out((KERN_ERR ":-------------------------------------------------------------:"RT));

}


MAILBOX_LOCAL void mailbox_show_receive(struct mb_buff *mbuf)
{
    struct mb_mntn      *mntn         =  &(mbuf->mntn);    /*此邮箱通道的可维可测数据*/
    struct mb_cb        *callback     =  mbuf->read_cb;    /*此邮箱通道的功能回调函数队列*/
    unsigned int        max_use      =  mailbox_get_use_id(mbuf->config->butt_id);
    unsigned int        i            =  0;

    mailbox_out((KERN_ERR ":---------------------------------------------:"RT));
    mailbox_out((KERN_ERR "Receive info:"RT));

    /*打印挂接在此邮箱通道上的应用号及应用回调函数地址*/
    mailbox_out((KERN_ERR "Mail Read Call Back show:"RT));
    mailbox_out((KERN_ERR "Use Id,  Call Back,     User Handle"RT));
    while (i < max_use) {
        mailbox_out((KERN_ERR "%d,       0x%08x,    0x%08x"RT, (unsigned int)i, (unsigned int)callback->func,
                                                (unsigned int)callback->handle));
        callback++;
        i++;
    }

    /*平均调度等待及最大调度等待时间*/
    if (MAILBOX_TRUE != mntn->sche.overflow) {
        mailbox_out((KERN_ERR "Schedule Avg. slice:%4d, total:%d"RT,
      (unsigned int)((mbuf->seq_num + 1) ? (mntn->sche.total)/(mbuf->seq_num + 1) : 0) , (unsigned int)(mbuf->seq_num)));
    } else {
        mailbox_out((KERN_ERR "Schedule Avg. data overflow "RT));
    }
    mailbox_out((KERN_ERR "Schedule Max. slice:%4d,  Use ID:0x%08x"RT, (unsigned int)(mntn->sche.max),
                                            (unsigned int)(mntn->sche.code)));

    /*平均传送及最大传送时间*/
    if (MAILBOX_TRUE != mntn->trans.overflow) {
        mailbox_out((KERN_ERR "Transfers Avg. slice:%4d, total:%d"RT,(int)((mbuf->seq_num + 1) ?
                ((mntn->trans.total)/(mbuf->seq_num + 1)) : 0 ),  (unsigned int)(mbuf->seq_num)));
    } else {
        mailbox_out((KERN_ERR "Transfers Max. data overflow"RT));
    }
    mailbox_out((KERN_ERR "Transfers Max. slice:%4d,  Use ID:0x%08x"RT, (unsigned int)(mntn->trans.max),
                (int)(mntn->trans.code)));/*lint -e539*/

    /*平均处理及最大处理时间*/
    if (MAILBOX_TRUE != mntn->deal.overflow) {
        mailbox_out((KERN_ERR "Call Back Avg. slice:%4d, total:%d"RT,(unsigned int)((mbuf->seq_num + 1) ?
            (mntn->deal.total)/(mbuf->seq_num + 1) : 0), (unsigned int)(mbuf->seq_num)));
    } else {
        mailbox_out((KERN_ERR "Call Back Avg. data overflow"RT));
    }
    mailbox_out((KERN_ERR "Call Back Max. slice:%4d,  Use ID:0x%08x"RT,
            (unsigned int)(mntn->deal.max), (unsigned int)(mntn->deal.code)));


    mailbox_out((KERN_ERR ":---------------------------------------------:"RT));
}
MAILBOX_LOCAL void mailbox_show_detail(struct mb *mb,
                struct mb_buff *mbuf,
                int clear)
{
    struct mb_mntn        *mntn         =  MAILBOX_NULL;  /*此邮箱通道的可维可测数据*/
    unsigned int          channel  =  mbuf->channel_id;
    struct mb_queue       *queue   =  &mbuf->mail_queue;
    struct mb_mail        *mail;
    unsigned int i;

    mailbox_out((KERN_ERR "mail box show channel(0x%08x) information:"RT, (unsigned int)channel));

    /*总体信息*/
    mailbox_show_general(mbuf->config);
    mailbox_out((KERN_ERR ":---------------------------------------------:"RT));
    mntn = &(mbuf->mntn);
    /*显示此邮箱最近传输的几个use ID信息, 最近的显示在前面*/
    i = mntn->track_prob ;
    mailbox_out((KERN_ERR "Latest transmit mails track:(%d total)"RT, (unsigned int)mbuf->seq_num));
    mailbox_out((KERN_ERR "id   ,address     ,send slice   ,recv slice  ,diff slice"RT));
    do {
        i = ((0 == i) ? (MAILBOX_RECORD_USEID_NUM - 1) : (i - 1));
        mail = (struct mb_mail *)(mntn->track_array[i].mail_addr);

        if (mail && (0 == mntn->track_array[i].recv_slice)) {
            mntn->track_array[i].recv_slice = mail->ulReadSlice;
        }
        mailbox_out((KERN_ERR "%02d   ,0x%-8x  ,0x%-8x   ,0x%-8x  ,0x%-8x(%d)"RT,
            mntn->track_array[i].use_id,
        (unsigned int)mail, (unsigned int)mntn->track_array[i].send_slice,
                                  (unsigned int)mntn->track_array[i].recv_slice,
        (unsigned int)mailbox_get_slice_diff(mntn->track_array[i].send_slice,
                                               mntn->track_array[i].recv_slice),
        (unsigned int)mailbox_get_slice_diff(mntn->track_array[i].send_slice,
                                            mntn->track_array[i].recv_slice)));
    } while (i != (mntn->track_prob));

    if (mb->local_id == mailbox_get_dst_id(channel)) {
        /*如果是接收通道*/
        /*打印通道方向*/
        mailbox_out((KERN_ERR "Receive Channel"RT));
    }

    /*打印通道方向*/
    if (mb->local_id == mailbox_get_src_id(channel)) {
        mailbox_out((KERN_ERR "Send Channel: sem id(0x%08x)"RT,(unsigned int)mbuf->mutex));
    } else if (mb->local_id == mailbox_get_dst_id(channel)) {
        mailbox_out((KERN_ERR "Receive Channel: sem id(0x%08x)"RT,(unsigned int)mbuf->mutex));
        mailbox_show_receive(mbuf);
    }

    if (MAILBOX_QUEUE_LEFT_INVALID == mntn->peak_traffic_left) {
        mntn->peak_traffic_left = queue->length;
    }
    mailbox_out((KERN_ERR "Peak Traffic: %d%%, Peak: 0x%x, Total: 0x%x"RT,
        (int)100*(queue->length - mntn->peak_traffic_left)/queue->length,
        (unsigned int)(queue->length - mntn->peak_traffic_left), (unsigned int)queue->length));

    mailbox_out((KERN_ERR ":------------------------------------------------:"RT));

    mailbox_clear_mntn(mntn, clear);
}


MAILBOX_EXTERN int mailbox_show(
                unsigned int    channel,
                unsigned int    show_flag)
{
    struct mb_cfg         *config     = &g_mailbox_global_cfg_tbl[0];
    struct mb_buff        *mbuf       = MAILBOX_NULL;
    struct mb_link        *send_tbl   = MAILBOX_NULL;    /*指向主结构体的发送通道数组基地址*/
    struct mb_link        *recv_tbl   = MAILBOX_NULL;    /*指向主结构体的接收通道数组基地址*/
    struct mb             *mb   = MAILBOX_NULL;
    struct mb_log         *record = MAILBOX_NULL;
    unsigned int          i;
    unsigned int          j;
    unsigned int          clear = MAILBOX_FALSE;

    mb =  mailbox_get_mb();
    if (MAILBOX_NULL == mb) {
        return (int)MAILBOX_ERRO;
    }

    if ( MAILBOX_SHOW_CLEAR & show_flag ) {
        clear = MAILBOX_TRUE;
    }

    if (MAILBOX_SHOW_ALL & show_flag) {
        /*Show all channel's general information.*/
        /*遍历显示所有的发送通道信息*/
        send_tbl = mb->send_tbl;
        for (i = 0; i < MAILBOX_CPUID_BUTT; i++) {
            if (MAILBOX_NULL != send_tbl[i].channel_buff) {
                mbuf  =   send_tbl[i].channel_buff;
                for (j = 0; j < send_tbl[i].carrier_butt; j++) {
                    mailbox_show_detail(mb, mbuf, (int)clear);
                    mbuf++;
                }
            }
        }

        recv_tbl = mb->recv_tbl;
        /*遍历显示所有的接收通道信息*/
        for (i = 0; i < MAILBOX_CPUID_BUTT; i++) {
            if (MAILBOX_NULL != recv_tbl[i].channel_buff) {
                mbuf  =   recv_tbl[i].channel_buff;
                for (j = 0; j < recv_tbl[i].carrier_butt; j++) {
                    mailbox_show_detail(mb, mbuf, (int)clear);
                    mbuf++;
                }
            }
        }
    } else {
        /*通道号指明，显示这个邮箱通道的详细信息*/
        mbuf = mailbox_get_channel_handle(mb, channel);

        if (MAILBOX_NULL != mbuf) {
            mailbox_show_detail(mb, mbuf, (int)clear);
        } else {
            /*如果通道号没有指明，列出邮箱模块各通道的总体状态:*/
            mailbox_out((KERN_ERR "mail box show global channel config:"RT));
            while (MAILBOX_MAILCODE_INVALID != config->butt_id) {
                mailbox_show_general(config);
                config++;
            }

            /*显示此邮箱最近几个错误信息, 最近的显示在前面*/
            i = mb->log_prob;
            record = &mb->log_array[0];
            mailbox_out((KERN_ERR "Latest error log track:"RT));
            mailbox_out((KERN_ERR "error num,   line num,   file name"RT));
            do {

               i = ((0 == i) ? (MAILBOX_ERRO_ARRAY_NUM - 1) : (i - 1));
               mailbox_out((KERN_ERR "0x%-8x,  %-8d,  %-8s"RT,(unsigned int)record[i].erro_num,
                (unsigned int)(record[i].line), (record[i].file)));

            } while (i != (mb->log_prob));
             mailbox_out((KERN_ERR "track end."RT));
        }
    }

    mailbox_out((KERN_ERR ":================================================:"RT));
    return MAILBOX_OK;
}
#else

MAILBOX_EXTERN int mailbox_show(
                unsigned int    channel,
                unsigned int    show_flag)
{
    return MAILBOX_OK;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

