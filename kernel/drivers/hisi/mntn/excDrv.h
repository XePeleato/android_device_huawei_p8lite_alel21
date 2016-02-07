

/*******************************************************************************
  1 其他头文件包含
*******************************************************************************/
#include  "global_ddr_map.h"
#include  <asm/io.h>
#include  <linux/spinlock.h>
#include  <linux/sched.h>
//#include  "BSP.h"

#ifndef __EXCDRV_H__
#define __EXCDRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*******************************************************************************
  2 宏定义
*******************************************************************************/

#define EXCH_TEST_FUNCTION_ON

#define EXC_READ_REG(reg, resu32t)  ((resu32t) = *(volatile unsigned int *)(reg))
#define EXC_WRITE_REG(addr,value) (*((volatile unsigned int  *)(addr)) = (value))

#define LINUX_VERSION   "Linux_"

#define EXCH_ERROR      (-1)
#define EXCH_OK         (0)

#define EXCH_READY      (1)
#define EXCH_NOT_READY  (0)
#define EXCH_REBOOT_FROM_EXC (0x12348765)
#define EXCH_FILE_OPEN_MODE         0666    //(S_IRUGO | S_IWUGO) (RW)
#define EXCH_FILE_NUM_MAX           2
#define EXCH_FILE_NAME_LEN_MAX      128
#define EXCH_DIR_ENTRY_SIZE         1024
#define EXCH_RST_LOG_BUF_SIZE       256
#define EXCH_COPY_BUF_SIZE          2048
#define EXCH_RESET_LOG_MAX          0x40000
#define EXCH_RTC_TIME_LEN           16
#define EXCH_MODID_PIDMASK	(1<<24)

/*记录任务信息个数*/
#define EXCH_RECORD_TASK_NUM_MAX    124
#define EXCH_CUST_TASK_NUM_MAX      8
#define EXCH_CUST_FUNC_NUM_MAX      64
#define EXCH_8K_SIZE                0x2000
/*#define EXCH_CB_NAME_SIZE           32  跟C核M核保存一致*/
#define EXCH_MEM_IS_READY           0x12345678
#define EXCH_TASK_NAME_LEN          16
#define EXCH_TASK_NAME_PART_LEN     12
#define EXCH_TASK_TCB_SIZE          (0x0400)
#define EXCH_QUEUE_NAME_LEN         12
#ifdef CONFIG_ARM64
#define EXCH_REG_NUM                36
#else
#define EXCH_REG_NUM                18
#endif

#define EXCH_A_CORE                 (1)
#define EXCH_C_CORE                 (2)
#define EXCH_M_CORE                 (3)

#define SYSTEM_NORMAL_RESET         0xffffffff
#define SC_SW_RST_VAL               0x48698284

#define EXCH_INT_EXIT               0xAAAA

#define EXCH_SYSTEM_ERROR           0x12345678

/*******************************************************************************
*                                                                              *
*                    以下保存异常时寄存器地址-Begin                            *
*                                                                              *
*******************************************************************************/

/*确定临终遗言模块DDR内存的起始地址与大小*/
#define EXCH_DDR_BASE_ADDR              ANDROID_DUMP_LOG_ADDR
#define EXCH_BASE_ADDR                  EXCH_A_CORE_BASE_ADDR
#define EXCH_ALL_SIZE                   (EXCH_A_CORE_SIZE + EXCH_C_CORE_SIZE + EXCH_M_CORE_SIZE)

#define EXCH_A_CORE_BASE_ADDR           g_exch_a_core_base_addr
#define EXCH_C_CORE_BASE_ADDR           g_exch_c_core_base_addr
#define EXCH_M_CORE_BASE_ADDR           g_exch_m_core_base_addr

#define EXCH_A_CORE_SIZE                ANDROID_DUMP_LOG_SIZE   //2M
#define EXCH_C_CORE_SIZE                MODEM_DUMP_LOG_SIZE     //2M
#define EXCH_M_CORE_SIZE                MCU_DUMP_LOG_SIZE       //1M

#define EXCH_EXC_BIN_SIZE               (EXCH_A_CORE_SIZE)





/********************************A核内存区规划*********************************/
/*A核起始地址*/
#define EXCH_A_CORE_BASE               (EXCH_A_CORE_BASE_ADDR)//

/*A核内存区起始段用于保存全局结构体g_pExchRoot*/
#define EXCH_ROOT_ADDR                 (EXCH_A_CORE_BASE) //0x0
#define EXCH_ROOT_SIZE                 (0x400)   //1K

/*记录每个CPU上最后一个任务TCB信息*/
#define EXCH_LAST_TASK_TCB_ADDR        (EXCH_ROOT_ADDR + EXCH_ROOT_SIZE)//0x400
#define EXCH_LAST_TASK_TCB_SIZE        (EXCH_TASK_TCB_SIZE * NR_CPUS)     //8K(0x400*8)

/*记录用户订制参数信息*/
#define EXCH_CUSTOM_ARG_ADDR           (EXCH_LAST_TASK_TCB_ADDR + EXCH_LAST_TASK_TCB_SIZE)//0x2400
#define EXCH_CUSTOM_ARG_SIZE           (0x0400)

/*记录每个CPU上中断切换信息*/
#define EXCH_INT_SWITCH_ADDR           (EXCH_CUSTOM_ARG_ADDR + EXCH_CUSTOM_ARG_SIZE)    //0x2800
#define EXCH_INT_SWITCH_SIZE           (0x10000)    //64K

/*记录每个CPU上任务切换信息*/
#define EXCH_TASK_SWITCH_ADDR          (EXCH_INT_SWITCH_ADDR + EXCH_INT_SWITCH_SIZE)    //0x12800
#define EXCH_TASK_SWITCH_SIZE          (0x40000)    //256K

/*记录每个CPU上最后一个异常中断栈信息*/
#define EXCH_LAST_INT_STACK_ADDR       (EXCH_TASK_SWITCH_ADDR + EXCH_TASK_SWITCH_SIZE)  //0x52800
#define EXCH_LAST_INT_STACK_SIZE       (0x0400)     //1K

/*记录每个CPU上最后一个异常任务栈信息*/
#define EXCH_LAST_TASK_STACK_ADDR      (EXCH_LAST_INT_STACK_ADDR + EXCH_LAST_INT_STACK_SIZE)    //0x52C00
#define EXCH_LAST_TASK_STACK_SIZE      (EXCH_8K_SIZE * NR_CPUS)     //64K(0x2000*8)

/*记录A核124个任务的任务名*/
#define EXCH_ALL_TASK_NAME_ADDR        (EXCH_LAST_TASK_STACK_ADDR + EXCH_LAST_TASK_STACK_SIZE)  //0x62C00
#define EXCH_ALL_TASK_NAME_SIZE        (0x1000)   //4K

/*记录A核124个任务的TCB*/
#define EXCH_ALL_TASK_TCB_ADDR         (EXCH_ALL_TASK_NAME_ADDR + EXCH_ALL_TASK_NAME_SIZE)      //0x63C00
#define EXCH_ALL_TASK_TCB_SIZE         (0x1F000)  //124K

/*记录OM模块异常信息*/
#define EXCH_OSA_ADDR                   (EXCH_ALL_TASK_TCB_ADDR + EXCH_ALL_TASK_TCB_SIZE)       //0x82C00
#define EXCH_OSA_SIZE                   (0x8000)  //32K

/*用于记录用户指定任务队列信息*/
#define EXCH_CUSTOM_TASK_QUEUE_ADDR     (EXCH_OSA_ADDR + EXCH_OSA_SIZE) //0x8AC00
#define EXCH_CUSTOM_TASK_QUEUE_SIZE     (0x400)
/*用于记录用户指定任务的任务信息*/
#define EXCH_CUSTOM_TASK_INFO_ADDR      (EXCH_CUSTOM_TASK_QUEUE_ADDR + EXCH_CUSTOM_TASK_QUEUE_SIZE) //0x8B000
#define EXCH_CUSTOM_TASK_INFO_SIZE      (0x10000) //64K
/*用于记录用户注册的回调接口信息*/
#define EXCH_CUSTOM_FUNC_QUEUE_ADDR     (EXCH_CUSTOM_TASK_INFO_ADDR + EXCH_CUSTOM_TASK_INFO_SIZE)   //0x9B000
#define EXCH_CUSTOM_FUNC_QUEUE_SIZE     (0x400)
/*用于记录应用注册的回调传入的buffer*/
#define EXCH_CUSTOM_FUNC_INFO_ADDR      (EXCH_CUSTOM_FUNC_QUEUE_ADDR + EXCH_CUSTOM_FUNC_QUEUE_SIZE) //0x9B400
#define EXCH_CUSTOM_FUNC_INFO_SIZE      (0x10000) //64K

/*记录系统GIC状态寄存器*/
#define EXCH_A_GIC_INT_STS_ADDR         (EXCH_CUSTOM_FUNC_INFO_ADDR + EXCH_CUSTOM_FUNC_INFO_SIZE)//0xAB400
#define EXCH_A_GIC_INT_STS_SIZE         (0x80)    //128B

/*记录系统异常打印信息*/
#define EXCH_LOG_KMSG_ADDR              (EXCH_A_GIC_INT_STS_ADDR + EXCH_A_GIC_INT_STS_SIZE)//0xAB480
#define EXCH_LOG_KMSG_SIZE              (0x40000) //256K

/* ram-console因页对齐占用一部分该空间 */
#define EXCH_LOG_KMSG_RESERVED_ADDR     (EXCH_LOG_KMSG_ADDR + EXCH_LOG_KMSG_SIZE)//0xEB480
#define EXCH_LOG_KMSG_RESERVED_SIZE     (0x1C00) //7K

#define PWRCTRL_EXC_ACPU_ADDR           (EXCH_LOG_KMSG_RESERVED_ADDR + EXCH_LOG_KMSG_RESERVED_SIZE)//0xED080
#define PWRCTRL_EXC_ACPU_SIZE           (0x400)  //1K

#define SOC_AO_SCTRL_ADDR               (PWRCTRL_EXC_ACPU_ADDR + PWRCTRL_EXC_ACPU_SIZE)   //0xED480
#define SOC_AO_SCTRL_SIZE               (0x2000)  //8K

#define EXCH_DRV_CUSTOM_INFO_ADDR       (SOC_AO_SCTRL_ADDR + SOC_AO_SCTRL_SIZE)   //0xEF480
#define EXCH_DRV_CUSTOM_INFO_SIZE       (0x2000)  //8K

/****************************A/C/M核内存共享区规划*****************************/

/* A/C/M核内存共享区，位于SRAM */
/* 共享内存区，用于保存各核共享信息，主控核写，各核均可读 */
#define EXCH_CORES_SHARE_ADDR             (EXCH_DRV_CUSTOM_INFO_ADDR + EXCH_DRV_CUSTOM_INFO_SIZE)   //0xF1480
#define EXCH_CORES_SHARE_SIZE             (0x1000)                //4K

/*sc off 4k,pmctrl 4k,ddrc 36k,sysnoc 128byte*/
#define SOC_PERI_SCTRL_ADDR                  (EXCH_CORES_SHARE_ADDR + EXCH_CORES_SHARE_SIZE)/*0xF2480*/
#define SOC_PERI_SCTRL_SIZE                  (0x1000)

#define SOC_PMCTRL_ADDR                 (SOC_PERI_SCTRL_ADDR + SOC_PERI_SCTRL_SIZE)/*0xF3480*/
#define SOC_PMCTRL_SIZE                 (0x1000)

#define SOC_SYS_NOC_ADDR                (SOC_PMCTRL_ADDR + SOC_PMCTRL_SIZE)/*0xF4480*/
#define SOC_SYS_NOC_SIZE                (0x80)

#define SOC_DDRC_ADDR                   (SOC_SYS_NOC_ADDR + SOC_SYS_NOC_SIZE)/*0xF4500*/
#define SOC_DDRC_SIZE                   (0x9000)

#define SOC_ACPU_SC_ADDR            (SOC_DDRC_ADDR + SOC_DDRC_SIZE)/*0xFD500*/
#define SOC_ACPU_SC_SIZE             (0x800)    //2k

#define SOC_MODEM_SC_ADDR         (SOC_ACPU_SC_ADDR + SOC_MODEM_SC_SIZE)/*0xFDD00*/
#define SOC_MODEM_SC_SIZE           (0x1000)   //4k

#define SOC_MEDIA_SC_ADDR           (SOC_MODEM_SC_ADDR + SOC_MODEM_SC_SIZE)/*0xFED00*/
#define SOC_MEDIA_SC_SIZE             (0x800)   //2k

#define CPU_NUMS setup_max_cpus

#define HIMNTN_VALID_SIZE   (32)

struct dump_log
{
    char root_info[EXCH_ROOT_SIZE];
    char last_task[EXCH_LAST_TASK_TCB_SIZE];
    char custom_arg[EXCH_CUSTOM_ARG_SIZE];
    char irq_trace[EXCH_INT_SWITCH_SIZE];
    char task_trace[EXCH_TASK_SWITCH_SIZE];
    char last_irq_stack[EXCH_LAST_INT_STACK_SIZE];
    char last_task_stack[EXCH_LAST_TASK_STACK_SIZE];
    char all_tasks_name[EXCH_ALL_TASK_NAME_SIZE];
    char all_tasks_tcb[EXCH_ALL_TASK_TCB_SIZE];
    char osa_info[EXCH_OSA_SIZE];
    char custom_task_queue[EXCH_CUSTOM_TASK_QUEUE_SIZE];
    char custom_task_info[EXCH_CUSTOM_TASK_INFO_SIZE];
    char custom_func_queue[EXCH_CUSTOM_FUNC_QUEUE_SIZE];
    char custom_func_info[EXCH_CUSTOM_FUNC_INFO_SIZE];
    char gic_irq_status[EXCH_A_GIC_INT_STS_SIZE];
    char log_kmsg[EXCH_LOG_KMSG_SIZE];
    char pm_info[PWRCTRL_EXC_ACPU_SIZE];
    char soc_ao_sctrl[SOC_AO_SCTRL_SIZE];
    char drv_custom_info[EXCH_DRV_CUSTOM_INFO_SIZE];
    char cores_share[EXCH_CORES_SHARE_SIZE];
    char soc_peri_sctrl[SOC_PERI_SCTRL_SIZE];
    char soc_pmctrl[SOC_PMCTRL_SIZE];
    char soc_sys_noc[SOC_SYS_NOC_SIZE];
    char soc_ddrc[SOC_DDRC_SIZE];
    char soc_acpu_sc[SOC_ACPU_SC_SIZE];
    char soc_modem_sc[SOC_MODEM_SC_SIZE];
    char soc_media_sc[SOC_MEDIA_SC_SIZE];
};

struct irq_info
{
    int irq;
	int cpu;
    unsigned long slice;
    unsigned long jiff;
};

struct task_info
{
    char taskname[TASK_COMM_LEN];
    int pid;
	int cpu;
    unsigned long slice;
    unsigned long jiff;
};


/*z00179562,ccore unite,远程文件系统可维可测,starts*/
#define EXCH_RFS_ADDR (MEMORY_RAM_CORESHARE_RFS_MNTN_ADDR)
#define EXCH_RFS_SIZE (MEMORY_RAM_CORESHARE_RFS_MNTN_SIZE)
/*z00179562,ccore unite,远程文件系统可维可测,ends*/

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
typedef enum
{
    EXCH_S_DIE,
    EXCH_S_PANIC,
    EXCH_S_NOC,   /*if the value change, please also change  EXCH_S_NOC of hisi_err_probe.h*/
    EXCH_S_PMU,   /*systemErrorCcore 保持跟C核一致*/
    EXCH_S_DORESET,     /*Acore reset,C核/hifi单独复位失败*/
    EXCH_S_LOGDUMP,     /*按组合键主动复位系统进入log dump*/
    EXCH_S_DDRC_SEC,     /*DDRC SECURITY reboot*/
    EXCH_S_WATCHPOINT,      /*for watchpoint 功能*/
    EXCH_S_GPIO,
    EXCH_S_MAILBOX,
    EXCH_S_OTHERS,
    EXCH_S_CSI0,
    EXCH_S_CSI1,
    EXCH_S_MAX
} EXCH_SOURCE;

#ifdef EXCH_TEST_FUNCTION_ON
enum exchErrType
{
    EXC_UNDEFINE_INSTRUCTION,
    EXC_SOFTWARE_INTERRUPT,
    EXC_PREFETCH_ABORT,
    EXC_DATA_ABORT,
    EXC_DEVISION,
    EXC_PANIC,
    EXC_LOOP,
    EXC_ERR_RESERVED
};
#endif

enum himntnEnum
{
    HIMNTN_DUMP_MEM,
    HIMNTN_IRQ_AND_TASK_SWITCH_RECORD,
    HIMNTN_PRESS_KEY_TO_FASTBOOT,
    HIMNTN_MCU_WDT,
    HIMNTN_ACPU_WDT,
    HIMNTN_CLOSE_ACPU_PANIC_LOOP = 5,
    HIMNTN_GOBAL_RESETLOG,
    HIMNTN_MODEM_RESETLOG,
    HIMNTN_HIFI_RESETLOG,
    HIMNTN_NOC_ErrLog,/* 该字节如果是字符0，则表示noc的超时上报errLog功能被关闭；其他表示打开 */
    HIMNTN_NOC_ERR_LOGGER_RESET = 10,    /*used in kernel, whether reset system when noc error logger irq happend*/
    HIMNTN_FIQ_DEBUGGER,
    HIMNTN_ACCESS_ERROR_REBOOT,/*DDR 安全中断复位使能位*/
    HIMNTN_MBX_ERRLOG_SAVE,     /*老邮箱异常log文件保存*/

    /* 功能默认关闭的,请放置23之后 */
    HIMNTN_LOAD_DDR_RESETLOG = 23,/*带宽 负载触发系统异常 记录LOG 使 能，默认 关闭 */
    HIMNTN_MBFULL_ASSERT,
    HIMNTN_PRINTK_WHO_WAKEUP_ACPU = 25,
    HIMNTN_LOAD_DDR_EN, /*带宽 统计 使 能，默认 打开*/
    HIMNTN_LOAD_DDR_SINGLE_PORT,   /*单带宽端口 统计使能*/
    HIMNTN_POWERKEY_RESET,
    HIMNTN_UNLOCK_CORE_FOR_MCUSAVEPC,/*MCU saves pc of acpu*/
    HIMNTN_WATCHPOINT_EN = 30,/*Watchpoint使能位*/
    HIMNTN_KMEMLEAK_SWITCH,

    /*Add above, and keep the same as definition in nv_number.h in fastboot !!!!*/
	HIMNTN_BOTTOM
};
extern unsigned int himntn_gobal_resetlog;
extern unsigned int himntn_modem_resetlog;
extern unsigned int himntn_hifi_resetlog;
#define HIMNTN_ALWAYS_RESETLOG     1
/*******************************************************************************
  4 消息头定义
*******************************************************************************/


/*******************************************************************************
  5 消息定义
*******************************************************************************/


/*******************************************************************************
  6 STRUCT定义
*******************************************************************************/


typedef struct
{
    unsigned int    reboot_reason;  //复位原因，A核写，各核可读
    unsigned int    reboot_core;    //复位核，A核写，各核可读
    unsigned int    mem_ready;      //内存准备好，A核写，各核读，开始初始化
    unsigned int    is_rebooting;   //系统重启中

}cores_share_info_t;

typedef struct
{
    unsigned int*    reboot_reason;  /*复位原因，A核写，各核可读*/
    unsigned int*    reboot_core;    /*复位核，A核写，各核可读*/
    unsigned int*    mem_ready;      /*内存准备好，A核写，各核读，开始初始化*/
    unsigned int*    is_rebooting;   /*系统重启中*/
}cores_share_addr_info_t;
typedef struct
{
	/*Don't modify any content bellow!!!, for using analyzing tool!!!*/
    unsigned int    dumpMagic;       /* 0x00  */
    unsigned char   version[32];       /* 0x04  */
    unsigned int    rebootTask;        /* 0x24  */  /*rebootTaskPid*/
    unsigned int    rebootTicks;       /* 0x28  */
    unsigned int    modId;             /* 0x2c  */
    int             arg1;              /* 0x30  */
    int             arg2;              /* 0x34  */
    unsigned int    omTimer;           /* 0x38  */
    unsigned int    vec;               /* 0x3c  */
#ifdef CONFIG_ARM64
	u64 			regSet[36];
	u64				cpsr;
#else
    unsigned int    regSet[18];        /* 0x40  */
    unsigned int    cpsr;              /* 0x88  */
#endif
    unsigned int    reboot_src;        /* 0x8c  */
    unsigned int    excSize;           /* 0x90  */
    spinlock_t *    spinlock;          /* 0x94 */
    cores_share_addr_info_t  share_addr;                 /* 0x98  *//* 0xE4  */
    cores_share_info_t  share_value;                /* 0xa8  */
    unsigned char       rtcTime[EXCH_RTC_TIME_LEN]; /* 0xb8 */


    unsigned char*    taskTcbAddr;       /* 0xc8  */   /*lastTaskTcbAddr*/
    unsigned char*    customAddr;        /* 0xcc  */   /*customArgAddr*/

    unsigned char*    taskSwitchAddr;    /* 0xd0  */
    unsigned char*    intStackAddr;      /* 0xd4  */   /*lastIntStackAddr*/
    unsigned char*    taskStackAddr;     /* 0xd8  */   /*lastTaskStackAddr*/

    unsigned char*    customFuncQueueAddr; /* 0xdc */
    unsigned char*    customTaskQueueAddr; /* 0xe0 */
    unsigned char*    customTaskInfoAddr;  /* 0xe4 */
    unsigned char*    customFuncInfoAddr;  /* 0xe8 */

    unsigned char*    allTaskNameAddr;     /* 0xec */
    unsigned char*    intSwitchAddr;       /* 0xf0 */
    unsigned char*    excBaseAddr;         /* 0xf4 */

    void*    taskId[NR_CPUS];      /* 0xf8 */  /*lastTaskId*/
    unsigned int    taskTicks[NR_CPUS];   /* 0x118 */  /*lastTaskTicks*/
    unsigned int    intId[NR_CPUS];       /* 0x138 */  /*lastIntId*/
    unsigned int    intTicks[NR_CPUS];    /* 0x158 */  /*lastIntTicks*/
	/*Don't modify any content above!!!, for using analyzing tool!!!*/
    unsigned long    ulslice_wdg;/**/
	/*Add new content here if need.*/
    unsigned int        struEndOffset[1] ;      /* 0x178 */
}
 EH_ROOT;


typedef struct
{
    int  pid;
    char taskname[EXCH_TASK_NAME_PART_LEN];
} TASK_NAME_STRU;

/*AP CP 解耦，该部分 include/drv/acore/mdrv_stub.h 中也定义了一份，需要同步修改 begin*/
#define EXCH_CB_NAME_SIZE           (32)
typedef struct
{
    char   aucName[EXCH_CB_NAME_SIZE];
    unsigned char *pucData;
    unsigned int  ulDataLen;
}exch_cb_buf_t;

typedef int  (*exch_CBReg)(exch_cb_buf_t *);
/*AP CP 解耦，该部分在该部分 include/drv/acore/mdrv_stub.h 中也定义了一份，需要同步修改 end*/

typedef void  (*exch_mdm_panic_reg)(void);

/*******************************************************************************
  7 UNION定义
*******************************************************************************/


/*******************************************************************************
  8 OTHERS定义
*******************************************************************************/


/*******************************************************************************
  9 全局变量声明
*******************************************************************************/
typedef void (*FUNC_VOID)(void);
/*typedef int  (*exchCBReg)(cb_buf_t *);, remove, redefined!*/
extern unsigned int g_exch_base_addr;
extern unsigned char* g_exch_a_core_base_addr;
extern unsigned char* g_exch_c_core_base_addr;
extern unsigned char* g_exch_m_core_base_addr;
extern char *g_irq_trace_name[NR_CPUS];
extern char *g_task_trace_name[NR_CPUS];
extern unsigned int g_irq_record_len[NR_CPUS];
extern unsigned int g_irq_record_offset[NR_CPUS];
extern unsigned int g_task_record_len[NR_CPUS];
extern unsigned int g_task_record_offset[NR_CPUS];
extern unsigned int g_acpu_unlock_flag_tmp;

/*******************************************************************************
  10 函数声明
*******************************************************************************/
extern void systemError(int modId, int arg1, int arg2, char *arg3, int arg3Length);
extern int  exch_file_save(char *file_name, void *address, unsigned int length);
extern int  DRV_EXCH_TASK_INFO_ADD(int taskPid);
extern int  echc_cust_funt_func_reg(exch_CBReg cb);
extern int  check_himntn(int feature);
extern void* exch_get_drv_addr(void);
extern void reg_save(unsigned int* ulRamAddr, unsigned int* ulReg, unsigned int ulSize);
extern void* exchMemMalloc(unsigned int ulSize);
extern void exchTaskSwitchHook(void *pOldTcb, void *pNewTcb);
extern void exchIntSwitchHook(unsigned int dir, unsigned int oldVec, unsigned int newVec);
extern void exchDieHook(void * currentTaskId, int vec, void * pReg);
extern void set_himntn(int feature, int val);
extern void exch_stand_guard(struct task_struct *tsk);
extern void exch_task_stack_dump(int taskPid);
extern void exch_buf_show(unsigned int offset, unsigned int size);
extern int exch_check_dump_space(unsigned long p_file);
extern void tsAcorePanic(int flag);
extern void exchTest(int exc_type);
extern int cb1(exch_cb_buf_t* p);
extern int exchPANIC(void *arg);
extern int exchDIV(void *arg);
extern void exchWDT(void);
extern void wdt_func(void *data);
extern int exchLOOP(void *arg);
extern int exchDABT(void *arg);
extern int exchPABT(void *arg);
extern int exchSWI(void *arg);
extern int exchUNDEF(void *arg);
extern void exch_hex_dump(unsigned char *buf, unsigned int size, unsigned char per_row);
extern unsigned int exch_mdm_panic_cb_reg(exch_mdm_panic_reg pfunc);
extern void exch_save_pointer_wdg(unsigned long pvalue);
extern void sc_reg_save(void);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of excDrv.h */

