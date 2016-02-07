#ifndef __MDRV_EFUSE_COMMOM_H__
#define __MDRV_EFUSE_COMMOM_H__
#ifdef __cplusplus
extern "C"
{
#endif

    /************************************************************************
     *  函 数 名  : mdrv_efuse_write_huk
     *  功能描述  :  锁网锁卡特性需要烧写的HUK值。
     *  输入参数  :
     *
     *  输出参数  : 无
     *  返 回 值  :  0           操作成功。
     *                           其他    操作失败。
     *
     *************************************************************************/
    int mdrv_efuse_write_huk(char *pBuf, unsigned int len);

    /************************************************************************
     *  函 数 名  : mdrv_efuse_check_huk_valid
     *  功能描述  :  判断锁网锁卡的HUK值是否有效。
     *  输入参数  :
     *
     *  输出参数  : 无
     *  返 回 值  :  0           有效。
     *                           其他    无效。
     *
     *************************************************************************/
    int mdrv_efuse_check_huk_valid (void);

#ifdef __cplusplus
}
#endif
#endif
