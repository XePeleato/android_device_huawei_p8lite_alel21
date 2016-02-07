#ifndef __MDRV_ACORE_ICC_H__
#define __MDRV_ACORE_ICC_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "mdrv_public.h"
/* will implement at moment of unified icc project */
//#include "mdrv_icc_common.h"

/************************************************************************
 * 函 数 名  : mdrv_icc_register_resume_cb
 * 功能描述  : 注册ipf 唤醒A核时的钩子函数，AP在睡眠状态下被CP唤醒
 * 方便定位是被什么事件唤醒的
 * 输入参数  :
 *            u32ChanId: icc channel id
 *            FUNCPTR_1: 回调函数
 *            param    : 回调函数参数
 * 输出参数  : 无
 * 返 回 值  :  0          操作成功。
 *             其他        操作失败。
 **************************************************************************/
int mdrv_icc_register_resume_cb(unsigned int u32ChanId, FUNCPTR_1 debug_routine, int param);

#ifdef __cplusplus
}
#endif
#endif
