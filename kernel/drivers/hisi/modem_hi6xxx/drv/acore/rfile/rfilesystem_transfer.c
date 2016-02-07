

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/semaphore.h>    /*创建mutex所需的头文件*/
#include <linux/kernel.h>       /*kmalloc,printk*/
#include <linux/kthread.h>      /*创建线程*/
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/err.h>
#include <linux/module.h>

#include "BSP.h"
#include "rfilesystem_transfer.h"
#include "rfilesystem_process.h"
#include "rfilesystem_logunit.h"
#include "mdrv_udi.h"

#ifndef _DRV_LLT_
#include <linux/wakelock.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
struct task_struct   *g_RfileInitThread    = NULL;
struct task_struct   *g_RfileProcessThread = NULL;

/*当ICC获取数据后，释放该信号量，交由任务进行处理*/
struct semaphore     sem_to_process_req;

struct semaphore     sem_to_read_req;

/*FS接口内部互斥*/
struct semaphore     sem_fopen;
struct semaphore     sem_fclose;
struct semaphore     sem_fread;
struct semaphore     sem_fwrite;
struct semaphore     sem_fseek;
struct semaphore     sem_remove;
struct semaphore     sem_ftell;
struct semaphore     sem_rename;
struct semaphore     sem_access;
struct semaphore     sem_mkdir;
struct semaphore     sem_rmdir;
struct semaphore     sem_opendir;
struct semaphore     sem_readdir;
struct semaphore     sem_closedir;
struct semaphore     sem_stat;
struct semaphore     sem_xcopy;
struct semaphore     sem_xdelete;
struct semaphore     sem_mass_read;
struct semaphore     sem_mass_write;


/*ICC 数据句柄*/
RFILECONTEXT_A rFileContext_a;

/*ICC 数据句柄*/
RFILECONTEXT_A rFileContext_cmd;

char *ops_2_str[OPS_MAX]  = {"open","A ",\
                             "seek","B ",\
                             "read","C ",\
                             "write","D ",\
                             "close","E ",\
                             "remove","F ",\
                             "tell","G ",\
                             "mkdir","H ",\
                             "rmdir","I ",\
                             "opendir","J ",\
                             "readdir","K ",\
                             "closedir","L ",\
                             "stat","M ",\
                             "xcopy","N ",\
                             "xdelete","O ",\
                             "access","P ",\
                             "rename","Q ",\
                             "massread","R ",\
                             "masswrite","S "};

extern int g_rfilesystem_init;/*only for compile check warning*/
int g_rfilesystem_init = FALSE;

extern fileOps rfileOpsSet[OPS_MAX];

extern struct semaphore     sem_to_rfile_sync_req;

extern fp_list_t fp_list_file;

extern fp_list_t fp_list_dir;

extern int g_rfile_dbg_lvl;

extern int check_acore_only(void);

#ifndef _DRV_LLT_
struct wake_lock g_ulRfswakelock;
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

int  rfile_write_cmdtoccpu_pkt(int opspid,char *send_buf,unsigned int send_len)
{
    int size = 0;

    /*调用ICC的发送，将响应消息发给CCPU*/
    size = mdrv_udi_write(rFileContext_cmd.rFileIccFd,send_buf,send_len);
	if (size != send_len)
	{
        printk(KERN_ERR "ICC write cmd failed : the size is %d\n",size);
        return RFILE_ERROR;
	}

    return RFILE_OK;
}

void rfile_ifc_cp2ap_ack(void)
{
   up(&sem_to_read_req);
}

#if 0
void rfile_ifc_regfunc(void)
{
    int ret = 0;
    ret = BSP_IFCP_RegFunc(IFCP_FUNC_RFILE_CP2AP_SEM,(BSP_IFCP_REG_FUNC)rfile_ifc_cp2ap_ack);
    if(0 != ret)
    {
        printk(KERN_INFO "C:BSP_IFCP_RegFunc is failed\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_ICC, "rfile_ifc_regfunc Fail!", 0, 0, RFILE_OPER_RESULT_FAIL, 0);
        return;
    }
    printk(KERN_INFO "BSP_IFCP_RegFunc\n");
}
#endif
int rfile_filesystem_sync(void)
{
    int ret = -1;
#if 0
    RFILE_IFC_MSG pMsg = {0};
    /*y00145322*/
    pMsg.ulModuleId = IFC_MODULE_BSP;
    pMsg.ulFuncId  = IFCP_FUNC_RFILE_AP_NOTIFY_CP_INIT;
    pMsg.ulFlag     = 1;

    if(0 != BSP_IFC_Send((BSP_VOID*)&pMsg, sizeof(RFILE_IFC_MSG)))
    {
        printk(KERN_INFO "FRILE: IFC Send Fail!\n");
        return RFILE_ERROR;
    }

    return RFILE_OK ;
#endif

    unsigned int rfile_sync = 0x5555aaaa;

    ret = rfile_write_cmdtoccpu_pkt(0,(char*)&rfile_sync,sizeof(unsigned int));
    if(ret < 0)
    {
        printk(KERN_ERR "rfile_filesystem_sync: rfile_write_cmdtoccpu_pkt: sync cmd send error\n");
        return RFILE_ERROR;
    }

    return RFILE_OK ;
}
int rfile_a_give_c_sem(void)
{
   int ret = -1;
#if 0
    RFILE_IFC_MSG pMsg = {0};
    /*y00145322*/
    pMsg.ulModuleId = IFC_MODULE_BSP;
    pMsg.ulFuncId  = IFCP_FUNC_RFILE_SEM;
    pMsg.ulFlag    = 1;
    if(0 != BSP_IFC_Send((BSP_VOID*)&pMsg, sizeof(RFILE_IFC_MSG)))
    {
        printk(KERN_INFO "FRILE: IFC Send Fail!\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_ICC, "FRILE: IFC Send Fail!", 0, 0, RFILE_OPER_RESULT_FAIL, 0);

        return RFILE_ERROR;
    }
#endif

    int cmd_sync = 0x55aa55aa;

    ret = rfile_write_cmdtoccpu_pkt(0,(char*)&cmd_sync,sizeof(int));
    if(ret < 0)
    {
        printk(KERN_ERR "rfile_a_give_c_sem: rfile_write_cmdtoccpu_pkt: sync cmd send error\n");
        return RFILE_ERROR;
    }

    return RFILE_OK ;
}
unsigned int rfile_icc_read_cmd_cb(unsigned int chenalId,signed int u32size)
{

    unsigned int readSize;

	if (1 != rFileContext_cmd.iccOpened)
	{
        /*printk(KERN_INFO "icc cmd is not Opened\n");*/
		return 0;
	}

	/*ICC读取数据*/
	readSize = mdrv_udi_read(rFileContext_cmd.rFileIccFd,rFileContext_cmd.sendBuf,u32size);
	if (readSize != u32size)
	{
	    printk(KERN_ERR "cmd udi_read 's size is error readSize:%d VS u32size:%d\n",readSize,u32size);
		return 0;
	}

    /*获取从CCPU 通过 ICC 发送过来的数据长度*/
    /*
	printk(KERN_DEBUG"icc read cmd ok, the receive size is %x\n",readSize);

	printk(KERN_DEBUG"icc read cmd ok, the receive data is %x\n",*((int*)rFileContext_cmd.sendBuf));
    */
	if(*((int*)rFileContext_cmd.sendBuf) == 0x12345678)
	{
	    rfile_ifc_cp2ap_ack();
	}

	return 0;
}



unsigned int rfile_icc_read_cb(unsigned int chenalId,signed int u32size)
{

    unsigned int readSize;

	/*printk(KERN_DEBUG"icc read cb\n");*/
	#ifndef _DRV_LLT_
    wake_lock(&g_ulRfswakelock);
    #endif

	if (1 != rFileContext_a.iccOpened)
	{
        printk(KERN_ERR "icc is not Opened\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_RECEIVEAP, "hsic or icc not Opened", 0, FLASHLESS_FILEOPER_SET, rFileContext_a.hsicOpened, rFileContext_a.iccOpened);

        #ifndef _DRV_LLT_
        wake_unlock(&g_ulRfswakelock);
        #endif
		return 0;
	}

	/*ICC读取数据*/
	readSize = mdrv_udi_read(rFileContext_a.rFileIccFd,rFileContext_a.sendBuf,u32size);
	if (readSize != u32size)
	{
	    printk(KERN_ERR "udi_read 's size is error readSize:%d VS u32size:%d\n",readSize,u32size);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_ICC, "udi_read error", rFileContext_a.rFileIccFd, FLASHLESS_FILEOPER_READ, readSize, u32size);

        #ifndef _DRV_LLT_
        wake_unlock(&g_ulRfswakelock);
        #endif
		return 0;
	}

    /*获取从CCPU 通过 ICC 发送过来的数据长度*/
	/*printk(KERN_DEBUG"icc read ok, the receive size is %x\n",readSize);*/

    up(&sem_to_process_req);

	return 0;
}



int rfile_transfer_init(void)
{

	printk(KERN_INFO "A:rfile_transfer_init start \n");
#if 0
    rfile_filesystem_sync();
#endif

	rFileContext_cmd.attr.enChanMode  = ICC_CHAN_MODE_PACKET;
	rFileContext_cmd.attr.u32Priority = 0;
	rFileContext_cmd.attr.u32TimeOut  = 100000;
	rFileContext_cmd.attr.event_cb    = NULL;
	rFileContext_cmd.attr.read_cb     = rfile_icc_read_cmd_cb;//ICC的回调
	rFileContext_cmd.attr.write_cb    = NULL;
	rFileContext_cmd.attr.u32FIFOInSize  = 1*1024;
	rFileContext_cmd.attr.u32FIFOOutSize = 1*1024;

     /*y00145322*/
	rFileContext_cmd.rFileIccParam.devid = UDI_ICC_GUOM6_ID;
	rFileContext_cmd.rFileIccParam.pPrivate = &rFileContext_cmd.attr;

	/*打开ICC通道*/
	printk(KERN_INFO "open udi ICC 0000000000000000000000000000000\n");
	rFileContext_cmd.rFileIccFd = mdrv_udi_open(&rFileContext_cmd.rFileIccParam);
	if (rFileContext_cmd.rFileIccFd <= 0)
	{
	    printk(KERN_INFO "open CMD ICC failed:[0x%x]\n",rFileContext_cmd.rFileIccFd);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "open CMD ICC failed!", rFileContext_cmd.rFileIccFd, FLASHLESS_FILEOPER_OPEN, RFILE_OPER_RESULT_FAIL, 0);

        return RFILE_ERROR;
	}

    /*开辟发送缓存*//*这里从HSIC传过来的数据是没有开辟空间*/
	rFileContext_cmd.sendBuf = (void*)kmalloc(TRANS_MAX_SIZE, GFP_KERNEL);
	if (NULL == rFileContext_cmd.sendBuf)
	{
        printk(KERN_ERR "malloc is failed\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_LOCAL, "malloc is failed", 0, 0, RFILE_OPER_RESULT_FAIL, 0);

		return RFILE_ERROR;
	}

	rFileContext_cmd.iccOpened  = 1;

#if defined(RUN_IN_EMULATOR)
#else
    printk(KERN_INFO "rfile_transfer_init: msleep before \n");
    msleep(1000);
    printk(KERN_INFO "rfile_transfer_init: msleep after \n");
#endif

	rFileContext_a.attr.enChanMode  = ICC_CHAN_MODE_PACKET;
	rFileContext_a.attr.u32Priority = 0;
	rFileContext_a.attr.u32TimeOut  = 10000;
	rFileContext_a.attr.event_cb    = NULL;
	rFileContext_a.attr.read_cb     = rfile_icc_read_cb;//ICC的回调
	rFileContext_a.attr.write_cb    = NULL;
	rFileContext_a.attr.u32FIFOInSize  = 16*1024;
	rFileContext_a.attr.u32FIFOOutSize = 16*1024;

     /*y00145322*/
	rFileContext_a.rFileIccParam.devid = UDI_ICC_GUOM5_ID;
	rFileContext_a.rFileIccParam.pPrivate = &rFileContext_a.attr;

	/*打开ICC通道*/
	printk(KERN_INFO "open udi ICC 1111111111111111111111111111111\n");
	rFileContext_a.rFileIccFd = mdrv_udi_open(&rFileContext_a.rFileIccParam);
	if (rFileContext_a.rFileIccFd <= 0)
	{
	    printk(KERN_INFO "open ICC failed:[0x%x]\n",rFileContext_a.rFileIccFd);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "open ICC failed!", rFileContext_a.rFileIccFd, FLASHLESS_FILEOPER_OPEN, RFILE_OPER_RESULT_FAIL, 0);

        return RFILE_ERROR;
	}

    /*开辟发送缓存*//*这里从HSIC传过来的数据是没有开辟空间*/
	rFileContext_a.sendBuf = (void*)kmalloc(BUF_LEN, GFP_KERNEL);
	if (NULL == rFileContext_a.sendBuf)
	{
        printk(KERN_ERR "malloc is failed\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_LOCAL, "malloc is failed", 0, 0, RFILE_OPER_RESULT_FAIL, 0);

		return RFILE_ERROR;
	}

	rFileContext_a.iccOpened  = 1;

	rfile_filesystem_sync();

    FLASHLESS_CMD_ADD_LOG(RFS_EVENT_LOCAL, "rfile transfer init ok", 0, FLASHLESS_FILEOPER_INIT, RFILE_OPER_RESULT_OK, 0);

	return RFILE_OK;
}



int rfile_init_cb(void)
{
    int ret = 0;

    ret = rfile_transfer_init();
    if(ret == -1)
    {
        printk(KERN_ERR "\r rfile_transfer_init fail\n");

        return RFILE_ERROR;
    }
#if 0
    rfile_ifc_regfunc();
#endif
    printk(KERN_INFO "\r rfile_transfer_init ok\n");

    return RFILE_OK;
}



int rfile_process_ccpu_pkt( RFILE_MNTN_HEADER *header )
{
    int opstype;
    int ret         = 0;
    char *data      = NULL;
    char *data_temp = NULL;

    /*获取从ICC获取的数据指针*/
    data_temp = (char*)rFileContext_a.sendBuf;

    /*去除可维可测头*/
    data_temp += sizeof(RFILE_MNTN_HEADER);


    /*数据位当前ICC接收数据，除掉可维可测头部分为数据*/
    data = data_temp;

    opstype=*(int *)data;
    if (opstype >= (int)OPS_MAX)
    {
        return RFILE_ERROR;
    }

    if (rfileOpsSet[opstype] != NULL)
    {
        RFILE_DEBUGPL(0x1,"the foptype is %s \n",ops_2_str[opstype]);
        ret = rfileOpsSet[opstype](header,data);
    }
    else
    {
        printk(KERN_ERR "the foptype is %s is null \n",ops_2_str[opstype]);

        ret = RFILE_ERROR;
    }

    return ret;

}
int rfile_process_task(void *arg)
{
    int                  ret = 0;
    RFILE_MNTN_HEADER mntn_header;

#ifdef _DRV_LLT_
#else
    for(;;)
#endif
    {
        /*ICC读取完毕后释放信号量*/
        down(&sem_to_process_req);

        /*数据接收判断处理*/
        ret = rfile_read_ccpu_pkt(&mntn_header);
        if(0 == ret)
        {
            ret = rfile_process_ccpu_pkt(&mntn_header);
            if(0 == ret)
            {
                ;
            }
            else
            {
                printk(KERN_DEBUG"rfile process error\n");
            }

        }
        else
        {
            printk(KERN_DEBUG"rfile_read_ccpu_pkt process error\n");
        }
        #ifndef _DRV_LLT_
        wake_unlock(&g_ulRfswakelock);
        #endif

    }
    return 0;
}




int rfile_init_thread(void)
{
#ifndef CONFIG_ARM64
    /*added by zhuzheng for v8r2*/
    if (1 == check_acore_only())
        return 0;
#endif

    /*等待emmc mount 完毕*/
    down(&sem_to_rfile_sync_req);

    /*接收数据处理同步信号量*/
    sema_init(&sem_to_process_req, 0);

    sema_init(&sem_to_read_req, 0);

    #ifndef _DRV_LLT_
    wake_lock_init(&g_ulRfswakelock,WAKE_LOCK_SUSPEND,"rfs");
    #endif

    /*FS防重入*/
    sema_init(&sem_fopen,1);
    sema_init(&sem_fclose,1);
    sema_init(&sem_fread,1);
    sema_init(&sem_fwrite,1);
    sema_init(&sem_fseek,1);
    sema_init(&sem_remove,1);
    sema_init(&sem_ftell,1);
    sema_init(&sem_rename,1);
    sema_init(&sem_access,1);
    sema_init(&sem_mkdir,1);
    sema_init(&sem_rmdir,1);
    sema_init(&sem_opendir,1);
    sema_init(&sem_readdir,1);
    sema_init(&sem_closedir,1);
    sema_init(&sem_stat,1);
    sema_init(&sem_xcopy,1);
    sema_init(&sem_xdelete,1);
    sema_init(&sem_mass_read,1);
    sema_init(&sem_mass_write,1);

    rfile_regProxyFunc();

	/*g_RfileInitThread    = kthread_run(rfile_init_cb,NULL,"rfile_init");*/
	rfile_init_cb();

	INIT_LIST_HEAD(&(fp_list_file.list));

	INIT_LIST_HEAD(&(fp_list_dir.list));

	g_RfileProcessThread = kthread_run(rfile_process_task,NULL,"rfile_process_task");
    g_rfilesystem_init = TRUE;
    return 0;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

