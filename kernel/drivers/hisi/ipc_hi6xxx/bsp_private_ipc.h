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
#include "soc_irqs.h"
#include "soc_ipc_interface.h"
#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************
*  添加新IPC资源，枚举命名格式:
*  IPC_<目标处理器>_INT_SRC_<源处理器>_<功能/作用>
*  目标处理器:ACPU、CCPU、MCU、HIFI、BBE16
*  源处理器  :ACPU、CCPU、MCU、HIFI、BBE16
*  功能/作用 :
*********************************************************/
typedef enum tagNOSEC_IPC2_INT_LEV_E
{
    NOSEC_IPC2_INT_BUTTOM                      = 32,
}NOSEC_IPC2_INT_LEV_E;

#define SIZE_4K             (4096)
#define IPC_REG_SIZE	    (SIZE_4K)


#define BSP_RegRd(uwAddr)                                     (*((volatile int *)(uwAddr)))
#define BSP_RegWr(uwAddr, uwValue)                            (*((volatile int *)(uwAddr)) = uwValue)

#define IPC_CHECK_PARA(ulLvl) \
	do{\
		if(ulLvl >= 32)\
		{\
			printk("Wrong para , line:%d\n", __LINE__);\
			return -1;\
		}\
	}while(0)

	
int BSP_DRV_PRIVATE_IPCIntInit(void);
void clear_private_ipc_int(unsigned int  enTarget, unsigned int enIntSrc);
int BSP_PRIVATE_IPC_IntEnable (unsigned int ulLvl);
int BSP_PRIVATE_IPC_IntDisable (NOSEC_IPC2_INT_LEV_E ulLvl);
	
#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_IPC_H_*/

