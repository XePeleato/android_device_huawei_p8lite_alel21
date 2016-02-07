#ifndef __MDRV_ACORE_WDT_H__
#define __MDRV_ACORE_WDT_H__
#ifdef __cplusplus
extern "C"
{
#endif

    /*****************************************************************************
     *  函 数 名  : mdrv_wdt_feed
     *  功能描述  : 重置开门狗
     *  输入参数  : wdtId    开门狗ID
     *
     *  输出参数  : 无
     *  返 回 值  :  0       操作成功。
     *               其他    操作失败。
     *
     ******************************************************************************/
    int  mdrv_wdt_feed(unsigned char wdtId);

#ifdef __cplusplus
}
#endif

#endif
