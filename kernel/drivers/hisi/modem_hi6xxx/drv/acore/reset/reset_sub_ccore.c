
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/notifier.h>         /* For notifier... */
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
#include "rfilesystem_process.h"
#include "rfilesystem_interface.h"
#include "reset_sub_ccore.h"
#include "../icc/bsp_icc_drv.h"
#include "bsp_drv_ipc.h"
#include "bsp_mailbox.h"
#include "../usb/usb_shell.h"
#include <linux/hisi/etb.h>
#include "excDrv.h"
#include "mdrv_udi.h"
#include "../ipf/ipf_driver.h"
#include "reset_sub_ccore.h"
#ifndef _DRV_LLT_
#include <linux/hisi/util.h>         /* For mach_call_usermoduleshell*/
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
宏以及变量定义
*****************************************************************************/
/*Ccore复位标志，寄存器值*/
#define CCORE_RESET_FLAG_VALUE            0x87654321


/*Ccore复位成功标志*/
#define CCORE_RESET_OK_FROM_CCORE         0x12345678

/*规避加载modem镜像失败的问题*/
#define MAX_CCORE_RESET_COUNT                          3
#if 0
/*C核狗相关值*/
#define CCORE_WDT_REG_VALUE_UNLOCK        0x1ACCE551
#define CCORE_WDT_REG_VALUE_LOCK          0x00000000
#endif

#define RESET_CBFUNC_PRIO_LEVEL_LOWT      0
#define RESET_CBFUNC_PRIO_LEVEL_HIGH      49

/*ms, time of wating mail msg reply from hifi/mcu*/
#define RESET_WAIT_TIMEOUT_MAILMSG        5000

/*ms, time of wating mail msg reply from ccpu*/
#define RESET_WAIT_CCPU_STARTUP_TIMEOUT   30000

#define RESET_CBFUN_IGNORE_NAME           "NONAME"

#ifndef NULL
#define NULL    0
#endif

#define CCORE_RESET_TASK_PHASE_SET(a)     (g_stResetDebugInfo.ulCcpuTaskPhase = (a))
#define CCORE_RESET_TASK_PHASE_GET()      (g_stResetDebugInfo.ulCcpuTaskPhase)


/*****************************************************************************
全局变量
*****************************************************************************/
/*用于协助完成复位过程*/
sreset_mgr_assistant_ccore   g_reset_assistant;
/*Link used for c core reset*/
sreset_mgr_LLI  *g_pmgr_ccorereset_data = NULL;

/*single reset debug info*/
sreset_debug_info g_stResetDebugInfo;

static int     s_iccore_pwr_on = 0;


void __iomem *reset_sub_mstrl = NULL; /*for modem sc register*/

extern unsigned int modem_reset_reason; /*0: reboot, 1: panic*/

/*****************************************************************************
函数声明
*****************************************************************************/
static void reset_set_cpu_status(unsigned int iOff, unsigned int iflag);
static void  do_reset_system(int ireset_type);
int ccorereset_tonotify(DRV_RESET_CB_MOMENT_E eparam, ereset_module emodule);
int ccorereset_runcbfun (DRV_RESET_CB_MOMENT_E eparam);
static int  set_for_cshell(DRV_RESET_CB_MOMENT_E eparam);
int  close_all_filehandle(DRV_RESET_CB_MOMENT_E eparam);

extern int ccorereset_loadcbin_loadaddr(void);
extern int platform_ddr_protect_init(int flag);
extern void BSP_CPU_StateSet(unsigned int iOff,unsigned int offset);
extern sreset_mgr_LLI * reset_do_regcbfunc(sreset_mgr_LLI *plink, const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel);
extern int ccorereset_loadbbebin(void);
extern int ccorereset_loadcbin(void);
extern void finish_reset_sub_ccore(void);
extern s32 load_check_sec_image(void);

#ifndef _DRV_LLT_
extern void check_modem_run(void);
#endif



/*****************************************************************************
函数定义
*****************************************************************************/
IFC_GEN_CALL1(MAILBOX_IFC_ACPU_TO_CCPU_RST_MODEM, BSP_CCPU_Reset_Byhimself,
                IFC_INCNT, int, is_normal, 0)




/*****************************************************************************
 函 数 名  : mailbox_recfun_mcu
 功能描述  : 用于接收来自于MCU的MAILBOX消息
 输入参数  : void  *UserHandle, 用户句柄
            void *MailHandle, 邮箱句柄
            unsigned long MailLen, 数据长度
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void mailbox_recfun_mcu(void  *UserHandle,void *MailHandle,unsigned int MailLen)
{
    unsigned long   ulresult = 0;
    unsigned int   msg_len = sizeof(int);
    int msg_mcu = 0;

    ulresult = mailbox_read_msg_data(MailHandle, (unsigned char *)(&msg_mcu), &msg_len);
    printk(KERN_INFO "%s: receive mail from mcu, msg_mcu = %d,result = %ld\n", __FUNCTION__, msg_mcu, ulresult);
    if (0 == msg_mcu)
    {
        up(&(g_reset_assistant.sem_wait_mcu_msg));
    }
    else
    {
        /*保存log*/
        reset_no_ok_savelog("mcu", (int)msg_mcu, MDRV_RESET_CB_BEFORE, BSP_RESET_MODULE_CCORE);

        /*复位系统*/
        printk(KERN_ERR "%s: mcu give wrong msg, msg is %d\n", __FUNCTION__, msg_mcu);
        do_reset_system(RESET_TYPE_MSG_FROM_MCU_WRONG);
    }
}

/*caixi 4 5 f*/


/*****************************************************************************
 函 数 名  : ccorereset_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理Modem复位前后相关数据。
 输入参数  : const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    g_pmgr_ccorereset_data = reset_do_regcbfunc(g_pmgr_ccorereset_data, pname, pcbfun, userdata, priolevel);
    printk(KERN_INFO "%s: %s registered a cbfun for ccore reset\n", __FUNCTION__, pname);
    return BSP_RESET_OK;
}

int mdrv_sysboot_register_reset_notify(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    return ccorereset_regcbfunc(pname, pcbfun, userdata, priolevel);
}



/*****************************************************************************
 函 数 名  : mailbox_recfun_hifi
 功能描述  : 用于接收来自于HIFI的MAILBOX消息
 输入参数  : void  *UserHandle, 用户句柄
            void *MailHandle, 邮箱句柄
            unsigned long MailLen, 数据长度
 返 回 值  : void
*****************************************************************************/
void mailbox_recfun_hifi(void  *UserHandle,void *MailHandle,unsigned int MailLen)
{
    unsigned long   ulresult = 0;
    unsigned int   msg_len = sizeof(HIFI_AP_CCPU_RESET_CNF_STRU);
    HIFI_AP_CCPU_RESET_CNF_STRU msg_hifi = {0};

    ulresult = mailbox_read_msg_data(MailHandle, (unsigned char *)(&msg_hifi), &msg_len);
    printk(KERN_INFO "%s: receive mail from hifi, msg_hifi ret = %d,result = %ld\n", __FUNCTION__, msg_hifi.uhwResult, ulresult);

    if (ID_HIFI_AP_CCPU_RESET_CNF == msg_hifi.uhwMsgId
        && 0 == msg_hifi.uhwResult)
    {
        up(&(g_reset_assistant.sem_wait_hifi_msg));
    }
    else
    {
        /*复位系统*/
        printk(KERN_ERR "%s: hifi's msg is wrong\n", __FUNCTION__);
        do_reset_system(RESET_TYPE_MSG_FROM_HIFI_WRONG);
    }
}

/*caixi 6*/

/*****************************************************************************
 函 数 名  : ccorereset_task
 功能描述  : ccpu复位消息通知其他cpu。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_notify_other_cpu(struct notifier_block *cb, unsigned long code, void *p)
{
        int iResult = BSP_RESET_OK;
        /*填充消息内容*/
        g_reset_assistant.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_CCORE_RESET_BEFORE;
        /*消息ID*/
        g_reset_assistant.smailboxmsg_hifi.uhwMsgId = ID_AP_HIFI_CCPU_RESET_REQ;
        /*发送消息给HIFI*/
        printk(KERN_INFO "%s: msg id send to hifi is %d\n", __FUNCTION__, g_reset_assistant.smailboxmsg_hifi.uhwMsgId);

        iResult = ccorereset_tonotify(MDRV_RESET_CB_BEFORE, BSP_RESET_MODULE_HIFI);
        if(BSP_RESET_OK != iResult)
        {
            printk(KERN_ERR "%s: fail send msg to hifi\n", __FUNCTION__);
        }

        iResult = ccorereset_tonotify(MDRV_RESET_CB_BEFORE, BSP_RESET_MODULE_MCU);
        if(BSP_RESET_OK != iResult)
        {
            printk(KERN_ERR "%s: fail send msg to mcu\n", __FUNCTION__);
            return BSP_RESET_ERROR;
        }
        return BSP_RESET_OK;
}

/*****************************************************************************
 函 数 名  : ccorereset_task
 功能描述  : 等待其他cpu反馈。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
void ccorereset_wait_other_cpu(void)
{
#ifndef _DRV_LLT_
        if (0 != down_timeout(&(g_reset_assistant.sem_wait_hifi_msg), msecs_to_jiffies(RESET_WAIT_TIMEOUT_MAILMSG)))
        {
            printk(KERN_ERR "%s: fail to get mail from hifi,but go on\n", __FUNCTION__);
        }
#endif

#ifndef _DRV_LLT_
        if (0 != down_timeout(&(g_reset_assistant.sem_wait_mcu_msg), msecs_to_jiffies(RESET_WAIT_TIMEOUT_MAILMSG)))
        {
            /*复位系统*/
            printk(KERN_ERR "%s: fail to get mail from mcu,reset system\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_FAILGET_MSG_FROM_MCU);
            return;
        }
#endif
        printk(KERN_INFO "%s: receive mail from mcu\n", __FUNCTION__);
        printk(KERN_INFO "%s: leave\n", __FUNCTION__);
}
void ccoreset_ccpu_freq(void)
{
    unsigned int reg_val;
    void __iomem *pmctrl_base = ioremap_nocache(SOC_PMCTRL_BASE_ADDR, 0x1000);
    void __iomem *peri_sctrl_base = ioremap_nocache(SOC_PERI_SCTRL_BASE_ADDR, 0x1000);
    void __iomem *modem_sctrl_base = ioremap_nocache(SOC_MODEM_SCTRL_BASE_ADDR, 0x1000);

    /*CCPU needs set pmctrl for CCPU ctrl sys_pll*/
    writel(0x0, (void __iomem *)SOC_PMCTRL_MEDPLLALIAS_ADDR(pmctrl_base));

    /*ACPU select sys_pll for CCPU*/
    reg_val = ((0x1 << SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_START) \
    | (0x1 << SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_msk_START));
    writel(reg_val, (void __iomem *)SOC_PERI_SCTRL_SC_CLK_SEL0_ADDR(peri_sctrl_base));

    /*config div0 for ccpu core pll, div2 for ccpu lbus pll*/
    reg_val = 0x3 | (0x1 << SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG0_clk_div0_ccpu_cfg_vld_START);
    writel(reg_val, (void __iomem *)SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG0_ADDR(modem_sctrl_base));

    reg_val = (0x1 << SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG2_clk_div2_ccpu_lbus_cfg_value_START) \
            | (0x1 << SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG2_clk_div2_ccpu_lbus_cfg_vld_START);
    writel(reg_val, (void __iomem *)SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG2_ADDR(modem_sctrl_base));

    iounmap(pmctrl_base);
    iounmap(peri_sctrl_base);
    iounmap(modem_sctrl_base);

	return;

}
/**
 *	mkdir recursive, change owner and group at the same time
 */
static int reset_log_mkdirs(const char *path, umode_t umode, uid_t user, gid_t group)
{
    char cur_path[RESET_LOG_FILE_PATH_SIZE] = {0};
    int index = 0;
    int iret = 0;

    if (*path != '/') {
        return -1;
    }
    cur_path[index++] = *path++;

    while (*path != '\0') {
        if (*path == '/') {
            iret = sys_mkdir(cur_path, umode);
            /*the dir may exist if the result is not equal to 0, so needn't return*/
            if (0 != iret) {
               printk(KERN_ERR" Fail to create the dir %s!, result is %d\n", cur_path, iret);
            }else{
                sys_chown((const char __user *)cur_path, user, group);
            }
        }
        cur_path[index] = *path;
        path++;
        index++;
    }
    return 0;
}

/**
 *	reset_log_fsave - create a file
 *	@dir: the path of the file
*	@name: just name, like reset.log
*	@data: pointer to buffer to write
*	@length: length to write
 */
static int reset_log_fsave(char *dir, char *name, void *data, int length)
{
    int ret = 0;
    int fd = 0;
    unsigned long fs = 0;
    char filename[RESET_LOG_FILE_PATH_SIZE];
    
    if((NULL == dir)||(NULL == name)||(NULL == data)||(0 == length))
    {
        printk(KERN_ERR "input para error \n");
        return -1;
    }
    	
    if((strlen(dir) + strlen("/") + strlen(name)) >= RESET_LOG_FILE_PATH_SIZE)
    {
        printk(KERN_ERR "pathname too loooooooong , %s, %s \n", dir, name);
        return -1;
    }    

    memset(filename, 0, RESET_LOG_FILE_PATH_SIZE);

    fs = get_fs();
    set_fs(KERNEL_DS);
    ret = reset_log_mkdirs(dir, RESET_LOG_CPDIR_PROVALUE, RESET_LOG_OWNER_UID, RESET_LOG_OWNER_GID);
    if(ret < 0) {
        printk(KERN_ERR "mkdir fail");
        set_fs(fs);
        return ret;
    }

    strncpy(filename, dir, strlen(dir));
    *(filename + strlen(dir)) = '\0';
    strncat(filename, "/", strlen("/"));
    strncat(filename, name, strlen(name));

    fd = sys_open((const char __user *)filename, (O_RDWR | O_CREAT | O_CLOEXEC), EXCH_FILE_OPEN_MODE);
    if (fd < 0) {
        printk(KERN_ERR "sys_open  fail, filename:%s, fd is %x \n", filename, fd);
        set_fs(fs);
        return fd;
    }

    ret = sys_write(fd, data, length);
    if (ret  >  0) {
        ret = sys_fsync(fd);
        if (ret < 0) {
            printk(KERN_ERR "sys_fsync fail,the ret is %x\n", ret);
            goto out;
            }
    }
    else 
    {
        printk(KERN_ERR "sys_write  fail,the ret is %x\n", ret);
        goto out;
     }
     
out:
    ret = sys_close(fd);
    if (ret < 0) {
        printk(KERN_ERR "sys_close  fail, the ret is %x\n",ret);
    }    
    set_fs(fs);
    return ret;
}

static char* get_reset_reason(unsigned int whyReboot)
{
    char * reason = NULL;
    switch (whyReboot) {
		case EXCH_CCORE_S_NORMAL:
			reason = "EXCH_S_NORMAL";
			break;
		case EXCH_CCORE_S_ARM:
			reason = "EXCH_S_ARM";
			break;
		case EXCH_CCORE_S_OS:
			reason = "EXCH_S_OS";
			break;
		case EXCH_CCORE_S_WDT:
			reason = "EXCH_S_WDT";
			break;
		case EXCH_CCORE_S_SF:
			reason = "EXCH_S_SF";
			break;
		case EXCH_CCORE_S_NOC:
			reason = "EXCH_S_NOC";
			break;
		case EXCH_CCORE_S_SD:
			reason = "EXCH_S_SD";
			break;
		case EXCH_CCORE_S_SCI:
			reason = "EXCH_S_SCI";
			break;
		case EXCH_CCORE_S_USB:
			reason = "EXCH_S_USB";
			break;
		case EXCH_CCORE_S_ONLINE:
			reason = "EXCH_S_ONLINE";
			break;
		case EXCH_CCORE_S_SSI_READ:
			reason = "EXCH_S_SSI_READ";
			break;
		case EXCH_CCORE_S_SSI_WRITE:
			reason = "EXCH_S_SSI_WRITE";
			break;
		case EXCH_CCORE_S_MODEM_REBOOT_BYRIL:
			reason = "EXCH_S_MODEM_REBOOT_BYRIL";
			break;
		case EXCH_CCORE_S_MODEM_PANIC_BYRIL:
			reason = "EXCH_S_MODEM_PANIC_BYRIL";
			break;
		case EXCH_CCORE_S_IRQ_REQUIRE_ERROR:
			reason = "EXCH_S_IRQ_REQUIRE_ERROR";
			break;
		case EXCH_CCORE_S_SC_ERR:
			reason = "EXCH_S_SC_ERR";
			break;
		case EXCH_CCORE_S_PMU:
			reason = "EXCH_S_PMU";
			break;
		case EXCH_CCORE_S_PMU_ERR:
			reason = "EXCH_S_PMU_ERR";
			break;
		default:
			reason = "UNKNOW EXCE";
			break;
		}

	return reason;
}


static char* get_reset_core(unsigned int whyReboot)
{
    if (EXCH_CCORE_S_MODEM_PANIC_BYRIL == whyReboot)
    {
        return "AP";
    }else
    {
        return "CP";
    }
}


/**
 *	save_modem_reset_info - save the modem crash info --reset.log
 *	It's very small, only 1024 bytes, can keep  in commercial version and upload to APR.
 */
void save_modem_reset_info(void)
{
    char tmp_out_buf[RESET_LOG_MAX_SIZE]={0};
    EH_ROOT_CCORE * exch_root = NULL;
    unsigned int current_int;
    int off=0, ret = 0;
    
    exch_root = (EH_ROOT_CCORE*)EXCH_C_CORE_BASE_ADDR;
    off = snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "rdr:system reboot reason: %s %s\n",
                   get_reset_reason(exch_root->arg1), get_reset_core(exch_root->arg1));
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reset_core:%s\n", get_reset_core(exch_root->arg1));
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reset_reason:%s\n", get_reset_reason(exch_root->arg1));
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reboot_context:0x%x\n", exch_root->rsv1);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reboot_task:0x%x\n", exch_root->rebootTask);
    if ( 0 == *(exch_root->taskName))
    {
        off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "task_name:%s\n", "NULL");
    }
    else
    {
        off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "task_name:%s\n", exch_root->taskName);
    }

    if (EXCH_STATE_INT == exch_root->rsv1)
    {
        current_int = exch_root->intId;
    }else
    {
        current_int = -1;
    }
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reboot_int:0x%x\n", current_int);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "modid:0x%x\n", exch_root->modId);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg1:0x%x\n", exch_root->arg1);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg2:0x%x\n", exch_root->arg2);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg3:0x%x\n", exch_root->rsv2);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg3_len:0x%x\n", exch_root->rsv3);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "vec:0x%x\n", exch_root->vec);

    if(RESET_LOG_MAX_SIZE > off)
        ret = reset_log_fsave(RESET_LOG_PATH, RESET_LOG_FILE, tmp_out_buf, off);
	
    if(ret < 0)
        printk(KERN_ERR "reset_log_fsave, ret is %x \n", ret);

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
   //构造输出文件时间，形成一个字符串，如2012-7-13-8:15:38,表示:系统时间下的 年月日时分秒
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
 函 数 名  : apr_upload_modemcrash
 功能描述  : 通知logservice打包相关路径LOG信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
static void apr_upload_modemcrash(void)
{
	char apr_cmd[MAX_APR_CMD_LEN]={0};
	int ret = 0;
	char output_zip_filename[MAX_APR_FILE_LEN] = {0};
	char local_time[MAX_APR_LOCAL_TIME_STR] = {0};
	get_local_time(local_time, MAX_APR_LOCAL_TIME_STR);
	snprintf(output_zip_filename,MAX_APR_FILE_LEN,"%s_%s",local_time,"modemcrash");
	snprintf(apr_cmd,MAX_APR_CMD_LEN,"archive -d %s -d %s -d %s -o %s -z zip",AP_LOG_PATH_APR, CP_LOG_PATH_APR, HIFI_LOG_PATH_APR,output_zip_filename);
	ret = log_to_exception("balong_hifi",apr_cmd);
	if(ret<0)
	{
		printk(KERN_ERR "modem log write apr_exception err\n");
	}
}
/*****************************************************************************
 函 数 名  : ccorereset_task
 功能描述  : 用于处理Modem复位。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_task(void *arg)
{
    int iresult = BSP_RESET_OK;
    /*规避加载modem镜像失败的问题*/
    int crstcount = 0;
#ifdef _DRV_LLT_
#else
    for ( ; ; )
#endif
    {
        down(&(g_reset_assistant.sem_wait_ccorereset));

        printk(KERN_ERR "%s: enter\n", __FUNCTION__);

        CCORE_RESET_TASK_PHASE_SET(0);

        /*等待其他cpu的回复,这里主要指hifi和mcu*/
        ccorereset_wait_other_cpu();

        CCORE_RESET_TASK_PHASE_SET(1);

        /*调用复位前各组件注册回调函数*/
        iresult = ccorereset_runcbfun(MDRV_RESET_CB_BEFORE);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run callback fun before reset\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_RUNCB_STEP1_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(2);

        save_modem_reset_info();
        #ifdef  CONFIG_TZDRIVER
        /*modem reset is moved into SEC_OS*/
        #else
        /*复位ccpu、modem uart以及ipf等*/
        iresult = ccorereset_doreset();
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s:: fail to reset ccore\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_DORESET);
            return BSP_RESET_ERROR;
        }
        #endif

        CCORE_RESET_TASK_PHASE_SET(3);

#ifdef _DRV_LLT_
#else
       /*重新加载BBE16核映像*/
        iresult = ccorereset_loadbbebin();
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to load bbe16 bin\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_LOAD_BBE16_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(4);

        /*规避加载modem镜像失败的问题
           检测到加载modem失败重新加载modem
           最多加载三次*/
        crstcount = MAX_CCORE_RESET_COUNT;
        iresult = BSP_RESET_ERROR;
        while(BSP_RESET_OK != iresult && crstcount > 0 )
        {
            #ifdef  CONFIG_TZDRIVER
            /*重新加载C核映像*/
            iresult = load_check_sec_image();
            #else
            iresult = ccorereset_loadcbin();
            #endif
            crstcount --;
            printk(KERN_ERR "%s: The count of modem loading:%d \n", __FUNCTION__,crstcount);
        }
        
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to load ccpu bin\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_LOAD_CCPU_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(5);
#endif
        /*用于表示ccore已经复位*/
        g_reset_assistant.iccore_reset = 1;

        /*设置寄存器值,告诉ccpu这是复位后开机*/
        (void)ccorereset_setCresetFlag();

        CCORE_RESET_TASK_PHASE_SET(6);
        #ifdef  CONFIG_TZDRIVER
        /*modem disreset is moved into SEC_OS*/
        #else
        /*解复位ccpu*/
        (void)ccorereset_dostart();
        #endif

        CCORE_RESET_TASK_PHASE_SET(7);

        //enable_etm_reset(); /*just for compile by x00195127*/
        CCORE_RESET_TASK_PHASE_SET(8);

        /*设置发送给mcu的mailbox数据值,发送消息给MCU*/
        g_reset_assistant.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_CCORE_RESET_AFTER;
        iresult = ccorereset_tonotify(MDRV_RESET_CB_AFTER, BSP_RESET_MODULE_MCU);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to notify mcu after reset\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_NOTIFY_AFTER);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(9);

        /*等待ccpu启动成功通知*/
        printk(KERN_ERR "%s: wait for ccore startup finish\n", __FUNCTION__);

        if (0 != down_timeout(&(g_reset_assistant.sem_wait_ccorereset_ok), msecs_to_jiffies(RESET_WAIT_CCPU_STARTUP_TIMEOUT)))
        {
            printk(KERN_ERR "%s: wait for ccpu startup, timeout\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_CRUN_NO_OK);
            return BSP_RESET_ERROR;
        }

        /*ccpu启动成功通知*/
        printk(KERN_ERR "%s: ccore startup finish yet\n", __FUNCTION__);

        CCORE_RESET_TASK_PHASE_SET(10);

        /*调用复位后回调函数*/
        iresult = ccorereset_runcbfun(MDRV_RESET_CB_AFTER);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run after-cb fun\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_RUNCB_STEP2_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(11);

        /*设置发送给mcu的mailbox数据值,发送消息给MCU*/
        g_reset_assistant.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_CCORE_RESET_FINISH;
        iresult = ccorereset_tonotify(MDRV_RESET_CB_AFTER, BSP_RESET_MODULE_MCU);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to notify mcu finish reset\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_NOTIFY_FINISH);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(12);
        printk(KERN_ERR "%s: leave\n", __FUNCTION__);
/*C核Noc不触发整机复位*/
#if 0
#ifndef _DRV_LLT_
        check_doreset_for_noc();
#endif
#endif
        if (modem_reset_reason)
        {
            apr_upload_modemcrash();
        }
    }
}

/*caixi 7*/


/*****************************************************************************
 函 数 名  : ccorereset_runcbfun
 功能描述  : Modem复位前后调用回调函数的函数。
 输入参数  : DRV_RESET_CB_MOMENT_E eparam, 0 表示modem复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int ccorereset_runcbfun (DRV_RESET_CB_MOMENT_E eparam)
{
    int  iresult = 0;

    sreset_mgr_LLI  *phead = g_pmgr_ccorereset_data;

    if (MDRV_RESET_CB_BEFORE == eparam)
    {
        reset_set_cpu_status(1, RESET_CPU_CCORE_STATUS_OFF);
    }
    else
    {
        reset_set_cpu_status(0, RESET_CPU_CCORE_STATUS_OFF);
    }
    /*根据回调函数优先级，调用回调函数*/
    while (NULL != phead)
    {
        if (NULL != phead->cbfuninfo.cbfun)
        {
            iresult = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
            if (BSP_RESET_OK != iresult)
            {
                /*如果返回失败，记录下组件名字,返回值*/
                printk(KERN_ERR "%s: fail to run cbfun of %s, at %d return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam, iresult);
                break;
            }
            printk(KERN_INFO "%s: run %s cb function 0x%x success\n", __FUNCTION__, phead->cbfuninfo.name, (unsigned int)phead->cbfuninfo.cbfun);
        }
        phead = phead->pnext;
    }
    printk(KERN_INFO "%s: leave, eparam = %d, iresult = %d\n", __FUNCTION__, eparam, iresult);
    return iresult;
}

/*caixi 1 2 11*/


/*****************************************************************************
 函 数 名  : ccorereset_tonotify
 功能描述  : CCORE复位时，通知MCU或者HIFI
 输入参数  : DRV_RESET_CB_MOMENT_E eparam,复位前还是复位后
            int imodule, 要通知的模块，0，MCU;1,HIFI
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_tonotify(DRV_RESET_CB_MOMENT_E eparam, ereset_module emodule)
{
    unsigned long   result = 0;

    if (MDRV_RESET_CB_BEFORE == eparam)
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            result = mailbox_send_msg(BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
                (void *)(&g_reset_assistant.smailboxmsg_mcu), sizeof(g_reset_assistant.smailboxmsg_mcu));
        }
        else if (BSP_RESET_MODULE_HIFI == emodule)
        {
            result = mailbox_send_msg(MAILBOX_MAILCODE_ACPU_TO_HIFI_CCORE_RESET_ID,
                (void *)(&g_reset_assistant.smailboxmsg_hifi), sizeof(g_reset_assistant.smailboxmsg_hifi));
        }
    }
    else
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            result = mailbox_send_msg(BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
                (void *)(&g_reset_assistant.smailboxmsg_mcu), sizeof(g_reset_assistant.smailboxmsg_mcu));
        }
        else
        {
            /*printk(KERN_INFO "%s: to notify none in ccorereset_tonotify\n", __FUNCTION__);*/
        }
    }
    /*printk(KERN_INFO "%s: send mail to %d, at %d, result=%d\n", __FUNCTION__, emodule, eparam, result);*/

    return ((MAILBOX_OK == result) ? BSP_RESET_OK : BSP_RESET_ERROR);
}
/*caixi 3*/
/*****************************************************************************
 函 数 名  : ccorereset_setCresetFlag
 功能描述  : 设置复位CCORE标志寄存器
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_setCresetFlag(void)
{
#ifdef _DRV_LLT_
#else
#if defined(CHIP_BB_HI6210)
    void __iomem * sctrl_off = (void __iomem *)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);
    unsigned uvalue = 0;

    uvalue = readl(SOC_PERI_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
    printk(KERN_INFO"RESET LOG: ccorereset_setCresetFlag,org value1 = 0x%x\n", uvalue);

    writel(CCORE_RESET_FLAG_VALUE, SOC_PERI_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
#else
    unsigned long sctrl_off = (unsigned long)IO_ADDRESS(SOC_SC_OFF_BASE_ADDR);
    unsigned long uvalue = 0;

    uvalue = readl((void __iomem *)SOC_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
    printk(KERN_INFO "%s: org value = 0x%lx\n", __FUNCTION__, uvalue);

    writel(CCORE_RESET_FLAG_VALUE, (void __iomem *)SOC_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
#endif

#endif
    printk(KERN_INFO "%s: leave\n", __FUNCTION__);
    return 0;
}
#ifdef  CONFIG_TZDRIVER
/*The  ccore reset and disreset  are  moved into SEC_OS*/
#else
/*****************************************************************************
 函 数 名  : ccorereset_doreset
 功能描述  : 复位CCORE
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_doreset(void)
{
#ifdef _DRV_LLT_
#else
    void __iomem *sctrl_on  = (void __iomem *)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);
    void __iomem *sctrl_off = (void __iomem *)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    unsigned  uvalue_jedge = 0;
    volatile unsigned  uvalue = 0;

    /*reset modem edma*/
    if((get_domain_access_status(ACCESS_DOMAIN_MODEM_SC) == 1)&&(NULL != reset_sub_mstrl) )
    {
        writel(BIT(SOC_MODEM_SCTRL_MDMSC_PERIPH_RSTEN_periph_rsten_mdmac_START),
                SOC_MODEM_SCTRL_MDMSC_PERIPH_RSTEN_ADDR(reset_sub_mstrl));
    }

    /*配置AO_Sctrl寄存器SC_PW_CTRL1的mdm_nsp_domain_idlereq为1'b1*/
    writel((BIT(SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_START)
           |BIT(SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_msk_START)),
            SOC_AO_SCTRL_SC_PW_CTRL1_ADDR(sctrl_on));

    /*轮询AO_Sctrl寄存器SC_PW_STAT1的mdm_nsp_domain_idle和mdm_nsp_domain_idleack，
    当其值均为"1'b1"时，说明MDM NOC隔离成功，才能对Modem子系统进行全局软复位*/
    while(1)
    {
        uvalue = readl(SOC_AO_SCTRL_SC_PW_STAT1_ADDR(sctrl_on));
        uvalue_jedge = (BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_START)
                        | BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idle_START));
        if ((uvalue & uvalue_jedge) == uvalue_jedge)
        {
            break;
        }
    }

    printk(KERN_INFO"RESET LOG: ccorereset_doreset,org value1 = 0x%x\n", uvalue);

    /*配置AO_Sctrl寄存器SC_PERIPH_RSTEN4的periph_rsten4_30mdm_subsys_glb，复位整个Modem子系统*/
    writel(BIT(SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_30mdm_subsys_glb_START),
            SOC_AO_SCTRL_SC_PERIPH_RSTEN4_ADDR(sctrl_on));


    /*iso*/
    writel(BIT(SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_4mcpu_START), SOC_AO_SCTRL_SC_PW_ISOEN0_ADDR(sctrl_on));

    uvalue = *(unsigned int *)(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_ADDR(sctrl_on));
    /*mcpu外设复位*/
    if ((uvalue & BIT(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_0peri_START)) > 0)
    {
        writel((BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_atb_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_lbus_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_por_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_core_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dbg_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_l2_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_socdbg_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_etm_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dap_START)),
                SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_ADDR(sctrl_off));
    }


    printk(KERN_INFO "%s: leave\n", __FUNCTION__);
#endif
    return BSP_RESET_OK;
}

/*****************************************************************************
 函 数 名  : ccorereset_dostart
 功能描述  : 解复位CCORE
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_dostart(void)
{
#ifdef _DRV_LLT_
#else

    unsigned long sctrl_on  = (unsigned long)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);
    unsigned long sctrl_off = (unsigned long)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    unsigned long   iviraddr_dest1 = 0;

    int      ivalue = 0;
    unsigned int uvalue = 0;
    int      iloadmodem_addr = 0;

    /*配置AO_Sctrl寄存器SC_PERIPH_RSTDIS4的periph_rsten4_30mdm_subsys_glb
      解除Modem子系统的全局复位*/
    writel(BIT(SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_30mdm_subsys_glb_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_ADDR(sctrl_on));

    /*配置AO_Sctrl寄存器SC_PW_CTRL1的mdm_nsp_domain_idlereq为1'b0*/
    writel(BIT(SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_msk_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_CTRL1_ADDR(sctrl_on));

    /*轮询AO_Sctrl寄存器SC_PW_STAT1的mdm_nsp_domain_idleack，当其值均为"1'b0"时，
      说明MDM NOC隔离解除，MDM NOC可以被MCU或ACPU正常访问*/
    while(1)
    {
        uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_PW_STAT1_ADDR(sctrl_on));

        if (0 == (uvalue & BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_START)))
        {
            break;
        }
    }

    /* modem 的起始地址固定为零地址，因此需要在零地址写跳转指令 */
    iviraddr_dest1 = (unsigned long)(ioremap_nocache(0x00000000, sizeof(int)));
    printk(KERN_INFO "%s: addr: 0x%lx, data: 0x%x\n", __FUNCTION__, iviraddr_dest1, ivalue);

    writel(0xE59F0000, (void __iomem *)iviraddr_dest1);            /*ldr  r0, 0x8*/
    iounmap((void*)iviraddr_dest1);

    iviraddr_dest1 = (unsigned long)(ioremap_nocache(0x00000004, sizeof(int)));
    printk(KERN_INFO "%s: addr: 0x%lx, data: 0x%x\n", __FUNCTION__, iviraddr_dest1, ivalue);
    writel(0xE1A0F000, (void __iomem *)iviraddr_dest1);   /*mov  pc, r0*/
    iounmap((void*)iviraddr_dest1);

    iloadmodem_addr = ccorereset_loadcbin_loadaddr();
    iviraddr_dest1 = (unsigned long)(ioremap_nocache(0x00000008, sizeof(int)));
    printk(KERN_INFO "%s: addr: 0x%lx, data: 0x%x\n", __FUNCTION__, iviraddr_dest1, iloadmodem_addr);
    writel(iloadmodem_addr, (void __iomem *)iviraddr_dest1);   /*写入modem 镜像起始地址*/
    iounmap((void*)iviraddr_dest1);

    /*w00140341 B050 modify 配置AO_Sctrl寄存器SC_PW_CTRL打开Modem Noc时钟，
    并查询AO_Sctrl寄存器SC_PW_STAT1的mdm_nsp_domain_idleack为1'b0*/
    writel((BIT(SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_START)
           |BIT(SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_msk_START)),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_CTRL_ADDR(sctrl_on));

    /*wait mdm_nsp_domain_idleack 0:idle req not ack*/
    while(1)
    {
        uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_PW_STAT1_ADDR(sctrl_on));
        if (0 == (uvalue & (BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_START))))
        {
            break;
        }
    }

	/*配置Ccpu频率为150M*/
	ccoreset_ccpu_freq();

    writel(BIT(SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_4mcpu_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_CLKEN0_ADDR(sctrl_on));

    writel((BIT(SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_lbus_START)
            |BIT(SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_kernel_START)),
            (void __iomem *)SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_ADDR(sctrl_off));

    writel(BIT(SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_4mcpu_START),
               (void __iomem *)SOC_AO_SCTRL_SC_PW_ISODIS0_ADDR(sctrl_on));

    writel(BIT(SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_4mcpu_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_RSTDIS0_ADDR(sctrl_on));
    /*配置AO_Sctrl寄存器SC_PW_RSTDIS0，解除CCPU分区复位。
    软件等待至少256个ARM时钟周期*/
    udelay(10);

    printk(KERN_INFO"RESET LOG: to restart\n");
    /*temp modify CCPU*/
   writel(BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_lbus_START),
          (void __iomem *)SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_ADDR(sctrl_off));

    writel((BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dap_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_atb_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_por_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_core_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dbg_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_l2_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_socdbg_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_etm_START)),
            (void __iomem *)SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_ADDR(sctrl_off));

    /* carm debug上电 */
    uvalue = *(unsigned int *)SOC_PERI_SCTRL_SC_CARM_CTRL0_ADDR(sctrl_off);
    uvalue |= (1 << SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_dbgpwrup_START);
    writel(uvalue, \
            (void __iomem *)SOC_PERI_SCTRL_SC_CARM_CTRL0_ADDR(sctrl_off));
#endif
    printk(KERN_INFO "%s: leave\n", __FUNCTION__);
    return 0;
}
#endif

/*****************************************************************************
 函 数 名  : ccorereset_icc_cok_notify
 功能描述  : 用于Modem复位成功后调用以便通知ACPU复位成功。
 输入参数  : 无

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
unsigned int ccorereset_icc_cok_notify(unsigned int chenalId,signed int u32size)
{
    unsigned int readSize;
    int ireadvalue = 0;


    /*ICC读取数据*/
    readSize = mdrv_udi_read(g_reset_assistant.ccorereset_IccFd,(void*)(&ireadvalue),u32size);
    printk(KERN_INFO "%s: get icc msg from c core, readsize = %d, wantsize = %d, ireadvalue = 0x%x\n", \
                    __FUNCTION__, readSize, u32size, ireadvalue);

    if (readSize != u32size)
    {
        printk(KERN_ERR "%s: udi_read 's size is error readSize:%d VS u32size:%d\n", __FUNCTION__, readSize, u32size);
        return BSP_RESET_ERROR;
    }
    printk(KERN_INFO "%s: g_reset_assistant.iccore_reset = %d\n", \
                    __FUNCTION__, g_reset_assistant.iccore_reset);

    if (1 == g_reset_assistant.iccore_reset)
    {
        if (CCORE_RESET_OK_FROM_CCORE == ireadvalue)
        {
            printk(KERN_INFO "%s: c tell me it's ok\n", __FUNCTION__);
            /*释放ｃ核启动成功信号量*/
            up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        }
        else
        {
            printk(KERN_ERR "%s: fail to run c core!return %d\n", __FUNCTION__, ireadvalue);
            /*do nothing*/
            do_reset_system(RESET_TYPE_CRESET_CRUN_NO_OK);
        }
    }
    /*非联编版本,用于冒烟时判断c核是否启动成功*/
    else
    {
        s_iccore_pwr_on = 1;
#ifndef _DRV_LLT_
        check_modem_run();
#endif

    }
    return BSP_RESET_OK;
}

/*caixi 14*/
#ifdef _DRV_LLT_
int platform_ddr_protect_init(int flag)
{
    return 0;
}
#endif

#ifndef _DRV_LLT_
void check_modem_run(void)
{
    RFILE    *pf = NULL;

    if (1 == s_iccore_pwr_on)
    {
        pf = BSP_fopen("/data/hisi_logs/cp_log/coredump/mdmpwron.log", "w");
        printk(KERN_INFO "%s: pf = 0x%x\n", \
                        __FUNCTION__, (unsigned int)pf);
        BSP_fclose(pf);
        pf = NULL;
    }
}
#endif
/*****************************************************************************
 函 数 名  : drv_ccorereset_cbfun
 功能描述  : 底软在Modem单独复位时用于处理IPF,ICC, FILEAGENT,CSHELL, IFC相关数据。
 输入参数  : DRV_RESET_CB_MOMENT_E eparam, 表示复位前还是复位后调用
            　int userdata,用户数据．

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int drv_ccorereset_cbfun(DRV_RESET_CB_MOMENT_E eparam, int userdata)
{
    int     iresult = 0;

#ifdef _DRV_LLT_
#else

    if (MDRV_RESET_CB_BEFORE == eparam)
    {
        /*复位ICC通道*/
        BSP_ICC_ResetFun();

        /*调用处理IPF数据的函数*/
        BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_FORBID);
        /*清除c核ipc锁*/
        BSP_IPC_SemGive_Ccore_All();
    }
    else
    {
        BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_ALLOW);

        /*使能复位前关闭的中断*/
        finish_reset_sub_ccore();
    }
#endif

    /*调用FILEAGENT关闭句柄*/
    iresult = close_all_filehandle(eparam);
    if (0 != iresult)
    {
        printk(KERN_ERR "%s: drv_close file fail, return %d\n", __FUNCTION__, iresult);
        goto error_return;
    }

    /*调用处理CSHELL相关的函数*/
    iresult = set_for_cshell(eparam);
    if (0 != iresult)
    {
        printk(KERN_ERR "%s: drv_cshell fail, return %d\n", __FUNCTION__, iresult);
        goto error_return;
    }

    return iresult;
error_return:
    printk(KERN_ERR "%s: drv cb fun run fail, return = %d\n", __FUNCTION__, iresult);
    return BSP_RESET_ERROR;
}


/*****************************************************************************
 函 数 名  : reset_icc_open
 功能描述  : 打开icc
 输入参数  : 无

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int reset_icc_open(void)
{
    g_reset_assistant.attr.enChanMode  = ICC_CHAN_MODE_PACKET;
    g_reset_assistant.attr.u32Priority = 0;
    g_reset_assistant.attr.u32TimeOut  = 10000;
    g_reset_assistant.attr.event_cb    = NULL;
    g_reset_assistant.attr.read_cb     = ccorereset_icc_cok_notify;
    g_reset_assistant.attr.write_cb    = NULL;
    g_reset_assistant.attr.u32FIFOInSize  = 1024;
    g_reset_assistant.attr.u32FIFOOutSize = 1024;

    g_reset_assistant.ccorereset_IccParam.devid = UDI_ICC_GUOM7_ID;
    g_reset_assistant.ccorereset_IccParam.pPrivate = &(g_reset_assistant.attr);

    /*打开ICC通道*/
    g_reset_assistant.ccorereset_IccFd = mdrv_udi_open(&(g_reset_assistant.ccorereset_IccParam));

    if (g_reset_assistant.ccorereset_IccFd <= 0)
    {
        printk(KERN_ERR "%s: in reset_icc_open ICC failed:[0x%x]\n", __FUNCTION__, g_reset_assistant.ccorereset_IccFd);

        return BSP_RESET_ERROR;
    }
    printk(KERN_INFO "%s: in reset_icc_open ICC SUCCESS:[0x%x]\n", __FUNCTION__, g_reset_assistant.ccorereset_IccFd);
    return BSP_RESET_OK;
}
static int  set_for_cshell(DRV_RESET_CB_MOMENT_E eparam)
{
#ifdef _DRV_LLT_
#else
    if(MDRV_RESET_CB_BEFORE == eparam)
    {
        /*cshell invalid*/
        /* bsp_ccpu_rest_cshell_handle(CSHELL_INVALID); */
    }
    else
    {
        bsp_ccpu_rest_cshell_handle(CSHELL_VALID);
    }
#endif
    return BSP_RESET_OK;
}


int  close_all_filehandle(DRV_RESET_CB_MOMENT_E eparam)
{
#ifdef _DRV_LLT_
#else
    if(MDRV_RESET_CB_BEFORE == eparam)
    {
        bsp_ccpu_rest_rfile_handle();
    }
    else
    {
        ;
    }
#endif
    return BSP_RESET_OK;
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
    printk(KERN_INFO "%s: reset_set_cpu_status, ioff = %d, iflag = %d\n", __FUNCTION__, iOff, iflag);
}

/*****************************************************************************
 函 数 名  : reset_ccore_up_semaphone
 功能描述  : 释放ccore reset信号量
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_ccore_up_semaphone(void)
{
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    up(&(g_reset_assistant.sem_wait_ccorereset));
    printk(KERN_INFO "%s: reset_ccore_up_semaphone\n", __FUNCTION__);
#endif
}

void reset_ccore_up_byhimself(int is_normal)
{
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    printk(KERN_INFO "%s: reset_ccore_up_byhimselfs\n", __FUNCTION__);
    BSP_CCPU_Reset_Byhimself(is_normal, 0);

#endif
}

/*************************************************************************
可维可测接口
*************************************************************************/




/*****************************************************************************
 函 数 名  : reset_info_show
 功能描述  : 显示可维可测信息
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_info_show(void)
{
    sreset_mgr_LLI  *phead = g_pmgr_ccorereset_data;

    while (NULL != phead)
    {
        printk(KERN_INFO "%s: name:%s cbfun:0x%8x data:%d priolevel:%d\n", __FUNCTION__, phead->cbfuninfo.name,
            (unsigned int)phead->cbfuninfo.cbfun, phead->cbfuninfo.userdata, phead->cbfuninfo.priolevel);
        phead = phead->pnext;
    }


    /*C核单独复位状态*/
    printk(KERN_INFO "%s: ccore reset task process %d phase\n", __FUNCTION__, CCORE_RESET_TASK_PHASE_GET());
}

/*测试用接口*/
#ifdef BSP_C_HIFI_RESET_ALONE_TESET_VALID
/*These functions are used for testing!!!only!!!, starts*/
extern int simulate_irq_c_freeze(void);
extern int simulate_irq_c_panic(void);
extern int simulate_irq_c_reset(int is_normal);

#ifdef _DRV_LLT_
#else
extern BSP_S32 BSP_IPF_ST_101(BSP_U32 u32PacketNum);
#endif

void test_clean_mgr_ccore_link(void)
{
    sreset_mgr_LLI  *phead = g_pmgr_ccorereset_data;
    sreset_mgr_LLI  *ptail = g_pmgr_ccorereset_data;

    while (NULL != phead)
    {
        ptail = phead->pnext;
        kfree(phead);
        phead = ptail;
    }
    g_pmgr_ccorereset_data = NULL;
}

/*caixi a*/

/*ends*/



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
void reset_only_for_test(int iparam)
{
    //int     iresult = 0;
#ifdef _DRV_LLT_
#else
    switch (iparam)
    {
    case 0:
        up(&(g_reset_assistant.sem_wait_hifi_msg));
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        reset_ccore_up_semaphone();
        break;
    case 1:
        up(&(g_reset_assistant.sem_wait_hifi_msg));
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        reset_ccore_up_semaphone();
        break;
    case 2:
        up(&(g_reset_assistant.sem_wait_hifi_msg));
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        simulate_irq_c_reset(1);
        break;
    case 5:
        //y00145322
        /*send ip*/
        //iresult = BSP_IPF_ST_101(20);

        drv_ccorereset_cbfun(MDRV_RESET_CB_BEFORE, 0);
        /*send ip*/
        //iresult = BSP_IPF_ST_101(20);
        break;
    case 7:
        ccorereset_regcbfunc("drv", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL);
        ccorereset_regcbfunc("OAM", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL+1);
        ccorereset_regcbfunc("TTF", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL-1);
        ccorereset_regcbfunc("NAS", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL+1);

        break;
    case 9:
        test_clean_mgr_ccore_link();
        break;
    case 11:
        ccorereset_runcbfun(MDRV_RESET_CB_BEFORE);
        ccorereset_runcbfun(MDRV_RESET_CB_AFTER);
        break;
    case 13:
        up(&(g_reset_assistant.sem_wait_hifi_msg));

        break;
    case 14:
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        break;
    case 15:
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        break;
    default:
        break;
    }
#endif
}

#endif



/*Added by l00212112,functions for c reset alone are defined, ends*/
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
/*****************************************************************************
 函 数 名  : reset_sub_mgr_init
 功能描述  : 用于C核,hifi单独复位机制的初始化,在reset_module_init中调用,用于创建信号量,创建任务等操作.
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
int reset_sub_mgr_init (void)
{
    struct task_struct    *pCCoreTask = NULL;
    int     iret = BSP_RESET_OK;

    /*默认CCPU使能*/
    reset_set_cpu_status(0, RESET_CPU_CCORE_STATUS_OFF);

    /*remap modem 系统控制寄存器*/
    reset_sub_mstrl = ioremap(SOC_MODEM_SCTRL_BASE_ADDR,0x1000);
    if (NULL == reset_sub_mstrl)
    {
        printk(KERN_ERR "%s: reset_sub_mstrl remap err\n", __FUNCTION__);
    }

    /*创建需要的信号量*/
    printk(KERN_INFO "%s: enter\n", __FUNCTION__);
    hisi_io_memset(&g_stResetDebugInfo, 0, sizeof(sreset_debug_info));
    hisi_io_memset(&g_reset_assistant, 0, sizeof(sreset_mgr_assistant_ccore));
    sema_init(&(g_reset_assistant.sem_wait_ccorereset), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_ccorereset_ok), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_hifi_msg), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_mcu_msg), SEM_EMPTY);

    /*创建C复位处理线程*/
    pCCoreTask = kthread_run(ccorereset_task,  NULL, "ccorereset_task");
    printk(KERN_INFO "%s: create ccorereset_task, return %p\n", __FUNCTION__, pCCoreTask);

    /*注册C核复位回调函数*/
    ccorereset_regcbfunc("DRV", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL);

    /*注册C核复位回调函数*/
    reset_icc_open();

    /*注册ACPU与MCU之间的MAILBOX，在c核单独复位时等待hifi的消息*/
    iret = mailbox_reg_msg_cb(MAILBOX_MAILCODE_MCU_TO_ACPU_CCORE_RESET_ID, mailbox_recfun_mcu, NULL);
    printk(KERN_INFO"RESET LOG: LEAVE reset_sub_mgr_init0! iret = %d\n", iret);


   /*注册ACPU与HIFI之间的MAILBOX,在c核单独复位时等待mcu的消息*/
    iret = mailbox_reg_msg_cb(MAILBOX_MAILCODE_HIFI_TO_ACPU_CCORE_RESET_ID, mailbox_recfun_hifi, NULL);
    printk(KERN_INFO "%s: leave, iret = %d\n", __FUNCTION__, iret);
    return BSP_RESET_OK;
}
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
