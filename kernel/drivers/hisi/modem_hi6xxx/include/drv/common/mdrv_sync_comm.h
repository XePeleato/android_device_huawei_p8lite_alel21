#ifndef __MDRV_SYNC_COMM_H__
#define __MDRV_SYNC_COMM_H__
#ifdef __cplusplus
extern "C"
{
#endif

/*宏定义*/
#define BSP_ERR_SYNC_BASE                (BSP_S32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_SYNC, 0)))
#define BSP_ERR_SYNC_TIMEOUT             (BSP_ERR_SYNC_BASE + 0x0)

typedef enum tagSYNC_MODULE_E
{
    SYNC_MODULE_MEM     = 0,
    SYNC_MODULE_NANDC,
    SYNC_MODULE_PMU,
    SYNC_MODULE_SOCP,
    SYNC_MODULE_DMA,
    SYNC_MODULE_IPF,
    SYNC_MODULE_NV,
    SYNC_MODULE_YAFFS,
    SYNC_MODULE_MSP,
    SYNC_MODULE_GPIO,
    SYNC_MODULE_CSHELL,
    SYNC_MODULE_MNTN,
	SYNC_MODULE_MSPNV,
	SYNC_MODULE_ONLINE,
	SYNC_MODULE_CHG,
	SYNC_MODULE_TFUPDATE,
    SYNC_MODULE_BUTT,
}SYNC_MODULE_E;



/*****************************************************************************
* 函 数 名  : mdrv_sync_lock
*
* 功能描述  : 锁定并查询模块的初始化状态
*
* 输入参数  : SYNC_MODULE_E u32Module 模块ID
*             unsigned int *pState         模块的初始化状态
*             unsigned int u32TimeOut      超时值，0为永远等待
* 输出参数  :
*
* 返 回 值  : 0，成功；-1，参数错误；其他值，等待超时
*
* 其它说明  :
*
*****************************************************************************/
int mdrv_sync_lock(SYNC_MODULE_E u32Module, unsigned int *pState, unsigned int u32TimeOut);

/*****************************************************************************
* 函 数 名  : mdrv_sync_unlock
*
* 功能描述  : 解锁并更新模块初始化状态
*
* 输入参数  : SYNC_MODULE_E u32Module  模块ID
*             unsigned int u32State         更新模块初始化状态
* 输出参数  :
*
* 返 回 值  : 0，成功；其他，失败
*
* 其它说明  :
*
*****************************************************************************/
int mdrv_sync_unlock(SYNC_MODULE_E u32Module, unsigned int u32State);

/*****************************************************************************
* 函 数 名  : mdrv_sync_wait
*
* 功能描述  : 从核等待主核操作完成函数
*
* 输入参数  : SYNC_MODULE_E u32Module  模块ID
*             unsigned int u32TimeOut       超时值，0为永远等待
* 输出参数  :
*
* 返 回 值  : 0，成功；-1，参数错误；其他值，等待超时
*
* 其它说明  :
*
*****************************************************************************/
int mdrv_sync_wait(SYNC_MODULE_E u32Module, unsigned int u32TimeOut);

/*****************************************************************************
* 函 数 名  : mdrv_sync_give
*
* 功能描述  : 主核通知从核操作已经完成
*
* 输入参数  : SYNC_MODULE_E u32Module 模块ID
* 输出参数  :
*
* 返 回 值  : 0，成功；其他，失败
*
* 其它说明  :
*
*****************************************************************************/
int mdrv_sync_give(SYNC_MODULE_E u32Module);

#ifdef __cplusplus
}
#endif
#endif
