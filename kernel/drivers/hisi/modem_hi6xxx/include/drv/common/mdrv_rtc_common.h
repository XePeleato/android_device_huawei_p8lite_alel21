#ifndef __MDRV_RTC_COMMON_H__
#define __MDRV_RTC_COMMON_H__
#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 函 数 名  : mdrv_rtc_get_value
 功能描述  : 用来获取RTC的定时器的值。
 输入参数  : 无
 输出参数  : 无
 返回值    ：RTC的定时器的值
*****************************************************************************/
unsigned int mdrv_rtc_get_value(void);

#ifdef __cplusplus
}
#endif
#endif
