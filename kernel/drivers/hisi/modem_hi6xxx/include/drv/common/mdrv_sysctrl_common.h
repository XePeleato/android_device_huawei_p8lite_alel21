#ifndef __MDRV_SYSCTRL_COMMON_H__
#define __MDRV_SYSCTRL_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif
	/*****************************************************************************
	 *  函 数 名  : mdrv_sysctrl_set_regval
	 *  功能描述  : sysctrl写接口供GU物理层调用。
	 *  输入参数  : regAddr:SC寄存器偏移地址; setData:SC寄存器值。
	 *  输出参数  : 无。
	 *  返 回 值  : 0:  操作成功；
	 *             -1：操作失败。
	 ******************************************************************************/
	int mdrv_sysctrl_set_regval(unsigned int regAddr, unsigned int data);

	/*****************************************************************************
	 *  函 数 名  : mdrv_sysctrl_get_regval
	 *  功能描述  : sysctrl读接口供GU物理层调用。
	 *  输入参数  : regAddr:SC寄存器偏移地址; getData:返回的SC寄存器值。
	 *  输出参数  : 无。
	 *  返 回 值  : 0:  操作成功；
	 *             -1： 操作失败。
	 ******************************************************************************/
	int mdrv_sysctrl_get_regval(unsigned int regAddr, unsigned int *data);

#ifdef __cplusplus
}
#endif
#endif
