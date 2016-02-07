#include <linux/module.h>
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
#include <linux/syscalls.h>
#include <linux/hisi/hisi_rproc.h>
#include <linux/hisi/ipc_msg.h>

#if defined(CONFIG_HI3XXX_RPROC)
#define test_rproc_get_slice()		(0xFFFFFFFF)
#elif defined(CONFIG_HI6XXX_RPROC)
extern unsigned int omTimerGet(void);
#define test_rproc_get_slice()		omTimerGet()
#endif
#define TEST_RPROC_NULL		((void*)0)
#define test_rproc_slice_diff(s,e)	(((e) >= (s))?((e) - (s)) : (0xFFFFFFFF - (s) + (e)))

struct test_rproc_cb
{
    void *           done_sema;
    void *           sync_sema;
    unsigned int    start_slice;       /*for calculate boundwidth*/
    unsigned int    end_slice;         /*for calculate boundwidth*/
    unsigned int    msg_count;
    unsigned int    sync_task_cnt;
    unsigned int    done_task_cnt;
    int    		    check_ret;
};

typedef int (*test_rproc_send_func)(unsigned int sync_id, 
	unsigned char rp_id, 
	unsigned int msg_len, 
	unsigned int msg_num,
	struct test_rproc_cb *rproc_cb);

struct test_rproc_proc
{
	test_rproc_send_func proc_cb;
	unsigned int sync_id;
	unsigned char rp_id;
	unsigned int msg_len;
	unsigned int msg_num;
	unsigned int task_num;
	struct test_rproc_cb *rproc_cb;
};

static char data_buf[] = "hello, the message looping back.";
static struct semaphore send_mutex_sema;
static struct semaphore task_mutex_sema;

static void rporc_async_callback(rproc_msg_t *ack_buffer,
				rproc_msg_len_t ack_buffer_len,
				int error, void *data)
{
	struct semaphore *complete_sema = (struct semaphore *)data;

	if(complete_sema) {
		/*printk(KERN_INFO "rporc_async_callback, complete ok\r\n");*/
		up(complete_sema);
	} else {
		printk(KERN_ERR "rporc_async_callback, complete null\r\n");
	}
}

int test_rproc_msg_send(unsigned int sync_id, 
		unsigned char rp_id,
		unsigned int msg_len,
		unsigned int msg_num, 
		struct test_rproc_cb *rproc_cb)
{
	int ret = 0;
	rproc_msg_t tx_buffer[8] = {0};
	rproc_msg_t ack_buffer[8] = {0};
	struct semaphore *complete_sema;
	unsigned int start_slice = 0;
	unsigned int end_slice = 0;
    unsigned int sync_task_cnt = 0;

	if (sync_id > 0x3) {
		printk(KERN_ERR "wrong sync id!\n");
		return -1;
	}

	if (msg_len > 8) {
		printk(KERN_ERR "illegal message length!\n");
		return -1;
	}

	if(rproc_cb) {
        down(&task_mutex_sema);
		rproc_cb->sync_task_cnt--;
        sync_task_cnt = rproc_cb->sync_task_cnt;
        up(&task_mutex_sema);
		if ((0 == sync_task_cnt) 
		&& (TEST_RPROC_NULL != rproc_cb->sync_sema)) {
			rproc_cb->start_slice = test_rproc_get_slice();
			up(rproc_cb->sync_sema);
		}

		/*进程同步*/
		if (TEST_RPROC_NULL != rproc_cb->sync_sema) {
			down(rproc_cb->sync_sema);
			up(rproc_cb->sync_sema);
		}
	}

	tx_buffer[0] = (OBJ_TEST << 16) | (CMD_TEST << 8)/*0x00120500*/;
	memcpy((void*)&tx_buffer[1], (void*)&data_buf[0], sizeof(tx_buffer) - sizeof(tx_buffer[0]));
	switch(sync_id)
	{
		case 0:/*同步发送同步消息*/
			while(msg_num--){
				ret = RPROC_SYNC_SEND(rp_id,tx_buffer, msg_len,SYNC_MSG,ack_buffer,msg_len);
				if (ret || (((OBJ_TEST << 16) | (CMD_TEST << 8)) != ack_buffer[0]) || (0x12345678 != ack_buffer[1])) {
					printk(KERN_ERR "rproc send error, ret %d, rp %d, sync %d, ack[0x%x][0x%x]!\r\n", 
							ret, rp_id, sync_id, ack_buffer[0], ack_buffer[1]);
					return -1;
				}
				if(rproc_cb)
					rproc_cb->msg_count++;
			}
			break;

		case 1:/*同步发送异步消息*/
			while(msg_num--){
				ret = RPROC_SYNC_SEND(rp_id ,tx_buffer, msg_len,ASYNC_MSG,NULL,0);
				if (ret) {
					printk(KERN_ERR "rproc send error, ret %d, rp %d, sync %d!\r\n", ret, rp_id, sync_id);
					return -1;
				}
				if(rproc_cb)
					rproc_cb->msg_count++;
			}
			break;

		case 2:/*异步发送同步消息*/
			while (msg_num--) {
				complete_sema = (struct semaphore*)kmalloc(sizeof(struct semaphore), GFP_KERNEL);
				sema_init(complete_sema, 0);
				ret = RPROC_ASYNC_SEND(rp_id,tx_buffer,msg_len,SYNC_MSG,rporc_async_callback,complete_sema);
				if (ret) {
					printk(KERN_ERR "rproc send error, ret %d, rp %d, sync %d!\r\n", ret, rp_id, sync_id);
					kfree(complete_sema);
					return -1;
				}
				start_slice = test_rproc_get_slice();
				if (0 != down_timeout(complete_sema, msecs_to_jiffies(1000)))
				{
					printk(KERN_ERR "msg_send timeout rp_id:%d rproc async send err!\r\n", rp_id);
					/*如果超时,不能释放内存*/
					/*kfree(complete_sema);*/
					return -1;
				}
				end_slice = test_rproc_get_slice();
				printk(KERN_INFO "async send sync msg spend %d slice!\r\n",
						test_rproc_slice_diff(start_slice, end_slice));
				kfree(complete_sema);
				if(rproc_cb)
					rproc_cb->msg_count++;
			}
			break;

		case 3:/*异步发送异步消息*/
			while (msg_num--) {
				ret = RPROC_ASYNC_SEND(rp_id,tx_buffer,msg_len,ASYNC_MSG,rporc_async_callback,NULL);
				if (ret) {
					printk(KERN_ERR "rproc send error, ret %d, rp %d, sync %d!\r\n", ret, rp_id, sync_id);
					return ret;
				}
				if(rproc_cb)
					rproc_cb->msg_count++;
			}
			break;

		default:
			printk(KERN_ERR "wrong sync ID!\n");
			return -1;
	}
	printk(KERN_INFO "rproc send ok, rp %d, sync %d!\n", rp_id, sync_id);

	return ret;
}

void test_rproc_msg_send_entry(void* _proc)
{
	struct test_rproc_proc *rproc_proc = _proc;
	struct test_rproc_cb *rproc_cb = TEST_RPROC_NULL;
    unsigned int done_task_cnt = 0;
    

	if (!rproc_proc) {
		printk(KERN_ERR "test_rproc_msg_send_entry rproc_proc is NULL\n ");
		return ;
	}

	if (!rproc_proc->proc_cb) {
		printk(KERN_ERR "test_rproc_msg_send_entry proc_cb is NULL\n ");
		kfree(rproc_proc);
		return ;
	}

	if (!rproc_proc->rproc_cb) {
		printk(KERN_ERR "test_rproc_msg_send_entry rproc_cb is NULL\n ");
		kfree(rproc_proc);
		return ;
	}

	rproc_cb = rproc_proc->rproc_cb;
	rproc_cb->check_ret = rproc_proc->proc_cb(
		rproc_proc->sync_id,
		rproc_proc->rp_id,
		rproc_proc->msg_len,
		rproc_proc->msg_num,
		rproc_cb);
	kfree(rproc_proc);

    down(&task_mutex_sema);
	rproc_cb->done_task_cnt--;
    done_task_cnt = rproc_cb->done_task_cnt;
    up(&task_mutex_sema);
	if ((0 == done_task_cnt) 
	&& (TEST_RPROC_NULL != rproc_cb->done_sema)) {
		up(rproc_cb->done_sema);
	}
}

void test_rproc_msg_create_task(test_rproc_send_func entryPtr,
                unsigned int sync_id,
                unsigned char rp_id,
                unsigned int msg_len,
                unsigned int msg_num,
                unsigned int task_num,
                struct test_rproc_cb *rproc_cb)
{
	struct test_rproc_proc *pEntryParam = kmalloc(sizeof(struct test_rproc_proc), GFP_KERNEL);
	char task_name[32] = {0};

	pEntryParam->proc_cb  = (test_rproc_send_func)entryPtr;
	pEntryParam->sync_id  = sync_id;
	pEntryParam->rp_id = rp_id;
	pEntryParam->msg_len  = msg_len;
	pEntryParam->msg_num    = msg_num;
	pEntryParam->task_num = task_num;
	pEntryParam->rproc_cb = rproc_cb;

	snprintf(task_name, sizeof(task_name), "trproc_task%d", (int)task_num);

	kthread_run((int (*)(void *))test_rproc_msg_send_entry, (void*)pEntryParam, task_name);
}

int test_rproc_msg_multi_send(unsigned int sync_id,
		unsigned char rp_id,
		unsigned int msg_len, 
		unsigned int msg_num, 
		unsigned int task_num)
{
	struct test_rproc_cb * rproc_cb = 0;
	unsigned int time_diff = 0;
	unsigned int task_num_t = task_num;
	int ret = 0;

	down(&send_mutex_sema);
	/*创建回调用户句柄*/
	rproc_cb = kcalloc(sizeof(struct test_rproc_cb), 1, GFP_KERNEL);
	if (!rproc_cb) {
		printk(KERN_ERR "error test_rproc_msg_multi_send kcalloc\n");
		up(&send_mutex_sema);
		return -1;
	}

	rproc_cb->done_sema = (struct semaphore*)kmalloc(sizeof(struct semaphore), GFP_KERNEL);
	sema_init(rproc_cb->done_sema, 0);
	rproc_cb->sync_sema = (struct semaphore*)kmalloc(sizeof(struct semaphore), GFP_KERNEL);
	sema_init(rproc_cb->sync_sema, 0);
	rproc_cb->check_ret = -1;
	rproc_cb->sync_task_cnt = task_num;
	rproc_cb->done_task_cnt = task_num;
	rproc_cb->msg_count = 0;

	/*创建诺干个任务，任务入口函数为本核往其他核发送消息的函数*/
	while (task_num) {
		test_rproc_msg_create_task(test_rproc_msg_send,
				sync_id,
				rp_id,
				msg_len,
				msg_num,
				task_num,
				rproc_cb);

		task_num--;
	}

	down(rproc_cb->done_sema);
	kfree(rproc_cb->done_sema);
	rproc_cb->done_sema = TEST_RPROC_NULL;
	kfree(rproc_cb->sync_sema);
	rproc_cb->sync_sema = TEST_RPROC_NULL;

	rproc_cb->end_slice= test_rproc_get_slice();

	time_diff = test_rproc_slice_diff(rproc_cb->start_slice, rproc_cb->end_slice);
	printk(KERN_INFO "rp: %d, sync: %d, total:%d(4byte), latency: %d (slice)\n", rp_id, sync_id,
		(int)(msg_len * msg_num * task_num_t), (int)time_diff);

	if(0 != rproc_cb->check_ret) {
		printk(KERN_ERR "test_rproc_msg_multi_send: CheckRet error\n");
		ret = -1;
	} else if(msg_num * task_num_t != rproc_cb->msg_count) {
		printk(KERN_ERR "test_rproc_msg_multi_send: MsgCount(%d) error\n", (int)rproc_cb->msg_count);
		ret = -1;
	} else {
		printk(KERN_INFO "test_rproc_msg_multi_send: Success!\n");
		ret = 0;
	}

	kfree(rproc_cb);
	up(&send_mutex_sema);
	return ret;
}

/*****************************************************************************
 函 数 名  : test_rproc_single_task
 功能描述  : 单线程对外测试函数
 输入参数  : int msg_type 0:sync 1:async
             int rp_id    rproc id
             int msg_len   邮件长度
             int msg_num   邮件数目

*****************************************************************************/
void test_rproc_single_task_sync(unsigned int msg_type, unsigned char rp_id, unsigned int msg_len, unsigned int msg_num)
{
	int ret = 0;
	unsigned int sync_id = 0;

	sync_id = (0 == msg_type) ? 0 : 1;
	ret = test_rproc_msg_multi_send(sync_id, rp_id, msg_len, msg_num, 1);
	if(0 != ret)
		printk(KERN_ERR "test_rproc_single_task_sync: Fail\r\n");
	else
		printk(KERN_ERR "test_rproc_single_task_sync: Success\r\n");
}

void test_rproc_single_task_async(unsigned int msg_type, 
	unsigned char rp_id, 
	unsigned int msg_len, 
	unsigned int msg_num)
{
	int ret = 0;
	unsigned int sync_id = 0;

	sync_id = (0 == msg_type) ? 2 : 3;
	ret = test_rproc_msg_multi_send(sync_id, rp_id, msg_len, msg_num, 1);
	if(0 != ret)
		printk(KERN_ERR "test_rproc_single_task_async: Fail\r\n");
	else
		printk(KERN_ERR "test_rproc_single_task_async: Success\r\n");
}

void test_rproc_multi_task_sync(unsigned int msg_type, 
	unsigned char task_num, 
	unsigned char rp_id, 
	unsigned int msg_len, 
	unsigned int msg_num)
{
	int ret = 0;
	unsigned int sync_id = 0;

	sync_id = (0 == msg_type) ? 0 : 1;
	ret = test_rproc_msg_multi_send(sync_id, rp_id, msg_len, msg_num, task_num);
	if(0 != ret)
		printk(KERN_ERR "test_rproc_multi_task_sync: Fail\r\n");
	else
		printk(KERN_ERR "test_rproc_multi_task_sync: Success\r\n");
}

void test_rproc_multi_task_async(unsigned int msg_type, 
	unsigned char task_num, 
	unsigned char rp_id, 
	unsigned int msg_len, 
	unsigned int msg_num)
{
	int ret = 0;
	unsigned int sync_id = 0;

	sync_id = (0 == msg_type) ? 2 : 3;
	ret = test_rproc_msg_multi_send(sync_id, rp_id, msg_len, msg_num, task_num);
	if(0 != ret)
		printk(KERN_ERR "test_rproc_multi_task_async: Fail\r\n");
	else
		printk(KERN_ERR "test_rproc_multi_task_async: Success\r\n");
}

static int __init test_rproc_init(void)
{
	sema_init(&send_mutex_sema, 1);
	sema_init(&task_mutex_sema, 1);
       return 0;
}

static void __exit test_rproc_exit(void)
{
}

module_init(test_rproc_init);
module_exit(test_rproc_exit);
MODULE_DESCRIPTION("hisi_rproc_test");
MODULE_LICENSE("GPL v2");


