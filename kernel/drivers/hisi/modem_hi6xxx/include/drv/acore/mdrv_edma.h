#ifndef __MDRV_ACORE_EDMA_H__
#define __MDRV_ACORE_EDMA_H__
#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
	 *  函 数 名  : mdrv_edma_bbp_sample_reboot
	 *  功能描述  :  BBP 采数使用的重启接口
	 *  输入参数  :
	 *
	 *  输出参数  : 无
	 *  返 回 值  :  0	       执行重启
        *                           其他   不执行重启
     	 *
	 ******************************************************************************/
int  mdrv_edma_bbp_sample_reboot(void);

#ifdef __cplusplus
}
#endif
#endif

