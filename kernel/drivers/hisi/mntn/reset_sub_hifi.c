
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/notifier.h>         /* For notifier... */
//#include <linux/hisi/reset.h>
#include "soc_peri_sctrl_interface.h"
#include <soc_modem_sctrl_interface.h>
#include "soc_pmctrl_interface.h"
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <soc_ao_sctrl_interface.h>
#include <soc_wdt_interface.h>
#include <soc_baseaddr_interface.h>
#include <soc_mddrc_axi_interface.h>
#include <linux/io.h>
#include "MemoryMap.h"
#include "../ipc_hi6xxx/bsp_drv_ipc.h"
#include "bsp_mailbox.h"
#include <linux/hisi/etb.h>
#include "excDrv.h"
#include "reset_sub_hifi.h"
#include <linux/hisi/hi6xxx_mntn.h>
#include <drv_mailbox_cfg.h>

extern void sochifi_watchdog_send_event(void);

#ifndef _DRV_LLT_
#include <mach/util.h>          /* For mach_call_usermoduleshell*/
#endif
#include <linux/hisi/ipc_msg.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
宏定义
*****************************************************************************/

/*因为b090以后已经支持把printk打印信息存储到文件，所以不需要本模块自己存储
如功能开启，请检查init.rc中已mkdir log/record */
#define PRINK_TO_FILE

/*****************************************************************************
全局变量
*****************************************************************************/
/*Link used for hifi reset*/
sreset_mgr_LLI  *g_pmgr_hifireset_data = NULL;
#define RESET_CBFUNC_PRIO_LEVEL_LOWT      0
#define RESET_CBFUN_IGNORE_NAME           "NONAME"
#define RESET_CBFUNC_PRIO_LEVEL_HIGH      49
/*用于协助完成复位过程*/
static sreset_mgr_assistant_hifi   g_reset_assistant_hifi;
/*ms, time of wating mail msg reply from hifi/mcu*/
#define RESET_WAIT_TIMEOUT_MAILMSG        5000

/*****************************************************************************
函数声明
*****************************************************************************/
static void  do_reset_system(int ireset_type);
static void reset_set_cpu_status(unsigned int iOff, unsigned int iflag);
static int ccorereset_enable_wdt_irq(void);
#ifndef _DRV_LLT_
extern void check_doreset_for_noc(void);
#endif
extern void BSP_CPU_StateSet(unsigned int iOff,unsigned int offset);
/*****************************************************************************
 函 数 名  : mailbox_recfun_mcu_hifireset
 功能描述  : 用于接收来自于MCU的MAILBOX消息
 输入参数  : void  *UserHandle, 用户句柄
            void *MailHandle, 邮箱句柄
            unsigned long MailLen, 数据长度
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
static int mailbox_recfun_mcu_hifireset(union ipc_data *msg)
{
    unsigned char msg_mcu = 0;

    msg_mcu = (msg->cmd_mix).cmd_para[0];
    printk(KERN_INFO "%s: receive mail from mcu, msg_mcu = %d\n", __FUNCTION__, msg_mcu);
    if (1 == msg_mcu)
    {
        up(&(g_reset_assistant_hifi.sem_wait_mcu_msg_hifireset));
    }
    else
    {
        /*复位系统*/
        printk(KERN_INFO "%s: mcu give wrong msg, msg is %d\n", __FUNCTION__, msg_mcu);
        do_reset_system(RESET_TYPE_MSG_FROM_MCU_WRONG);
    }
    return 0;
}
/*****************************************************************************
 函 数 名  : reset_do_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理Modem复位前后相关数据。
 输入参数  :
         sreset_mgr_LLI *plink,管理链表，注意，允许为空.
            const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
sreset_mgr_LLI * reset_do_regcbfunc(sreset_mgr_LLI *plink, const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    sreset_mgr_LLI  *phead = plink;
    sreset_mgr_LLI  *pmgr_unit = NULL;

    /*判断入参是否合法，不合法返回错误*/
    if (NULL == pname
        || NULL == pcbfun
        || (priolevel < RESET_CBFUNC_PRIO_LEVEL_LOWT || priolevel > RESET_CBFUNC_PRIO_LEVEL_HIGH))
    {
        printk(KERN_ERR "%s: fail in ccore reset regcb,fail, name 0x%p, cbfun 0x%p, prio %d\n", __FUNCTION__, \
                        (void*)pname, (void*)pcbfun, priolevel);
        return NULL;
    }

    /*分配空间*/
    pmgr_unit = (sreset_mgr_LLI*)kmalloc(sizeof(sreset_mgr_LLI), GFP_KERNEL);
    if (NULL != pmgr_unit)
    {
        hisi_io_memset((void*)pmgr_unit, 0, (sizeof(sreset_mgr_LLI)));
        /*赋值*/
        strncpy(pmgr_unit->cbfuninfo.name, pname, DRV_RESET_MODULE_NAME_LEN);
        pmgr_unit->cbfuninfo.priolevel = priolevel;
        pmgr_unit->cbfuninfo.userdata = userdata;
        pmgr_unit->cbfuninfo.cbfun = pcbfun;
    }

    /*第一次调用该函数，链表为空*/
    if (NULL == phead)
    {
        phead = pmgr_unit;
    }
    else
    {
    /*根据优先级插入链表*/
        phead = reset_link_insert(phead, pmgr_unit);
    }
    return phead;
}

/*caixi 6*/
/*****************************************************************************
 函 数 名  : hifireset_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理HIFI复位前后相关数据。
 输入参数  : const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifireset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    g_pmgr_hifireset_data = reset_do_regcbfunc(g_pmgr_hifireset_data, pname, pcbfun, userdata, priolevel);
    printk(KERN_INFO "%s: %s registered a cbfun for hifi reset\n", __FUNCTION__, pname);
    return BSP_RESET_OK;
}
/*****************************************************************************
 函 数 名  : get_local_time
 功能描述  : kernel层获取系统时间
 输入参数  : char *pbuf, 保存系统时间字符串数组
            int buf_len, 字符串数组长度
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
static void get_local_time(char *pbuf, int buf_len)
{
   //构造输出文件时间，形成一个字符串，如2012-7-13-8-15-38,表示:系统时间下的 年月日时分秒
   struct timeval tv;
   struct rtc_time tm;
   memset(&tv, 0, sizeof(struct timeval));
   memset(&tm, 0, sizeof(struct rtc_time));
   do_gettimeofday(&tv);
   tv.tv_sec -= sys_tz.tz_minuteswest * 60; 
   rtc_time_to_tm(tv.tv_sec, &tm);
   snprintf(pbuf,buf_len,"%d-%d-%d-%d-%d-%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
/*****************************************************************************
 函 数 名  : apr_upload_hificrash
 功能描述  : 通知logservice打包相关路径LOG信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void apr_upload_hificrash(void)
{
	char apr_cmd[MAX_APR_CMD_LEN]={0};
	int ret = 0;
	char output_zip_filename[MAX_APR_FILE_LEN] = {0};
	char local_time[MAX_APR_LOCAL_TIME_STR] = {0};
	get_local_time(local_time, MAX_APR_LOCAL_TIME_STR);
	snprintf(output_zip_filename,MAX_APR_FILE_LEN,"%s_%s",local_time,"hificrash");
	snprintf(apr_cmd,MAX_APR_CMD_LEN,"archive -d %s -d %s -d %s -o %s -z zip",AP_LOG_PATH_APR, CP_LOG_PATH_APR, HIFI_LOG_PATH_APR,output_zip_filename);
	ret = log_to_exception("balong_hifi",apr_cmd);
	if(ret<0)
	{
		printk(KERN_ERR "hifi log write apr_exception err\n");
	}
}
/*caixi 7*/
/*****************************************************************************
 函 数 名  : hifireset_task
 功能描述  : 用于处理HIFI复位。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifireset_task(void *arg)
{
    int iresult = BSP_RESET_OK;

#ifdef _DRV_LLT_
#else
    for ( ; ; )
#endif
    {
        down(&(g_reset_assistant_hifi.sem_wait_hifireset));

        printk(KERN_INFO "%s: enter hifireset_task\n", __FUNCTION__);

        printk(KERN_INFO "%s: hifi reset int is coming!\n", __FUNCTION__);

        reset_for_savelog("\n=============hifi reset=============\n");

        /*调用回调函数*/
        iresult = hifireset_runcbfun(MDRV_RESET_CB_BEFORE);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run cb func before hifi reset\n", __FUNCTION__);

            /*复位系统*/
            do_reset_system(RESET_TYPE_HIFIRESET_RUNCB_STEP1_FAIL);
            return BSP_RESET_ERROR;
        }

#ifdef _DRV_LLT_
#else
        if (0 != down_timeout(&(g_reset_assistant_hifi.sem_wait_mcu_msg_hifireset), msecs_to_jiffies(RESET_WAIT_TIMEOUT_MAILMSG)))
        {
            /*复位系统*/
            printk(KERN_ERR "%s: fail to get mail from mcu,reset system\n", __FUNCTION__);
            /*do_reset_system(RESET_TYPE_FAILGET_MSG_FROM_MCU);*/
            return BSP_RESET_ERROR;
        }

        printk(KERN_INFO "%s: to load hifi bin!\n", __FUNCTION__);

        reset_for_savelog("To load hifi bin\n");

       /*重新加载HIFI映像*/
        iresult = hifireset_loadhifibin();
        if (BSP_RESET_OK != iresult)
        {
            reset_for_savelog("fail to load hifi bin\n");
            printk(KERN_ERR "%s: fail to load hifi bin! reset system\n", __FUNCTION__);
            /*复位系统*/
            do_reset_system(RESET_TYPE_HIFIRESET_LOAD_BIN_FAIL);
            return BSP_RESET_ERROR;
        }
#endif

        printk(KERN_INFO "%s: to run cb fun after hifi reset!\n", __FUNCTION__);

        reset_for_savelog("To run cb func after hifi reset\n");

        /*HIFI加载完毕后，调用回调函数*/
        iresult = hifireset_runcbfun(MDRV_RESET_CB_AFTER);

        /*恢复中断使能*/
        finish_reset_sub();

        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run cb fun after hifi reset! reset system\n", __FUNCTION__);
            /*复位系统*/
            do_reset_system(RESET_TYPE_HIFIRESET_RUNCB_STEP2_FAIL);
            return BSP_RESET_ERROR;
        }

        printk(KERN_INFO "%s: reset hifi successfully!\n", __FUNCTION__);

        reset_for_savelog("hifi reset ok\n");
#ifndef _DRV_LLT_
        check_doreset_for_noc();
#endif
		if(himntn_hifi_resetlog==1)
		{
			apr_upload_hificrash();	
		}
    }
}


/*caixi 1 2 11*/
/*****************************************************************************
 函 数 名  :  hifireset_doruncbfun
 功能描述  : HIFI复位前后调用回调函数的函数。由于圈复杂度超标，所以这里封装函数
 输入参数  : DRV_RESET_CB_MOMENT_E eparam, 0 表示HIFI复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int hifireset_doruncbfun (const char *pname, DRV_RESET_CB_MOMENT_E eparam)
{
    int  iresult = BSP_RESET_OK;

    sreset_mgr_LLI  *phead = g_pmgr_hifireset_data;

    if (NULL != pname)
    {
        /*不判断模块名字,按顺序执行*/
        if (strcmp(pname, RESET_CBFUN_IGNORE_NAME) == 0)
        {
            while (NULL != phead)
            {
                if (NULL != phead->cbfuninfo.cbfun)
                {
                    iresult = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
                    if (BSP_RESET_OK != iresult)
                    {
                        /*如果返回失败，记录下组件名字,返回值*/
                        reset_no_ok_savelog(phead->cbfuninfo.name, iresult, eparam, BSP_RESET_MODULE_CCORE);
                        printk(KERN_ERR "%s: fail to run cbfun of %s, at %d return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam, iresult);
                        break;
                    }
                    printk(KERN_INFO "%s: run %s cb function 0x%p\n", __FUNCTION__, phead->cbfuninfo.name, phead->cbfuninfo.cbfun);
                }
                phead = phead->pnext;
            }
        }
        else/*需要判断模块名字，执行指定的回调函数*/
        {
            while (NULL != phead)
            {
                if (strcmp(pname, phead->cbfuninfo.name) == 0
                    && NULL != phead->cbfuninfo.cbfun)
                {
                    iresult  = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
                    printk(KERN_INFO "%s: run %s cb function 0x%p\n", __FUNCTION__, phead->cbfuninfo.name, phead->cbfuninfo.cbfun);
                    break;
                }
                phead = phead->pnext;
            }
        }
    }
    else
    {
        iresult = BSP_RESET_ERROR;
    }
    if (BSP_RESET_OK != iresult)
    {
        if (NULL != phead)
        {
            reset_no_ok_savelog(phead->cbfuninfo.name, iresult, eparam, BSP_RESET_MODULE_HIFI);
            printk(KERN_ERR "%s: fail to run cbfun of %s, at %d, return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam ,iresult);

        }
        else
        {
            printk(KERN_ERR "%s: fail to run cbfun, but phead or pname is null\n", __FUNCTION__);
        }
    }
    return iresult;
}
/*****************************************************************************
 函 数 名  :  hifireset _runcbfun
 功能描述  : HIFI复位前后调用回调函数的函数。
 输入参数  : DRV_RESET_CB_MOMENT_E eparam, 0 表示HIFI复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int hifireset_runcbfun (DRV_RESET_CB_MOMENT_E eparam)
{
    int  iresult = 0;

    if (MDRV_RESET_CB_BEFORE == eparam)
    {
        /*Mailbox 处理*/
        reset_set_cpu_status(1, RESET_CPU_HIFI_STATUS_OFF);
        /*遍历回调函数链表，调用NAS的回调*/
        iresult = hifireset_doruncbfun("NAS_AT", eparam);
        if (BSP_RESET_OK != iresult)
        {
        /*如果返回失败，记录下组建name, 返回值，保存到文件*/
            goto return_error;
        }

        /*遍历回调函数链表，调用音频的回调*/
        iresult = hifireset_doruncbfun("CODEC", eparam);
        if (BSP_RESET_OK != iresult)
        {
        /*如果返回失败，记录下组建name, 返回值，保存到文件*/
            goto return_error;
        }

        /*通知MCU*/
        g_reset_assistant_hifi.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant_hifi.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_HIFI_RESET_BEFORE;
        hifireset_tonotify(eparam, BSP_RESET_MODULE_MCU);
        /*遍历回调函数链表，调用OM的回调*/
        iresult = hifireset_doruncbfun("OAM", eparam);
        if (BSP_RESET_OK != iresult)
        {
        /*如果返回失败，记录下组建name, 返回值，保存到文件*/
            goto return_error;
        }
    }
    else
    {
        /*通知MCU*/
        g_reset_assistant_hifi.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_HIFI_RESET_AFTER;
        hifireset_tonotify(eparam, BSP_RESET_MODULE_MCU);

        /*MAILBOX处理*/
        reset_set_cpu_status(0, RESET_CPU_HIFI_STATUS_OFF);

        /*遍历回调函数链表，调用回调函数*/
        iresult = hifireset_doruncbfun(RESET_CBFUN_IGNORE_NAME, eparam);
        if (BSP_RESET_OK != iresult)
        {
            goto return_error;
        }
    }
    printk(KERN_INFO "%s: end of run cb functions for hifi reset at %d, %d\n", __FUNCTION__, eparam, iresult);
    return BSP_RESET_OK;
return_error:
    return BSP_RESET_ERROR;
}
/*****************************************************************************
 函 数 名  : reset_link_insert
 功能描述  : 将数据插入链表
 输入参数  : sreset_mgr_LLI *plink, 链表指针
             sreset_mgr_LLI *punit，待插入的节点指针
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
sreset_mgr_LLI * reset_link_insert(sreset_mgr_LLI *plink, sreset_mgr_LLI *punit)
{
    sreset_mgr_LLI    *phead = plink;
    sreset_mgr_LLI    *ppose = plink;
    sreset_mgr_LLI    *ptail = plink;

    if (NULL == plink || NULL == punit)
    {
        return NULL;
    }
    while (NULL != ppose)
    {
        /*根据优先级插入到链表中*/
        if (ppose->cbfuninfo.priolevel > punit->cbfuninfo.priolevel)
        {
            if (phead == ppose)
            {
                punit->pnext = ppose;
                phead = punit;
            }
            else
            {
                ptail->pnext = punit;
                punit->pnext = ppose;
            }
            break;
        }
        ptail = ppose;
        ppose = ppose->pnext;
    }
    if (NULL == ppose)
    {
        ptail->pnext = punit;
    }
    return phead;
}

/*caixi 3*/
/*****************************************************************************
 函 数 名  : ccorereset_tonotify
 功能描述  : HIFI复位时，通知MCU
 输入参数  : DRV_RESET_CB_MOMENT_E eparam,复位前还是复位后
            int imodule, 要通知的模块，0，MCU;1,HIFI
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifireset_tonotify(DRV_RESET_CB_MOMENT_E eparam, ereset_module emodule)
{
    struct ipc_msg msg;
    int       iret = 0;

    if (MDRV_RESET_CB_BEFORE == eparam)
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            (msg.cmd_mix).cmd_type = TYPE_RESET;
            (msg.cmd_mix).cmd = CMD_NOTIFY;
            (msg.cmd_mix).cmd_obj = OBJ_HIFI;
            (msg.cmd_mix).cmd_src = OBJ_AP;
            msg.mode =ASYNC_CMD;
            memcpy(&msg.data[1], &(g_reset_assistant_hifi.smailboxmsg_mcu),sizeof(g_reset_assistant_hifi.smailboxmsg_mcu));
            iret = ipc_msg_send(OBJ_LPM3, &msg, ASYNC_MODE);
	     if (-1 == iret)
            {
                printk(KERN_ERR "%s: fail to send msg to mcu before resetting hifi\n", __FUNCTION__);  
            }
        }
        else
        {
            printk(KERN_ERR "%s: to notify none in hifireset_tonotify\n", __FUNCTION__);
        }
    }
    else
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            (msg.cmd_mix).cmd_type = TYPE_RESET;
            (msg.cmd_mix).cmd = CMD_NOTIFY;
            (msg.cmd_mix).cmd_obj = OBJ_HIFI;
            (msg.cmd_mix).cmd_src = OBJ_AP;
            msg.mode =ASYNC_CMD;
            memcpy(&msg.data[1], &(g_reset_assistant_hifi.smailboxmsg_mcu),sizeof(g_reset_assistant_hifi.smailboxmsg_mcu));
            iret = ipc_msg_send(OBJ_LPM3, &msg, ASYNC_MODE);
	     if (-1 == iret)
            {
                printk(KERN_ERR "%s: fail to send msg to mcu after resetting hifi\n", __FUNCTION__);  
            }

        }
        else
        {
            printk(KERN_ERR "%s: to notify none in hifireset_tonotify\n", __FUNCTION__);
        }
    }
    printk(KERN_INFO "%s: after send mail to %d, %d\n", __FUNCTION__, emodule, eparam);
    return BSP_RESET_OK;
}

/*caixi 14*/
/*****************************************************************************
 函 数 名  : ccorereset_enable_wdt_irq
 功能描述  : 通过写寄存器实现其他核中断到a核
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
static int ccorereset_enable_wdt_irq(void)
{
#ifdef _DRV_LLT_
#else
	unsigned long sctrl_on  = (unsigned long)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);

    unsigned long uvalue = 0;


    /*WatchDog1 HIFI WatchDog超时复位请求*/
    uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_ADDR(sctrl_on));
    printk(KERN_INFO "%s: org val = 0x%lx\n", __FUNCTION__, uvalue);

    writel((uvalue | (1 << 6)), (void __iomem *)SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_ADDR(sctrl_on));
    uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_ADDR(sctrl_on));
    printk(KERN_INFO "%s: new val = 0x%lx\n", __FUNCTION__, uvalue);

#if 0
        unsigned int  sc_int_en0_0awd           : 1;  /* bit[0] : [0]： ACPU Watchdog复位请求中断，送给MCU。 */
        unsigned int  sc_int_en0_1cm3wdg0       : 1;  /* bit[1] : [1]： CM3 Watchdog0复位请求中断，送给ACPU。 */
        unsigned int  sc_int_en0_2hifiwd        : 1;  /* bit[2] : [2]： HIFI Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en0_3bakwd         : 1;  /* bit[3] : [3]： 备份Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en0_4acpu_sft_fiq  : 1;  /* bit[4] : [4]： acpu fiq int，送给acpu，用于调试，由acpu_sft_fiq_req配置产生。 */
#endif
    writel(0x1f, (void __iomem *)SOC_AO_SCTRL_SC_INT_EN_SET_ADDR(sctrl_on));
    uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_INT_EN_SET_ADDR(sctrl_on));
    printk(KERN_INFO "%s: new uvalue = 0x%lx\n", __FUNCTION__, uvalue);

#endif
    return 0;
}

/*caixi e*/
/*****************************************************************************
 函 数 名  : do_reset_system
 功能描述  : 用于复位系统
 输入参数  : int ireset_type,复位类型
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
static void  do_reset_system(int ireset_type)
{
#ifdef _DRV_LLT_
#else

    systemError((int)BSP_MODU_DORESET, EXCH_S_DORESET, ireset_type, 0, 0);

#endif
}


/*caixi d*/
/*****************************************************************************
 函 数 名  : reset_set_cpu_status
 功能描述  : 当hifi或者ccpu复位时，记下状态，用于mailbox拦击发向hifi  的mail.
 输入参数  : unsigned int iOff  1-不使能;0-关闭
             unsigned int iflag 标识cpu状态对应的偏移
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
static void reset_set_cpu_status(unsigned int iOff, unsigned int iflag)
{
    BSP_CPU_StateSet(iOff,iflag);

    printk(KERN_INFO "%s: reset_set_cpu_status_hifi, ioff = %d, iflag = %d\n", __FUNCTION__, iOff, iflag);
}

/*caixi 9*/
/*****************************************************************************
 函 数 名  : reset_hifi_up_semaphone
 功能描述  : 释放hifi reset信号量
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_hifi_up_semaphone(void)
{
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    up(&(g_reset_assistant_hifi.sem_wait_hifireset));
    printk(KERN_INFO "%s: reset_hifi_up_semaphone\n", __FUNCTION__);
#endif
}



/*caixi 8*/
/*****************************************************************************
 函 数 名  : drv_hifireset_cbfun
 功能描述  : 底软在hifi单独复位时用于处理mailbox相关数据。
 输入参数  : DRV_RESET_CB_MOMENT_E eparam, 表示复位前还是复位后调用
            　int userdata,用户数据．

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
static int drv_hifireset_cbfun(DRV_RESET_CB_MOMENT_E eparam, int userdata)
{
    int     iresult = 0;

#ifdef _DRV_LLT_
#else

    unsigned int   *hifi_nv_vir_addr = NULL;
    phys_addr_t     hifi_nv_phy_addr = 0;

    if (MDRV_RESET_CB_BEFORE == eparam)
    {
        /* clean hifi nv flag */
        hifi_nv_phy_addr = (phys_addr_t)(HIFI_SYS_MEM_ADDR + sizeof(CARM_HIFI_DYN_ADDR_SHARE_STRU));

        /* nv flag is 4byte */
        hifi_nv_vir_addr = (unsigned int*)ioremap_wc(hifi_nv_phy_addr, 4);

        if (NULL == hifi_nv_vir_addr)
        {
            printk(KERN_INFO"RESET LOG %s: hifi_nv_vir_addr ioremap fail.\n", __FUNCTION__);
        }
        else
        {
            (*hifi_nv_vir_addr) = 0;

            printk(KERN_INFO"RESET LOG %s: hifi_nv_phy_addr  = 0x%x, hifi_nv_vir_addr = 0x%p, (*hifi_nv_vir_addr) = 0x%x\n",
                   __FUNCTION__, (unsigned int )hifi_nv_phy_addr, hifi_nv_vir_addr, (*hifi_nv_vir_addr));

            iounmap(hifi_nv_vir_addr);
        }

        printk(KERN_INFO"RESET LOG: reset mediaserver task! before\n");

        sochifi_watchdog_send_event();

        printk(KERN_INFO"RESET LOG: reset mediaserver task! after\n");
    }
    else
    {
    }

    printk(KERN_INFO"RESET LOG: HIFI cb fun %d run ok(%d)\n", eparam, iresult);

#endif

    return iresult;

}

/*caixi c 10*/
/*****************************************************************************
 函 数 名  : reset_no_ok_savelog
 功能描述  : 在调用回调函数时如果回调函数返回失败，则记录下模块名字，返回值
 输入参数  : char *pname,组件注册回调函数时注册的名字；
             int iresult,回调函数的返回值。
             ereset_module emodule, 复位的模块,ccore or hifi
 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int reset_no_ok_savelog(char *pname, int iresult, DRV_RESET_CB_MOMENT_E eparam, ereset_module emodule)
{
#ifdef PRINK_TO_FILE    /*如果实现了printk打印信息生成文件功能，则不需要该函数功能*/
    return BSP_RESET_OK;
#else

    int     ilen = 0;
    char    *psavelog = NULL;
    char    *ptime = (MDRV_RESET_CB_BEFORE == eparam?"before":"after");

    if (NULL == pname)
    {
        printk(KERN_ERR "[%s ]name is NULL, in savelog fun\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }

    psavelog = (char*)kmalloc(BSP_RESET_LOG_INFO_ITEM_LEN, GFP_KERNEL);
    if (NULL == psavelog)
    {
        printk(KERN_ERR "%s: fail to malloc, in savelog fun\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }
    hisi_io_memset((void*)psavelog, 0, BSP_RESET_LOG_INFO_ITEM_LEN);
    switch (emodule)
    {
    case BSP_RESET_MODULE_CCORE:
        sprintf(psavelog, "%s ccore reset, %s fail, return %d\n",ptime, pname, iresult);
        break;
    case BSP_RESET_MODULE_HIFI:
        sprintf(psavelog, "%s hifi reset, %s fail, return %d\n",ptime, pname, iresult);
        break;
    default:
        sprintf(psavelog, "valid module, %s fail, return %d\n",pname, iresult);
        printk(KERN_ERR "%s: module id %d invalid!!, in savelog fun\n", __FUNCTION__, emodule);
        break;

    }
    reset_for_savelog(psavelog);
    kfree(psavelog);
    psavelog = NULL;

    return BSP_RESET_OK;
#endif /* PRINK_TO_FILE */
}
/*****************************************************************************
 函 数 名  : reset_for_savelog
 功能描述  : 保存log
 输入参数  : char *pstr,log内容
 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int reset_for_savelog(char *pstr)
{
#ifdef PRINK_TO_FILE    /*如果实现了printk打印信息生成文件功能，则不需要该函数功能*/
    return BSP_RESET_OK;
#else

#ifdef _DRV_LLT_
#else
    struct file *fd = NULL;
    mm_segment_t fs = 0;
    int iwrite = 0, ilen = 0;

    if (NULL == pstr)
    {
        printk(KERN_ERR "%s: pstr is NULL\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }

    fd = filp_open(BSP_RESET_LOG_FILE, O_RDWR | O_CREAT | O_APPEND, 0666);
    if (IS_ERR(fd))
    {
        printk(KERN_ERR "%s: file can not open\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    ilen = strlen(pstr);
    iwrite = vfs_write(fd, (const char __user *)pstr, ilen, &(fd->f_pos));
    if (iwrite < 1)
    {
        printk(KERN_ERR "%s: can not write all %d/%d\n", __FUNCTION__, iwrite, ilen);
    }
    set_fs(fs);

    printk(KERN_INFO "%s: iwrite %d/%d\n", __FUNCTION__, iwrite, ilen);

    filp_close(fd, NULL);

#endif

    return BSP_RESET_OK;

#endif /* PRINK_TO_FILE */
}

/*****************************************************************************
 函 数 名  : reset_info_show
 功能描述  : 显示可维可测信息
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_info_show_hifi(void)
{
    sreset_mgr_LLI  *phead = g_pmgr_hifireset_data;

    while (NULL != phead)
    {
        printk(KERN_INFO "%s: hifi reset: name:%s cbfun:0x%p data:%d priolevel:%d\n", __FUNCTION__, phead->cbfuninfo.name,
            (void *)phead->cbfuninfo.cbfun, phead->cbfuninfo.userdata, phead->cbfuninfo.priolevel);
        phead = phead->pnext;
    }
}



#ifdef BSP_C_HIFI_RESET_ALONE_TESET_VALID
extern int simulate_irq_hifi_reset(void);
/*caixi a*/
void test_clean_mgr_hifi_link(void)
{
    sreset_mgr_LLI  *phead = g_pmgr_hifireset_data;
    sreset_mgr_LLI  *ptail = g_pmgr_hifireset_data;

    while (NULL != phead)
    {
        ptail = phead->pnext;
        kfree(phead);
        phead = ptail;
    }
    g_pmgr_hifireset_data = NULL;
}
#endif

#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
static int __init reset_sub_mgr_init_hifi(void)
{
    struct task_struct    *pHifiTask = NULL;
    int     iret = BSP_RESET_OK;

    /*默认HIFI使能*/
    reset_set_cpu_status(0, RESET_CPU_HIFI_STATUS_OFF);



    /*创建需要的信号量*/
    printk(KERN_INFO "%s: enter\n", __FUNCTION__);
    hisi_io_memset(&g_reset_assistant_hifi, 0, sizeof(g_reset_assistant_hifi));
    sema_init(&(g_reset_assistant_hifi.sem_wait_hifireset), SEM_EMPTY);
    sema_init(&(g_reset_assistant_hifi.sem_wait_mcu_msg_hifireset), SEM_EMPTY);

    /*创建hifi复位处理线程*/
    pHifiTask = kthread_run(hifireset_task,  NULL, "hifireset_task");
    printk(KERN_INFO "%s: create hifireset_task, return %p\n", __FUNCTION__, pHifiTask);

    ccorereset_enable_wdt_irq();


    /*注册HIFI复位回调函数*//*hifi复位，底软不注册*/
    hifireset_regcbfunc("CODEC", drv_hifireset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL);

    iret = ipc_msg_req_callback(OBJ_HIFI, CMD_NOTIFY, mailbox_recfun_mcu_hifireset);
    printk(KERN_INFO"RESET LOG: LEAVE reset_sub_mgr_init0! iret = %d\n", iret);

    return BSP_RESET_OK;
}

extern void hifi_freeze_give_semaphone(void);
extern int simulate_irq_mcuwdt_reset(void);
/*****************************************************************************
 函 数 名  : reset_only_for_test
 功能描述  : 该函数只用于测试!!
 输入参数  : int iparam, 测试case:
             0,测试modem 狗复位
             1,测试modem 软件异常复位
             2,测试modem 正常复位
             3,测试hifi复位
             4,测试mcu狗复位
             5,测试底软在ccore复位时所做的处理，如ipf,icc,fileagent,cshell相应功能是否正确
 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
void reset_only_for_test_hifi(int iparam)
{
    //int     iresult = 0;
#ifdef _DRV_LLT_
#else
    switch (iparam)
    {
    case 3:
        up(&(g_reset_assistant_hifi.sem_wait_hifireset));
		up(&(g_reset_assistant_hifi.sem_wait_mcu_msg_hifireset));
        hifi_freeze_give_semaphone();
        break;
    case 4:
        simulate_irq_mcuwdt_reset();
        break;
    case 6:
       reset_no_ok_savelog("drv", -2, 0, BSP_RESET_MODULE_HIFI);
       do_reset_system(RESET_TYPE_FOR_TEST);
       break;
    case 10:
        test_clean_mgr_hifi_link();
        break;
    case 12:
        hifireset_runcbfun(MDRV_RESET_CB_BEFORE);
        hifireset_runcbfun(MDRV_RESET_CB_AFTER);

        break;
    case 16:
        up(&(g_reset_assistant_hifi.sem_wait_hifireset));
        break;
    case 17:
        reset_no_ok_savelog("drv", -2, 0, BSP_RESET_MODULE_HIFI);
        break;
    case 18:
        break;
    default:
        break;
    }
#endif
}
#endif
subsys_initcall(reset_sub_mgr_init_hifi);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
