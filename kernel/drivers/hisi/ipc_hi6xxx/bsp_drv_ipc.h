/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_DRV_IPC.h
*
*   作    者 :  wangjing
*
*   描    述 :  IPC模块用户接口文件
*
*   修改记录 :  2011年4月11日  v1.00  wangjing  创建
*************************************************************************/

#ifndef _BSP_DRV_IPC_H_
#define _BSP_DRV_IPC_H_

#include <asm/io.h>
#include "soc_baseaddr_interface.h"
#include "soc_ipc_interface.h"
#include "soc_irqs.h"
#include <linux/hisi/hi6xxx_ipc_enum.h>

#ifdef __cplusplus
extern "C" {
#endif


extern void __iomem *ipc_base;

#define SIZE_4K             (4096)
#define BSP_IPC_BASE_ADDR   (SOC_IPC_S_BASE_ADDR)
#define IPC_REG_SIZE	    (SIZE_4K)


#define BSP_RegRd(uwAddr)                                     readl(uwAddr)
#define BSP_RegWr(uwAddr, uwValue)                            writel(uwValue, uwAddr)


#define IPC_CORE_ACPU         0


#define SOC_IPC_CPU_INT_MASK_DIS_ADDR(base, i)           SOC_IPC_CPU_INT_EN_ADDR(base, i)        
#define SOC_IPC_CPU_INT_MASK_EN_ADDR(base, i)         SOC_IPC_CPU_INT_DIS_ADDR(base, i)  


#define BSP_IPC_CPU_RAW_INT(i)      (SOC_IPC_CPU_RAW_INT_ADDR((unsigned char*)ipc_base, i))    /* CPU_i快速中断原始中断寄存器     */
#define BSP_IPC_CPU_INT_MASK(i)     (SOC_IPC_CPU_INT_MASK_ADDR((unsigned char*)ipc_base, i))   /* CPU_i快速中断掩码寄存器         */
#define BSP_IPC_CPU_INT_STAT(i)     (SOC_IPC_CPU_INT_STAT_ADDR((unsigned char*)ipc_base, i))   /* CPU_i屏蔽后的快速中断状态寄存器 */
#define BSP_IPC_CPU_INT_CLR(i)      (SOC_IPC_CPU_INT_CLR_ADDR((unsigned char*)ipc_base, i))    /* CPU_i快速中断清除寄存器         */
#define BSP_IPC_INT_MASK_EN(i)      (SOC_IPC_CPU_INT_MASK_EN_ADDR((unsigned char*)ipc_base, i))    /* CPU_i快速中断使能           */
#define BSP_IPC_INT_MASK_DIS(i)     (SOC_IPC_CPU_INT_MASK_DIS_ADDR((unsigned char*)ipc_base, i))   /* CPU_i快速中断去使能         */

#define BSP_IPC_SEM_RAW_INT(j)      (SOC_IPC_SEM_RAW_INT_ADDR((unsigned char*)ipc_base, j))    /* CPU_j信号量释放原始中断寄存器   */
#define BSP_IPC_SEM_INT_MASK(j)     (SOC_IPC_SEM_INT_MASK_ADDR((unsigned char*)ipc_base, j))   /* CPU_j信号量释放中断掩码寄存器   */
#define BSP_IPC_SEM_INT_STAT(j)     (SOC_IPC_SEM_INT_STAT_ADDR((unsigned char*)ipc_base, j))   /* CPU_j信号量释放中断状态寄存器   */
#define BSP_IPC_SEM_INT_CLR(j)      (SOC_IPC_SEM_INT_CLR_ADDR((unsigned char*)ipc_base, j))    /* CPU_j信号量释放中断清除寄存器   */
#define BSP_IPC_HS_CTRL(j,k)        (SOC_IPC_HS_CTRL_ADDR((unsigned char*)ipc_base, j, k))     /* CPU_j信号量k请求寄存器      */
#define BSP_IPC_HS_STAT(j,k)        (SOC_IPC_HS_STAT_ADDR((unsigned char*)ipc_base, j, k))     /* CPU_j信号量k状态寄存器      */


#define BSP_IPC_CPU_RAW_INT_ACPU    (BSP_IPC_CPU_RAW_INT(IPC_CORE_ACPU))           /* ACPU快速中断原始中断寄存器     */
#define BSP_IPC_CPU_INT_MASK_ACPU   (BSP_IPC_CPU_INT_MASK(IPC_CORE_ACPU))          /* ACPU快速中断掩码寄存器         */
#define BSP_IPC_CPU_INT_STAT_ACPU   (BSP_IPC_CPU_INT_STAT(IPC_CORE_ACPU))          /* ACPU屏蔽后的快速中断状态寄存器 */
#define BSP_IPC_CPU_INT_CLR_ACPU    (BSP_IPC_CPU_INT_CLR(IPC_CORE_ACPU))           /* ACPU快速中断清除寄存器         */
#define BSP_IPC_CPU_INT_MASK_EN_ACPU    (BSP_IPC_INT_MASK_EN(IPC_CORE_ACPU))       /* ACPU快速中断使能               */
#define BSP_IPC_CPU_INT_MASK_DIS_ACPU   (BSP_IPC_INT_MASK_DIS(IPC_CORE_ACPU))      /* ACPU快速中断去使能             */



#define UCOM_COMM_UINT32_MAX                (0xffffffff)                          /*32bit无符号数最大值*/
#define BSP_IPC_MAX_INT_NUM                 (32)                                  /*  */
#define IPC_MASK						0xFFFFFF0F       /*用于屏蔽[4:7]bit位*/

/*后续需要写到ARM_paxb.h中*/
#define INT_LEV_IPC_CPU				(IRQ_IPC0_S)
#define INT_LEV_IPC_SEM				(IRQ_IPC1_S)

#define INT_VEC_IPC_SEM				IVEC_TO_INUM(INT_LEV_IPC_SEM)
#define INTSRC_NUM					32

#define INT_VEC_IPC_CPU				IVEC_TO_INUM(INT_LEV_IPC_CPU)
#define HI6XXX_IPC_TRUE      (1)
#define HI6XXX_IPC_ERROR     (-1)
#ifndef SEM_EMPTY
#define SEM_EMPTY            (0)
#endif
#define IPC_CHECK_PARA(ulLvl) \
	do{\
		if(ulLvl >= INTSRC_NUM)\
		{\
			printk("Wrong para , line:%d\n", __LINE__);\
        return HI6XXX_IPC_ERROR;\
		}\
	}while(0)

typedef void (*IPCVOIDFUNCPTR)(unsigned int );
/*#define BSP_REG_READ(base, reg, resule) \
	(resule = readl(base + reg))*/

/*#define BSP_REG_WRITE(base, reg, data) \
	(writel(data, (base + reg)))*/
typedef struct tagIPC_DEV_S
{
	int            bInit;
}IPC_DEV_S;

typedef struct {
	IPCVOIDFUNCPTR	routine;
	unsigned int	arg;
} BSP_IPC_ENTRY;

typedef struct tagIPC_DEBUG_E
{
	unsigned int u32RecvIntCore;
	unsigned int u32IntHandleTimes[INTSRC_NUM];
	unsigned int u32IntSendTimes[INTSRC_NUM];
	unsigned int u32SemId;
	unsigned int u32SemTakeTimes[INTSRC_NUM];
	unsigned int u32SemGiveTimes[INTSRC_NUM];
}IPC_DEBUG_E;


int BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl);

int BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl, IPCVOIDFUNCPTR routine, unsigned int parameter);

int BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);

void BSP_IPC_SpinLock (unsigned int u32SignalNum);

void BSP_IPC_SpinUnLock (unsigned int u32SignalNum);
/*Modify_for_c_reset, l00212112,20130511, starts*/
void BSP_IPC_SemGive_Ccore_All(void);
/*Modify_for_c_reset, l00212112,20130511, ends*/

void ipc_debug_int_info(unsigned long *poffset, unsigned int *psize);
int BSP_IPC_SemTake(unsigned int u32SignalNum,int s32timeout);
void BSP_IPC_SemGive(unsigned int u32SignalNum);
int BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl);
int BSP_IPC_SemCreate(unsigned int u32SignalNum);
int BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,IPCVOIDFUNCPTR routine, unsigned int parameter);

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_IPC_H_*/

